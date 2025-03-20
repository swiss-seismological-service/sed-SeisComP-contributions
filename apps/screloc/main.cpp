/***************************************************************************
 *   Copyright (C) by ETHZ/SED                                             *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU Affero General Public License as published*
 * by the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU Affero General Public License for more details.                     *
 *                                                                         *
 *   Developed by gempa GmbH                                               *
 ***************************************************************************/

#define SEISCOMP_COMPONENT screloc

#include <seiscomp/logging/log.h>
#include <seiscomp/core/datamessage.h>
#include <seiscomp/client/application.h>
#include <seiscomp/client/inventory.h>
#include <seiscomp/datamodel/eventparameters.h>
#include <seiscomp/datamodel/origin.h>
#include <seiscomp/datamodel/pick.h>
#include <seiscomp/datamodel/network.h>
#include <seiscomp/datamodel/station.h>
#include <seiscomp/datamodel/sensorlocation.h>
#include <seiscomp/datamodel/utils.h>
#include <seiscomp/datamodel/publicobjectcache.h>
#include <seiscomp/seismology/locatorinterface.h>
#include <seiscomp/io/archive/xmlarchive.h>


#include <iostream>

using namespace std;
using namespace Seiscomp;
using namespace Seiscomp::Core;
using namespace Seiscomp::DataModel;
using namespace Seiscomp::Seismology;


class Reloc : public Client::Application {
	public:
		Reloc(int argc, char **argv) : Client::Application(argc, argv) {
			setMessagingEnabled(true);
			setDatabaseEnabled(true, true);
			setLoadStationsEnabled(true);
			setPrimaryMessagingGroup("LOCATION");

			// Listen for picks (caching) and origins
			addMessagingSubscription("PICK");
			addMessagingSubscription("LOCATION");

			// set up 1 hour of pick caching
			// otherwise the picks will be read from the database
			_cache.setTimeSpan(Core::TimeSpan(60 * 60, 0));

			_useWeight = false;
			_originEvaluationMode = "AUTOMATIC";
			_adoptFixedDepth = false;
			_repeatedRelocationCount = 1;
			_storeSourceOriginID = false;
		}


	protected:
		void createCommandLineDescription() override {
			commandline().addGroup("Mode");
			commandline().addOption("Mode", "test",
			                        "Test mode, do not send any message.");
			commandline().addOption("Mode", "dump",
			                        "Dump processed origins as XML to stdout.");
			commandline().addGroup("Input");
			commandline().addOption("Input", "origin-id,O",
			                        "Reprocess the origin and send a message.",
			                        &_originIDs);
			commandline().addOption("Input", "locator",
			                        "The locator type to use.", &_locatorType, false);
			commandline().addOption("Input", "profile",
			                        "The locator profile to use.", &_locatorProfile, false);
			commandline().addOption("Input", "use-weight",
			                        "Use current picks weight.", &_useWeight, true);
			commandline().addOption("Input", "ep",
			                        "Event parameters XML file for offline "
			                        "processing of all contained origins. This "
			                        "option should not be mixed with --dump.",
			                        &_epFile);
			commandline().addOption("Input", "replace",
			                        "Used in combination with --ep and defines "
			                        "if origins are to be replaced by their "
			                        "relocated counterparts or just added to the output.");
			commandline().addOption("Input", "drop-failure",
			                        "Used in combination with --replace/--ep "
			                        "and drops from the output the origins for "
			                        "which the relocation failed.");
			commandline().addGroup("Output");
			commandline().addOption("Output", "origin-id-suffix",
			                        "Create origin ID from that of the input "
			                        "origin plus the specfied suffix.",
			                        &_originIDSuffix);
			commandline().addOption("Output", "evaluation-mode",
			                        "Evaluation mode of the new origin (AUTOMATIC or MANUAL).",
			                        &_originEvaluationMode, true);
			commandline().addGroup("Profiling");
			commandline().addOption("Profiling", "measure-relocation-time",
			                        "Measure and log the time it takes to run each relocation.");
			commandline().addOption("Profiling", "repeated-relocations",
			                        "Improve measurement of relocation time by "
			                        "running each relocation multiple times.",
			                        &_repeatedRelocationCount);
		}


