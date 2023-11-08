/***************************************************************************
 * Copyright (C) gempa GmbH                                                *
 * All rights reserved.                                                    *
 * Contact: gempa GmbH (seiscomp-dev@gempa.de)                             *
 *                                                                         *
 * Author: Jan Becker                                                      *
 * Email: jabe@gempa.de                                                    *
 *                                                                         *
 * GNU Affero General Public License Usage                                 *
 * This file may be used under the terms of the GNU Affero                 *
 * Public License version 3.0 as published by the Free Software Foundation *
 * and appearing in the file LICENSE included in the packaging of this     *
 * file. Please review the following information to ensure the GNU Affero  *
 * Public License version 3.0 requirements will be met:                    *
 * https://www.gnu.org/licenses/agpl-3.0.html.                             *
 *                                                                         *
 * Other Usage                                                             *
 * Alternatively, this file may be used in accordance with the terms and   *
 * conditions contained in a signed written agreement between you and      *
 * gempa GmbH.                                                             *
 ***************************************************************************/


#define SEISCOMP_COMPONENT dbstore
#include <seiscomp/logging/log.h>
#include <seiscomp/core/plugin.h>
#include <seiscomp/core/system.h>
#include <seiscomp/datamodel/databasearchive.h>
#include <seiscomp/datamodel/notifier.h>
#include <seiscomp/datamodel/version.h>
#include <seiscomp/io/database.h>
#include <seiscomp/utils/timer.h>
#include <seiscomp/system/environment.h>
#include <seiscomp/system/settings.h>

#include <seiscomp/broker/message.h>
#include <seiscomp/broker/messageprocessor.h>


using namespace std;
using namespace Seiscomp;
using namespace Seiscomp::System;


namespace {


auto SchemaVersion = Core::Version(DataModel::Version::Major, DataModel::Version::Minor).toString();


class DBStore : public Messaging::Broker::MessageProcessor {
	public:
		DBStore() {
			setMode(Messages | Connections);
		}

		bool init(const Config::Config &cfg, const string &configPrefix) override {
			ConfigSettingsLinker linker;
			linker.configPrefix = configPrefix;
			linker.proc().get(cfg);
			_settings.accept(linker);

			if ( !linker ) {
				return false;
			}

			if ( _settings.driver.empty() ) {
				SEISCOMP_ERROR("'%sdriver' is not set", configPrefix.c_str());
				return false;
			}

			if ( _settings.write.empty() ) {
				SEISCOMP_ERROR("'%swrite' is not set", configPrefix.c_str());
				return false;
			}

			if ( _settings.read.empty() ) {
				SEISCOMP_WARNING("'%sread' is not set, " "no service will be provided",
				                 configPrefix.c_str());
			}

			SEISCOMP_DEBUG("Checking database '%s' and trying to connect", _settings.driver.c_str());

			_db = IO::DatabaseInterface::Create(_settings.driver.c_str());
			if ( _db == NULL ) {
				SEISCOMP_ERROR("Could not get database driver '%s'", _settings.driver.c_str());
				return false;
			}

			_operational = true;
			bool res = connect(0);

			_stopWatch.restart();
			_statistics = Statistics();
			_firstMessage = true;

			return res;
		}


		bool acceptConnection(Messaging::Broker::Client *,
		                      const KeyCStrValues, int,
		                      KeyValues &outParams) override {
			outParams.push_back(KeyValuePair("DB-Schema-Version", SchemaVersion));
			if ( !_settings.read.empty() ) {
				if ( _settings.proxy ) {
					outParams.push_back(KeyValuePair("DB-Access", "proxy://"));
				}
				else {
					outParams.push_back(KeyValuePair("DB-Access", _settings.driver + "://" + _settings.read));
				}
			}
			return true;
		}


		void dropConnection(Messaging::Broker::Client *) override {}


		bool process(Messaging::Broker::Message *tmsg) override {
			SEISCOMP_DEBUG("Writing message to database");

			if ( _firstMessage ) {
				DataModel::PublicObject::SetRegistrationEnabled(false);
				_firstMessage = false;
			}

			if ( !tmsg->object ) {
				tmsg->decode();
				if ( !tmsg->object ) {
					// Nothing to do
					return true;
				}
			}

			auto msg = Core::Message::Cast(tmsg->object.get());
			if ( !msg ) {
				// Just ignore unknown messages
				return true;
			}

			// int error = 0;
			for ( auto it = msg->iter(); *it; ++it ) {
				auto notifier = DataModel::Notifier::Cast(*it);
				if ( notifier && notifier->object() ) {
					bool result = false;
					while ( !result ) {
						switch ( notifier->operation() ) {
							case DataModel::OP_ADD: {
								++_statistics.addedObjects;
								DataModel::DatabaseObjectWriter writer(*_dbArchive.get());
								result = writer(notifier->object(), notifier->parentID());
							}
								break;
							case DataModel::OP_REMOVE:
								++_statistics.removedObjects;
								result = _dbArchive->remove(notifier->object(), notifier->parentID());
								break;
							case DataModel::OP_UPDATE:
								++_statistics.updatedObjects;
								result = _dbArchive->update(notifier->object(), notifier->parentID());
								break;
							default:
								break;
						}

						if ( !result ) {
							if ( !_db->isConnected() ) {
								SEISCOMP_ERROR("Lost connection to database: %s", _settings.write.c_str());
								while ( !connect() );
								if ( !_operational ) {
									SEISCOMP_INFO("Stopping dbstore");
									break;
								}
								else {
									SEISCOMP_INFO("Reconnected to database: %s", _settings.write.c_str());
								}
							}
							else {
								SEISCOMP_WARNING(
									"Error handling message from %s to %s",
									tmsg->sender.c_str(), tmsg->target.c_str()
								);

								// If no client connection error occurred -> go ahead because
								// wrong queries cannot be fixed here
								++_statistics.errors;
								result = true;
							}
						}
					}
				}
			}

			// For now we return true otherwise the master will stop because
			// e.g. an erroneous module sends the same notifier twice or more
			//return (error < 0) ? false : true;
			return true;
		}

