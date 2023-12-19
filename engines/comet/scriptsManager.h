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

#ifndef COMET_SCRIPTSMANAGER_H
#define COMET_SCRIPTSMANAGER_H
#include "comet/comet_structs.h"
#include "common/scummsys.h"

namespace Cometengine {
class CometEngine;
//class ScriptManager;
class ScriptsManager {
public:
	ScriptsManager(CometEngine *vm);
	void initResources();
	~ScriptsManager();
	void loadAndPrepareRoomScripts(char *filename, uint8 roomIdx, bool isLoading = false);
	void executeRoomScripts();
	enum ScriptFlags {
		END = 0x20,
		SYNC = 0x08,
		WAIT_TIMER = 0x02,
		WAIT_DESTINATION = 0x01,
		WAIT_ANIMATION = 0x04,
		WAIT_CHOICE = 0x10,
		WAIT_TEXT = 0x40,
		NO_FLAGS = 0x00

	};
	void setWait(uint8);
	//			uint16 getNumScriptsInRoom();
	RoomScript *getScript(uint16 idx);

protected:
private:
	//
	typedef bool (ScriptsManager::*opfunc)(RoomScript * const script);
	static const opfunc shadowTable[];
	const opfunc *_opTable;
	//01
	bool op_setActorFacingDir(RoomScript *const script); 
	//02
	bool op_endScript(RoomScript *const script);         
	//03
	bool op_jmp(RoomScript *const script);         
	//04
	bool op_setActorDestX(RoomScript *const script);     
	//05
	bool op_setActorDestY(RoomScript *const script);     
	//06
	bool op_loop(RoomScript *const script);				 
	//07
	bool op_setActorPos(RoomScript *const script);		 
	//08
	bool op_setSyncTo(RoomScript *const script);		 
	//09
	bool op_setWait(RoomScript *const script);
	//0a
	bool op_if(RoomScript *const script);         
	//0e
	bool op_ifActorInArea(RoomScript *const script);
	//0f
	bool op_setActorDestMainActorX(RoomScript *const script);
	//10
	bool op_setActorDestMainActorY(RoomScript *const script);
	//11
	bool op_moveActorTowardMainActor(RoomScript *const script);         
	//12
	bool op_lockMainActor(RoomScript *const script);
	//13
	bool op_unlockMainActor(RoomScript *const script);
	//14
	bool op_rotateActorTowardsMainActor(RoomScript *const script);
	//15
	bool op_setCurrentActor(RoomScript *const script);         
	//16
	bool op_loadWalkAreaTopSegments(RoomScript *const script);         
	//17
	bool op_setSceneExits(RoomScript *const script);         
	//19
	bool op_addHotspot_type0(RoomScript *const script);
	//1a
	bool op_setEscPressed(RoomScript *const script);         
   	//1b
	bool op_startScript(RoomScript *const script);
	//1c
	bool op_stopScript(RoomScript *const script);
	//1d
	bool op_restartScript(RoomScript *const script);
	//1e
	bool op_playCutscene(RoomScript *const script);
	//1f
	bool op_setGameVar(RoomScript *const script);
	//20
	bool op_addToGameVar(RoomScript *const script);
	//21
	bool op_subFromGameVar(RoomScript *const script);
	//22
	bool op_disableActorCollisions(RoomScript *const script);
	//23
	bool op_enableActorCollisions(RoomScript *const script);
	//24
	bool op_setActorDestinationXY(RoomScript *const script);
	//25
	bool op_setStageColorsReplace(RoomScript *const script);
	//26
	bool op_fadePalette(RoomScript *const script);
	//27
	bool op_waitForMainActorToLeaveArea(RoomScript *const script);
	//28
	bool op_waitForMainActorToEnterArea(RoomScript *const script);
	//29
	bool op_freeActor(RoomScript *const script);
	//2A
	bool op_setActorViewportX(RoomScript *const script);
	//2B
	bool op_setActorViewportY(RoomScript *const script);
	//2C
	bool op_setNewRoom(RoomScript *const script);
	//2D
	bool op_setActorAnimationFreezeFrame(RoomScript *const script);
	//2E
	bool op_activateSecondaryVoices(RoomScript *const script);
	//2F
	bool op_setResourceType(RoomScript *const script);
	//30
	bool op_pivotActorToMainActor(RoomScript *const script);
	//32
	bool op_drawRandomEffect(RoomScript *const script);
	//33
	bool op_setNewZoomLevel(RoomScript *const script);
	//34
	bool op_zoomOnActor(RoomScript *const script);
	//35
	bool op_multiChoice(RoomScript *const script);
	//36
	bool op_actorSaySentence(RoomScript *const script);
	//37
	bool op_setClearScreen(RoomScript *const script);
	//38
	bool op_orVar(RoomScript *const script);
	//39
	bool op_andVar(RoomScript *const script);
	//3A
	bool op_loadScene(RoomScript *const script);
	//3B
	bool op_setWalkVerticalAdjustment(RoomScript *const script);
	//3C
	bool op_addObstacle(RoomScript *const script);
	//3D
	bool op_ifTalk(RoomScript *const script);
	//3E
	bool op_isActorInTalkRange(RoomScript *const script);
	//3F
	bool op_ifTalkInArea(RoomScript *const script);
	//40
	bool op_ifLook(RoomScript *const script);
	//41
	bool op_ifLookActorsInRange(RoomScript *const script);
	//42
	bool op_ifLookInArea(RoomScript *const script);
	//43
	bool op_addLightning(RoomScript *const script);
	//44
	bool op_removeObstacle(RoomScript *const script);
	//45
	bool op_setActorSpeechColor(RoomScript *const script);
	//46
	bool op_setActorSpeechPivot(RoomScript *const script);
	//47
	//55
	bool op_nop(RoomScript *const script);
	//48
	bool op_resetScriptCounterAndJmp(RoomScript *const script);
	//49
	bool op_activateRandomStars(RoomScript *const script);
	//4A
	bool op_setActorIsKillable(RoomScript *const script);
	//4C
	bool op_setClearScreenReturn(RoomScript *const script);
	//4D
	bool op_startSong(RoomScript *const script);
	//4E
	bool op_setMusicVolume(RoomScript *const script);
	//4F
	bool op_generateRandomNumber(RoomScript *const script);
	//50
	bool op_changePak(RoomScript *const script);
	//51
	bool op_readArea(RoomScript *const script);
	//52
	bool op_actorSpeakWithAnimation(RoomScript *const script);
	//53
	bool op_setForceLoading(RoomScript *const script);
	//54
	bool op_addHotspotType1(RoomScript *const script);
	//56
	bool op_seedRandomizerWhileIdle(RoomScript *const script);
	//57
	bool op_setActorAnimationAndWait(RoomScript *const script);
	//58
	bool op_setActorAnimation(RoomScript *const script);
	//59
	bool op_actorSpeakWithPortrait(RoomScript *const script);
	//5A
	bool op_resetCurrentActor(RoomScript *const script);
	//5B
	bool op_changeActorResouce(RoomScript *const script);
	//5C
	bool op_setActorVisibility(RoomScript *const script);
	//5D
	bool op_fadeIn(RoomScript *const script);
	//5E
	bool op_fadeOut(RoomScript *const script);
	//5F
	bool op_fadePaletteBlock(RoomScript *const script);
	//60
	bool op_setTextChapter(RoomScript *const script);
	//61
	bool op_ifSpecificActorInRange(RoomScript *const script);
	//62
	bool op_disableHotspot(RoomScript *const script);
	//63
	bool op_playDigiSample(RoomScript *const script);
	//64
	bool op_playDigiSampleLooped(RoomScript *const script);
	//65
	bool op_setPaletteTint(RoomScript *const script);
	//66
	bool op_changePaletteBrightness(RoomScript *const script);
	//67
	bool op_deactivateRandomDraw(RoomScript *const script);
	//68
	bool op_activateRandomDraw(RoomScript *const script);
	//69
	bool op_setDrawWithEffect(RoomScript *const script);
	//6A
	bool op_drawRandomTheEnd(RoomScript *const script);