		bool initConfiguration() override {
			// first call the application's configuration routine
			if ( !Client::Application::initConfiguration() ) {
				return false;
			}

			if (_repeatedRelocationCount < 1) {
				_repeatedRelocationCount = 1;
			}

			try { _locatorType = configGetString("reloc.locator"); }
			catch ( ... ) {}

			try { _locatorProfile = configGetString("reloc.profile"); }
			catch ( ... ) {}

			try { _ignoreRejected = configGetBool("reloc.ignoreRejectedOrigins"); }
			catch ( ... ) {}

			try { _allowPreliminary = configGetBool("reloc.allowPreliminaryOrigins"); }
			catch ( ... ) {}

			try { _allowAnyStatus = configGetBool("reloc.allowAnyStatus"); }
			catch ( ... ) {}

			try { _allowManual = configGetBool("reloc.allowManualOrigins"); }
			catch ( ... ) {
				if ( !_originIDs.empty() || !_epFile.empty()) {
					_allowManual = true;
					std::cerr << "OriginID or XML file provided: activating option reloc.allowManualOrigins" << endl;
				}
			}

			try { _adoptFixedDepth = configGetBool("reloc.adoptFixedDepth"); }
			catch ( ... ) {}

			try { _useWeight = configGetBool("reloc.useWeight"); }
			catch ( ... ) {}

			try { _originIDSuffix = configGetString("reloc.originIDSuffix"); }
			catch ( ... ) {}

			try { _storeSourceOriginID = configGetBool("reloc.storeSourceOriginID"); }
			catch ( ... ) {}

			return true;
		}


		bool validateParameters() override {
			if ( !Client::Application::validateParameters() ) {
				return false;
			}

			if ( !_epFile.empty() ) {
				setMessagingEnabled(false);
			}

			if ( !isInventoryDatabaseEnabled() ) {
				setDatabaseEnabled(false, false);
			}

			if ( _allowAnyStatus ) {
				_allowPreliminary = true;
			}

			return true;
		}

		bool init() override {
			if ( !Client::Application::init() ) {
				return false;
			}

			_locator = LocatorInterfaceFactory::Create(_locatorType.c_str());
			if ( !_locator ) {
				SEISCOMP_ERROR("Locator %s is not available -> abort", _locatorType.c_str());
				SEISCOMP_DEBUG("  + examples:");
				SEISCOMP_DEBUG("    + LOCSAT");
				SEISCOMP_DEBUG("    + NonLinLoc (add locnll plugin)");
				SEISCOMP_DEBUG("    + Hypo71 (add hypo71 plugin)");
				return false;
			}

			_inputOrgs = addInputObjectLog("origin");
			_outputOrgs = addOutputObjectLog("origin", primaryMessagingGroup());

			_cache.setDatabaseArchive(query());
			_locator->init(configuration());

			if ( !_locatorProfile.empty() )
				_locator->setProfile(_locatorProfile);

			if ( _originEvaluationMode != "AUTOMATIC" && _originEvaluationMode != "MANUAL") {
				SEISCOMP_ERROR("output evaluation-mode is %s ", _originEvaluationMode.c_str());
				SEISCOMP_ERROR("  + must be (AUTOMATIC|MANUAL)");
				return false;
			}

			SEISCOMP_DEBUG("Running with configuration:");
			SEISCOMP_DEBUG("  + Locator / profile: %s / %s", _locatorType.c_str(), _locatorProfile.c_str());
			SEISCOMP_DEBUG("  + ignoreRejectOrigins: %s", _ignoreRejected?"True":"False");
			SEISCOMP_DEBUG("  + allowAnyStatus: %s", _allowAnyStatus?"True":"False");
			SEISCOMP_DEBUG("  + allowPreliminaryOrigins: %s", _allowPreliminary?"True":"False");
			SEISCOMP_DEBUG("  + allowManualOrigins: %s", _allowManual?"True":"False");
			SEISCOMP_DEBUG("  + useWeight: %s", _useWeight?"True":"False");
			SEISCOMP_DEBUG("  + adoptFixedDepth: %s", _adoptFixedDepth?"True":"False");
			SEISCOMP_DEBUG("  + evaluation mode of relocated origins: %s", _originEvaluationMode.c_str());
			if ( !_originIDs.empty() ) {
				SEISCOMP_DEBUG("  + considered origins: %s", toString<string>(_originIDs).c_str());
			}
			if ( !_originIDSuffix.empty() ) {
				SEISCOMP_DEBUG("  + for relocated origins add suffix to ID or original origins: %s",
				               _originIDSuffix.c_str());
			}
			if ( !_epFile.empty() ) {
				SEISCOMP_DEBUG("  + input XML file: %s", _epFile.c_str());
			}

			return true;
		}


