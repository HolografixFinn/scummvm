/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/translation.h"

#include "comet/detection.h"
#include "comet/detection_tables.h"
#include "comet/metaengine.h"
#include "comet/comet.h"

namespace Cometengine {


	static const ADExtraGuiOptionsMap optionsList[] = {
		{GAMEOPTION_ORIGINAL_SAVELOAD,
		 {_s("Use original save/load screens"),
		  _s("Use the original save/load screens instead of the ScummVM ones"),
		  "original_menus",
		  false,
		  0,
		  0}},
		AD_EXTRA_GUI_OPTIONS_TERMINATOR };


} // End of namespace Cometengine

class CometengineMetaEngine : public AdvancedMetaEngine<Cometengine::CometGameDescription> {
public:
	const char* getName() const override {
		return "Comet Engine";
	}

	Common::Error createInstance(OSystem* syst, Engine** engine, const Cometengine::CometGameDescription* desc) const override;

	/**
	 * Determine whether the engine supports the specified MetaEngine feature.
	 *
	 * Used by e.g. the launcher to determine whether to enable the Load button.
	 */
	bool hasFeature(MetaEngineFeature f) const override;

	const ADExtraGuiOptionsMap* getAdvancedExtraGuiOptions() const override {
		return Cometengine::optionsList;
	}
};


Common::Error CometengineMetaEngine::createInstance(OSystem* syst, Engine** engine, const Cometengine::CometGameDescription* desc) const {
	*engine = new Cometengine::CometEngine(syst, desc);
	return Common::kNoError;
}

bool CometengineMetaEngine::hasFeature(MetaEngineFeature f) const {
	return checkExtendedSaves(f) ||
		(f == kSupportsLoadingDuringStartup);
}

#if PLUGIN_ENABLED_DYNAMIC(COMETENGINE)
REGISTER_PLUGIN_DYNAMIC(COMET, PLUGIN_TYPE_ENGINE, CometengineMetaEngine);
#else
REGISTER_PLUGIN_STATIC(COMET, PLUGIN_TYPE_ENGINE, CometengineMetaEngine);
#endif