		bool close() override {
			if ( _db && _db->isConnected() ) {
				_db->disconnect();
			}
			_operational = false;
			return true;
		}

		void getInfo(const Core::Time &, ostream &os) override {
			double elapsed = (double)_stopWatch.elapsed();
			if ( elapsed > 0.0 ) {
				double aa = _statistics.addedObjects / elapsed;
				double au = _statistics.updatedObjects / elapsed;
				double ar = _statistics.removedObjects / elapsed;
				double ae = _statistics.errors / elapsed;

				SEISCOMP_DEBUG("DBPLUGIN (aps,ups,dps,errors) %.2f %.2f %.2f %.2f",
				               aa, au, ar, ae);

				_stopWatch.restart();
				_statistics.addedObjects =
				_statistics.updatedObjects =
				_statistics.removedObjects =
				_statistics.errors = 0;

				os << "&dbadds=" << aa
				   << "&dbupdates=" << au
				   << "&dbdeletes=" << ar
				   << "&dberrors=" << ae;
			}
		}


	private:
		bool connect(int retries = 10) {
			int counter = 0;
			while ( _operational && !_db->connect(_settings.write.c_str()) ) {
				if ( !counter ) {
					SEISCOMP_ERROR("Database check... connection refused, retry");
				}

				if ( counter >= retries ) {
					SEISCOMP_ERROR("Database check... connection not available, abort");
					return false;
				}

				++counter;
				Core::sleep(1);
			}

			SEISCOMP_INFO("Database connection established");

			_dbArchive = new DataModel::DatabaseArchive(_db.get());

			if ( !_dbArchive ) {
				SEISCOMP_ERROR("DbPlugin: Could not create DBArchive");
				return false;
			}

			if ( _dbArchive->hasError() ) {
				return false;
			}

			auto localSchemaVersion = Core::Version(DataModel::Version::Major, DataModel::Version::Minor);
			if ( localSchemaVersion > _dbArchive->version() ) {
				SEISCOMP_WARNING("Database schema v%s is older than schema v%s "
				                 "currently supported. Information will be lost when "
				                 "saving objects to the database! This should be fixed!",
				                 _dbArchive->version().toString().c_str(),
				                 localSchemaVersion.toString().c_str());
				if ( _settings.strictVersionMatch ) {
					SEISCOMP_ERROR("Strict version check is enabled and schema versions "
					               "do not match.");
					return false;
				}
				else {
					SEISCOMP_INFO("Strict version check is disabled and different "
					              "schema versions are not treated as error");
				}
			}
			else {
				SEISCOMP_DEBUG("Database check... ok");
			}

			return true;
		}


	private:
		struct Settings {
			string driver;
			string write;
			string read;
			bool   proxy{false};
			bool   strictVersionMatch{true};

			void accept(ConfigSettingsLinker &linker) {
				linker
				& ConfigSettingsLinker::cfg(driver, "driver")
				& ConfigSettingsLinker::cfg(write, "write")
				& ConfigSettingsLinker::cfg(read, "read")
				& ConfigSettingsLinker::cfg(proxy, "proxy")
				& ConfigSettingsLinker::cfg(strictVersionMatch, "strictVersionMatch");
			}
		};

		struct Statistics {
			Statistics()
			: addedObjects(0), updatedObjects(0)
			, removedObjects(0), errors(0) {}

			size_t addedObjects;
			size_t updatedObjects;
			size_t removedObjects;
			size_t errors;
		};

		Settings                      _settings;
		IO::DatabaseInterfacePtr      _db;
		DataModel::DatabaseArchivePtr _dbArchive;
		bool                          _operational{false};
		bool                          _firstMessage{true};

		mutable Util::StopWatch       _stopWatch;
		mutable Statistics            _statistics;

};


ADD_SC_PLUGIN("scmaster dbstore plugin", "gempa GmbH <seiscomp-dev@gempa.de>", 0, 1, 0)
REGISTER_BROKER_MESSAGE_PROCESSOR(DBStore, "dbstore");


}
