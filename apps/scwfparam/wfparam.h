/***************************************************************************
 *   Copyright (C) by ETHZ/SED, GNS New Zealand, GeoScience Australia      *
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


#ifndef __SEISCOMP_APPLICATIONS_WFPARAM_H__
#define __SEISCOMP_APPLICATIONS_WFPARAM_H__

#include <seiscomp/client/streamapplication.h>
#include <seiscomp/processing/amplitudeprocessor.h>
#include <seiscomp/datamodel/publicobjectcache.h>
#include <seiscomp/datamodel/eventparameters.h>
#include <seiscomp/datamodel/amplitude.h>
#include <seiscomp/datamodel/journaling.h>
#include <seiscomp/seismology/ttt.h>
#include <seiscomp/utils/timer.h>

#define SEISCOMP_COMPONENT WfParam
#include <seiscomp/logging/log.h>

#include "app.h"
#include "util.h"

#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>


namespace Seiscomp {

class Record;

namespace DataModel {

class Pick;
class Origin;
class Event;

}


class WFParam : public Application {
	public:
		WFParam(int argc, char **argv);
		~WFParam();


	protected:
		void createCommandLineDescription();
		bool validateParameters();

		bool init();
		bool run();
		void done();

		bool storeRecord(Record *rec);
		void handleRecord(Record *rec);

		bool dispatchNotification(int type, Core::BaseObject *obj);

		void acquisitionFinished();

		void handleMessage(Core::Message *msg);
		void addObject(const std::string&, DataModel::Object* object);
		void updateObject(const std::string&, DataModel::Object* object);

		void handleTimeout();


	private:
		DEFINE_SMARTPOINTER(Process);

		bool addProcess(DataModel::Event *event);
		bool startProcess(Process *proc);
		void stopProcess(Process *proc);

		bool handle(DataModel::Event *event);
		bool handle(DataModel::Origin *origin);

		void process(DataModel::Origin *origin);

		void feed(DataModel::Pick *pick);

		int addProcessor(const DataModel::WaveformStreamID &streamID,
		                 DataModel::Stream *selectedStream,
		                 const Core::Time &time,
		                 Processing::WaveformProcessor::StreamComponent component
		                 = Processing::WaveformProcessor::Vertical);
		bool createProcessor(Record *rec);

		void removedFromCache(DataModel::PublicObject *);

		template <typename KEY, typename VALUE>
		bool getValue(VALUE &res, const std::map<KEY,VALUE> &map,
		              double ref) const;

		void setup(PGAVResult &res, Processing::PGAV *proc);
		void collectResults();
		void printReport();

		// Creates an event id from event. This id has the following format:
		// EventOriginTime_Mag_Lat_Lon_CreationDate
		// eg 20111210115715_12_46343_007519_20111210115740
		std::string generateEventID(const DataModel::Event *evt);


	private:
		struct Config {
			Config();

			std::vector<std::string> streamsWhiteList;
			std::vector<std::string> streamsBlackList;

			double totalTimeWindowLength;
			double preEventWindowLength;
			std::map<double,double> magnitudeTimeWindowTable;
			std::vector<std::string> vecMagnitudeTimeWindowTable;

			double maximumEpicentralDistance;
			std::map<double,double> magnitudeDistanceTable;
			std::vector<std::string> vecMagnitudeDistanceTable;

			std::map<double,FilterFreqs> magnitudeFilterTable;
			std::vector<std::string> vecMagnitudeFilterTable;

			std::string processingLogfile;
			double      saturationThreshold;
			double      STAlength;
			double      LTAlength;
			double      STALTAratio;
			double      STALTAmargin;

			double      durationScale;

			std::vector<double> dampings;
			std::string naturalPeriodsStr;
			int         naturalPeriods;
			bool        naturalPeriodsLog;
			bool        naturalPeriodsFixed;
			double      Tmin;
			double      Tmax;
			bool        clipTmax;

			bool        afterShockRemoval;
			bool        eventCutOff;

			double      fExpiry;
			std::string eventID;

			std::string eventParameterFile;

			bool        enableShortEventID;

			struct {
				struct {
					bool                     enable;
					std::vector<std::string> pgm;
					std::string              script;
					std::string              path;
					bool                     scriptWait;
					bool                     SC3EventID;
					bool                     regionName;
					std::string              XMLEncoding;
					bool                     useMaximumOfHorizontals;
					int                      version;
				} output;
			}           shakeMap;

			bool        enableMessagingOutput;

			std::string waveformOutputPath;
			bool        waveformOutputEventDirectory;

			std::string spectraOutputPath;
			bool        spectraOutputEventDirectory;

			bool        enableDeconvolution;
			bool        enableNonCausalFilters;
			double      taperLength;
			double      padLength;

			int         order;
			FilterFreqs filter;

			int         PDorder;
			FilterFreqs PDfilter;

			bool        offline;
			bool        force;
			bool        forceShakemap;
			bool        testMode;
			bool        logCrontab;
			bool        saveProcessedWaveforms;
			bool        saveSpectraFiles;

			int         wakeupInterval;
			int         initialAcquisitionTimeout;
			int         runningAcquisitionTimeout;
			int         eventMaxIdleTime;

			double      magnitudeTolerance;
			bool        dumpRecords;

			std::string organization;

			// Cron options
			int         updateDelay;
			std::vector<int> delayTimes;
		};


		// Cronjob struct created per event
		DEFINE_SMARTPOINTER(Cronjob);
		struct Cronjob : public Core::BaseObject {
			std::list<Core::Time> runTimes;
		};

		typedef std::map<std::string, CronjobPtr> Crontab;

		struct CompareWaveformStreamID {
			bool operator()(const DataModel::WaveformStreamID &lhs,
			                const DataModel::WaveformStreamID &rhs) const;
		};

		typedef std::set<DataModel::WaveformStreamID,CompareWaveformStreamID> WaveformIDSet;
		struct StationRequest {
			Core::TimeWindow timeWindow;
			WaveformIDSet streams;
		};

		typedef std::vector<Processing::TimeWindowProcessorPtr>  ProcessorSlot;
		typedef std::map<std::string, ProcessorSlot>             ProcessorMap;
		typedef std::map<std::string, Util::KeyValuesPtr>        KeyMap;
		typedef std::map<std::string, StationRequest>            RequestMap;

		typedef std::map<std::string, Processing::StreamPtr>     StreamMap;
		typedef DataModel::PublicObjectTimeSpanBuffer            Cache;

		void removeProcess(Crontab::iterator &, Process *proc);

		void dumpWaveforms(Process *p, PGAVResult &result,
		                   const Processing::PGAV *proc);

		void dumpSpectra(Process *p, const PGAVResult &result,
		                 const Processing::PGAV *proc);

		void writeShakeMapComponent(const PGAVResult *, bool &headerWritten,
		                            std::ostream *os, bool withComponent);

		typedef std::list<PGAVResult> PGAVResults;

		struct Process : Core::BaseObject {
			Core::Time          created;
			Core::Time          lastRun;
			Core::Time          referenceTime;
			DataModel::EventPtr event;
			PGAVResults         results;
			int                 remainingChannels;
			int                 newValidResults;

			OPT(double)         lastMagnitude;

			bool hasBeenProcessed(DataModel::Stream *) const;
		};

		using ProcessQueue = std::list<ProcessPtr>;
		using Processes    = std::map<std::string, ProcessPtr>;
		using Todos        = std::set<DataModel::EventPtr>;
		using PeriodID     = std::pair<std::string, double>;

		std::set<std::string>      _processedEvents;

		DataModel::EventParametersPtr _eventParameters;
		ProcessPtr                 _currentProcess;

		StreamMap                  _streams;
		Private::StringFirewall    _streamFirewall;

		TravelTimeTable            _travelTime;
		ProcessorMap               _processors;
		RequestMap                 _stationRequests;
		KeyMap                     _keys;

		Crontab                    _crontab;
		ProcessQueue               _processQueue;
		Processes                  _processes;

		Cache                      _cache;

		bool                       _firstRecord;

		Config                     _config;

		Core::Time                 _originTime;
		double                     _latitude;
		double                     _longitude;
		double                     _depth;
		double                     _maximumEpicentralDistance;
		double                     _totalTimeWindowLength;
		FilterFreqs                _filter;
		int                        _cronCounter;
		int                        _acquisitionTimeout;
		bool                       _wantShakeMapPGA;
		bool                       _wantShakeMapPGV;
		std::vector<PeriodID>      _wantShakeMapPSAPeriods;

		Util::StopWatch            _acquisitionTimer;
		Util::StopWatch            _noDataTimer;

		Todos                      _todos;

		Logging::Channel          *_processingInfoChannel;
		Logging::Output           *_processingInfoOutput;

		std::stringstream          _report;
		std::stringstream          _result;

		std::ofstream              _recordDumpOutput;
};

}

#endif
