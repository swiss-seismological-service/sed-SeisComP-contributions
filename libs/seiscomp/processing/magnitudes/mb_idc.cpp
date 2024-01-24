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
 *                                                                         *
 * Other Usage                                                             *
 * Alternatively, this file may be used in accordance with the terms and   *
 * conditions contained in a signed written agreement between you and      *
 * gempa GmbH.                                                             *
 ***************************************************************************/


#define SEISCOMP_COMPONENT Magnitudes/mb_idc

#include "mb_idc_private.h"

#include <seiscomp3/logging/log.h>
#include <seiscomp3/system/environment.h>

#include <boost/thread/mutex.hpp>

#include <cmath> // log10
#include <vector> // log10
#include <cfloat>
#include <fstream>


#define AMP_TYPE "A5/2"
#define MAG_TYPE "mb(IDC)"

#define MINIMUM_DISTANCE 20.0   // in degrees
#define MAXIMUM_DISTANCE 105.0  // in degrees


using namespace std;


namespace {


std::string ExpectedAmplitudeUnit = "nm";

Util::TabValues tableQ;
bool validTableQ = false;
bool readTableQ = false;
boost::mutex mutexTableQ;
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Magnitude_mb_idc::Magnitude_mb_idc()
: MagnitudeProcessor(MAG_TYPE) {
	_amplitudeType = AMP_TYPE;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
std::string Magnitude_mb_idc::amplitudeType() const {
	return MagnitudeProcessor::amplitudeType();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
bool Magnitude_mb_idc::setup(const Settings &settings) {
	_Q.reset();

	if ( !MagnitudeProcessor::setup(settings) )
		return false;

	try {
		string tablePath = Environment::Instance()->absolutePath(settings.getString("magnitudes.mb(IDC).Q"));
		size_t p;

		p = tablePath.find("{net}");
		if ( p != string::npos ) {
			tablePath.replace(p, 5, settings.networkCode);
		}

		p = tablePath.find("{sta}");
		if ( p != string::npos ) {
			tablePath.replace(p, 5, settings.stationCode);
		}

		p = tablePath.find("{loc}");
		if ( p != string::npos ) {
			tablePath.replace(p, 5, settings.locationCode);
		}

		SEISCOMP_DEBUG("Read station specific mb(IDC) Q table at %s",
		               tablePath.c_str());

		_Q = new Util::TabValues;
		bool validLocalTable = _Q->read(tablePath);
		if ( !validLocalTable ) {
			_Q.reset();
			SEISCOMP_ERROR("Failed to read A values from: %s", tablePath.c_str());
			return false;
		}
	}
	catch ( ... ) {}

	if ( !_Q ) {
		mutexTableQ.lock();
		if ( !readTableQ ) {
			string tablePath = Environment::Instance()->absolutePath("@DATADIR@/magnitudes/IDC/qfvc.mb");
			SEISCOMP_DEBUG("Read global mb(IDC) Q table at %s", tablePath.c_str());

			validTableQ = tableQ.read(tablePath);
			if ( !validTableQ ) {
				SEISCOMP_ERROR("Failed to read Q values from: %s", tablePath.c_str());
			}

			readTableQ = true;
		}
		else if ( !validTableQ ) {
			SEISCOMP_ERROR("Invalid Q value table");
		}
		mutexTableQ.unlock();
		return validTableQ;
	}

	return true;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
MagnitudeProcessor::Status
Magnitude_mb_idc::computeMagnitude(double amplitude,
                                   const std::string &unit,
                                   double period, double,
                                   double delta, double depth,
                                   const DataModel::Origin *,
                                   const DataModel::SensorLocation *,
                                   const DataModel::Amplitude *,
                                   const Locale *,
                                   double &value) {
	if ( !validTableQ ) {
		return IncompleteConfiguration;
	}

	if ( period <= 0 ) {
		return PeriodOutOfRange;
	}

	if ( (delta < MINIMUM_DISTANCE) || (delta > MAXIMUM_DISTANCE) ) {
		return DistanceOutOfRange;
	}

	if ( !convertAmplitude(amplitude, unit, ExpectedAmplitudeUnit) ) {
		return InvalidAmplitudeUnit;
	}

	double x_1st_deriv, z_1st_deriv, x_2nd_deriv, z_2nd_deriv;
	double interpolated_value;
	int interp_err;

	if ( !tableQ.interpolate(interpolated_value, false, true,
	                         delta, depth, &x_1st_deriv, &z_1st_deriv,
	                         &x_2nd_deriv, &z_2nd_deriv, &interp_err) ) {
		return Error;
	}

	if ( interp_err ) {
		return Error;
	}

	value = log10(amplitude / period) + double(interpolated_value);

	return OK;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
REGISTER_MAGNITUDEPROCESSOR(Magnitude_mb_idc, MAG_TYPE);
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>}
}
