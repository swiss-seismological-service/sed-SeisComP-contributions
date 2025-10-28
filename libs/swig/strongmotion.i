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

%module(package="seiscomp.datamodel") strongmotion

%{
#define SWIG_FILE_WITH_INIT
#include <seiscomp/core/typedarray.h>
#include <seiscomp/core/record.h>
#include <seiscomp/core/greensfunction.h>
#include <seiscomp/core/genericrecord.h>
#include <seiscomp/core/datamessage.h>
#include <seiscomp/geo/coordinate.h>
#include <seiscomp/geo/boundingbox.h>
#include <seiscomp/geo/feature.h>
#include <seiscomp/geo/featureset.h>
#include <seiscomp/math/geo.h>
#include <seiscomp/math/coord.h>
#include <seiscomp/math/math.h>
#include <seiscomp/math/filter.h>
#include <seiscomp/math/filter/rmhp.h>
#include <seiscomp/math/filter/taper.h>
#include <seiscomp/math/filter/average.h>
#include <seiscomp/math/filter/stalta.h>
#include <seiscomp/math/filter/chainfilter.h>
#include <seiscomp/math/filter/biquad.h>
#include <seiscomp/math/filter/butterworth.h>
#include <seiscomp/math/filter/taper.h>
#include <seiscomp/math/filter/seismometers.h>
#include <seiscomp/math/restitution/transferfunction.h>
#include <seiscomp/io/database.h>
#include <seiscomp/io/recordinput.h>
#include <seiscomp/io/recordinput.h>
#include <seiscomp/io/recordfilter.h>
#include <seiscomp/io/recordfilter/crop.h>
#include <seiscomp/io/recordfilter/demux.h>
#include <seiscomp/io/recordfilter/iirfilter.h>
#include <seiscomp/io/recordfilter/mseedencoder.h>
#include <seiscomp/io/recordfilter/pipe.h>
#include <seiscomp/io/recordfilter/resample.h>
#include <seiscomp/io/importer.h>
#include <seiscomp/io/exporter.h>
#include <seiscomp/io/gfarchive.h>
#include <seiscomp/io/archive/binarchive.h>
#include <seiscomp/io/archive/xmlarchive.h>
#include <seiscomp/io/records/mseedrecord.h>
#include <seiscomp/datamodel/notifier.h>
#include <seiscomp/datamodel/publicobjectcache.h>
#include <seiscomp/datamodel/utils.h>
#include <seiscomp/datamodel/diff.h>
#include <seiscomp/datamodel/messages.h>
#include <seiscomp/io/recordstream/file.h>
#include <seiscomp/io/recordstream/slconnection.h>
#include <seiscomp/io/recordstream/combined.h>

#include <seiscomp/datamodel/strongmotion/strongmotionparameters_package.h>
#include <seiscomp/datamodel/strongmotion/databasereader.h>
#include <seiscomp/core/interruptible.h>
%}

%include "stl.i"
%include "std_string.i"
%include "std_complex.i"
%include "std_vector.i"
%include "exception.i"

/* SeisComP */
%import "swig/datamodel.i"

%newobject *::Create;
%newobject *::find;
%newobject *::Find;

%newobject Seiscomp::DataModel::DatabaseReader::loadStrongMotionParameters;

%include "seiscomp/datamodel/strongmotion/api.h"
%include "seiscomp/datamodel/strongmotion/types.h"
%include "seiscomp/datamodel/strongmotion/simplefilterchainmember.h"
%include "seiscomp/datamodel/strongmotion/simplefilter.h"
%include "seiscomp/datamodel/strongmotion/contact.h"
%include "seiscomp/datamodel/strongmotion/eventrecordreference.h"
%include "seiscomp/datamodel/strongmotion/fileresource.h"
%include "seiscomp/datamodel/strongmotion/filterparameter.h"
%include "seiscomp/datamodel/strongmotion/literaturesource.h"
%include "seiscomp/datamodel/strongmotion/surfacerupture.h"
%include "seiscomp/datamodel/strongmotion/peakmotion.h"
%include "seiscomp/datamodel/strongmotion/record.h"
%include "seiscomp/datamodel/strongmotion/rupture.h"
%include "seiscomp/datamodel/strongmotion/strongorigindescription.h"
%include "seiscomp/datamodel/strongmotion/strongmotionparameters.h"
%include "seiscomp/datamodel/strongmotion/databasereader.h"