		bool run() override {
			if ( !_epFile.empty() ) {
				// Disable database
				setDatabase(nullptr);
				_cache.setDatabaseArchive(nullptr);

				SEISCOMP_INFO("Processing file %s", _epFile.c_str());
				IO::XMLArchive ar;
				if ( !ar.open(_epFile.c_str()) ) {
					SEISCOMP_ERROR("  + failed to open %s", _epFile.c_str());
					return false;
				}

				EventParametersPtr ep;
				ar >> ep;
				ar.close();

				if ( !ep ) {
					SEISCOMP_ERROR("  + no event parameters found");
					return false;
				}

				int numberOfOrigins = (int)ep->originCount();
				SEISCOMP_INFO("  + found %i origins", numberOfOrigins);
				bool replace = commandline().hasOption("replace");
				bool dropfailure = commandline().hasOption("drop-failure");

				if ( !_originIDs.empty() ) {
					// test if requested origin was loaded
					for ( vector<string>::const_iterator it = _originIDs.begin();
						  it != _originIDs.end(); ++it ) {

						if ( !Origin::Find(it->c_str()) ) {
							SEISCOMP_ERROR("  + found no origin with ID: %s", it->c_str());
							return false;
						}

					}
				}

				int processed = 0, numRelocated = 0;
				for ( int i = 0; i < numberOfOrigins; ++i, ++processed ) {
					OriginPtr org = ep->origin(i);
					std::string publicID = org->publicID();
					SEISCOMP_DEBUG("Processing origin %s", publicID.c_str());

					bool processing = true;
					if ( !_originIDs.empty() ) {
						processing = false;
						for ( size_t i = 0; i < _originIDs.size(); ++i ) {
							if ( publicID == _originIDs[i] ) {
								processing = true;
							}
						}
						if ( !processing ) {
							SEISCOMP_DEBUG("  + skip origin, not in origin-id list");
							continue;
						}
					}

					bool relocated;
					try {
						org = process(org.get());
						relocated = true;
					}
					catch ( std::exception &e ) {
						SEISCOMP_ERROR("Failed processing origin %s - %s",
						               publicID.c_str(), e.what());
						relocated = false;
					}
					if ( !org ) { // safety belt, but it should not happen
						SEISCOMP_ERROR("Failed processing origin %s", publicID.c_str());
						relocated = false;
					}
					else if ( _ignoreRejected ) {
						try {
							if ( org->evaluationStatus() == REJECTED ) {
								SEISCOMP_DEBUG("  + evaluation status is REJECTED, drop relocation "
								               "of origin %s", publicID.c_str());
								relocated = false;
							}
						}
						catch ( ... ) {}
					}

					if ( replace && (relocated || dropfailure) ) {
						ep->removeOrigin(i);
						--i;
						--numberOfOrigins;
					}

					if ( ! relocated ) {
						continue;
					}

					if ( !_originIDSuffix.empty()) {
						org->setPublicID(publicID+_originIDSuffix);
						SEISCOMP_DEBUG("  + new origin publicID is derived from original");
					}
					SEISCOMP_DEBUG("  + relocated origin has publicID: %s ",
					                org->publicID().c_str());

					ep->add(org.get());
					numRelocated++;

					if (((processed+1) % 100) == 0 ) {
						SEISCOMP_INFO("  + processed %d origins", processed+1);
					}
				}
 				SEISCOMP_INFO("Origins processed / sucessfully relocated: %d / %d",
				              processed, numRelocated);

				ar.create("-");
				ar.setFormattedOutput(true);
				ar << ep;
				ar.close();

				return true;
			}
			else if ( !_originIDs.empty() ) {
				for ( size_t i = 0; i < _originIDs.size(); ++i ) {
					OriginPtr org = Origin::Cast(query()->getObject(Origin::TypeInfo(), _originIDs[i]));
					if ( !org ) {
						std::cerr << "ERROR: Origin with ID '" << _originIDs[i] << "' has not been found" << endl;
						continue;
					}

					std::string publicID = org->publicID();
					OriginPtr newOrg;
					SEISCOMP_DEBUG("Processing origin %s", publicID.c_str());
					try {
						newOrg = process(org.get());
					}
					catch ( std::exception &e ) {
						SEISCOMP_ERROR("  + processing failed - %s", e.what());
						continue;
					}
					if ( !_originIDSuffix.empty()) {
						newOrg->setPublicID(publicID+_originIDSuffix);
						SEISCOMP_DEBUG("  + new origin publicID is derived from original");
					}
					SEISCOMP_DEBUG("  + relocated origin has publicID: %s ",
					                newOrg->publicID().c_str());

					// Log warning messages
					string msgWarning = _locator->lastMessage(LocatorInterface::Warning);
					if ( !msgWarning.empty() )
						std::cerr << "WARNING: " << msgWarning << std::endl;

					bool sendOrigin = true;
					if ( _ignoreRejected ) {
						try {
							if ( newOrg->evaluationStatus() == REJECTED ) {
								SEISCOMP_DEBUG("  + Origin status is REJECTED, skip sending");
								sendOrigin = false;
							}
						}
						catch ( ... ) {}
					}

					if ( sendOrigin && !send(newOrg.get()) ) {
						SEISCOMP_ERROR("  + sending of processed origin failed");
						continue;
					}

					std::cerr << "INFO: new Origin created OriginID=" << newOrg.get()->publicID().c_str() << std::endl;
				}

				return true;
			}

			return Client::Application::run();
		}