	//

	RoomScript *_currScript;
	CometEngine *_vm;
	char *_scriptsBuffer;
	char *_multiChoiceDialogBuffer;
	void prepareScriptsAndInitRoom();
	void relocateScripts();
	//			RoomScript _scripts[20];
	//uint8 _scriptsInRoom;
	void initRoomScript(uint8 idx);
	void executeScript(uint8 idx);
	//			void skipBytes(RoomScript *script);
	uint8 readByteParameter(const char *&script);
	uint16 readXCoordParameter(const char *&script);
	int16 readWordParameter(const char *&script);

	int8 _resourceType;
	int16 getFlagValue();
	uint16 getVarOrImmediate();
	uint16 getImmediate();
	void skipBytes();
	bool compare(uint16 op1, uint16 op2, uint16 operation);
//	bool _mustReturn;
	bool setActorDestionation(bool isYcoord);
	bool mainActorIntersectsRectangle();
	bool mainActorIntersectsRectangle(int16 left, int16 top, int16 right, int16 bottom);
	bool setActorDestinationMainActor(bool isYOffset);
	void addHotspot(uint8 type);
	bool areActorsInRange(uint8 actor1Idx, uint8 actor2Idx, uint16 xRange, uint16 yRange);
	void fixScriptsBugs(int16 pak, uint8 room);

	};
} // namespace Cometengine
#endif
