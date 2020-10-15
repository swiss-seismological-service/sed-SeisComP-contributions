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


#ifndef SEISCOMP_DATAMODEL_STRONGMOTION_FILTERPARAMETER_H
#define SEISCOMP_DATAMODEL_STRONGMOTION_FILTERPARAMETER_H


#include <string>
#include <seiscomp/datamodel/realquantity.h>
#include <seiscomp/datamodel/object.h>
#include <seiscomp/core/exceptions.h>
#include <seiscomp/datamodel/strongmotion/api.h>


namespace Seiscomp {
namespace DataModel {
namespace StrongMotion {


DEFINE_SMARTPOINTER(FilterParameter);

class SimpleFilter;


class SC_STRONGMOTION_API FilterParameter : public Object {
	DECLARE_SC_CLASS(FilterParameter);
	DECLARE_SERIALIZATION;
	DECLARE_METAOBJECT;

	// ------------------------------------------------------------------
	//  Xstruction
	// ------------------------------------------------------------------
	public:
		//! Constructor
		FilterParameter();

		//! Copy constructor
		FilterParameter(const FilterParameter& other);

		//! Destructor
		~FilterParameter();
	

	// ------------------------------------------------------------------
	//  Operators
	// ------------------------------------------------------------------
	public:
		//! Copies the metadata of other to this
		FilterParameter& operator=(const FilterParameter& other);
		//! Checks for equality of two objects. Childs objects
		//! are not part of the check.
		bool operator==(const FilterParameter& other) const;
		bool operator!=(const FilterParameter& other) const;

		//! Wrapper that calls operator==
		bool equal(const FilterParameter& other) const;


	// ------------------------------------------------------------------
	//  Setters/Getters
	// ------------------------------------------------------------------
	public:
		void setValue(const RealQuantity& value);
		RealQuantity& value();
		const RealQuantity& value() const;

		void setName(const std::string& name);
		const std::string& name() const;

	
	// ------------------------------------------------------------------
	//  Public interface
	// ------------------------------------------------------------------
	public:
		SimpleFilter* simpleFilter() const;

		//! Implement Object interface
		bool assign(Object* other);
		bool attachTo(PublicObject* parent);
		bool detachFrom(PublicObject* parent);
		bool detach();

		//! Creates a clone
		Object* clone() const;

		void accept(Visitor*);


	// ------------------------------------------------------------------
	//  Implementation
	// ------------------------------------------------------------------
	private:
		// Attributes
		RealQuantity _value;
		std::string _name;
};


}
}
}


#endif
