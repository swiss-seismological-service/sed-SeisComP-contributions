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

#ifndef __SEISCOMP_APPLICATIONS_WFPARAM_MSG_H__
#define __SEISCOMP_APPLICATIONS_WFPARAM_MSG_H__


#include "util.h"
#include <seiscomp/messaging/connection.h>


bool sendMessages(Seiscomp::Client::Connection *con,
                  Seiscomp::DataModel::Event *evt,
                  Seiscomp::DataModel::Origin *org,
                  Seiscomp::DataModel::Magnitude *mag,
                  const Seiscomp::StationMap &results);


#endif
