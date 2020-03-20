/***************************************************************************
 *   Copyright (C) by ETHZ/SED, GNS New Zealand, GeoScience Australia      *
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


#ifndef __SEISCOMP_APPLICATIONS_SCENVELOPE_UTIL_H__
#define __SEISCOMP_APPLICATIONS_SCENVELOPE_UTIL_H__


#include <seiscomp/datamodel/inventory_package.h>
#include <seiscomp/datamodel/waveformstreamid.h>
#include <seiscomp/processing/waveformprocessor.h>
#include <string>
#include <set>
#include <map>


namespace Seiscomp {
namespace Private {


typedef std::set<std::string> StringSet;
typedef std::map<std::string, bool> StringPassMap;

struct StringFirewall {
	StringSet allow;
	StringSet deny;
	mutable StringPassMap cache;

	bool isAllowed(const std::string &s) const;
	bool isBlocked(const std::string &s) const;
};


std::string
toStreamID(const DataModel::WaveformStreamID &wfid);


DataModel::Stream*
findStreamMaxSR(DataModel::Station *station, const Core::Time &time,
                Processing::WaveformProcessor::SignalUnit requestedUnit,
                const StringFirewall *firewall);


}
}


#endif