	protected:
		void handleMessage(Core::Message* msg) override {
			Application::handleMessage(msg);

			DataMessage *dm = DataMessage::Cast(msg);
			if ( dm == NULL ) {
				return;
			}

			for ( DataMessage::iterator it = dm->begin(); it != dm->end(); ++it ) {
				Origin *org = Origin::Cast(it->get());
				if ( org ) {
					addObject("", org);
				}
			}
		}

		void printUsage() const override {
			cout << "Usage:"  << endl << "  " << name() << " [options]" << endl << endl
			     << "Relocate origins" << endl;

			Client::Application::printUsage();

			cout << "Examples:" << endl;
			cout << "Offline processing of all origins stored in XML file" << endl
			     << "  " << name() << " -d localhost --ep origins.xml"
			     << endl << endl;
		}

		void addObject(const std::string &parentID, Object *obj) override {
			Pick *pick = Pick::Cast(obj);
			if ( pick ) {
				_cache.feed(pick);
				return;
			}

			Origin *org = Origin::Cast(obj);
			if ( org ) {
				logObject(_inputOrgs, Core::Time::UTC());

				if ( isAgencyIDBlocked(objectAgencyID(org)) ) {
					SEISCOMP_DEBUG("%s: skipping: agencyID '%s' is blocked",
					               org->publicID().c_str(), objectAgencyID(org).c_str());
					return;
				}

				try {
					// ignore non automatic origins
					if ( org->evaluationMode() != AUTOMATIC && !_allowManual ) {
						SEISCOMP_DEBUG("%s: skipping: mode is not 'automatic'", org->publicID().c_str());
						return;
					}
				}
				// origins without an evaluation mode are treated as
				// automatic origins
				catch ( ... ) {}

				// Skip confirmed or otherwise tagged solutions unless
				// preliminary origins are allowed
				if ( !_allowAnyStatus ) {
					try {
						EvaluationStatus stat = org->evaluationStatus();
						if ( stat != PRELIMINARY || !_allowPreliminary ) {
							SEISCOMP_DEBUG("%s: skipping due to valid evaluation status", org->publicID().c_str());
							return;
						}
					}
					catch ( ... ) {}
				}

				OriginPtr newOrg;

				try {
					newOrg = process(org);
					if ( !newOrg ) {
						SEISCOMP_ERROR("processing of origin '%s' failed", org->publicID().c_str());
						return;
					}
				}
				catch ( std::exception &e ) {
					SEISCOMP_ERROR("%s: %s", org->publicID().c_str(), e.what());
					return;
				}

				string msgWarning = _locator->lastMessage(LocatorInterface::Warning);
				if ( !msgWarning.empty() )
					SEISCOMP_WARNING("%s: %s", org->publicID().c_str(), msgWarning.c_str());

				bool sendOrigin = true;
				if ( _ignoreRejected ) {
					try {
						if ( newOrg->evaluationStatus() == REJECTED ) {
							SEISCOMP_WARNING("%s: relocated origin has evaluation status REJECTED: result not sent",
							                 org->publicID().c_str());
							sendOrigin = false;
						}
					}
					catch ( ... ) {}
				}

				if ( sendOrigin && !send(newOrg.get()) ) {
					SEISCOMP_ERROR("%s: sending of derived origin failed", org->publicID().c_str());
					return;
				}

				return;
			}
		}


