/***************************************************************************
 * Copyright (C) gempa GmbH                                                *
 * All rights reserved.                                                    *
 * Contact: gempa GmbH (seiscomp-dev@gempa.de)                             *
 *                                                                         *
 * GNU Affero General Public License Usage                                 *
 * This file may be used under the terms of the GNU Affero                 *
 * Public License version 3.0 as published by the Free Software Foundation *
 * and appearing in the file LICENSE included in the packaging of this     *
 * file. Please review the following information to ensure the GNU Affero  *
 * Public License version 3.0 requirements will be met:                    *
 * https://www.gnu.org/licenses/agpl-3.0.html.                             *
 ***************************************************************************/


#define SEISCOMP_COMPONENT SMDUMP

#include "app.h"

#include <seiscomp/logging/log.h>
#include <seiscomp/core/strings.h>
#include <seiscomp/datamodel/strongmotion/databasereader.h>
#include <seiscomp/datamodel/strongmotion/strongmotionparameters_package.h>
#include <seiscomp/io/archive/xmlarchive.h>

#include <iostream>


using namespace std;
using namespace Seiscomp;


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
SMDump::SMDump(int argc, char** argv)
: Application(argc, argv) {
	setMessagingEnabled(false);
	setDatabaseEnabled(true, true);
	setLoggingToStdErr(true);
	bindSettings(&_settings);
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
bool SMDump::run() {
	DataModel::StrongMotion::StrongMotionReader reader(database());
	DataModel::StrongMotion::StrongMotionParameters sp;
	DataModel::PublicObjectPtr po;

	std::set<std::string> recordIds;

	auto [baseQ, hasWhereClause] = reader.getObjectsQuery(string(), DataModel::StrongMotion::StrongOriginDescription::TypeInfo());
	if ( baseQ.empty() ) {
		SEISCOMP_ERROR("Internal error: failed to create database query");
		return false;
	}

	if ( !hasWhereClause ) {
		baseQ += " where ";
	}
	else {
		baseQ += " and ";
	}

	baseQ += DataModel::StrongMotion::StrongOriginDescription::TypeInfo().className();
	baseQ += ".";
	baseQ += reader.driver()->convertColumnName("originID");
	baseQ += "=";

	std::set<std::string> strongOriginIDs;

	auto fetch = [&](string q) {
		// Fetch all associated StrongOriginDescription
		vector<DataModel::StrongMotion::StrongOriginDescriptionPtr> orgs;

		auto it = reader.getObjectIterator(q, DataModel::StrongMotion::StrongOriginDescription::TypeInfo());
		for ( ; it.get(); ++it ) {
			auto sod = DataModel::StrongMotion::StrongOriginDescription::Cast(it.get());
			if ( !sod || (strongOriginIDs.find(sod->publicID()) != strongOriginIDs.end()) ) {
				continue;
			}

			strongOriginIDs.insert(sod->publicID());
			orgs.push_back(sod);
		}
		it.close();

		for ( auto &sod : orgs ) {
			if ( _settings.withEventRecordReferences ) {
				reader.loadEventRecordReferences(sod.get());

				if ( _settings.withRecords ) {
					for ( size_t i = 0; i < sod->eventRecordReferenceCount(); ++i ) {
						recordIds.insert(sod->eventRecordReference(i)->recordID());
					}
				}
			}

			if ( _settings.withRupture ) {
				reader.loadRuptures(sod.get());
			}

			sp.add(sod.get());
		}
	};

	for ( const auto &id : _settings.originIDs ) {
		string sid;
		reader.driver()->escape(sid, id);
		fetch(baseQ + "'" + sid + "'");
	}

	if ( _settings.preferredOnly ) {
		baseQ = Core::stringify(
			"SELECT PublicObject.%s, StrongOriginDescription.* "
			        "FROM  PublicObject, StrongOriginDescription, "
			      "Event, PublicObject as PEvent "
			"WHERE PublicObject._oid=StrongOriginDescription._oid AND "
			      "Event._oid=PEvent._oid AND PEvent.%s='%%s' AND "
			      "StrongOriginDescription.%s=Event.%s",
			reader.driver()->convertColumnName("publicID"),
			reader.driver()->convertColumnName("publicID"),
			reader.driver()->convertColumnName("originID"),
			reader.driver()->convertColumnName("preferredOriginID")
		);
	}
	else {
		baseQ = Core::stringify(
			"SELECT PublicObject.%s, StrongOriginDescription.* "
			        "FROM  PublicObject, StrongOriginDescription, "
			      "Event, PublicObject as PEvent, "
			      "OriginReference "
			"WHERE PublicObject._oid=StrongOriginDescription._oid AND "
			      "Event._oid=PEvent._oid AND PEvent.%s='%%s' AND "
			      "OriginReference._parent_oid=Event._oid AND "
			      "StrongOriginDescription.%s=OriginReference.%s",
			reader.driver()->convertColumnName("publicID"),
			reader.driver()->convertColumnName("publicID"),
			reader.driver()->convertColumnName("originID"),
			reader.driver()->convertColumnName("originID")
		);
	}

	for ( const auto &id : _settings.eventIDs ) {
		string sid;
		reader.driver()->escape(sid, id);
		fetch(Core::stringify(baseQ, sid));
	}

	for ( const auto &recordId : recordIds ) {
		po = reader.getObject(DataModel::StrongMotion::Record::TypeInfo(), recordId);
		if ( !po ) {
			SEISCOMP_WARNING("%s: Record not found", recordId);
			continue;
		}

		auto rec = DataModel::StrongMotion::Record::Cast(po);
		if ( !rec ) {
			SEISCOMP_ERROR("%s: Internal error, returned object is not a Record: aborting",
			               recordId);
			return false;
		}

		sp.add(rec);
	}

	IO::XMLArchive ar;

	ar.setFormattedOutput(_settings.formatted);
	ar.create((_settings.output.empty() ? "-" : _settings.output).data());

	auto tmp = &sp;
	ar << tmp;

	ar.close();

	return false;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
