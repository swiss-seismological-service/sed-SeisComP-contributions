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


#ifndef SEISCOMP_DATAMODEL_STRONGMOTION_STRONGMOTIONPARAMETERS_H
#define SEISCOMP_DATAMODEL_STRONGMOTION_STRONGMOTIONPARAMETERS_H


#include <vector>
#include <seiscomp/datamodel/notifier.h>
#include <seiscomp/datamodel/publicobject.h>
#include <seiscomp/core/exceptions.h>
#include <seiscomp/datamodel/strongmotion/api.h>


namespace Seiscomp {
namespace DataModel {
namespace StrongMotion {


DEFINE_SMARTPOINTER(StrongMotionParameters);
DEFINE_SMARTPOINTER(SimpleFilter);
DEFINE_SMARTPOINTER(Record);
DEFINE_SMARTPOINTER(StrongOriginDescription);


class SC_STRONGMOTION_API StrongMotionParameters : public PublicObject {
	DECLARE_SC_CLASS(StrongMotionParameters);
	DECLARE_SERIALIZATION;
	DECLARE_METAOBJECT;

	// ------------------------------------------------------------------
	//  Xstruction
	// ------------------------------------------------------------------
	public:
		//! Constructor
		StrongMotionParameters();

		//! Copy constructor
		StrongMotionParameters(const StrongMotionParameters& other);

		//! Destructor
		~StrongMotionParameters();
	

	// ------------------------------------------------------------------
	//  Operators
	// ------------------------------------------------------------------
	public:
		//! Copies the metadata of other to this
		//! No changes regarding child objects are made
		StrongMotionParameters& operator=(const StrongMotionParameters& other);
		//! Checks for equality of two objects. Childs objects
		//! are not part of the check.
		bool operator==(const StrongMotionParameters& other) const;
		bool operator!=(const StrongMotionParameters& other) const;

		//! Wrapper that calls operator==
		bool equal(const StrongMotionParameters& other) const;

	
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
		bool add(SimpleFilter* obj);
		bool add(Record* obj);
		bool add(StrongOriginDescription* obj);

		/**
		 * Removes an object.
		 * @param obj The object pointer
		 * @return true The object has been removed
		 * @return false The object has not been removed
		 *               because it does not exist in the list
		 */
		bool remove(SimpleFilter* obj);
		bool remove(Record* obj);
		bool remove(StrongOriginDescription* obj);

		/**
		 * Removes an object of a particular class.
		 * @param i The object index
		 * @return true The object has been removed
		 * @return false The index is out of bounds
		 */
		bool removeSimpleFilter(size_t i);
		bool removeRecord(size_t i);
		bool removeStrongOriginDescription(size_t i);

		//! Retrieve the number of objects of a particular class
		size_t simpleFilterCount() const;
		size_t recordCount() const;
		size_t strongOriginDescriptionCount() const;

		//! Index access
		//! @return The object at index i
		SimpleFilter* simpleFilter(size_t i) const;
		Record* record(size_t i) const;
		StrongOriginDescription* strongOriginDescription(size_t i) const;

		//! Find an object by its unique attribute(s)
		SimpleFilter* findSimpleFilter(const std::string& publicID) const;
		Record* findRecord(const std::string& publicID) const;
		StrongOriginDescription* findStrongOriginDescription(const std::string& publicID) const;

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
		// Aggregations
		std::vector<SimpleFilterPtr> _simpleFilters;
		std::vector<RecordPtr> _records;
		std::vector<StrongOriginDescriptionPtr> _strongOriginDescriptions;

	DECLARE_SC_CLASSFACTORY_FRIEND(StrongMotionParameters);
};


}
}
}


#endif
