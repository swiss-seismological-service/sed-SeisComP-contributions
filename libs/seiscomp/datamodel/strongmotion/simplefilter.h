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


#ifndef SEISCOMP_DATAMODEL_STRONGMOTION_SIMPLEFILTER_H
#define SEISCOMP_DATAMODEL_STRONGMOTION_SIMPLEFILTER_H


#include <vector>
#include <string>
#include <seiscomp/datamodel/notifier.h>
#include <seiscomp/datamodel/publicobject.h>
#include <seiscomp/core/exceptions.h>
#include <seiscomp/datamodel/strongmotion/api.h>


namespace Seiscomp {
namespace DataModel {
namespace StrongMotion {


DEFINE_SMARTPOINTER(SimpleFilter);
DEFINE_SMARTPOINTER(FilterParameter);

class StrongMotionParameters;


class SC_STRONGMOTION_API SimpleFilter : public PublicObject {
	DECLARE_SC_CLASS(SimpleFilter);
	DECLARE_SERIALIZATION;
	DECLARE_METAOBJECT;

	// ------------------------------------------------------------------
	//  Xstruction
	// ------------------------------------------------------------------
	protected:
		//! Protected constructor
		SimpleFilter();

	public:
		//! Copy constructor
		SimpleFilter(const SimpleFilter& other);

		//! Constructor with publicID
		SimpleFilter(const std::string& publicID);

		//! Destructor
		~SimpleFilter();
	

	// ------------------------------------------------------------------
	//  Creators
	// ------------------------------------------------------------------
	public:
		static SimpleFilter* Create();
		static SimpleFilter* Create(const std::string& publicID);


	// ------------------------------------------------------------------
	//  Lookup
	// ------------------------------------------------------------------
	public:
		static SimpleFilter* Find(const std::string& publicID);


	// ------------------------------------------------------------------
	//  Operators
	// ------------------------------------------------------------------
	public:
		//! Copies the metadata of other to this
		//! No changes regarding child objects are made
		SimpleFilter& operator=(const SimpleFilter& other);
		//! Checks for equality of two objects. Childs objects
		//! are not part of the check.
		bool operator==(const SimpleFilter& other) const;
		bool operator!=(const SimpleFilter& other) const;

		//! Wrapper that calls operator==
		bool equal(const SimpleFilter& other) const;


	// ------------------------------------------------------------------
	//  Setters/Getters
	// ------------------------------------------------------------------
	public:
		void setType(const std::string& type);
		const std::string& type() const;

	
	// ------------------------------------------------------------------
	//  Public interface
	// ------------------------------------------------------------------
	public:
		/**
		 * Add an object.
		 * @param obj The object pointer
		 * @return true The object has been added
		 * @return false The object has not been added
		 *               because it already exists in the list
		 *               or it already has another parent
		 */
		bool add(FilterParameter* obj);

		/**
		 * Removes an object.
		 * @param obj The object pointer
		 * @return true The object has been removed
		 * @return false The object has not been removed
		 *               because it does not exist in the list
		 */
		bool remove(FilterParameter* obj);

		/**
		 * Removes an object of a particular class.
		 * @param i The object index
		 * @return true The object has been removed
		 * @return false The index is out of bounds
		 */
		bool removeFilterParameter(size_t i);

		//! Retrieve the number of objects of a particular class
		size_t filterParameterCount() const;

		//! Index access
		//! @return The object at index i
		FilterParameter* filterParameter(size_t i) const;

		//! Find an object by its unique attribute(s)
		FilterParameter* findFilterParameter(FilterParameter* filterParameter) const;

		StrongMotionParameters* strongMotionParameters() const;

		//! Implement Object interface
		bool assign(Object* other);
		bool attachTo(PublicObject* parent);
		bool detachFrom(PublicObject* parent);
		bool detach();

		//! Creates a clone
		Object* clone() const;

		//! Implement PublicObject interface
		bool updateChild(Object* child);

		void accept(Visitor*);


	// ------------------------------------------------------------------
	//  Implementation
	// ------------------------------------------------------------------
	private:
		// Attributes
		std::string _type;

		// Aggregations
		std::vector<FilterParameterPtr> _filterParameters;

	DECLARE_SC_CLASSFACTORY_FRIEND(SimpleFilter);
};


}
}
}


#endif
