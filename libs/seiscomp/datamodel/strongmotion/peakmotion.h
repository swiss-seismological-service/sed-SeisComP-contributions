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


#ifndef SEISCOMP_DATAMODEL_STRONGMOTION_PEAKMOTION_H
#define SEISCOMP_DATAMODEL_STRONGMOTION_PEAKMOTION_H


#include <seiscomp/datamodel/timequantity.h>
#include <string>
#include <seiscomp/datamodel/realquantity.h>
#include <seiscomp/datamodel/object.h>
#include <seiscomp/core/exceptions.h>
#include <seiscomp/datamodel/strongmotion/api.h>


namespace Seiscomp {
namespace DataModel {
namespace StrongMotion {


DEFINE_SMARTPOINTER(PeakMotion);

class Record;


class SC_STRONGMOTION_API PeakMotion : public Object {
	DECLARE_SC_CLASS(PeakMotion);
	DECLARE_SERIALIZATION;
	DECLARE_METAOBJECT;

	// ------------------------------------------------------------------
	//  Xstruction
	// ------------------------------------------------------------------
	public:
		//! Constructor
		PeakMotion();

		//! Copy constructor
		PeakMotion(const PeakMotion& other);

		//! Destructor
		~PeakMotion();
	

	// ------------------------------------------------------------------
	//  Operators
	// ------------------------------------------------------------------
	public:
		//! Copies the metadata of other to this
		PeakMotion& operator=(const PeakMotion& other);
		//! Checks for equality of two objects. Childs objects
		//! are not part of the check.
		bool operator==(const PeakMotion& other) const;
		bool operator!=(const PeakMotion& other) const;

		//! Wrapper that calls operator==
		bool equal(const PeakMotion& other) const;


	// ------------------------------------------------------------------
	//  Setters/Getters
	// ------------------------------------------------------------------
	public:
		void setMotion(const RealQuantity& motion);
		RealQuantity& motion();
		const RealQuantity& motion() const;

		void setType(const std::string& type);
		const std::string& type() const;

		void setPeriod(const OPT(double)& period);
		double period() const;

		void setDamping(const OPT(double)& damping);
		double damping() const;

		void setMethod(const std::string& method);
		const std::string& method() const;

		void setAtTime(const OPT(TimeQuantity)& atTime);
		TimeQuantity& atTime();
		const TimeQuantity& atTime() const;

	
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
		// Attributes
		RealQuantity _motion;
		std::string _type;
		OPT(double) _period;
		OPT(double) _damping;
		std::string _method;
		OPT(TimeQuantity) _atTime;
};


}
}
}


#endif
