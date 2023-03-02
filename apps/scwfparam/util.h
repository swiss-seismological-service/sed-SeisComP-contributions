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


#ifndef __SEISCOMP_APPLICATIONS_AMPTOOL_UTIL_H__
#define __SEISCOMP_APPLICATIONS_AMPTOOL_UTIL_H__


#include <seiscomp/datamodel/types.h>
#include <seiscomp/datamodel/waveformstreamid.h>
#include <seiscomp/datamodel/inventory_package.h>
#include <seiscomp/processing/waveformprocessor.h>
#include "processors/pgav.h"
#include <set>


namespace Seiscomp {

namespace DataModel {

class Origin;
class Arrival;
class Amplitude;
class WaveformStreamID;

}


typedef std::pair<double,double> FilterFreqs;


struct PGAVResult {
	DataModel::WaveformStreamID streamID;

	bool             processed;
	bool             valid;
	bool             isVelocity;
	bool             isVertical;
	bool             isAcausal;
	double           maxRawAmplitude;
	double           pga;
	double           pgv;
	double           psa03;
	double           psa10;
	double           psa30;
	OPT(double)      duration;
	int              pdFilterOrder;
	FilterFreqs      pdFilter;
	int              filterOrder;
	FilterFreqs      filter;
	std::string      recordID;

	Core::Time       trigger;
	Core::Time       startTime;
	Core::Time       endTime;

	std::string      filename;

	Processing::PGAV::ResponseSpectra responseSpectra;
	const Processing::PGAV::ResponseSpectrum *responseSpectrum;
};


typedef std::vector<PGAVResult*> StationResults;
typedef std::map<std::string, StationResults> StationMap;


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


bool equivalent(const DataModel::WaveformStreamID&, const DataModel::WaveformStreamID&);

double
arrivalWeight(const DataModel::Arrival *arr, double defaultWeight=1.);

double
arrivalDistance(const DataModel::Arrival *arr);

DataModel::EvaluationStatus
status(const DataModel::Origin *origin);

char
shortPhaseName(const std::string &phase);

std::string
toStationID(const DataModel::WaveformStreamID &wfid);

std::string
toStreamID(const DataModel::WaveformStreamID &wfid);

DataModel::WaveformStreamID
setStreamComponent(const DataModel::WaveformStreamID &wfid, char comp);

DataModel::Stream*
findStream(DataModel::Station *station, const Core::Time &time,
           Processing::WaveformProcessor::SignalUnit requestedUnit);

DataModel::Stream*
findStreamMaxSR(DataModel::Station *station, const Core::Time &time,
                Processing::WaveformProcessor::SignalUnit requestedUnit,
                const StringFirewall *firewall);


}
}


#endif

