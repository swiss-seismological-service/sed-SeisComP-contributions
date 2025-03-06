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
 ***************************************************************************/


#ifndef SMDUMP_APP_H
#define SMDUMP_APP_H


#include <seiscomp/client/application.h>

#include <vector>


class SMDump : public Seiscomp::Client::Application {
	public:
		SMDump(int argc, char** argv);

	public:
		bool run() override;

	private:
		struct Settings : AbstractSettings {
			void accept(SettingsLinker &linker) override {
				linker
				& cli(
					originIDs, "Input", "origin,O",
					"A publicID of an origin to be exported. "
					"This option can be given more than once."
				)
				& cfg(originIDs, "origins")
				& cliSwitch(
					withEventRecordReferences, "Dump", "with-event-records,r",
					"Include event records."
				)
				& cliSwitch(
					withRecords, "Dump", "with-records,S",
					"Include records referred to from event records."
				)
				& cliSwitch(
					withRecords, "Dump", "with-ruptures,R",
					"Include ruptures."
				)
				& cliSwitch(
					formatted, "Output", "formatted,f",
					"Use formatted XML output."
				)
				& cli(
					output, "Output", "output,o",
					"Name of output file. If not given or "
					"'-', output is sent to stdout."
				)
				;
			}

			bool                     formatted{false};
			bool                     withEventRecordReferences{false};
			bool                     withRecords{false};
			bool                     withRupture{false};
			std::string              output;
			std::vector<std::string> originIDs;
		} _settings;
};


#endif
