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
namespace Cometengine {
	const DebugChannelDef CometMetaEngineDetection::debugFlagList[] = {
		
			{Cometengine::kDebugGraphics, "Graphics", "Graphics debug level"},
			{Cometengine::kDebugPath, "Path", "Pathfinding debug level"},
			{Cometengine::kDebugFilePath, "FilePath", "File path debug level"},
			{Cometengine::kDebugScan, "Scan", "Scan for unrecognised games"},
			{Cometengine::kDebugScript, "Script", "Enable debug script dump"},
		
			DEBUG_CHANNEL_END };
	CometMetaEngineDetection::CometMetaEngineDetection() : AdvancedMetaEngineDetection(Cometengine::gameDescriptions, Cometengine::cometGames) {
	}

}
REGISTER_PLUGIN_STATIC(COMET_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, Cometengine::CometMetaEngineDetection);

