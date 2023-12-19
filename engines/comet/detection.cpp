/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "base/plugins.h"

#include "engines/advancedDetector.h"
#include "common/file.h"

//#include "comet/comet.h"
#include "comet/detection.h"
#include "comet/detection_tables.h"

/*
#include "base/plugins.h"
#include "common/config-manager.h"
#include "common/file.h"
#include "common/md5.h"
#include "common/str-array.h"
#include "common/translation.h"
#include "common/util.h"
#include "testengine/detection.h"
#include "testengine/detection_tables.h"
*/

const DebugChannelDef CometengineMetaEngineDetection::debugFlagList[] = {
	/*
	{Cometengine::kDebugGraphics, "Graphics", "Graphics debug level"},
	{Cometengine::kDebugPath, "Path", "Pathfinding debug level"},
	{Cometengine::kDebugFilePath, "FilePath", "File path debug level"},
	{Cometengine::kDebugScan, "Scan", "Scan for unrecognised games"},
	{Cometengine::kDebugScript, "Script", "Enable debug script dump"},
	*/
	DEBUG_CHANNEL_END};

CometengineMetaEngineDetection::CometengineMetaEngineDetection() : AdvancedMetaEngineDetection(Cometengine::gameDescriptions,
																							   sizeof(ADGameDescription), Cometengine::testengineGames) {
}
REGISTER_PLUGIN_STATIC(COMET_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, CometengineMetaEngineDetection);


/*
namespace Comet {
const char *CometGame::getGameId() const { return _gameDescription->gameId; }
Common::Platform CometGame::getPlatform() const { return _gameDescription->platform; }
}

static const PlainGameDescriptor cometGames[] = {
	{"comet", "Shadow of the comet"},
	{0, 0}
};

namespace Comet {

static const ADGameDescription gameDescriptions[] = {
	// Comet PC version
	{
		"comet",
		"English Floppy Version",
		AD_ENTRY1s("E.CC4", 0, -1),
		Common::EN_ANY,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO1(GUIO_NONE)
	},
	{
		"comet",
		"Italian Floppy Version",
		AD_ENTRY1s("I.CC4", 0, -1),
		Common::IT_ITA,
		Common::kPlatformDOS,
		ADGF_NO_FLAGS,
		GUIO1(GUIO_NONE)
	},
	{
		"comet",
		"Multilanguage CD Version",
		AD_ENTRY1s("SHADO_CD.CFG", 0, -1),
		Common::EN_ANY,
		Common::kPlatformDOS,
		ADGF_CD | ADGF_DROPLANGUAGE,
		GUIO1(GUIO_NONE)
	},
	AD_TABLE_END_MARKER
};

} // End of namespace Plumbers

class CometMetaEngine : public AdvancedMetaEngine {
public:
	CometMetaEngine() : AdvancedMetaEngine(Comet::gameDescriptions, sizeof(ADGameDescription), cometGames) {
//		_singleId = "comet";
	}

	virtual const char *getName() const {
		return "Shadow of the comet";
	}

	virtual const char *getOriginalCopyright() const {
		return "Shadow of the comet (C) Infogrames 1993";
	}

	virtual bool hasFeature(MetaEngineFeature f) const;
	virtual Common::Error createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const override;
	virtual const char* getEngineId() const;
};
const char* CometMetaEngine::getEngineId() const {
	return "comet";
}
bool CometMetaEngine::hasFeature(MetaEngineFeature f) const {
	return false;
}

Common::Error CometMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	if (desc)
		*engine = new Comet::CometGame(syst, desc);

//	return desc != nullptr;
	return Common::kNoError;
}

#if PLUGIN_ENABLED_DYNAMIC(COMET)
REGISTER_PLUGIN_DYNAMIC(COMET, PLUGIN_TYPE_ENGINE, CometMetaEngine);
#else
REGISTER_PLUGIN_STATIC(COMET, PLUGIN_TYPE_ENGINE, CometMetaEngine);
#endif
*/
