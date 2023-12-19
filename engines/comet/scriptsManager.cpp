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

#include "comet/scriptsManager.h"
#include "comet/comet.h"

namespace Cometengine {
ScriptsManager::ScriptsManager(CometEngine *vm) : _vm(vm), _scriptsBuffer(nullptr), _multiChoiceDialogBuffer(nullptr),
												  //_scripts(),
												  //_scriptsInRoom(0),
												  _resourceType(0), _currScript(nullptr) //,_mustReturn(false)
{
	_opTable = shadowTable;
}
ScriptsManager::~ScriptsManager() {
	if (_scriptsBuffer != nullptr) {
		delete[] _scriptsBuffer;
		_scriptsBuffer = nullptr;
	}
	if (_multiChoiceDialogBuffer != nullptr) {
		delete[] _multiChoiceDialogBuffer;
		_multiChoiceDialogBuffer = nullptr;
	}
}
void ScriptsManager::initResources() {
	//ok
	_scriptsBuffer = new char[3000];
	_multiChoiceDialogBuffer = new char[1000];
}
void ScriptsManager::loadAndPrepareRoomScripts(char *filename, uint8 roomIdx, bool isLoading) {
	_vm->_archMgr->readFileBlock(filename, roomIdx, _scriptsBuffer, 3000);
	fixScriptsBugs(_vm->_gameState.currPakNum, roomIdx);
	if (!isLoading) {
		prepareScriptsAndInitRoom();
	} else {
		relocateScripts();
	}
}
void ScriptsManager::fixScriptsBugs(int16 pak, uint8 room) {
	// fixes for some game-breaking bugs in the scripts
	if (pak == 6 && room == 8) {
		_scriptsBuffer[0x13c7 - 0xf22] = 0x22;
		_scriptsBuffer[0x13f6 - 0xf22] = 0x22;
	}
}

void ScriptsManager::prepareScriptsAndInitRoom() {
	_vm->_gmMgr->resetHotspotsAndOtherValues();
	_vm->_gameState.numScriptsInRoom = READ_LE_INT16(_scriptsBuffer) / 2;
	for (uint8 i = 0; i < _vm->_gameState.numScriptsInRoom; i++) {
		initRoomScript(i);
	}
	_vm->_gameState.scripts[0].flags = ScriptFlags::NO_FLAGS;
	_vm->_gmMgr->killAllNPCs();
	executeScript(0);
}
void ScriptsManager::initRoomScript(uint8 idx) {
	char *data = _scriptsBuffer;
	uint16 offset = READ_LE_UINT16(data + (idx * 2));
	data = data + offset;
	RoomScript *script = &_vm->_gameState.scripts[idx];
	script->start = data;
	script->current = data;
	script->actorIdx = 0;
	script->flags = ScriptFlags::END;
	script->syncCounter = 0;
	script->loopCounter = 0;
}
//01
bool ScriptsManager::op_setActorFacingDir(RoomScript *const script) {
	uint8 facingDir = readByteParameter(script->current);
	Actor *actor = _vm->_gmMgr->getActor(script->actorIdx);
	actor->animationType = 0;
	_vm->_gmMgr->setActorFacingDirection(actor, facingDir);
	return false;
}
//02
bool ScriptsManager::op_endScript(RoomScript *const script) {
	script->flags |= ScriptFlags::END;
	return true;
}
//03
bool ScriptsManager::op_jmp(RoomScript *const script) { //03
	skipBytes();
	return false;
}
//04
bool ScriptsManager::op_setActorDestX(RoomScript *const script) {
	return setActorDestionation(false);
}
//05
bool ScriptsManager::op_setActorDestY(RoomScript *const script) {
	return setActorDestionation(true);
}
//06
bool ScriptsManager::op_loop(RoomScript *const script) {
	uint8 destVal = *(script->current + 2);
	script->loopCounter++;
	if (script->loopCounter < destVal) {
		skipBytes();
	} else {
		script->loopCounter = 0;
		script->current += 3;
	}
	return false;
}
//07
bool ScriptsManager::op_setActorPos(RoomScript *const script) {
	uint16 x = readXCoordParameter(script->current);
	uint8 y = readByteParameter(script->current);
	_vm->_gmMgr->setActorPosition(script->actorIdx, x, y);
	return false;
}
//08
bool ScriptsManager::op_setSyncTo(RoomScript *const script) {
	script->syncCounter = readByteParameter(script->current);
	script->flags |= ScriptFlags::SYNC;
	return true;
}
//09
bool ScriptsManager::op_setWait(RoomScript *const script) {
	script->syncCounter = readByteParameter(script->current);
	script->flags |= ScriptFlags::WAIT_TIMER;
	return true;
}
//0a
bool ScriptsManager::op_if(RoomScript *const script) {
	uint16 para1 = getFlagValue();
	uint8 operation = readByteParameter(script->current);
	uint16 para2 = getVarOrImmediate();
	bool compareResult = compare(para1, para2, operation);
	if (!compareResult) {
		skipBytes();
	} else {
		script->current += 2;
	}
	return false;
}
//0e
bool ScriptsManager::op_ifActorInArea(RoomScript *const script) {
	if (mainActorIntersectsRectangle()) {
		script->current += 2;
	} else {
		skipBytes();
	}
	return false;
}
//0f
bool ScriptsManager::op_setActorDestMainActorX(RoomScript *const script) {
	return setActorDestinationMainActor(false);
}
//10
bool ScriptsManager::op_setActorDestMainActorY(RoomScript *const script) {
	return setActorDestinationMainActor(true);
}
//11
bool ScriptsManager::op_moveActorTowardMainActor(RoomScript *const script) {
	uint8 p1 = readByteParameter(script->current);
	uint8 p2 = readByteParameter(script->current);
	Actor *act = _vm->_gmMgr->getActor(script->actorIdx);
	Actor *main = _vm->_gmMgr->getActor(0);
	act->animationType = 0;
	int16 x = main->pivotX + p1;
	int16 y = main->pivotY + p2;
	_vm->_gmMgr->clipRectToWalkingArea(x, y, act->halfWidth, act->height);
	act->whereToWalk = 0;
	if (_vm->_gmMgr->setActorDestination(script->actorIdx, x, y)) {
		script->flags |= ScriptFlags::WAIT_DESTINATION;
		return true;
	}
	return false;
}
//12
bool ScriptsManager::op_lockMainActor(RoomScript *const script) {
	uint8 p = readByteParameter(script->current);
	if (p == 0) {
		_vm->_gmMgr->setFacingDirection(0);
		_vm->_gameState.movementMask = 0x0f;
		_vm->_gmMgr->resetWalkAnimation(_vm->_gmMgr->getActor(0));
	} else {
		uint16 mask = _vm->_gameState.movementMask;
		switch (p) {
		case 0:
			break;
		case GameManager::kUp:
			_vm->_gameState.movementMask = (mask | GameManager::kUpFlag);
			break;
		case GameManager::kRight:
			_vm->_gameState.movementMask = (mask | GameManager::kRightFlag);
			break;
		case GameManager::kDown:
			_vm->_gameState.movementMask = (mask | GameManager::kDownFlag);
			break;
		case GameManager::kLeft:
			_vm->_gameState.movementMask = (mask | GameManager::kLeftFlag);
			break;
		}
	}
	return false;
}
//13
bool ScriptsManager::op_unlockMainActor(RoomScript *const script) {
	_vm->_gmMgr->unlockMainActor();
	return false;
}
//14
bool ScriptsManager::op_rotateActorTowardsMainActor(RoomScript *const script) {
	Actor *act = _vm->_gmMgr->getActor(script->actorIdx);
	Actor *parker = _vm->_gmMgr->getActor(0);
	uint8 comparison = _vm->_gmMgr->comparePoints(act->pivotX, act->pivotY, parker->pivotX, parker->pivotY);
	act->animationType = 0;
	_vm->_gmMgr->setActorFacingDirection(act, comparison);
	return false;
}
//15
bool ScriptsManager::op_setCurrentActor(RoomScript *const script) {
	script->actorIdx = readByteParameter(script->current);
	return false;
}
//16
bool ScriptsManager::op_loadWalkAreaTopSegments(RoomScript *const script) {
	uint8 numPoints = readByteParameter(script->current);
	_vm->_gmMgr->setWalkAreaTopLineNumPoints(numPoints);
	for (uint8 i = 0; i < numPoints; i++) {
		uint16 x = readXCoordParameter(script->current);
		uint8 y = readByteParameter(script->current);
		_vm->_gmMgr->setWalkAreaTopLinePoint(i, x, y);
	}
	_vm->_gmMgr->loadWalkArea();
	return false;
}
//17
bool ScriptsManager::op_setSceneExits(RoomScript *const script) {
	uint8 numExits = readByteParameter(script->current);
	for (uint8 i = 0; i < numExits; i++) {
		uint8 dir = readByteParameter(script->current);
		uint8 pak = readByteParameter(script->current);
		uint8 room = readByteParameter(script->current);
		uint16 x1 = readXCoordParameter(script->current);
		uint16 x2 = readXCoordParameter(script->current);
		_vm->_gmMgr->setExit(i, dir, pak, room, x1, x2);
	}
	_vm->_gameState.numExits = numExits;
	return false;
}
//19
bool ScriptsManager::op_addHotspot_type0(RoomScript *const script) {
	addHotspot(0);
	return false;
}
//1a
bool ScriptsManager::op_setEscPressed(RoomScript *const script) {
	_vm->setEscPressed();
	return false;
}
//1b
bool ScriptsManager::op_startScript(RoomScript *const script) {
	uint8 p = readByteParameter(script->current);
	if (p < _vm->_gameState.numScriptsInRoom) {
		initRoomScript(p);
		_vm->_gameState.scripts[p].flags &= ~ScriptFlags::END;
	}
	return false;
}
//1c
bool ScriptsManager::op_stopScript(RoomScript *const script) {
	uint8 procIdx = readByteParameter(script->current);
	_vm->_gameState.scripts[procIdx].flags |= ScriptFlags::END;
	return false;
}
//1d
bool ScriptsManager::op_restartScript(RoomScript *const script) {
	uint8 procIdx = 0;
	do {
	
		procIdx = readByteParameter(script->current);
		if (procIdx == 0xff) {
			break;
		}
		initRoomScript(procIdx);
		_vm->_gameState.scripts[procIdx].flags &= ~ScriptFlags::END;
	} while (true);
	return false;
}
//1e
bool ScriptsManager::op_playCutscene(RoomScript *const script) {
	uint8 subfile = readByteParameter(script->current);
	uint8 animidx = readByteParameter(script->current);
	int16 startIdxAndFlags = readWordParameter(script->current);
	uint8 numLoops = readByteParameter(script->current);
	uint8 numDigiFXinAnim = readByteParameter(script->current);
	const char *audioCues = script->current;
	script->current += numDigiFXinAnim * 3;
	_vm->_gmMgr->showLoopedAnimation(subfile, animidx, startIdxAndFlags, numLoops, numDigiFXinAnim, audioCues);
	return true;
}
//1f
bool ScriptsManager::op_setGameVar(RoomScript *const script) {
	uint16 varIdx = readWordParameter(script->current);
	int16 val = getVarOrImmediate();
	int16 *ptr = _vm->_gmMgr->getVarPtr(varIdx);
	*ptr = val;
	return false;
}
//20
bool ScriptsManager::op_addToGameVar(RoomScript *const script) {
	uint16 varIdx = readWordParameter(script->current);
	int16 value = getVarOrImmediate();
	int16 *varPtr = _vm->_gmMgr->getVarPtr(varIdx);
	*varPtr += value;
	return false;
}
//21
bool ScriptsManager::op_subFromGameVar(RoomScript *const script) {
	uint16 varIdx = readWordParameter(script->current);
	int16 val = getVarOrImmediate();
	int16 *varPtr = _vm->_gmMgr->getVarPtr(varIdx);
	*varPtr -= val;
	return false;
}
//22
bool ScriptsManager::op_disableActorCollisions(RoomScript *const script) {
	Actor *act = _vm->_gmMgr->getActor(script->actorIdx);
	act->intersectionType = 8;
	return false;
}
//23
bool ScriptsManager::op_enableActorCollisions(RoomScript *const script) {
	Actor *act = _vm->_gmMgr->getActor(script->actorIdx);
	act->intersectionType = 0;
	return false;
}
//24
bool ScriptsManager::op_setActorDestinationXY(RoomScript *const script) {
	uint16 x = readXCoordParameter(script->current);
	uint8 y = readByteParameter(script->current);
	Actor *act = _vm->_gmMgr->getActor(script->actorIdx);
	act->animationType = 0;
	if (_vm->_gmMgr->setActorDestination(script->actorIdx, x, y)) {
		script->flags |= ScriptFlags::WAIT_DESTINATION;
		return true;
	}
	return false;
}
//25
bool ScriptsManager::op_setStageColorsReplace(RoomScript *const script) {
	_vm->_gMgr->stageColorReplace = 1;
	_vm->_gameState.paletteFadeLevel = 0;
	return false;
}
//26
bool ScriptsManager::op_fadePalette(RoomScript *const script) {
	_vm->_gameState.paletteFadeLevel = readByteParameter(script->current);
	_vm->_gMgr->fadePalette(_vm->_gMgr->mainGamePalette, _vm->_gMgr->tmpPalette, _vm->_gameState.paletteFadeLevel, 256);
	_vm->_gMgr->uploadPalette(_vm->_gMgr->tmpPalette);
	return false;
}
//27
bool ScriptsManager::op_waitForMainActorToLeaveArea(RoomScript *const script) {
	script->current--;
	if (mainActorIntersectsRectangle(script->left, script->top, script->right, script->bottom)) {
		return true;
	} else {
		script->current++;
	}
	return false;
}
//28
bool ScriptsManager::op_waitForMainActorToEnterArea(RoomScript *const script) {
	int16 left, top, right, bottom;
	left = script->left = readXCoordParameter(script->current);
	top = script->top = readByteParameter(script->current);
	right = script->right = readXCoordParameter(script->current);
	bottom = script->bottom = readByteParameter(script->current);
	if (!mainActorIntersectsRectangle(left, top, right, bottom)) {
		script->current -= 5;
		return true;
	}
	return false;
}
//29
bool ScriptsManager::op_freeActor(RoomScript *const script) {
	if (script->actorIdx != 0) {
		Actor *act = _vm->_gmMgr->getActor(script->actorIdx);
		act->life = 0;
		if (act->resourceIdx != -1) {
			_vm->_gmMgr->freeResourceFromCharacter(act);
		}
	}
	return false;
}
//2A
bool ScriptsManager::op_setActorViewportX(RoomScript *const script) {
	Actor *actor = _vm->_gmMgr->getActor(script->actorIdx);
	actor->drawAreaMinX = readXCoordParameter(script->current);
	actor->drawAreaMaxX = readXCoordParameter(script->current);
	return false;
}
//2B
bool ScriptsManager::op_setActorViewportY(RoomScript *const script) {
	Actor *actor = _vm->_gmMgr->getActor(script->actorIdx);
	actor->drawAreaMinY = readByteParameter(script->current);
	actor->drawAreaMaxY = readByteParameter(script->current);
	return false;
}
//2C
bool ScriptsManager::op_setNewRoom(RoomScript *const script) {
		int16 room = readByteParameter(script->current);
	if (room == 0x00ff) {
		room = -1;
	}
	_vm->_gameState.newRoomNum = room;
	return false;
}
//2D
bool ScriptsManager::op_setActorAnimationFreezeFrame(RoomScript *const script) {
	uint8 anim = readByteParameter(script->current);
	uint8 frame = readByteParameter(script->current);
	Actor *actor = _vm->_gmMgr->getActor(script->actorIdx);
	actor->animationIdx = anim;
	actor->numFrames = 0;
	actor->animationType = 2;
	actor->frameToDraw = frame;
	actor->fixedAnimationFrame = frame;
	return false;
}
//2E
bool ScriptsManager::op_activateSecondaryVoices(RoomScript *const script) {
	uint8 voice = readByteParameter(script->current);
	if (_vm->_gameState.musicVolume > 50) {
		_vm->_audioMgr->setVoiceParams(50, 0, 0x2000);
		_vm->_audioMgr->setVoiceParams(_vm->_gameState.musicVolume, 0, 0x8000);
		_vm->_audioMgr->setSecondaryVoiceParams(0, 0, 0x40);
		_vm->_audioMgr->setSecondaryVoiceParams(0, voice, 0x80);
	}
	return false;
}
//2F
bool ScriptsManager::op_setResourceType(RoomScript *const script) {
	int16 type = readByteParameter(script->current);
	_resourceType = type;
	return false;
}
//30
bool ScriptsManager::op_pivotActorToMainActor(RoomScript *const script) {
	_vm->_gmMgr->setActorPosition(script->actorIdx, _vm->_gameState.actors[0].pivotX, _vm->_gameState.actors[0].pivotY + 1);
	return false;
}
//32
bool ScriptsManager::op_drawRandomEffect(RoomScript *const script) {
	uint8 p1 = readByteParameter(script->current);
	_vm->_gMgr->randCoordsBaseX = readXCoordParameter(script->current);
	_vm->_gMgr->randCoordsBaseY = readXCoordParameter(script->current);
	switch (p1) {
	case 0x00: { //0x67
				 //		_vm->_gMgr->randomDrawActive = 0;
		this->op_deactivateRandomDraw(script);
	} break;
	case 0x1: { //0x68
		this->op_activateRandomDraw(script);
		//_vm->_gMgr->randomDrawActive = 1;
		//_vm->_gMgr->generateRandCoords(_vm->_gMgr->randCoordsBaseX, _vm->_gMgr->randCoordsBaseY, 50, 500);
	} break;
	case 0x2: { //0x69
		this->op_setDrawWithEffect(script);
		//_vm->_gMgr->setDrawWithEffect();
	} break;
	case 0x3: { //0x6a
		this->op_drawRandomTheEnd(script);
		//_vm->_gmMgr->updateFontDataAndColor(0x49);
		//_vm->_txtMgr->drawString(_vm->_gMgr->randCoordsBaseX, _vm->_gMgr->randCoordsBaseY, _vm->_gMgr->getBackbuffer(), "The End");
	} break;
	}
	return false;
}
//33
bool ScriptsManager::op_setNewZoomLevel(RoomScript *const script) {
	_vm->_gameState.roomZoomX = readXCoordParameter(script->current);
	_vm->_gameState.roomZoomY = readByteParameter(script->current);
	uint16 prevZomm = _vm->_gmMgr->_roomZoomLevel;
	_vm->_gmMgr->_roomZoomLevel = readByteParameter(script->current);
	if (prevZomm != 0 && _vm->_gmMgr->_roomZoomLevel == 0) {
		_vm->_gmMgr->_zoomRestored = true;
	}
	return false;
}
//34
bool ScriptsManager::op_zoomOnActor(RoomScript *const script) {
	uint8 actIdx = readByteParameter(script->current);
	_vm->_gameState.roomZoomX = _vm->_gameState.actors[actIdx].pivotX;
	_vm->_gameState.roomZoomY = _vm->_gameState.actors[actIdx].pivotY;
	uint16 prevZomm = _vm->_gmMgr->_roomZoomLevel;
	_vm->_gmMgr->_roomZoomLevel = readByteParameter(script->current);
	if (prevZomm != 0 && _vm->_gmMgr->_roomZoomLevel == 0) {
		_vm->_gmMgr->_zoomRestored = true;
	}
	return false;
}
//35
bool ScriptsManager::op_multiChoice(RoomScript *const script) {
	_vm->_txtMgr->hideDialogue();
	uint16 introSentence = readWordParameter(script->current);
	_vm->_txtMgr->setMultiChoiceIntroSentence(introSentence);
	char *buffer = _vm->_txtMgr->getMultiChoiceText();
	*((uint32 *)buffer) = (uint32)28;
	uint32 sentenceLen = 0;
	if (introSentence != 0xffff) {
		sentenceLen = _vm->_txtMgr->getSentence(_vm->_gameState.textChapterID + 3, introSentence, buffer + 28);
		_vm->_txtMgr->setMultiChoiceIntroSentenceData(_vm->_txtMgr->getSentencePtr(0, buffer));
	}
	uint16 x = readXCoordParameter(script->current);
	uint16 y = readByteParameter(script->current);
	uint8 num = readByteParameter(script->current);
	_vm->_txtMgr->setMultiChoiceTextCoords(x, y);
	_vm->_txtMgr->setMultiChoiceNum(num);
	for (uint8 i = 0; i < num; i++) {
		TextManager::Choice *choice = _vm->_txtMgr->getChoice(i);
		choice->sentenceIdx = readWordParameter(script->current);
		((uint32 *)buffer)[i + 1] = sentenceLen + 28;
		sentenceLen += _vm->_txtMgr->getSentence(_vm->_gameState.textChapterID + 3, choice->sentenceIdx, buffer + sentenceLen + 28);
		choice->sentence = _vm->_txtMgr->getSentencePtr(i + 1, buffer);
		choice->scriptData = script->current;
		script->current += 2;
	}
	if (introSentence == _vm->_txtMgr->getChoice(0)->sentenceIdx) {
		_vm->_txtMgr->setMultiChoiceIntroSentence(0xffff);
	}
	_vm->_txtMgr->setChoice(0);
	_vm->_gmMgr->setWaitForEnter();
	script->flags |= ScriptFlags::WAIT_CHOICE;
	return true;
}
//36
bool ScriptsManager::op_actorSaySentence(RoomScript *const script) {
	uint16 p1 = readWordParameter(script->current);
	uint8 p2 = readByteParameter(script->current);
	Actor *act = _vm->_gmMgr->getActor(script->actorIdx);
	_vm->_txtMgr->actorSaySentence(script->actorIdx, p1, act->speechColor);
	_vm->_txtMgr->setTextRemainingFrame(p2);
	_vm->_txtMgr->setTextDuration(p2);
	return false;
}
//37
bool ScriptsManager::op_setClearScreen(RoomScript *const script) {
	_vm->_gMgr->setClearScreen();
	return false;
}
//38
bool ScriptsManager::op_orVar(RoomScript *const script) {
	uint16 varIdx = readWordParameter(script->current);
	int16 value = getVarOrImmediate();
	int16 *varPtr = _vm->_gmMgr->getVarPtr(varIdx);
	*varPtr |= value;
	return false;
}
//39
bool ScriptsManager::op_andVar(RoomScript *const script) {
	uint16 varIdx = readWordParameter(script->current);
	int16 value = getVarOrImmediate();
	int16 *varPtr = _vm->_gmMgr->getVarPtr(varIdx);
	*varPtr &= value;
	return false;
}
//3A
bool ScriptsManager::op_loadScene(RoomScript *const script) {
	_vm->_gameState.stageIndex = readByteParameter(script->current) * 2;
	_vm->_gMgr->loadStage();
	return false;
}
//3B
bool ScriptsManager::op_setWalkVerticalAdjustment(RoomScript *const script) {
	_vm->_gameState.walkVerticalAdjustment = readByteParameter(script->current);
	return false;
}
//3C
bool ScriptsManager::op_addObstacle(RoomScript *const script) {
	uint8 x1 = readByteParameter(script->current);
	uint8 y1 = readByteParameter(script->current);
	uint8 x2 = readByteParameter(script->current);
	uint8 y2 = readByteParameter(script->current);
	_vm->_gMgr->addStageElement(x1, y1, x2, y2);
	return false;
}
//3D
bool ScriptsManager::op_ifTalk(RoomScript *const script) {
	if (!_vm->_gmMgr->isTalk()) {
		skipBytes();
	} else {
		script->current += 2;
		_vm->_gmMgr->unsetTalk();
	}
	return false;
}
//3E
bool ScriptsManager::op_isActorInTalkRange(RoomScript *const script) {
	if (!_vm->_gmMgr->isTalk()) {
		script->current++;
		skipBytes();
	} else {
		uint8 act2Idx = readByteParameter(script->current);
		if (!areActorsInRange(0, act2Idx, 40, 40)) {
			skipBytes();
		} else {
			script->current += 2;
			_vm->_gmMgr->unsetTalk();
		}
	}
	return false;
}
//3F
bool ScriptsManager::op_ifTalkInArea(RoomScript *const script) {
	if (!_vm->_gmMgr->isTalk()) {
		script->current += 4;
		skipBytes();
	} else {
		if (!mainActorIntersectsRectangle()) {
			skipBytes();
		} else {
			script->current += 2;
			_vm->_gmMgr->unsetTalk();
		}
	}
	return false;
}
//40
bool ScriptsManager::op_ifLook(RoomScript *const script) {
	if (!_vm->_gmMgr->isLook()) {
		skipBytes();
	} else {
		script->current += 2;
		_vm->_gmMgr->unsetLook();
	}
	return false;
}
//41
bool ScriptsManager::op_ifLookActorsInRange(RoomScript *const script) {
	if (!_vm->_gmMgr->isLook()) {
		script->current++;
		skipBytes();
	} else {
		uint8 act2Idx = readByteParameter(script->current);
		if (!areActorsInRange(0, act2Idx, 40, 40)) {
			skipBytes();
		} else {
			script->current += 2;
			_vm->_gmMgr->unsetLook();
		}
	}
	return false;
}
//42
bool ScriptsManager::op_ifLookInArea(RoomScript *const script) {
	if (!_vm->_gmMgr->isLook()) {
		script->current += 4;
		skipBytes();
	} else {
		if (!mainActorIntersectsRectangle()) {
			skipBytes();
		} else {
			script->current += 2;
		}
	}
	return false;
}
//43
bool ScriptsManager::op_addLightning(RoomScript *const script) {
	uint16 *ptr = _vm->_gMgr->randCoords;
	ptr += _vm->_gMgr->numLightnings * 4;
	_vm->_gMgr->numLightnings++;
	ptr[0] = readXCoordParameter(script->current);
	ptr[1] = readByteParameter(script->current);
	ptr[3] = readXCoordParameter(script->current);
	ptr[2] = readByteParameter(script->current);
	return false;
}
//44
bool ScriptsManager::op_removeObstacle(RoomScript *const script) {
	uint8 numElems = _vm->_gameState.numStageElements;
	StageElement *ptr = &_vm->_gameState.stageElements[numElems]; // yeah, it's out of bounds, that's intended
	uint8 left = readByteParameter(script->current);
	uint8 top = readByteParameter(script->current);
	for (uint8 i = 0; i < numElems; i++) {
		ptr--;
		//					if (ptr->halfLeft == left && ptr->halfRight == right) {
		if (ptr->halfLeft == left && ptr->top == top) {
			ptr->halfRight = ptr->halfLeft;
		}
	}
	return false;
}
//45
bool ScriptsManager::op_setActorSpeechColor(RoomScript *const script) {
	_vm->_gmMgr->getActor(script->actorIdx)->speechColor = readByteParameter(script->current);
	return false;
}
//46
bool ScriptsManager::op_setActorSpeechPivot(RoomScript *const script) {
	Actor *actor = _vm->_gmMgr->getActor(script->actorIdx);
	actor->textPivotX = readXCoordParameter(script->current);
	actor->textPivotY = readByteParameter(script->current);
	return false;
}
//47
//55
bool ScriptsManager::op_nop(RoomScript *const script) {
	return false;
}
//48
bool ScriptsManager::op_resetScriptCounterAndJmp(RoomScript *const script) {
	script->loopCounter = 0;
	skipBytes();
	return false;
}
//49
bool ScriptsManager::op_activateRandomStars(RoomScript *const script) {
	_vm->_gMgr->randomStarsActive = 1;
	_vm->_gMgr->randomStarsLeft = readXCoordParameter(script->current);
	_vm->_gMgr->randomStarsTop = readByteParameter(script->current);
	_vm->_gMgr->randomStarsRight = readXCoordParameter(script->current);
	_vm->_gMgr->randomStarsBottom = readByteParameter(script->current);
	return false;
}
//4A
bool ScriptsManager::op_setActorIsKillable(RoomScript *const script) {
	Actor *act = &_vm->_gameState.actors[script->actorIdx];
	act->isKillable = readByteParameter(script->current);
	return false;
}
//4C
bool ScriptsManager::op_setClearScreenReturn(RoomScript *const script) {
	uint8 v1 = readByteParameter(script->current);
	_vm->_gMgr->setClearScreen();
	return true;
}
//4D
bool ScriptsManager::op_startSong(RoomScript *const script) {
	uint8 songNum = readByteParameter(script->current);
	if (songNum != 0xff) {
		_vm->_audioMgr->playMusicSubfile(songNum);
	} else {
		_vm->_audioMgr->muteAllVoices();
	}
	return false;
}
//4E
bool ScriptsManager::op_setMusicVolume(RoomScript *const script) {
	uint8 vol = readByteParameter(script->current);
	_vm->_gameState.musicVolume = vol;
	_vm->_audioMgr->setVoiceParams(vol, 0, 0x2000);
	return false;
}
//4F
bool ScriptsManager::op_generateRandomNumber(RoomScript *const script) {
	_vm->_gmMgr->setStatusVar(33, _vm->_gmMgr->getRandom(readByteParameter(script->current)));
	return false;
}
//50
bool ScriptsManager::op_changePak(RoomScript *const script) {
	_vm->_gameState.newPakNum = readByteParameter(script->current);
	return true;
}
//51
bool ScriptsManager::op_readArea(RoomScript *const script) {
	readXCoordParameter(script->current);
	readByteParameter(script->current);
	readXCoordParameter(script->current);
	readByteParameter(script->current);
	return false;
}
//52
bool ScriptsManager::op_actorSpeakWithAnimation(RoomScript *const script) {
	uint8 actor = readByteParameter(script->current);
	uint16 sentence = readWordParameter(script->current);
	uint8 animation = readByteParameter(script->current);
	_vm->_gMgr->actorSaySentenceWithAnimation(actor, sentence, animation);
	return true;
}
//53
bool ScriptsManager::op_setForceLoading(RoomScript *const script) {
	_vm->_gmMgr->forceLoading = 1;
	return true;
}
//54
bool ScriptsManager::op_addHotspotType1(RoomScript *const script) {
	addHotspot(1);
	return false;
}
//56
bool ScriptsManager::op_seedRandomizerWhileIdle(RoomScript *const script) {
	do {
		_vm->_gmMgr->updateInputStatus();
	} while (_vm->_gmMgr->_mouseButtons != 0 || _vm->_gmMgr->_movementStatus != 0 || _vm->_gmMgr->_lastPressedKey != 0);
	while (_vm->_gmMgr->_mouseButtons == 0 && _vm->_gmMgr->_movementStatus == 0 && _vm->_gmMgr->_lastPressedKey == 0) {
		_vm->_gmMgr->_rnd.setSeed(_vm->_gmMgr->_rnd.getSeed());
		_vm->_gmMgr->updateInputStatus();
	}
	return false;
}
//57
bool ScriptsManager::op_setActorAnimationAndWait(RoomScript *const script) {
	//uint8 actAnim = readByteParameter(script->current);
	//Actor *act = _vm->_gmMgr->getActor(script->actorIdx);
	//_vm->_gmMgr->actorPrepareAnimation(act, actAnim);
	//act->animationType = 2;
	this->op_setActorAnimation(script);
	script->flags |= ScriptFlags::WAIT_ANIMATION;
	return true;
}
//58
bool ScriptsManager::op_setActorAnimation(RoomScript *const script) {
	uint8 actAnim = readByteParameter(script->current);
	Actor *act = _vm->_gmMgr->getActor(script->actorIdx);
	_vm->_gmMgr->actorPrepareAnimation(act, actAnim);
	act->animationType = 2;
	return false;
}
//59
bool ScriptsManager::op_actorSpeakWithPortrait(RoomScript *const script) {
	Actor *actor = _vm->_gmMgr->getActor(script->actorIdx);
	uint16 talkingActorIdx = readByteParameter(script->current);
	uint16 sentenceIdx = readWordParameter(script->current);
	uint8 animIdx = readByteParameter(script->current);
	uint8 subfile = readByteParameter(script->current);
	_vm->_gmMgr->createActor(10, _vm->_gmMgr->getResourceIdx(_resourceType, subfile));
	if (talkingActorIdx != 0xffff) { //sic in the original code, can't happen
		Actor *portrait = _vm->_gmMgr->getActor(10);
		portrait->textPivotX = 0;
		portrait->textPivotY = 160;
		portrait->speechColor = _vm->_gmMgr->getActor(talkingActorIdx)->speechColor;
	}
	_resourceType = 0;
	_vm->_gmMgr->setActorPosition(10, 0, 199);
	_vm->_gMgr->actorSaySentenceWithAnimation(10, sentenceIdx, animIdx);
	_vm->_txtMgr->setPrevAnimationOrActor(talkingActorIdx);
	_vm->_gMgr->setDrawWithEffect();
	return true;
}
//5A
bool ScriptsManager::op_resetCurrentActor(RoomScript *const script) {
	//script->actorIdx = readByteParameter(script->current);
	this->op_setCurrentActor(script);
	if (script->actorIdx != 0) {
		_vm->_gmMgr->createActor(script->actorIdx, -1);
	}
	return false;
}
//5B
bool ScriptsManager::op_changeActorResouce(RoomScript *const script) {
	Actor *act = _vm->_gmMgr->getActor(script->actorIdx);
	int8 subfile = readByteParameter(script->current);
	if (subfile == 84) {
		int8 brekki = 1;
	}
	_vm->_gmMgr->freeResourceFromCharacter(act);
	act->resourceIdx = _vm->_gmMgr->getResourceIdx(_resourceType, subfile);
	_resourceType = 0;
	return false;
}
//5C
bool ScriptsManager::op_setActorVisibility(RoomScript *const script) {
	uint8 v = readByteParameter(script->current);
	_vm->_gmMgr->getActor(script->actorIdx)->isVisible = v;
	return false;
}
//5D
bool ScriptsManager::op_fadeIn(RoomScript *const script) {
	uint8 v = readByteParameter(script->current);
	_vm->_gMgr->setFadeSpeed(v);
	_vm->_gMgr->setFadeStatus(1);
	return false;
}
//5E
bool ScriptsManager::op_fadeOut(RoomScript *const script) {
	uint8 v = readByteParameter(script->current);
	_vm->_gMgr->setFadeSpeed(v);
	_vm->_gMgr->setFadeStatus(2);
	return false;
}
//5F
bool ScriptsManager::op_fadePaletteBlock(RoomScript *const script) {
	uint8 block = readByteParameter(script->current);
	uint8 fadeLevel = readByteParameter(script->current);
	_vm->_gMgr->fadePaletteBlock(block, fadeLevel);
	return false;
}
//60
bool ScriptsManager::op_setTextChapter(RoomScript *const script) {
	_vm->_gameState.textChapterID = readByteParameter(script->current);
	_vm->_spMgr->openNARarchive(_vm->_gameState.textChapterID);
	return false;
}
//61
bool ScriptsManager::op_ifSpecificActorInRange(RoomScript *const script) {
	uint8 actor2Idx = readByteParameter(script->current);
	uint8 xRange = readByteParameter(script->current);
	uint8 yRange = readByteParameter(script->current);
	if (!areActorsInRange(script->actorIdx, actor2Idx, xRange, yRange)) {
		skipBytes();
	} else {
		script->current += 2;
	}
	return false;
}
//62
bool ScriptsManager::op_disableHotspot(RoomScript *const script) {
	uint8 objIdx = readByteParameter(script->current);
	for (uint16 i = 0; i < 20; i++) {
		Hotspot *hot = &_vm->_gameState.hotspots[i];
		if (hot->isActive && hot->objIdx == objIdx) {
			hot->isActive = false;
		}
	}
	return false;
}
//63
bool ScriptsManager::op_playDigiSample(RoomScript *const script) {
	uint8 smpIdx = readByteParameter(script->current);
	_vm->_audioMgr->playDigiSubfile(smpIdx);
	return false;
}
//64
bool ScriptsManager::op_playDigiSampleLooped(RoomScript *const script) {
	uint8 smpIdx = readByteParameter(script->current);
	uint8 numLoops = readByteParameter(script->current);
	_vm->_audioMgr->playDigiSubfile(smpIdx, numLoops);
	return false;
}
//65
bool ScriptsManager::op_setPaletteTint(RoomScript *const script) {
	uint8 tint = readByteParameter(script->current);
	_vm->_gMgr->setPaletteTint(tint);
	return false;
}
//66
bool ScriptsManager::op_changePaletteBrightness(RoomScript *const script) {
	uint8 factor = readByteParameter(script->current);
	_vm->_gMgr->scalePaletteBrightness(factor);
	return false;
}
//67
bool ScriptsManager::op_deactivateRandomDraw(RoomScript *const script) {
	_vm->_gMgr->randomDrawActive = 0;
	return false;
}
//68
bool ScriptsManager::op_activateRandomDraw(RoomScript *const script) {
	_vm->_gMgr->randomDrawActive = 1;
	_vm->_gMgr->generateRandCoords(_vm->_gMgr->randCoordsBaseX, _vm->_gMgr->randCoordsBaseY, 50, 500);
	return false;
}
//69
bool ScriptsManager::op_setDrawWithEffect(RoomScript *const script) {
	_vm->_gMgr->setDrawWithEffect();
	return false;
}
//6A
bool ScriptsManager::op_drawRandomTheEnd(RoomScript *const script) {
	_vm->_gmMgr->updateFontDataAndColor(0x49);
	_vm->_txtMgr->drawString(_vm->_gMgr->randCoordsBaseX, _vm->_gMgr->randCoordsBaseY, _vm->_gMgr->getBackbuffer(), "The End");
	return false;
}

void ScriptsManager::executeScript(uint8 idx) {
	//char stringa[256];
	//snprintf(stringa, 256, "Eseguo script %d", idx);
	//if (_vm->_gameState.currPakNum == 4 && _vm->_gameState.currRoomNum == 11) {
	//	debug(stringa);
	//}
	bool _mustReturn = false;
	RoomScript *script = &_vm->_gameState.scripts[idx];
	_currScript = script;
	if (script->flags & ScriptFlags::END) {
		return;
	}

	//handle "sync with other script" case
	if (script->flags & ScriptFlags::SYNC) {
		RoomScript *script2 = &_vm->_gameState.scripts[script->syncCounter];
		if ((script2->flags & ScriptFlags::SYNC) && script2->syncCounter == idx) {
			script2->flags &= ~ScriptFlags::SYNC;
			script2->syncCounter = 0;
			script->flags &= ~ScriptFlags::SYNC;
			script->syncCounter = 0;
		} else {
			_mustReturn = true;
		}
	}
	//handle "wait timer" case
	if (script->flags & ScriptFlags::WAIT_TIMER) {
		if (script->syncCounter > 0) {
			script->syncCounter--;
		}
		if (script->syncCounter == 0) {
			script->flags &= ~ScriptFlags::WAIT_TIMER;
		} else {
			_mustReturn = true;
		}
	}
	//handle "wait destination" case
	if (script->flags & ScriptFlags::WAIT_DESTINATION) {
		Actor *actor = &_vm->_gameState.actors[script->actorIdx];
		if (!(actor->whereToWalk & 3) || actor->life == 0) {
			script->flags &= ~ScriptFlags::WAIT_DESTINATION;
			_vm->_gmMgr->actorPrepareAnimation(actor, 0);
		} else {
			_mustReturn = true;
		}
		//some debug code here
	}
	//handle "wait animation end" case
	if (script->flags & ScriptFlags::WAIT_ANIMATION) {
		Actor *actor = &_vm->_gameState.actors[script->actorIdx];
		if (actor->frameToDraw == actor->numFrames - 1) {
			script->flags &= ~ScriptFlags::WAIT_ANIMATION;
		} else {
			_mustReturn = true;
		}
	}
	//handle "wait dialogue choice" case
	if (script->flags & ScriptFlags::WAIT_CHOICE) {
		if (_vm->_gmMgr->waitingForEnter()) {
			_mustReturn = true;
		} else {
			script->flags &= ~ScriptFlags::WAIT_CHOICE;
			script->current = _vm->_txtMgr->getChoice(_vm->_txtMgr->getChoice())->scriptData;
			skipBytes();
		}
	}
	//handle "wait text" case
	if (script->flags & ScriptFlags::WAIT_TEXT) {
		if (_vm->_txtMgr->isTextDisplayed()) {
			_mustReturn = true;
		} else {
			script->flags &= ~ScriptFlags::WAIT_TEXT;
			if (_vm->_txtMgr->getSpeakingActor() == 0x0a) {
				if (_vm->_txtMgr->getPrevAnimationOrActor() != -1) {
					_vm->_gameState.actors[_vm->_txtMgr->getPrevAnimationOrActor()].isVisible = true;
				}
				_vm->_gameState.actors[10].life = 0;
				_vm->_gMgr->setDrawWithEffect();
			} else {
				if (_vm->_txtMgr->getPrevAnimationOrActor() != -1) {
					Actor *actor = _vm->_gmMgr->getActor(_vm->_txtMgr->getSpeakingActor());
					_vm->_gmMgr->actorPrepareAnimation(actor, _vm->_txtMgr->getPrevAnimationOrActor());
					actor->fixedAnimationFrame = _vm->_txtMgr->getPrevFixedFrame();
					actor->frameToDraw = _vm->_txtMgr->getPrevFrameToDraw();
					_vm->_txtMgr->setPrevAnimationOrActor(-1);
				}
			}
		}
	}

	//ok let's execute opcodes
	while (!_mustReturn) {
		if (_vm->isQuitRequested()) {
			return;
		}
		uint8 opcode = *script->current;
		//snprintf(stringa, 256, "Opcode %02X, offset:%04X", opcode, script->current - script->start);
		//if (_vm->_gameState.currPakNum == 4 && _vm->_gameState.currRoomNum == 11) {
		//	debug(stringa);
		//}

		//		debug(stringa);
		script->current++;
		if (_opTable[opcode] != nullptr) {
			_mustReturn = (this->*_opTable[opcode])(script);
		}
		else {
			char debug[200];
			snprintf(debug, 200, "[%02x] - (%02x) opcode %02x", idx, script->current - script->start - 1, opcode);
		}
	}
}
bool ScriptsManager::areActorsInRange(uint8 actor1Idx, uint8 actor2Idx, uint16 xRange, uint16 yRange) {
	Actor *act1 = _vm->_gmMgr->getActor(actor1Idx);
	Actor *act2 = _vm->_gmMgr->getActor(actor2Idx);
	Rectangle r1, r2;
	r1.left = act1->pivotX - act1->halfWidth;
	r1.top = act1->pivotY - act1->height;
	r1.right = act1->pivotX + act1->halfWidth;
	r1.bottom = act1->pivotY;

	r2.left = act2->pivotX - (xRange / 2);
	r2.top = act2->pivotY - (yRange / 2);
	r2.right = act2->pivotX + (xRange / 2);
	r2.bottom = act2->pivotY + (yRange / 2);

	return _vm->_gmMgr->rectanglesIntersect(r1, r2);
}

void ScriptsManager::addHotspot(uint8 type) {
	uint16 idx;
	if (type == 0) {
		idx = readByteParameter(_currScript->current);
	} else {
		idx = readWordParameter(_currScript->current);
	}
	uint16 x = readXCoordParameter(_currScript->current);
	uint16 y = readByteParameter(_currScript->current);
	Hotspot *hotspot = _vm->_gmMgr->getFirstInactiveHotspot();
	hotspot->isActive = true;
	hotspot->objIdx = idx;
	hotspot->objType = type;
	hotspot->x = x;
	hotspot->y = y;
}

uint8 ScriptsManager::readByteParameter(const char *&script) {
	return *script++;
}
uint16 ScriptsManager::readXCoordParameter(const char *&script) {
	unsigned char b = static_cast<unsigned char>(*script++);
	return b * 2;
}
int16 ScriptsManager::readWordParameter(const char *&script) {
	int16 w = READ_LE_INT16(script);
	script += 2;
	return w;
}
/*
	void ScriptsManager::skipBytes(RoomScript *script) {
		script->current += READ_LE_INT16(script->current);
	}
	*/
void ScriptsManager::relocateScripts() {
	for (uint8 i = 0; i < _vm->_gameState.numScriptsInRoom; i++) {
		uint16 offset = READ_LE_UINT16(_scriptsBuffer + (i * 2));
		char *data = _scriptsBuffer + offset;
		_vm->_gameState.scripts[i].current = data + (_vm->_gameState.scripts[i].current - _vm->_gameState.scripts[i].start);
		_vm->_gameState.scripts[i].start = data;
	}
}
void ScriptsManager::executeRoomScripts() {
	for (uint8 i = 1; i < _vm->_gameState.numScriptsInRoom; i++) {
		executeScript(i);
	}
}
int16 ScriptsManager::getFlagValue() {
	uint16 idx = readWordParameter(_currScript->current);
	int16 *ptr = _vm->_gmMgr->getVarPtr(idx);
	return *ptr;
}
uint16 ScriptsManager::getVarOrImmediate() {
	if (*_currScript->current == 0x0b) {
		_currScript->current++;
		return getFlagValue();
	} else {
		return getImmediate();
	}
}
uint16 ScriptsManager::getImmediate() {
	if (*_currScript->current++ == 0x0c) {
		return *_currScript->current++;
	} else {
		uint16 v = READ_LE_UINT16(_currScript->current);
		_currScript->current += 2;
		return v;
	}
}
void ScriptsManager::skipBytes() {
	_currScript->current += READ_LE_INT16(_currScript->current);
}
bool ScriptsManager::compare(uint16 op1, uint16 op2, uint16 operation) {
	bool retval = false;
	if (operation <= 7) {
		switch (operation) {
		case 0:
			if (op1 == op2) {
				retval = true;
			}
			break;
		case 1:
			if (op1 != op2) {
				retval = true;
			}
			break;
		case 2:
			if (op1 > op2) {
				retval = true;
			}
			break;
		case 3:
			if (op1 >= op2) {
				retval = true;
			}
			break;
		case 4:
			if (op1 < op2) {
				retval = true;
			}
			break;
		case 5:
			if (op1 <= op2) {
				retval = true;
			}
			break;
		case 6:
			if (op1 & op2) {
				retval = true;
			}
			break;
		case 7:
			if (op1 | op2) {
				retval = true;
			}
			break;
		}
	}
	return retval;
}
void ScriptsManager::setWait(uint8 w) {
	_currScript->flags |= w;
	//	_mustReturn = true;
}
bool ScriptsManager::setActorDestionation(bool isYcoord) {
	uint8 val = readByteParameter(_currScript->current);
	Actor *actor = _vm->_gmMgr->getActor(_currScript->actorIdx);
	actor->animationType = 0;
	int16 x = actor->pivotX;
	int16 y = actor->pivotY;
	if (!isYcoord) {
		x = val * 2;
	} else {
		y = val;
	}
	if (_vm->_gmMgr->setActorDestination(_currScript->actorIdx, x, y)) {
		if (!isYcoord) {
			actor->whereToWalk |= 0x08;
		} else {
			actor->whereToWalk |= 0x10;
		}
		_currScript->flags |= ScriptFlags::WAIT_DESTINATION;
		return true;
	}
	return false;
}
bool ScriptsManager::mainActorIntersectsRectangle() {
	Actor *actor = _vm->_gmMgr->getActor(0);
	Rectangle r1;
	Rectangle r2;
	r1.left = _currScript->left = readXCoordParameter(_currScript->current);
	r1.top = _currScript->top = readByteParameter(_currScript->current);
	r1.right = _currScript->right = readXCoordParameter(_currScript->current);
	r1.bottom = _currScript->bottom = readByteParameter(_currScript->current);

	r2.left = actor->pivotX - actor->halfWidth;
	r2.top = actor->pivotY - actor->height;
	r2.right = actor->pivotX + actor->halfWidth;
	r2.bottom = actor->pivotY;

	return _vm->_gmMgr->rectanglesIntersect(r1, r2);
}
bool ScriptsManager::mainActorIntersectsRectangle(int16 left, int16 top, int16 right, int16 bottom) {
	Actor *actor = _vm->_gmMgr->getActor(0);
	Rectangle r1;
	Rectangle r2;
	r1.left = left;
	r1.top = top;
	r1.right = right;
	r1.bottom = bottom;

	r2.left = actor->pivotX - actor->halfWidth;
	r2.top = actor->pivotY - actor->height;
	r2.right = actor->pivotX + actor->halfWidth;
	r2.bottom = actor->pivotY;

	return _vm->_gmMgr->rectanglesIntersect(r1, r2);
}
bool ScriptsManager::setActorDestinationMainActor(bool isYOffset) {
	int16 x = 0, y = 0;
	int8 offs = readByteParameter(_currScript->current);
	Actor *actor = _vm->_gmMgr->getActor(_currScript->actorIdx);
	Actor *main = _vm->_gmMgr->getActor(0);
	actor->animationType = 0;
	if (!isYOffset) {
		x = main->pivotX + offs;
		y = actor->pivotY;
	} else {
		x = actor->pivotX;
		y = main->pivotY + offs;
	}
	if (_vm->_gmMgr->setActorDestination(_currScript->actorIdx, x, y)) {
		if (!isYOffset) {
			actor->whereToWalk |= 8;
		} else {
			actor->whereToWalk |= 0x10;
		}
		_currScript->flags |= ScriptFlags::WAIT_DESTINATION;
		return true;
	}
	return false;
}
RoomScript *ScriptsManager::getScript(uint16 idx) {
	return &_vm->_gameState.scripts[idx];
}

//
const ScriptsManager::opfunc ScriptsManager::shadowTable[] = {
	// 0x00
	nullptr,
	&ScriptsManager::op_setActorFacingDir,
	&ScriptsManager::op_endScript,
	&ScriptsManager::op_jmp,
	&ScriptsManager::op_setActorDestX,
	&ScriptsManager::op_setActorDestY,
	&ScriptsManager::op_loop,
	&ScriptsManager::op_setActorPos,
	&ScriptsManager::op_setSyncTo,
	&ScriptsManager::op_setWait,
	&ScriptsManager::op_if,
	nullptr,
	nullptr,
	nullptr,
	&ScriptsManager::op_ifActorInArea,
	&ScriptsManager::op_setActorDestMainActorX,
	// 0x10
	&ScriptsManager::op_setActorDestMainActorY,
	&ScriptsManager::op_moveActorTowardMainActor,
	&ScriptsManager::op_lockMainActor,
	&ScriptsManager::op_unlockMainActor,
	&ScriptsManager::op_rotateActorTowardsMainActor,
	&ScriptsManager::op_setCurrentActor,
	&ScriptsManager::op_loadWalkAreaTopSegments,
	&ScriptsManager::op_setSceneExits,
	nullptr,
	&ScriptsManager::op_addHotspot_type0,
	&ScriptsManager::op_setEscPressed,
	&ScriptsManager::op_startScript,
	&ScriptsManager::op_stopScript,
	&ScriptsManager::op_restartScript,
	&ScriptsManager::op_playCutscene,
	&ScriptsManager::op_setGameVar,
	// 0x20
	&ScriptsManager::op_addToGameVar,
	&ScriptsManager::op_subFromGameVar,
	&ScriptsManager::op_disableActorCollisions,
	&ScriptsManager::op_enableActorCollisions,
	&ScriptsManager::op_setActorDestinationXY,
	&ScriptsManager::op_setStageColorsReplace,
	&ScriptsManager::op_fadePalette,
	&ScriptsManager::op_waitForMainActorToLeaveArea,
	&ScriptsManager::op_waitForMainActorToEnterArea,
	&ScriptsManager::op_freeActor,
	&ScriptsManager::op_setActorViewportX,
	&ScriptsManager::op_setActorViewportY,
	&ScriptsManager::op_setNewRoom,
	&ScriptsManager::op_setActorAnimationFreezeFrame,
	&ScriptsManager::op_activateSecondaryVoices,
	&ScriptsManager::op_setResourceType,
	// 0x30
	&ScriptsManager::op_pivotActorToMainActor,
	nullptr,
	&ScriptsManager::op_drawRandomEffect,
	&ScriptsManager::op_setNewZoomLevel,
	&ScriptsManager::op_zoomOnActor,
	&ScriptsManager::op_multiChoice,
	&ScriptsManager::op_actorSaySentence,
	&ScriptsManager::op_setClearScreen,
	&ScriptsManager::op_orVar,
	&ScriptsManager::op_andVar,
	&ScriptsManager::op_loadScene,
	&ScriptsManager::op_setWalkVerticalAdjustment,
	&ScriptsManager::op_addObstacle,
	&ScriptsManager::op_ifTalk,
	&ScriptsManager::op_isActorInTalkRange,
	&ScriptsManager::op_ifTalkInArea,
	// 0x40
	&ScriptsManager::op_ifLook,
	&ScriptsManager::op_ifLookActorsInRange,
	&ScriptsManager::op_ifLookInArea,
	&ScriptsManager::op_addLightning,
	&ScriptsManager::op_removeObstacle,
	&ScriptsManager::op_setActorSpeechColor,
	&ScriptsManager::op_setActorSpeechPivot,
	&ScriptsManager::op_nop,
	&ScriptsManager::op_resetScriptCounterAndJmp,
	&ScriptsManager::op_activateRandomStars,
	&ScriptsManager::op_setActorIsKillable,
	nullptr,
	&ScriptsManager::op_setClearScreenReturn,
	&ScriptsManager::op_startSong,
	&ScriptsManager::op_setMusicVolume,
	&ScriptsManager::op_generateRandomNumber,
	// 0x50
	&ScriptsManager::op_changePak,
	&ScriptsManager::op_readArea,
	&ScriptsManager::op_actorSpeakWithAnimation,
	&ScriptsManager::op_setForceLoading,
	&ScriptsManager::op_addHotspotType1,
	&ScriptsManager::op_nop,
	&ScriptsManager::op_seedRandomizerWhileIdle,
	&ScriptsManager::op_setActorAnimationAndWait,
	&ScriptsManager::op_setActorAnimation,
	&ScriptsManager::op_actorSpeakWithPortrait,
	&ScriptsManager::op_resetCurrentActor,
	&ScriptsManager::op_changeActorResouce,
	&ScriptsManager::op_setActorVisibility,
	&ScriptsManager::op_fadeIn,
	&ScriptsManager::op_fadeOut,
	&ScriptsManager::op_fadePaletteBlock,
	// 0x60
	&ScriptsManager::op_setTextChapter,
	&ScriptsManager::op_ifSpecificActorInRange,
	&ScriptsManager::op_disableHotspot,
	&ScriptsManager::op_playDigiSample,
	&ScriptsManager::op_playDigiSampleLooped,
	&ScriptsManager::op_setPaletteTint,
	&ScriptsManager::op_changePaletteBrightness,
	&ScriptsManager::op_deactivateRandomDraw,
	&ScriptsManager::op_activateRandomDraw,
	&ScriptsManager::op_setDrawWithEffect,
	&ScriptsManager::op_drawRandomTheEnd,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};

} // namespace Cometengine
