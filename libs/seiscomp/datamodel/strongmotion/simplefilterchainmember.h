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


#ifndef SEISCOMP_DATAMODEL_STRONGMOTION_SIMPLEFILTERCHAINMEMBER_H
#define SEISCOMP_DATAMODEL_STRONGMOTION_SIMPLEFILTERCHAINMEMBER_H


#include <string>
#include <seiscomp/datamodel/object.h>
#include <seiscomp/core/exceptions.h>
#include <seiscomp/datamodel/strongmotion/api.h>


namespace Seiscomp {
namespace DataModel {
namespace StrongMotion {


DEFINE_SMARTPOINTER(SimpleFilterChainMember);

class Record;


class SC_STRONGMOTION_API SimpleFilterChainMemberIndex {
	// ------------------------------------------------------------------
	//  Xstruction
	// ------------------------------------------------------------------
	public:
		//! Constructor
		SimpleFilterChainMemberIndex();
		SimpleFilterChainMemberIndex(int sequenceNo);

		//! Copy constructor
		SimpleFilterChainMemberIndex(const SimpleFilterChainMemberIndex&);


	// ------------------------------------------------------------------
	//  Operators
	// ------------------------------------------------------------------
	public:
		bool operator==(const SimpleFilterChainMemberIndex&) const;
		bool operator!=(const SimpleFilterChainMemberIndex&) const;


	// ------------------------------------------------------------------
	//  Attributes
	// ------------------------------------------------------------------
	public:
		int sequenceNo;
};


class SC_STRONGMOTION_API SimpleFilterChainMember : public Object {
	DECLARE_SC_CLASS(SimpleFilterChainMember);
	DECLARE_SERIALIZATION;
	DECLARE_METAOBJECT;

	// ------------------------------------------------------------------
	//  Xstruction
	// ------------------------------------------------------------------
	public:
		//! Constructor
		SimpleFilterChainMember();

		//! Copy constructor
		SimpleFilterChainMember(const SimpleFilterChainMember& other);

		//! Destructor
		~SimpleFilterChainMember();
	

	// ------------------------------------------------------------------
	//  Operators
	// ------------------------------------------------------------------
	public:
		//! Copies the metadata of other to this
		SimpleFilterChainMember& operator=(const SimpleFilterChainMember& other);
		//! Checks for equality of two objects. Childs objects
		//! are not part of the check.
		bool operator==(const SimpleFilterChainMember& other) const;
		bool operator!=(const SimpleFilterChainMember& other) const;

		//! Wrapper that calls operator==
		bool equal(const SimpleFilterChainMember& other) const;


	// ------------------------------------------------------------------
	//  Setters/Getters
	// ------------------------------------------------------------------
	public:
		void setSequenceNo(int sequenceNo);
		int sequenceNo() const;

		void setSimpleFilterID(const std::string& simpleFilterID);
		const std::string& simpleFilterID() const;


	// ------------------------------------------------------------------
	//  Index management
	// ------------------------------------------------------------------
	public:
		//! Returns the object's index
		const SimpleFilterChainMemberIndex& index() const;

		//! Checks two objects for equality regarding their index
		bool equalIndex(const SimpleFilterChainMember* lhs) const;

	
	// ------------------------------------------------------------------
	//  Public interface
	// ------------------------------------------------------------------
	public:
		Record* record() const;

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
		// Index
		SimpleFilterChainMemberIndex _index;

		// Attributes
		std::string _simpleFilterID;
};


}
}
}


#endif
