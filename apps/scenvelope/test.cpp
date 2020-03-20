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

#define SEISCOMP_COMPONENT scenvelope

#include <seiscomp/logging/log.h>
#include <seiscomp/client/application.h>
#include <seiscomp/io/archive/xmlarchive.h>

#include <seiscomp/datamodel/vs/vs_package.h>


using namespace std;
using namespace Seiscomp;


class EnvelopeTest : public Client::Application {
	public:
		EnvelopeTest(int argc, char **argv) : Client::Application(argc, argv) {
			setMessagingEnabled(false);
			setDatabaseEnabled(false, false);
		}

	protected:
		bool run() {
			DataModel::VS::VSPtr vs = new DataModel::VS::VS;

			DataModel::CreationInfo ci;
			ci.setAgencyID(agencyID());
			ci.setAuthor(author());
			ci.setCreationTime(Core::Time::GMT());

			DataModel::VS::EnvelopePtr env = DataModel::VS::Envelope::Create();
			env->setCreationInfo(ci);
			env->setNetwork("CH");
			env->setStation("ZUR");

			DataModel::VS::EnvelopeChannelPtr cha = DataModel::VS::EnvelopeChannel::Create();
			cha->setName("Z");
			cha->setWaveformID(DataModel::WaveformStreamID("CH", "ZUR", "", "HGZ", ""));

			cha->add(new DataModel::VS::EnvelopeValue(0.3, "vel", Core::None));
			cha->add(new DataModel::VS::EnvelopeValue(0.2, "acc", Core::None));
			cha->add(new DataModel::VS::EnvelopeValue(0.1, "disp", Core::None));

			env->add(cha.get());

			vs->add(env.get());

			IO::XMLArchive ar;
			ar.create("-");
			ar.setFormattedOutput(true);
			ar << vs;
			return true;
		}
};


int main(int argc, char **argv) {
	EnvelopeTest app(argc, argv);
	return app.exec();
}

