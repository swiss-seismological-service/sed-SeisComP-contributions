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


#ifndef SEISCOMP_DATAMODEL_STRONGMOTION_FILERESOURCE_H
#define SEISCOMP_DATAMODEL_STRONGMOTION_FILERESOURCE_H


#include <seiscomp/datamodel/creationinfo.h>
#include <string>
#include <seiscomp/core/baseobject.h>
#include <seiscomp/core/exceptions.h>
#include <seiscomp/datamodel/strongmotion/api.h>


namespace Seiscomp {
namespace DataModel {
namespace StrongMotion {


DEFINE_SMARTPOINTER(FileResource);


class SC_STRONGMOTION_API FileResource : public Core::BaseObject {
	DECLARE_SC_CLASS(FileResource);
	DECLARE_SERIALIZATION;
	DECLARE_METAOBJECT;

	// ------------------------------------------------------------------
	//  Xstruction
	// ------------------------------------------------------------------
	public:
		//! Constructor
		FileResource();

		//! Copy constructor
		FileResource(const FileResource& other);

		//! Destructor
		~FileResource();
	

	// ------------------------------------------------------------------
	//  Operators
	// ------------------------------------------------------------------
	public:
		//! Copies the metadata of other to this
		FileResource& operator=(const FileResource& other);
		//! Checks for equality of two objects. Childs objects
		//! are not part of the check.
		bool operator==(const FileResource& other) const;
		bool operator!=(const FileResource& other) const;

		//! Wrapper that calls operator==
		bool equal(const FileResource& other) const;


	// ------------------------------------------------------------------
	//  Setters/Getters
	// ------------------------------------------------------------------
	public:
		void setCreationInfo(const OPT(CreationInfo)& creationInfo);
		CreationInfo& creationInfo();
		const CreationInfo& creationInfo() const;

		void setClass(const std::string& Class);
		const std::string& Class() const;

		void setType(const std::string& type);
		const std::string& type() const;

		void setFilename(const std::string& filename);
		const std::string& filename() const;

		void setUrl(const std::string& url);
		const std::string& url() const;

		void setDescription(const std::string& description);
		const std::string& description() const;


	// ------------------------------------------------------------------
	//  Implementation
	// ------------------------------------------------------------------
	private:
		// Attributes
		OPT(CreationInfo) _creationInfo;
		std::string _class;
		std::string _type;
		std::string _filename;
		std::string _url;
		std::string _description;
};


}
}
}


#endif
