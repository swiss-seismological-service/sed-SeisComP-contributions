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


#define SEISCOMP_COMPONENT Application

#include <seiscomp/system/environment.h>
#include <seiscomp/config/config.h>
#include <seiscomp/system/pluginregistry.h>
#include <seiscomp/logging/log.h>
#include <seiscomp/utils/files.h>
#include <seiscomp/core/strings.h>
#include <seiscomp/core/version.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#ifndef WIN32
#include <dlfcn.h>
#endif
#include <stdlib.h>

namespace fs = boost::filesystem;


namespace {

std::string sysLastError() {
#ifdef WIN32
	char *s;
	std::string msg;
	int err = ::GetLastError();
	if ( ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	                     FORMAT_MESSAGE_FROM_SYSTEM,
	                     nullptr, err, 0, (LPTSTR)&s, 0, nullptr) == 0 ) {
		/* failed */
		// Unknown error code %08x (%d)
		msg = "unknown error: " + Seiscomp::Core::toString(err);
	} /* failed */
	else {
		/* success */
		msg = s;
		::LocalFree(s);
	} /* success */
	return msg;
#else
	return dlerror();
#endif
}

}


namespace Seiscomp {
namespace System {
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
PluginRegistry *PluginRegistry::_instance = nullptr;
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
PluginRegistry::iterator::iterator() : base() {}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
PluginRegistry::iterator::iterator(const base &other) : base(other) {}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
const Core::Plugin* PluginRegistry::iterator::operator*() const {
	return (base::operator*()).plugin.get();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
Core::Plugin* PluginRegistry::iterator::value_type(const iterator &it) {
	return nullptr;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
PluginRegistry::PluginRegistry() {
	addPluginPath(".");
	if ( Environment::Instance() ) {
		addPluginPath(Environment::Instance()->configDir() + "/plugins");
		addPluginPath(Environment::Instance()->installDir() + "/lib/plugins");
		addPluginPath(Environment::Instance()->installDir() + "/lib");
		addPluginPath(Environment::Instance()->shareDir() + "/plugins");
	}

#ifndef WIN32
	const char *env = getenv("LD_LIBRARY_PATH");
	if ( env != nullptr ) {
		std::vector<std::string> paths;
		Core::split(paths, env, ":");
		for ( size_t i = 0; i < paths.size(); ++i ) {
			if ( paths[i].empty() ) continue;
			if ( paths[i][paths[i].size()-1] == '/' )
				addPluginPath(paths[i].substr(0, paths[i].size()-1));
			else
				addPluginPath(paths[i]);
		}
	}
#endif
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
PluginRegistry::~PluginRegistry() {
	freePlugins();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
PluginRegistry *PluginRegistry::Instance() {
	if ( _instance == nullptr )
		_instance = new PluginRegistry();

	return _instance;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void PluginRegistry::addPluginName(const std::string &name) {
	if ( std::find(_pluginNames.begin(), _pluginNames.end(), name) == _pluginNames.end() )
		_pluginNames.push_back(name);
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void PluginRegistry::addPluginPath(const std::string &path) {
	if ( std::find(_paths.begin(), _paths.end(), path) == _paths.end() ) {
		SEISCOMP_DEBUG("Adding plugin path: %s", path.c_str());
		_paths.push_back(path);
	}
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void PluginRegistry::addPackagePath(const std::string &package) {
	addPluginPath(Environment::Instance()->shareDir() + "/plugins/" + package);
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int PluginRegistry::loadPlugins() {
	for ( NameList::const_iterator it = _pluginNames.begin();
	      it != _pluginNames.end(); ++it ) {
		if ( it->empty() ) continue;
		std::string filename = find(*it);
		if ( filename.empty() ) {
			SEISCOMP_ERROR("Did not find plugin %s", it->c_str());
			return -1;
		}

		SEISCOMP_DEBUG("Trying to open plugin at %s", filename.c_str());
		PluginEntry e = open(filename);
		if ( e.plugin == nullptr ) {
			if ( e.handle == nullptr ) {
				SEISCOMP_ERROR("Unable to load plugin %s", it->c_str());
				return -1;
			}
			else
				SEISCOMP_WARNING("The plugin %s has been loaded already",
				                 it->c_str());
			continue;
		}

		SEISCOMP_INFO("Plugin %s registered", it->c_str());
		_plugins.push_back(e);
	}

	return _plugins.size();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int PluginRegistry::loadConfiguredPlugins(const Config::Config *config) {
	try { _pluginNames = config->getStrings("core.plugins"); }
	catch ( ... ) {}

	try {
		NameList appPlugins = config->getStrings("plugins");
		_pluginNames.insert(_pluginNames.end(), appPlugins.begin(), appPlugins.end());
	}
	catch ( ... ) {}

	return loadPlugins();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
std::string PluginRegistry::find(const std::string &name) const {
//#ifndef WIN32
//	std::string alternativeName = name + ".so";
//#else
//	std::string alternativeName = name + ".dll";
//#endif
	std::string alternativeName = name + SHARED_MODULE_SUFFIX;
	for ( PathList::const_iterator it = _paths.begin(); it != _paths.end();
	      ++it ) {
		std::string path = *it + "/" + name;

		if ( Util::fileExists(path) )
			return path;

		path = *it + "/" + alternativeName;

		if ( Util::fileExists(path) )
			return path;
	}

	return std::string();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void PluginRegistry::freePlugins() {
	for ( PluginList::iterator it = _plugins.begin(); it != _plugins.end();
	      ++it ) {
		SEISCOMP_DEBUG("Unload plugin '%s'", it->plugin->description().description.c_str());

		it->plugin = nullptr;
#ifndef WIN32
		dlclose(it->handle);
#else
		FreeLibrary((HMODULE)it->handle);
#endif
	}

	_plugins.clear();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int PluginRegistry::pluginCount() const {
	return _plugins.size();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
PluginRegistry::iterator PluginRegistry::begin() const {
	return _plugins.begin();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
PluginRegistry::iterator PluginRegistry::end() const {
	return _plugins.end();
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
bool PluginRegistry::findLibrary(void *handle) const {
	for ( PluginList::const_iterator it = _plugins.begin(); it != _plugins.end();
	      ++it ) {
		if ( it->handle == handle )
			return true;
	}

	return false;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
PluginRegistry::PluginEntry PluginRegistry::open(const std::string &file) const {
	// Load shared library
#ifdef WIN32
	void *handle = LoadLibrary(file.c_str());
#else
	void *handle = dlopen(file.c_str(), RTLD_NOW | RTLD_GLOBAL);
#endif
	if ( !handle ) {
		SEISCOMP_ERROR("Loading plugin %s failed: %s", file.c_str(), sysLastError().c_str());
		return PluginEntry(nullptr, nullptr, file);
	}

	if ( findLibrary(handle) ) {
		return PluginEntry(handle, nullptr, file);
	}

#ifndef WIN32
	// Reset errors
	dlerror();

	// Load factory
	Core::Plugin::CreateFunc func;
	*(void **)(&func) = dlsym(handle, "createSCPlugin");
#else
	Core::Plugin::CreateFunc func = (Core::Plugin::CreateFunc)GetProcAddress((HMODULE)handle, "createSCPlugin");
#endif
	if ( !func ) {
		SEISCOMP_ERROR("Could not load symbol createPlugin: %s", sysLastError().c_str());
#ifndef WIN32
		dlclose(handle);
#else
		FreeLibrary((HMODULE)handle);
#endif
		return PluginEntry(nullptr, nullptr, file);
	}

	Core::Plugin *plugin = func();
	if ( !plugin ) {
		SEISCOMP_ERROR("No plugin return from %s", file.c_str());
#ifndef WIN32
		dlclose(handle);
#else
		FreeLibrary((HMODULE)handle);
#endif
		return PluginEntry(nullptr, nullptr, file);
	}

	// Do not warn for different patch versions. They must be binary compatible
	// by definition.
	if ( (SC_API_VERSION_MAJOR(plugin->description().apiVersion) != SC_API_VERSION_MAJOR(SC_API_VERSION)) ||
	     (SC_API_VERSION_MINOR(plugin->description().apiVersion) > SC_API_VERSION_MINOR(SC_API_VERSION)) ) {
		SEISCOMP_WARNING("API version mismatch (plugin %d.%d != API %d.%d) can lead to unpredicted behaviour: %s",
		                 SC_API_VERSION_MAJOR(plugin->description().apiVersion),
		                 SC_API_VERSION_MINOR(plugin->description().apiVersion),
		                 SC_API_VERSION_MAJOR(SC_API_VERSION), SC_API_VERSION_MINOR(SC_API_VERSION),
		                 file.c_str());
	}

	return PluginEntry(handle, plugin, file);
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
}
}