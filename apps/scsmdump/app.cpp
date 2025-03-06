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

	for ( const auto &id : _settings.originIDs ) {
		po = reader.getObject(
			DataModel::StrongMotion::StrongOriginDescription::TypeInfo(), id
		);
		if ( !po ) {
			SEISCOMP_WARNING("%s: StrongOriginDescription not found", id);
			continue;
		}

		auto sod = DataModel::StrongMotion::StrongOriginDescription::Cast(po);
		if ( !sod ) {
			SEISCOMP_ERROR("%s: Internal error, returned object is not a StrongOriginDescription: aborting",
			               id);
			return false;
		}

		if ( _settings.withEventRecordReferences ) {
			reader.loadEventRecordReferences(sod);

			if ( _settings.withRecords ) {
				for ( size_t i = 0; i < sod->eventRecordReferenceCount(); ++i ) {
					recordIds.insert(sod->eventRecordReference(i)->recordID());
				}
			}
		}

		if ( _settings.withRupture ) {
			reader.loadRuptures(sod);
		}

		sp.add(sod);
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
