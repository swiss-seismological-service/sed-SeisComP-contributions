/***************************************************************************
 *   Copyright (C) by ETHZ/SED, GNS New Zealand, GeoScience Australia      *
 *                                                                         *
 *   You can redistribute and/or modify this program under the             *
 *   terms of the SeisComP Public License.                                 *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   SeisComP Public License for more details.                             *
 *                                                                         *
 *   Developed by gempa GmbH                                               *
 ***************************************************************************/

#include <seiscomp/core/plugin.h>
#include <seiscomp/datamodel/strongmotion/databasereader.h>


ADD_SC_PLUGIN("Data model extension for strong motion parameters",
              "ETHZ/SED, gempa GmbH <jabe@gempa.de>", 1, 0, 0)

// Dummy method to force linkage of libseiscomp_datamodel_sm
Seiscomp::DataModel::StrongMotion::StrongMotionReader *
createReader() {
	return new Seiscomp::DataModel::StrongMotion::StrongMotionReader(NULL);
}