	private:
		OriginPtr process(Origin *org) {
			if ( org->arrivalCount() == 0 ) {
				query()->loadArrivals(org);
			}

			LocatorInterface::PickList picks;

			// Load all referenced picks and store them locally. Through the
			// global PublicObject pool they can then be found by the locator.
			for ( size_t i = 0; i < org->arrivalCount(); ++i ) {
				Arrival *ar = org->arrival(i);
				PickPtr pick = _cache.get<Pick>(ar->pickID());
				if ( !pick ) {
					continue;
				}

				if ( !_useWeight ) {
					// Set weight to 1
					ar->setWeight(1.0);
					ar->setTimeUsed(true);
					ar->setBackazimuthUsed(true);
					ar->setHorizontalSlownessUsed(true);
				}

				picks.push_back(pick.get());
			}

			_locator->useFixedDepth(false);

			if ( _adoptFixedDepth ) {
				try {
					if ( org->depthType() == OPERATOR_ASSIGNED )
						_locator->setFixedDepth(org->depth().value());
				}
				catch ( ... ) {}

				try {
					if ( org->depth().uncertainty() == 0.0 )
						_locator->setFixedDepth(org->depth().value());
				}
				catch ( ... ) {}
			}

			OriginPtr newOrg;
			Seiscomp::Util::StopWatch timer;
			timer.restart();

			for (size_t i=0; i<_repeatedRelocationCount; i++) {
				newOrg = _locator->relocate(org);
			}
			double seconds = (double) timer.elapsed() / _repeatedRelocationCount;

			if ( newOrg ) {
				if ( _originEvaluationMode == "AUTOMATIC" ) {
					newOrg->setEvaluationMode(EvaluationMode(AUTOMATIC));
				}
				else {
					newOrg->setEvaluationMode(EvaluationMode(MANUAL));
					SEISCOMP_DEBUG("  + set evaluation mode: 'manual'");
				}

				CreationInfo *ci;

				try {
					ci = &newOrg->creationInfo();
				}
				catch ( ... ) {
					newOrg->setCreationInfo(CreationInfo());
					newOrg->creationInfo().setCreationTime(Core::Time::UTC());
					ci = &newOrg->creationInfo();
				}

				ci->setAgencyID(agencyID());
				ci->setAuthor(author());

				// keep track of the triggering origin of this relocation
				if (_storeSourceOriginID ) {
					DataModel::CommentPtr comment = new DataModel::Comment();
					comment->setId("relocation::sourceOrigin");
					comment->setText(org->publicID());
					comment->setCreationInfo(*ci);
					newOrg->add(comment.get());
				}
			}

			if ( commandline().hasOption("measure-relocation-time") ) {
				double milliseconds = 1000.*seconds;
				SEISCOMP_DEBUG("  + time for relocating: %.3f ms", milliseconds);
			}

			if ( commandline().hasOption("dump") ) {
				EventParametersPtr ep = new EventParameters;
				ep->add(newOrg.get());

				for ( LocatorInterface::PickList::iterator it = picks.begin();
				      it != picks.end(); ++it ) {
					ep->add(it->pick.get());
				}

				IO::XMLArchive ar;
				ar.setFormattedOutput(true);
				ar.create("-");
				ar << ep;
				ar.close();
			}

			return newOrg;
		}


		bool send(Origin *org) {
			if ( !org ) {
				return false;
			}

			logObject(_outputOrgs, Core::Time::UTC());

			if ( commandline().hasOption("test") ) {
				return true;
			}

			EventParametersPtr ep = new EventParameters;

			bool wasEnabled = Notifier::IsEnabled();
			Notifier::Enable();

			// Insert origin to event parameters
			ep->add(org);

			NotifierMessagePtr msg = Notifier::GetMessage();

			bool result = false;
			if ( connection() ) {
				result = connection()->send(msg.get());
			}

			Notifier::SetEnabled(wasEnabled);

			return result;
		}


	private:
		std::vector<std::string>   _originIDs;
		std::string                _originIDSuffix;
		std::string                _locatorType;
		std::string                _locatorProfile;
		bool                       _ignoreRejected{false};
		bool                       _allowPreliminary{false};
		bool                       _allowAnyStatus{false};
		bool                       _allowManual{false};
		bool                       _adoptFixedDepth{false};
		LocatorInterfacePtr        _locator;
		PublicObjectTimeSpanBuffer _cache;
		ObjectLog                 *_inputOrgs;
		ObjectLog                 *_outputOrgs;
		bool                       _useWeight{false};
		bool                       _storeSourceOriginID{false};
		std::string                _originEvaluationMode;
		std::string                _epFile;
		size_t                     _repeatedRelocationCount;
};


int main(int argc, char **argv) {
	int retCode = EXIT_SUCCESS;

	// Create an own block to make sure the application object
	// is destroyed when printing the overall objectcount
	{
		Reloc app(argc, argv);
		retCode = app.exec();
	}

	return retCode;
}
