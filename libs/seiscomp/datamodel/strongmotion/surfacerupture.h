/***************************************************************************
 *   Copyright (C) by ETHZ/SED                                             *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as published*
 * by the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU Affero General Public License for more details.                     *
 *                                                                         *
 *   Developed by gempa GmbH                                               *
 ***************************************************************************/

// This file was created by a source code generator.
// Do not modify the contents. Change the definition and run the generator
// again!


#ifndef SEISCOMP_DATAMODEL_STRONGMOTION_SURFACERUPTURE_H
#define SEISCOMP_DATAMODEL_STRONGMOTION_SURFACERUPTURE_H


#include <string>
#include <seiscomp/datamodel/strongmotion/literaturesource.h>
#include <seiscomp/core/baseobject.h>
#include <seiscomp/core/exceptions.h>
#include <seiscomp/datamodel/strongmotion/api.h>


namespace Seiscomp {
namespace DataModel {
namespace StrongMotion {


DEFINE_SMARTPOINTER(SurfaceRupture);


class SC_STRONGMOTION_API SurfaceRupture : public Core::BaseObject {
	DECLARE_SC_CLASS(SurfaceRupture);
	DECLARE_SERIALIZATION;
	DECLARE_METAOBJECT;

	// ------------------------------------------------------------------
	//  Xstruction
	// ------------------------------------------------------------------
	public:
		//! Constructor
		SurfaceRupture();

		//! Copy constructor
		SurfaceRupture(const SurfaceRupture& other);

		//! Destructor
		~SurfaceRupture();
	

	// ------------------------------------------------------------------
	//  Operators
	// ------------------------------------------------------------------
	public:
		//! Copies the metadata of other to this
		SurfaceRupture& operator=(const SurfaceRupture& other);
		//! Checks for equality of two objects. Childs objects
		//! are not part of the check.
		bool operator==(const SurfaceRupture& other) const;
		bool operator!=(const SurfaceRupture& other) const;

		//! Wrapper that calls operator==
		bool equal(const SurfaceRupture& other) const;


	// ------------------------------------------------------------------
	//  Setters/Getters
	// ------------------------------------------------------------------
	public:
		void setObserved(bool observed);
		bool observed() const;

		void setEvidence(const std::string& evidence);
		const std::string& evidence() const;

		void setLiteratureSource(const OPT(LiteratureSource)& literatureSource);
		LiteratureSource& literatureSource();
		const LiteratureSource& literatureSource() const;


	// ------------------------------------------------------------------
	//  Implementation
	// ------------------------------------------------------------------
	private:
		// Attributes
		bool _observed;
		std::string _evidence;
		OPT(LiteratureSource) _literatureSource;
};


}
}
}


#endif
