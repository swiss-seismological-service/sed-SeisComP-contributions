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
#include <seiscomp/seismology/locatorinterface.h>
#include <string>


namespace Seiscomp {

namespace Seismology {

namespace Plugins {


class NLLocator : public Seiscomp::Seismology::LocatorInterface {
	// ----------------------------------------------------------------------
	//  Public types
	// ----------------------------------------------------------------------
	public:
		DEFINE_SMARTPOINTER(Region);

		struct Region : public Core::BaseObject {
			virtual bool isGlobal() const { return false; }
			virtual bool init(const Config::Config &config, const std::string &prefix) = 0;
			virtual bool isInside(double lat, double lon) const = 0;
		};


	// ----------------------------------------------------------------------
	//  X'truction
	// ----------------------------------------------------------------------
	public:
		//! C'tor
		NLLocator();

		//! D'tor
		~NLLocator();


	// ----------------------------------------------------------------------
	//  Locator interface implementation
	// ----------------------------------------------------------------------
	public:
		//! Initializes the locator and reads at least the path of the
		//! default control file used for NLLoc.
		virtual bool init(const Config::Config &config);

		//! Returns supported parameters to be changed.
		virtual IDList parameters() const;

		//! Returns the value of a parameter.
		virtual std::string parameter(const std::string &name) const;

		//! Sets the value of a parameter.
		virtual bool setParameter(const std::string &name,
		                          const std::string &value);

		virtual IDList profiles() const;

		//! specify the Earth model to be used, e.g. "iasp91"
		virtual void setProfile(const std::string &name);

		virtual int capabilities() const;

		virtual DataModel::Origin* locate(PickList& pickList);
		virtual DataModel::Origin* locate(PickList& pickList,
		                                  double initLat, double initLon, double initDepth,
		                                  const Core::Time &initTime);
		virtual DataModel::Origin* relocate(const DataModel::Origin* origin);

		virtual std::string lastMessage(MessageType) const;


	// ----------------------------------------------------------------------
	//  Private methods
	// ----------------------------------------------------------------------
	private:
		void updateProfile(const std::string &name);

		bool NLL2SC3(DataModel::Origin *origin, std::string &locComment,
		             const void *node, const PickList &picks,
		             bool depthFixed);


	// ----------------------------------------------------------------------
	//  Private members
	// ----------------------------------------------------------------------
	private:
		struct Profile {
			std::string name;
			std::string earthModelID;
			std::string methodID;
			std::string tablePath;
			std::string controlFile;
			RegionPtr   region;
		};

		typedef std::map<std::string, std::string> ParameterMap;
		typedef std::vector<std::string> TextLines;
		typedef std::list<Profile> Profiles;

		static IDList _allowedParameters;

		std::string   _publicIDPattern;
		std::string   _outputPath;
		std::string   _controlFilePath;
		std::string   _lastWarning;
		std::string   _SEDqualityTag;
		std::string   _SEDdiffMaxLikeExpectTag;
		TextLines     _controlFile;
		IDList        _profileNames;

		double        _fixedDepthGridSpacing;
		double        _defaultPickError;
		bool          _allowMissingStations;
		bool          _enableSEDParameters;
		bool          _enableNLLOutput;
		bool          _enableNLLSaveInput;

		ParameterMap  _parameters;
		Profiles      _profiles;
		Profile      *_currentProfile;
};


}

}

}
