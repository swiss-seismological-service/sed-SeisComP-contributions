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

#include <seiscomp/core/plugin.h>
#include <seiscomp/datamodel/strongmotion/databasereader.h>


ADD_SC_PLUGIN("Data model extension for strong motion parameters",
              "ETHZ/SED, gempa GmbH <jabe@gempa.de>", 1, 0, 0)

// Dummy method to force linkage of libseiscomp_datamodel_sm
Seiscomp::DataModel::StrongMotion::StrongMotionReader *
createReader() {
	return new Seiscomp::DataModel::StrongMotion::StrongMotionReader(NULL);
}

