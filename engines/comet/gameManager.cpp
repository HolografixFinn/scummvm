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

#include "comet/gameManager.h"
#include "comet/comet.h"
#include "common/events.h"
#include "common/savefile.h"
#include "common\serializer.h"
#include "engines/advancedDetector.h"

#define _COMET_CTRL 0x04
#define _COMET_ALT 0x01
#define _COMET_SHIFT 0x02
#define _COMET_FLAG_UP 0x01
#define _COMET_FLAG_DOWN 0x02
#define _COMET_FLAG_LEFT 0x04
#define _COMET_FLAG_RIGHT 0x08
#define _COMET_FLAG_CLICKLEFT 0x01
#define _COMET_FLAG_CLICKRIGHT 0x02

namespace Cometengine {
GameManager::GameManager(CometEngine *vm) : _vm(vm), handleQuit(nullptr),
											_movementStatus(0), _movementStatus2(0), _lastPressedKey(Common::KeyCode::KEYCODE_INVALID), _mouseButtons(0), _specialKeys(0),
											_evtMgr(vm->_system->getEventManager()), _keysFlags(),
											_resPakData(nullptr), _fontData(nullptr),
											_rnd("comet"),
											_mainActor(0), _NPCActors(0),
											_topWalkAreaLineStrip(),
											_isRandomPixelsAndLinesActive(false), //_walkHorizAdjustment(0),
																				  //_walkVertAdjustment(0),
																				  //_walkVertAdjustment_HorizMovement(0),
																				  // _paletteFadeLevel(255),
											_isGet(false), _isLook(false), _isUse(false), _isTalk(false),
											_animsFilename("A00.PAK"), _stagesFilename("D00.PAK"), _roomsFilename("R00.CC4"),
											_startPakNumber(0), _startRoom(0), _waitingForEnter(false),
											_isLoadingSavedGame(false),
											_roomZoomLevel(0), _hotspotInSight(false), _hotspotDirection(0), _hotspotX(0), _hotspotY(0),
											_statusVars(), //_objectsFlags(),
														   //_gameFlags(),
														   //_unk3(0),
											_randomNumber(0),
											_portratiAnimationCounter(0), _portraitRandomAnimation(0), _intersectingElementRectangle(),
											_lastTakenObjId(0), //_numObjsTaken(0),
											_currentlyHandledKey(Common::KeyCode::KEYCODE_INVALID), _currentlyHandledMovementFlags(0), _isConfirmKeyFlag(0), _isConfirmKeyOrEnter(false),
											_moveFlagsToFacingDir(), _facingDirChangeMatrix(), _requestedFacingDir(0),
											_numMenusOnScreen(0),
											_lastDiskMenuSelectedItem(0), _lastSettingsMenuSelectedItem(0),
											//copy protection stuff
											_copyProtectionTitleSentence(nullptr), _copyProtectionInstructionsSentence(nullptr), _copyProtectionsConstellationsData(), _copyProtectionStarsGraphics(nullptr),
											_copyProtectionRightConstellationRow(0), _copyProtectionRightConstellationColumn(0), _copyProtectionDisplayedConstellationsData(),
											_copyProtectionNumColumns(11), _copyProtectionNumRows(16),
											_zoomRestored(false), skullTilesState(), skullCursorX(0), skullCursorY(0), skullPuzzleData(nullptr), forceLoading(0), defaultLanguage(0) {
	if (_vm->isComet()) {
		if (_vm->isCD()) {
			_startPakNumber = 9; //5
			_startRoom = 0;	//1
			handleQuit = &GameManager::handleQuit_CD;
		} else {
			_startPakNumber = 5;
			_startRoom = 0;
			handleQuit = &GameManager::handleQuit_floppy;
		}
	}
	_mainActor = &_vm->_gameState.actors[0];
	_NPCActors = &_vm->_gameState.actors[1];

	_topWalkAreaLineStrip.numPoints = 2;
	_topWalkAreaLineStrip.points[0].x = 0;
	_topWalkAreaLineStrip.points[0].y = 100;
	_topWalkAreaLineStrip.points[1].x = 319;
	_topWalkAreaLineStrip.points[1].y = 100;

	_oppositeDirections[0] = 0;
	_oppositeDirections[kUp] = kDown;
	_oppositeDirections[kRight] = kLeft;
	_oppositeDirections[kDown] = kUp;
	_oppositeDirections[kLeft] = kRight;

	_statusVars[0] = (int16 *)&_vm->_gameState.prevRoomNum;
	for (uint8 i = 0; i < 10; i++) {
		_statusVars[1 + (i * 3)] = (int16 *)&_vm->_gameState.actors[i].life;
		_statusVars[1 + (i * 3) + 1] = (int16 *)&_vm->_gameState.actors[i].pivotX;
		_statusVars[1 + (i * 3) + 2] = (int16 *)&_vm->_gameState.actors[i].pivotY;
	}
	_statusVars[31] = (int16 *)&this->_currentlyHandledMovementFlags; //this->_lastMovementStatus;
	_statusVars[32] = (int16 *)&this->_isConfirmKeyOrEnter;
	_statusVars[33] = (int16 *)&this->_randomNumber;
	_statusVars[34] = (int16 *)&_vm->_gameState.prevPakNum;

	// fill all possible combinations, other impossible combinations (i.e. up+down) are left with 0 value
	_moveFlagsToFacingDir[kUpFlag] = kUp;
	_moveFlagsToFacingDir[kDownFlag] = kDown;
	_moveFlagsToFacingDir[kLeftFlag] = kLeft;
	_moveFlagsToFacingDir[kLeftFlag | kUpFlag] = kLeft;
	_moveFlagsToFacingDir[kLeftFlag | kDownFlag] = kLeft;
	_moveFlagsToFacingDir[kRightFlag] = kRight;
	_moveFlagsToFacingDir[kRightFlag | kUpFlag] = kRight;
	_moveFlagsToFacingDir[kRightFlag | kDownFlag] = kRight;

	// this table tells, starting from a certain facing direction and applying movement input, which is the resulting facing direction
	// i.e. we are currently facing up, and input key is DOWN (requesting a facing down direction), which direction will we end up facing?
	// first index is the current facing direction, second index is the facing direction requeste by input command
	_facingDirChangeMatrix[kNoDir][kNoDir] = kNoDir;
	_facingDirChangeMatrix[kNoDir][kUp] = kNoDir;
	_facingDirChangeMatrix[kNoDir][kRight] = kNoDir;
	_facingDirChangeMatrix[kNoDir][kDown] = kNoDir;
	_facingDirChangeMatrix[kNoDir][kLeft] = kNoDir;

	_facingDirChangeMatrix[kUp][kNoDir] = kNoDir;
	_facingDirChangeMatrix[kUp][kUp] = kUp;
	_facingDirChangeMatrix[kUp][kRight] = kRight;
	_facingDirChangeMatrix[kUp][kDown] = kRight;
	_facingDirChangeMatrix[kUp][kLeft] = kLeft;

	_facingDirChangeMatrix[kRight][kNoDir] = kNoDir;
	_facingDirChangeMatrix[kRight][kUp] = kUp;
	_facingDirChangeMatrix[kRight][kRight] = kRight;
	_facingDirChangeMatrix[kRight][kDown] = kDown;
	_facingDirChangeMatrix[kRight][kLeft] = kDown;

	_facingDirChangeMatrix[kDown][kNoDir] = kNoDir;
	_facingDirChangeMatrix[kDown][kUp] = kLeft;
	_facingDirChangeMatrix[kDown][kRight] = kRight;
	_facingDirChangeMatrix[kDown][kDown] = kDown;
	_facingDirChangeMatrix[kDown][kLeft] = kLeft;

	_facingDirChangeMatrix[kLeft][kNoDir] = kNoDir;
	_facingDirChangeMatrix[kLeft][kUp] = kUp;
	_facingDirChangeMatrix[kLeft][kRight] = kDown;
	_facingDirChangeMatrix[kLeft][kDown] = kDown;
	_facingDirChangeMatrix[kLeft][kLeft] = kLeft;

	memset(_saveFilesDescription, 0, 10 * 30);
}
GameManager::~GameManager() {
	if (_resPakData != nullptr) {
		delete[] _resPakData;
		_resPakData = nullptr;
	}
	if (_tempBuffer != nullptr) {
		delete[] _tempBuffer;
		_tempBuffer = nullptr;
	}
}
void GameManager::init() {
	this->resetDefaultKeyFlags();
	this->_tempBuffer = new char[8000];
}
void GameManager::resetDefaultKeyFlags() {
	//this initialization is just horrible, but right now I don't find any other way that works for c++98 and custom Array type
	_keysFlags.resize(29);
	_keysFlags.empty();
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_RCTRL, kSpecialKey | kVolatileKey | kCtrl));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_LCTRL, kSpecialKey | kVolatileKey | kCtrl));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_TILDE, kMoveKey | kVolatileKey | kUpFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_LSHIFT, kSpecialKey | kVolatileKey | kShift));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_RSHIFT, kSpecialKey | kVolatileKey | kShift));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_BACKSLASH, kMoveKey | kVolatileKey | kLeftFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_RALT, kSpecialKey | kVolatileKey | kAlt));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_LALT, kSpecialKey | kVolatileKey | kAlt));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_SPACE, kMoveKey | kVolatileKey | kConfirm | (kClickLeft << 4)));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_KP7, kMoveKey | kVolatileKey | kLeftFlag | kUpFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_HOME, kMoveKey | kVolatileKey | kLeftFlag | kUpFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_KP8, kMoveKey | kVolatileKey | kUpFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_UP, kMoveKey | kVolatileKey | kUpFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_KP9, kMoveKey | kVolatileKey | kRightFlag | kUpFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_PAGEUP, kMoveKey | kVolatileKey | kRightFlag | kUpFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_KP_MINUS, kMoveKey | kVolatileKey | kDownFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_DOWN, kMoveKey | kVolatileKey | kDownFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_KP2, kMoveKey | kVolatileKey | kDownFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_KP4, kMoveKey | kVolatileKey | kLeftFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_LEFT, kMoveKey | kVolatileKey | kLeftFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_KP6, kMoveKey | kVolatileKey | kRightFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_RIGHT, kMoveKey | kVolatileKey | kRightFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_KP_PLUS, kMoveKey | kVolatileKey | kRightFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_KP1, kMoveKey | kVolatileKey | kDownFlag | kLeftFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_END, kMoveKey | kVolatileKey | kDownFlag | kLeftFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_KP3, kMoveKey | kVolatileKey | kDownFlag | kRightFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_PAGEDOWN, kMoveKey | kVolatileKey | kDownFlag | kRightFlag));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_INSERT, kClickKey | kClickLeft));
	_keysFlags.push_back(keySpecialFunc(Common::KEYCODE_DELETE, kClickKey | kClickRight));
	//joystick keycodes ignored now
}
bool GameManager::updateInputStatus() {
	Common::Event evt;
	while (_evtMgr->pollEvent(evt)) {
		if (_evtMgr->shouldQuit()) {
			_vm->setScummVMQuit();
			return false;
		}
		switch (evt.type) {
		case Common::EVENT_QUIT:
		case Common::EVENT_RETURN_TO_LAUNCHER:
			//			case Common::EVENT_RTL:
			return false;
			break;
		case Common::EVENT_KEYDOWN:
		case Common::EVENT_KEYUP:
			//conversion for QZERTY keyboard? maybe not needed
			{
				bool pressed = true;
				if (evt.type == Common::EVENT_KEYUP) {
					pressed = false;
				}
				uint32 flags = this->getKeySpecialFunction(evt.kbd.keycode);
				uint32 operation = (flags >> 13) & 0x07;
				uint32 isVolatile = flags & kVolatileKey;
				uint32 param = flags & 0x0fff;
				switch (operation) {
				case 1:
					this->handleSpecialKey(param, pressed);
					break;
				case 2:
					this->handleMovementKey(param, pressed);
					break;
				case 4:
					this->handleClickKey(param, pressed);
				}
				if (!isVolatile) {
					if (pressed) {
						this->_lastPressedKey = evt.kbd.keycode;
					} else {
						this->_lastPressedKey = Common::KEYCODE_INVALID;
					}
				}
			}
			break;
		default:
			break;
		}
	}
	if (_vm->isCD()) {
		_vm->_moMgr->updateMouseStatus();
		_mouseButtons = _vm->_moMgr->getButStatus();
	}
	return true;
}
void GameManager::handleSpecialKey(uint32 idx, bool pressed) {
	if (pressed) {
		this->_specialKeys |= idx;
	} else {
		this->_specialKeys &= ~idx;
	}
}
void GameManager::handleClickKey(uint32 idx, bool pressed) {
	if (pressed) {
		this->_mouseButtons |= idx;
	} else {
		this->_mouseButtons &= ~idx;
	}
}
void GameManager::handleMovementKey(uint32 idx, bool pressed, uint32 type) {
	uint16 &mov = this->_movementStatus;
	if (type != 0) {
		mov = this->_movementStatus2;
	}
	idx = idx & 0x0fff;
	uint32 butt = (idx >> 4) & 0x03;
	idx = idx & 0x008f;
	if (pressed) {
		mov |= idx;
		_mouseButtons |= butt;
	} else {
		mov &= ~idx;
		_mouseButtons &= ~butt;
	}
}
uint32 GameManager::getKeySpecialFunction(Common::KeyCode code) {
	//this would be more efficient if _keyFlags was a std::map or something like that, I'll look into it
	for (Common::Array<keySpecialFunc>::iterator i = _keysFlags.begin(); i != _keysFlags.end(); i++) {
		if ((*i)._code == code) {
			return (*i)._flags;
		}
	}
	return 0;
}
void GameManager::loadResPak() {
	if (!_vm->isCD()) {
		_resPakData = _vm->_archMgr->allocateAndGetFile("RES.PAK", 0);
		_fontData = _resPakData + READ_LE_UINT32(_resPakData);

		this->_vm->_gMgr->setBasicResources(_resPakData + READ_LE_UINT32(_resPakData + 4), _resPakData + READ_LE_UINT32(_resPakData + 8), _resPakData + READ_LE_UINT32(_resPakData + 12), _resPakData + READ_LE_UINT32(_resPakData + 16));
		this->_vm->_gMgr->initializePalette(_resPakData + READ_LE_UINT32(_resPakData + 20), _resPakData + READ_LE_UINT32(_resPakData + 24));
	} else {
		_fontData = _vm->_archMgr->allocateAndGetFile("RES.PAK", 0);
		this->_vm->_gMgr->setBasicResources(
			_vm->_archMgr->allocateAndGetFile("RES.PAK", 1), //speech box
			_vm->_archMgr->allocateAndGetFile("RES.PAK", 2), //parker walk
			_vm->_archMgr->allocateAndGetFile("RES.PAK", 3), //icons
			_vm->_archMgr->allocateAndGetFile("RES.PAK", 4)  //objects
		);
		_vm->_moMgr->loadCursorsGraphics(_vm->_archMgr->allocateAndGetFile("RES.PAK", 9), 7);
		updateFontDataAndColor(0);
		this->_vm->_gMgr->initializePalette(
			_vm->_archMgr->allocateAndGetFile("RES.PAK", 5), //main palette
			_vm->_archMgr->allocateAndGetFile("RES.PAK", 6), //flashback palette
			_vm->_archMgr->allocateAndGetFile("RES.PAK", 7), //cd intro palette
			_vm->_archMgr->allocateAndGetFile("RES.PAK", 8)  //another palette
		);
//		_vm->_moMgr->setMouseCursor(5);
//		_vm->_moMgr->setMouseVisibility(true);
	}
	initAllResources();
	setPakNumber(_startPakNumber, _startRoom);
	_vm->_txtMgr->loadSystemAndObjects();


}
void GameManager::initAllResources() {
	//ok
	this->_vm->_audioMgr->initResources();
	// the following is how the original code seeded the RNG
	TimeDate t;
	this->_vm->_system->getTimeAndDate(t);
	for (int32 i = 0; i < (t.tm_hour + t.tm_min + t.tm_sec); i++) {
		_rnd.getRandomNumber(32767);
	}
	this->_vm->_scMgr->initResources();
	this->_vm->_txtMgr->initResources();
	this->_vm->_txtMgr->loadSystemAndObjects();

	this->_vm->_gMgr->initGameObjectsAndFlags();
	updateFontDataAndColor(0x13);
	unlockMainActor();
	_vm->_gameState.currPakNum = 0;
	_vm->_gameState.newPakNum = 0;
	_vm->_gameState.currRoomNum = -1;
	loadWalkArea();
	resetHotspotsAndOtherValues();
	killAllNPCs();

	createActor(0, getResourceIdx(1, 0));
	setActorPosition(0, 0xbe, 0xa0);
	_vm->_gameState.actors[0].life = 99;
}
void GameManager::updateFontDataAndColor(uint8 color) {
	//ok
	this->_vm->_txtMgr->setFontDataAndColor(this->_fontData, color);
}
void GameManager::unlockMainActor() {
	_vm->_gameState.movementMask = 0;
	if (_mainActor->animationType == 2) {
		_mainActor->animationType = 0;
	}
}
void GameManager::calcTopWalkArea(WalkAreaLineStrip strip, uint8 *values) {
	uint16 x1, y1, x2, y2;
	int16 xDiff, yDiff, step;
	uint8 val = 0;
	uint16 accumulator;
	uint16 valuesIdx = 0;
	for (uint16 i = 0; i < strip.numPoints - 1; i++) {
		x1 = strip.points[i].x;
		y1 = strip.points[i].y;
		x2 = strip.points[i + 1].x;
		y2 = strip.points[i + 1].y;
		val = x1;
		xDiff = x2 - x1;
		if (xDiff != 0) {
			yDiff = y2 - y1;
			val = y1;
			step = 1;
			if (yDiff < 0) {
				yDiff = -yDiff;
				step = -step;
			}
			if (xDiff > yDiff) {
				accumulator = xDiff / 2;
				for (uint16 j = 0; j < xDiff; j++) {
					values[valuesIdx++] = val;
					accumulator += yDiff;
					if (accumulator >= xDiff) {
						accumulator -= xDiff;
						val += step;
					}
				}
			} else {
				accumulator = yDiff / 2;
				for (uint16 j = 0; j < yDiff; j++) {
					val += step;
					accumulator += xDiff;
					if (accumulator >= yDiff) {
						accumulator -= yDiff;
						values[valuesIdx++] = val;
					}
				}
			}
		}
	}
	values[valuesIdx++] = val;
}
void GameManager::loadWalkArea() {
	_vm->_gameState.currTopWalkAreaNumPoints = _topWalkAreaLineStrip.numPoints;
	calcTopWalkArea(_topWalkAreaLineStrip, &_vm->_gameState.topWalkAreaYValues[0]);
}
void GameManager::deactivateRandomPixelsAndLines() {
	_isRandomPixelsAndLinesActive = false;
}
void GameManager::resetHotspotsAndOtherValues() {
	deactivateRandomPixelsAndLines();
	_vm->_gameState.paletteFadeLevel = 255;
	_vm->_gameState.walkVerticalAdjustment = 0;
	_isGet = false;
	_isLook = false;
	_isTalk = false;
	_isUse = false;
	_vm->_gameState.numExits = 0;
	_vm->_gameState.walkHorizAdjustment = 0;
	_vm->_gameState.walkVertAdjustment_HorizMovement = 0;
	_vm->_gameState.movementMask = 0;
	for (uint8 i = 0; i < 20; i++) {
		_vm->_gameState.hotspots[i].isActive = false;
	}
}
void GameManager::killAllNPCs() {
	for (uint8 i = 1; i < 11; i++) {
		_vm->_gameState.actors[i].life = 0;
	}
}
void GameManager::showLoopedAnimation(uint8 subfile, uint8 anim, int16 frame_flags, uint8 numLoops, uint8 numDigiFX, const char *cues) {
	uint8 *subfileData = nullptr;
	uint8 *paletteData = nullptr;
	uint8 *animData = nullptr;
	uint8 hasPalette = 0;
	uint8 sampleIdx = 0;
	uint8 nextCue = 0;
	uint8 lastMerge = 0;
	uint8 currFrame = 0;
	const char *cuesStart;
	uint8 startNumDigi = numDigiFX;
	_vm->_gMgr->resetFullscreenViewport();
	subfileData = _vm->_gmMgr->priorityAllocateAndGetFile(_animsFilename, subfile);
	if (READ_LE_UINT16(subfileData + 2) == 0 && READ_LE_UINT16(subfileData) == 20) {
		hasPalette = 1;
		paletteData = _vm->_gMgr->getTypeData(4, subfileData);
	}
	animData = _vm->_gMgr->getAnimData(anim, subfileData);
	uint16 numFrames = animData[1];
	animData += 2;
	if (frame_flags == 0) {
		_vm->_gMgr->clearBackbuffer();
	} else if (frame_flags < 0) {
		_vm->_gMgr->decodeFrame(-frame_flags, 0, 0, subfileData);
	} else if (frame_flags < 0x7d00) {
		_vm->_gMgr->decodeFrame(frame_flags, 0, 0, _vm->_gMgr->_stageAnims);
	} else if (frame_flags == 0x7d00) {
		_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->lockMainSurface(), _vm->_gMgr->_videoBackbuffer);
		_vm->_gMgr->unlockMainSurface();
	}
	_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->_videoBackbuffer, _vm->_gMgr->_backgroundBuffer);
	cuesStart = cues;
	if (numDigiFX != 0) {
		_vm->_audioMgr->stopDigi();
		sampleIdx = *cues++;
		_vm->_audioMgr->loadDigiSubfile(sampleIdx);
	}
	for (uint8 i = 0; i < numLoops; i++) {
		if (_vm->isCD()) {
			_vm->_gMgr->callCapFPS();
		}
		numDigiFX = startNumDigi;
		if (numDigiFX != 0) {
			cues = cuesStart;
			cues++;
			nextCue = *cues++;
		}
		lastMerge = 0;
		for (currFrame = 0; currFrame < numFrames; currFrame++) {
			_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->_backgroundBuffer, _vm->_gMgr->_videoBackbuffer);
			lastMerge = _vm->_gMgr->drawAnimationFrame(subfileData, animData, currFrame, lastMerge, 0, 0, numFrames);
			if (_vm->isCD()) {
				_vm->_gMgr->callCapFPS();
			}
			_vm->_txtMgr->handleOnScreenText();
			if (hasPalette == 1) {
				hasPalette = 2;
				_vm->_gMgr->uploadPalette(paletteData);
			}
			_vm->_gMgr->paintBackbuffer_mouse();
			if (numDigiFX != 0 && currFrame == nextCue) {
				sampleIdx = *cues;
				cues += 2;
				_vm->_audioMgr->playDigiCurrent(1);
				numDigiFX--;
				if (numDigiFX != 0) {
					nextCue = *cues++;
				}
			}
			handlePause();
			if (lastMerge != 0) {
				currFrame--;
			}
			updateInputStatus();
			if (_vm->isQuitRequested()) {
				break;
			}
			if (_lastPressedKey == Common::KeyCode::KEYCODE_RETURN || _lastPressedKey == Common::KeyCode::KEYCODE_KP_ENTER) {
				currFrame = numFrames;
				i = numLoops;
				if (_vm->_txtMgr->isTextDisplayed()) {
					_vm->_txtMgr->hideDialogue();
				}
			}
			if (_lastPressedKey == Common::KeyCode::KEYCODE_ESCAPE) {
				(this->*handleQuit)();
				if (_vm->isQuitRequested()) {
					break;
				}
			}
		}
		if (_vm->isQuitRequested()) {
			break;
		}
	}
	delete[] subfileData;
	if (hasPalette != 0) {
		_vm->_gMgr->uploadPalette(_vm->_gMgr->tmpPalette);
	}
	if (_vm->_txtMgr->isTextDisplayed()) {
		_vm->_txtMgr->hideDialogue();
	}
	_vm->_gMgr->loadStageBackground();
	_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->_backgroundBuffer, _vm->_gMgr->_videoBackbuffer);
}

uint8 GameManager::getResourceIdx(uint8 type, uint8 fileIdx) {
	int8 resourceIdx;
	resourceIdx = findResource(type, fileIdx);
	if (resourceIdx == -1) {
		resourceIdx = findFreeResourceIdx();
		if (resourceIdx == -1) {
			clearUnusedResources();
			resourceIdx = findFreeResourceIdx();
		}
	}
	_vm->_gameState.resources[resourceIdx].type = type;
	_vm->_gameState.resources[resourceIdx].fileIdx = fileIdx;
	if (type != 0) {
		_vm->_gameState.resources[resourceIdx].data = _vm->_gMgr->getResourceData(type);
	} else {
		if (_vm->_gameState.resources[resourceIdx].data == nullptr) {
			_vm->_gameState.resources[resourceIdx].data = priorityAllocateAndGetFile(_animsFilename, fileIdx);
		}
	}
	return resourceIdx;
}
uint8 *GameManager::priorityAllocateAndGetFile(const char *archiveName, uint16 fileNum) {
	uint8 *data = 0;
	data = _vm->_archMgr->allocateAndGetFile(archiveName, fileNum);
	if (data == 0) {
		clearUnusedResources();
		data = _vm->_archMgr->allocateAndGetFile(archiveName, fileNum);
		if (data == 0) {
			freeUnusedAnimResources();
			data = _vm->_archMgr->allocateAndGetFile(archiveName, fileNum);
			if (data == 0) {
			}
		}
	}
	return data;
}
void GameManager::freeUnusedAnimResources() {
	for (uint8 i = 0; i < 20; i++) {
		if (_vm->_gameState.resources[i].type == 0 && _vm->_gameState.resources[i].data != 0) {
			delete[] _vm->_gameState.resources[i].data;
		}
	}
	reloadResources();
}
void GameManager::reloadResources() {
	for (uint8 i = 0; i < 20; i++) {
		if (_vm->_gameState.resources[i].type == 0 && _vm->_gameState.resources[i].data != nullptr) {
			//BUG: the original game does not check if the subfile is valid.
			if (_vm->_archMgr->checkValidSubfile(_animsFilename, _vm->_gameState.resources[i].fileIdx)) {
				_vm->_gameState.resources[i].data = priorityAllocateAndGetFile(_animsFilename, _vm->_gameState.resources[i].fileIdx);
			} else {
				_vm->_gameState.resources[i].data = nullptr;
				_vm->_gameState.resources[i].type = 0;
				_vm->_gameState.resources[i].fileIdx = 0;
			}
		} else {
			_vm->_gameState.resources[i].data = _vm->_gMgr->getResourceData(_vm->_gameState.resources[i].type);
		}
	}
}
int8 GameManager::findResource(uint8 type, uint8 fileIdx) {
	for (uint8 i = 0; i < 20; i++) {
		if (_vm->_gameState.resources[i].type == type && _vm->_gameState.resources[i].fileIdx == fileIdx) {
			return i;
		}
	}
	return -1;
}
int8 GameManager::findFreeResourceIdx() {
	for (uint8 i = 0; i < 20; i++) {
		if (_vm->_gameState.resources[i].data == nullptr) {
			return i;
		}
	}
	return -1;
}
void GameManager::clearUnusedResources() {
	for (uint8 i = 0; i < 20; i++) {
		if (_vm->_gameState.resources[i].data != nullptr && _vm->_gameState.resources[i].type == 0 && (!isResourceUsedByActor(i))) {
			removeResourceFromActors(i);
			delete[] _vm->_gameState.resources[i].data;
			_vm->_gameState.resources[i].data = nullptr;
		}
	}
}
bool GameManager::isResourceUsedByActor(uint8 resourceIdx) {
	for (uint8 i = 0; i < 11; i++) {
		if (_vm->_gameState.actors[i].life != 0 && _vm->_gameState.actors[i].resourceIdx == resourceIdx) {
			return true;
		}
	}
	return false;
}
void GameManager::removeResourceFromActors(uint8 resourceIdx) {
	for (uint8 i = 1; i < 11; i++) {
		if (_vm->_gameState.actors[i].resourceIdx == resourceIdx) {
			_vm->_gameState.actors[i].resourceIdx = -1;
			_vm->_gameState.actors[i].life = 0;
		}
	}
}

void GameManager::createActor(uint8 actorIdx, int8 resourceIdx) {
	Actor *_actor = &_vm->_gameState.actors[actorIdx];
	memset(_actor, 0, sizeof(Actor));
	_actor->offsetWalkAnimation = 4;
	_actor->facingDirection = 1;
	setActorOffsetWalk(_actor, 0);
	actorPrepareAnimation(_actor, 0);
	_actor->resourceIdx = resourceIdx;
	_actor->halfWidth = 4;
	_actor->height = 2;
	_actor->isKillable = false;
	_actor->drawAreaMaxX = 319;
	_actor->drawAreaMaxY = 199;
	_actor->isVisible = true;
	_actor->textPivotX = -1;
	_actor->textPivotY = -1;
	_actor->fixedAnimationFrame = -1;
	if (actorIdx == 0) {
		_actor->speechColor = 0x15;
	} else {
		_actor->life = 10;
	}
}
void GameManager::setActorOffsetWalk(Actor *actor, uint8 offs) {
	if (actor->offsetWalkAnimation != offs && actor->animationType != 2) {
		actor->offsetWalkAnimation = offs;
		actor->animationType = 1;
	}
}
void GameManager::actorPrepareAnimation(Actor *actor, uint8 animationIdx) {
	if (actor->resourceIdx == -1) {
		actor->numFrames = 0;
	} else {
		uint8 *animationData = _vm->_gMgr->getAnimData(animationIdx, _vm->_gameState.resources[actor->resourceIdx].data);
		actor->numFrames = *(animationData + 1);
	}
	actor->frameToDraw = 0;
	actor->animationIdx = animationIdx;
	actor->fixedAnimationFrame = -1;
}
void GameManager::setActorPosition(uint8 actorIdx, uint16 x, uint16 y) {
	_vm->_gameState.actors[actorIdx].pivotX = x;
	_vm->_gameState.actors[actorIdx].pivotY = y;
	_vm->_gameState.actors[actorIdx].lastIntersectionType = 0;
	_vm->_gameState.actors[actorIdx].whereToWalk = 0;
}
void GameManager::setPakNumber(uint8 pak, uint8 room) {
	_vm->_gameState.newPakNum = pak;
	_vm->_gameState.newRoomNum = room;
	setFileNames(pak);
}
void GameManager::setFileNames(uint8 pakNum) {
	char units = '0' + (pakNum % 10);
	char tens = '0' + (pakNum / 10);
	_animsFilename[1] = tens;
	_animsFilename[2] = units;
	_stagesFilename[1] = tens;
	_stagesFilename[2] = units;
	_roomsFilename[1] = tens;
	_roomsFilename[2] = units;
}

void GameManager::enterPressed() {
	if (_vm->isCD()) {
		_vm->_txtMgr->setTextRemainingFrame(1);
		_vm->_txtMgr->setTextDisplayed(false);
		_vm->_spMgr->stopSpeech();
		//TODO handle mouse case
		while (_lastPressedKey != Common::KeyCode::KEYCODE_INVALID) {
			updateInputStatus();
			_vm->_gMgr->_hfCappedUpdate();
		}
	} else {

		if (_vm->_txtMgr->getTextRemainingFrame() > 1 && (_vm->_txtMgr->getTextDuration() - 3) > _vm->_txtMgr->getTextRemainingFrame()) {
			_vm->_txtMgr->setTextRemainingFrame(1);
		}
		if (_waitingForEnter) {
			_waitingForEnter = false;
			while (_lastPressedKey != Common::KeyCode::KEYCODE_INVALID) {
				updateInputStatus();
				_vm->_gMgr->_hfCappedUpdate();
				//			_vm->_system->delayMillis(10);
			}
		}
	}
}
void GameManager::handlePause() {
	char stringBuffer[20];
	uint16 xPos;
	uint16 yPos;
	strncpy(stringBuffer, "Game Paused", 20);
	if (_lastPressedKey == Common::KeyCode::KEYCODE_p) {
		while (_lastPressedKey == Common::KeyCode::KEYCODE_p) {
			updateInputStatus();
			//			_vm->_system->delayMillis(10);
			_vm->_gMgr->_hfCappedUpdate();
		}
		xPos = (320 - _vm->_txtMgr->calcStringWidth(stringBuffer)) / 2;
		yPos = 0xb4;
		uint8 *videoBuffer = _vm->_gMgr->lockMainSurface();
		updateFontDataAndColor(0x50);
		_vm->_txtMgr->drawString(xPos + 1, yPos + 1, videoBuffer, stringBuffer);
		_vm->_txtMgr->drawString(xPos + 1, yPos - 1, videoBuffer, stringBuffer);
		_vm->_txtMgr->drawString(xPos + 1, yPos, videoBuffer, stringBuffer);
		_vm->_txtMgr->drawString(xPos - 1, yPos, videoBuffer, stringBuffer);
		_vm->_txtMgr->drawString(xPos, yPos + 1, videoBuffer, stringBuffer);
		_vm->_txtMgr->drawString(xPos, yPos - 1, videoBuffer, stringBuffer);
		_vm->_txtMgr->drawString(xPos - 1, yPos + 1, videoBuffer, stringBuffer);
		_vm->_txtMgr->drawString(xPos - 1, yPos - 1, videoBuffer, stringBuffer);
		updateFontDataAndColor(0x5f);
		_vm->_txtMgr->drawString(xPos, yPos, videoBuffer, stringBuffer);
		_vm->_gMgr->unlockMainSurface();
		//		_vm->_system->updateScreen();
		_vm->_gMgr->_hfCappedUpdate();
		_movementStatus = 0;
		updateInputStatus();
		while (_lastPressedKey == Common::KeyCode::KEYCODE_INVALID && _movementStatus == 0 && _mouseButtons == 0) {
			//			_vm->_system->delayMillis(10);
			_vm->_gMgr->_hfCappedUpdate();
			updateInputStatus();
		}
	}
}
void GameManager::gameFrame() { //ok
	_vm->_gameState.lifeRegenCounter++;
	_vm->_txtMgr->toggleEvenFrame();
	if (_vm->_gameState.newPakNum != _vm->_gameState.currPakNum) {
		changeCurrentPak();
	}
	if (_vm->_gameState.newRoomNum != _vm->_gameState.currRoomNum) {
		loadAndExecuteNewRoom();
	}
	_vm->_gMgr->backgroundToBackbuffer();
	if (_isLook) {
		getMainCharIntersectionWithHotspots(true);
	}
	if (_isGet) {
		tryTakingObject();
	}

	moveMainActor();
	_vm->_scMgr->executeRoomScripts();
	if (forceLoading == 0) {
		drawBottomExits();
		handleAnimations();
		handleWalk();
		_vm->_gMgr->addStageElementsToDisplayList();
		_vm->_gMgr->addActorsToDisplayList();
		getMainCharIntersectionWithHotspots(false);
		_vm->_gMgr->drawActorsAndStageElements();
		if (!_vm->isCD()) {
			_vm->_txtMgr->handleOnScreenText();
		} else {
			if (_vm->_gameState.speechOptions == 0) {
				_vm->_txtMgr->handleOnScreenText();
			}
			if (_vm->_gameState.speechOptions == 1) {
				if (_vm->_txtMgr->isTextDisplayed() || _vm->_spMgr->isSayingDescription()) {
					_vm->_txtMgr->showDialogue();
				}
				if (waitingForEnter()) {
					_vm->_txtMgr->handleMultiChoice();
				}
				_vm->_spMgr->handleSpeech();
			}
			if (_vm->_gameState.speechOptions == 2) {
				if (_vm->_spMgr->isSayingDescription()) {
					_vm->_txtMgr->showDialogue();
				}
				if (waitingForEnter()) {
					_vm->_txtMgr->handleMultiChoice();
				}
				_vm->_spMgr->handleSpeech();
			}
		}
		uint16_t illsmouthOverlayDurationVar = 0xac;
		uint16_t firstSceneVar = 0x03;
		if (_vm->isCD()) {
			illsmouthOverlayDurationVar = 0x0b;
			firstSceneVar = 0x0a;
		}
		if (_vm->_gameState.gameVars[illsmouthOverlayDurationVar] < 100) {
			if (_vm->_gameState.gameVars[firstSceneVar] == 1) {
				drawIllsmouthTitle();
			}
		}

		debugDraw();

		_vm->_gMgr->drawFrameToScreen();
		regenMainCharLife();
		if (forceLoading != 0) {
			forceLoading = 0;
		}
		_isTalk = false;
		_isGet = false;
		_isUse = false;
		_isLook = false;
	}
}
uint8 GameManager::comparePositions(int16 actorX, int16 actorY, int16 mouseX, int16 mouseY) {

	actorY -= 8;
	int16 xdiff = mouseX - actorX;
	int16 ydiff = mouseY - actorY;
	int16 abs_xdiff = abs(mouseX - actorX);
	int16 abs_ydiff = abs(mouseY - actorY);
	if (xdiff == 0 && ydiff == 0) {
		return kNoDir;
	}
	if (ydiff > 0 && ydiff > abs_xdiff && abs_ydiff > 2) {
		return kDown;
	}
	if (ydiff < 0 && (-ydiff) > abs_xdiff && abs_ydiff > 2) {
		return kUp;
	}
	if (xdiff > 0 && xdiff > abs_ydiff && abs_xdiff > 2) {
		return kRight;
	}
	if (xdiff < 0 && (-xdiff) > abs_ydiff && abs_xdiff > 2) {
		return kLeft;
	}
	return kNoDir;
}
void GameManager::moveMainActor() { //ok
	bool leftButtonPressed = false;

	_currentlyHandledMovementFlags = _movementStatus;
	Actor* actor = _mainActor;

	_requestedFacingDir = _moveFlagsToFacingDir[_currentlyHandledMovementFlags & 0x0f];
	if (_vm->isCD()) {
		if (_waitingForEnter || _vm->_txtMgr->isTextDisplayed() || _vm->_gameState.movementMask == 0x0f) {
			if (_vm->_txtMgr->isTextDisplayed()) {
				_vm->_moMgr->setMouseCursor(0, _vm->_moMgr->loadedCursors[2]);
			}
			else if (_waitingForEnter) {
				_vm->_moMgr->setMouseCursor(0, _vm->_moMgr->loadedCursors[6]);
			}
			else if (_vm->_gameState.movementMask == 0x0f) {
				_vm->_moMgr->setMouseCursor(0, _vm->_moMgr->loadedCursors[5]);
			}
			else {
				_vm->_moMgr->setMouseCursor(1,nullptr);
			}
			_vm->_moMgr->setMouseVisibility(true);
		}
		else {
			uint16 mouse_actor_direction = kNoDir;
			if (leftButtonPressed == false && _requestedFacingDir == kNoDir) {
				mouse_actor_direction = comparePositions(actor->pivotX, actor->pivotY, _vm->_moMgr->getCurrX(), _vm->_moMgr->getCurrY());
			}
			else {
				if (_requestedFacingDir != kNoDir) {
					mouse_actor_direction = _requestedFacingDir;
				}
			}
			if ((_mouseButtons & 0x02) == 2) {
				leftButtonPressed = true;
			}
			else {
				leftButtonPressed = false;
			}
			if (mouse_actor_direction > 0 && mouse_actor_direction < 5) {
				switch (mouse_actor_direction) {
				case kUp:
					if (leftButtonPressed) {
						_currentlyHandledMovementFlags = (_currentlyHandledMovementFlags & kConfirm) | kUpFlag;
						_requestedFacingDir = mouse_actor_direction;
					}
					_vm->_moMgr->setMouseCursor(0, _vm->_moMgr->loadedCursors[1]);
					break;
				case kRight:
					if (leftButtonPressed) {
						_currentlyHandledMovementFlags = (_currentlyHandledMovementFlags & kConfirm) | kRightFlag;
						_requestedFacingDir = mouse_actor_direction;
					}
					_vm->_moMgr->setMouseCursor(0, _vm->_moMgr->loadedCursors[4]);
					break;
				case kDown:
					if (leftButtonPressed) {
						_currentlyHandledMovementFlags = (_currentlyHandledMovementFlags & kConfirm) | kDownFlag;
						_requestedFacingDir = mouse_actor_direction;
					}
					_vm->_moMgr->setMouseCursor(0, _vm->_moMgr->loadedCursors[0]);
					break;
				case kLeft:
					if (leftButtonPressed) {
						_currentlyHandledMovementFlags = (_currentlyHandledMovementFlags & kConfirm) | kLeftFlag;
						_requestedFacingDir = mouse_actor_direction;
					}
					_vm->_moMgr->setMouseCursor(0, _vm->_moMgr->loadedCursors[3]);
					break;
				}
				_vm->_moMgr->setMouseVisibility(true);
			}
		}
	}
	if ((_vm->_gameState.movementMask & _currentlyHandledMovementFlags) || _waitingForEnter) {
		_requestedFacingDir = kNoDir;
		_isConfirmKeyFlag = 0;
	} else {
		_isConfirmKeyFlag = _currentlyHandledMovementFlags & kConfirm;
	}
	if ((_currentlyHandledKey == Common::KeyCode::KEYCODE_RETURN || _currentlyHandledKey == Common::KeyCode::KEYCODE_KP_ENTER) || (_isConfirmKeyFlag & kConfirm) || _mouseButtons!=0) {
		_isConfirmKeyOrEnter = true;
	} else {
		_isConfirmKeyOrEnter = false;
	}
	uint16 walkAnimOffset = actor->offsetWalkAnimation;
	if (actor->whereToWalk & 0x03) {
		return;
	}
	if (_waitingForEnter && (actor->offsetWalkAnimation != 0)) {
		resetWalkAnimation(_mainActor);
		return;
	}
	actor->destinationX = actor->pivotX;
	actor->destinationY = actor->pivotY;
	if (walkAnimOffset == 4) {
		walkAnimOffset = 0;
	}
	if (_requestedFacingDir == actor->facingDirection && !(_currentlyHandledMovementFlags & _vm->_gameState.movementMask)) {
		walkAnimOffset = 4;
	}
	setActorFacingDirection(actor, _facingDirChangeMatrix[actor->facingDirection][_requestedFacingDir]);
	setActorOffsetWalk(actor, walkAnimOffset);
}
void GameManager::drawBottomExits() {
	for (uint8 i = 0; i < _vm->_gameState.numExits; i++) {
		RoomExit *exit = &_vm->_gameState.exits[i];
		uint16 x1 = exit->x1;
		uint16 x2 = exit->x2;
		if (exit->direction == 3) {
			uint16 y1 = _COMET_YRESOLUTION - 1;
			uint16 y2 = y1;
			_vm->_gMgr->drawRectangleOutline(x1, y1 - 1, x2, y2, 0x78);
			_vm->_gMgr->drawLine(x1 + 1, y1, x2 - 1, y1, 0x7f);
		}
	}
}

void GameManager::regenMainCharLife() {
	//actually useless in SotC
	if (_mainActor->life > 0) {
		if ((_vm->_gameState.lifeRegenCounter % 512) == 0) {
			if (_mainActor->life < 100) {
				_mainActor->life++;
			}
		}
	} else {
		// here the games calls an empty proc
	}
}
void GameManager::drawIllsmouthTitle() {
	if (_vm->_gameState.gameVars[0xac] == 0) {
		_vm->_txtMgr->getSentence(2, 0x24, _illsmouthTitle);
	}
	int16 xOffs = (_COMET_XRESOLUTION - _vm->_txtMgr->calcStringWidth(_illsmouthTitle)) / 2;
	_vm->_txtMgr->drawOutlinedString(xOffs, 0xb4, _illsmouthTitle + 1, 7, 0);
	_vm->_gameState.gameVars[0xac]++;
}

void GameManager::changeCurrentPak() {
	if (_vm->_gameState.newPakNum != -1) {
		freeResourcesExceptMainActor();
		freeStageResources();
		setPakNumber(_vm->_gameState.newPakNum, _vm->_gameState.newRoomNum);
		loadAndExecuteNewRoom();
	}
}
void GameManager::freeResourcesExceptMainActor() {
	for (uint8 i = 0; i < 20; i++) {
		if (_vm->_gameState.resources[i].data != nullptr && _vm->_gameState.resources[i].type == 0 && i != _mainActor->resourceIdx) {
			removeResourceFromActors(i);
			delete[] _vm->_gameState.resources[i].data;
			_vm->_gameState.resources[i].data = nullptr;
		}
	}
}
void GameManager::freeStageResources() {
	freeResourcesExceptMainActor();
	if (_vm->_gMgr->_stageAnims != nullptr) {
		delete[] _vm->_gMgr->_stageAnims;
		_vm->_gMgr->_stageAnims = nullptr;
	}
}
void GameManager::loadAndExecuteNewRoom() {
	if (_vm->isCD()) {
		_vm->_moMgr->setMouseVisibility(false);
	}
	if (_mainActor->whereToWalk != 0 && ((_mainActor->facingDirection == 2 && _mainActor->pivotX < 319) || (_mainActor->facingDirection == 4 && _mainActor->pivotX > 0))) {
		_mainActor->pivotY = _mainActor->destinationY;
		return;
	}
	hideTextAndFreeStageResources();
	_vm->_gameState.prevPakNum = _vm->_gameState.currPakNum;
	_vm->_gameState.prevRoomNum = _vm->_gameState.currRoomNum;
	_vm->_gameState.currPakNum = _vm->_gameState.newPakNum;
	_vm->_gameState.currRoomNum = _vm->_gameState.newRoomNum;
	resetWalkAnimation(_mainActor);
	_mainActor->isVisible = true;
	_mainActor->intersectionType = 0;
	_mainActor->lastIntersectionType = 0;
	_mainActor->drawAreaMinX = 0;
	_mainActor->drawAreaMinY = 0;
	_mainActor->drawAreaMaxX = 319;
	_mainActor->drawAreaMaxY = 199;
	_vm->_gMgr->setJustFadedOut(false);
	_vm->_scMgr->loadAndPrepareRoomScripts(_roomsFilename, _vm->_gameState.currRoomNum, _isLoadingSavedGame);
	handleRoomsTransition(_vm->_gameState.prevRoomNum, _vm->_gameState.prevPakNum);
	if (_vm->isCD()) {
		_vm->_moMgr->setMouseVisibility(true);
	}

}
void GameManager::hideTextAndFreeStageResources() {
	hideText();
	freeResourcesExceptMainActor();
	if (_vm->_gMgr->_stageAnims != nullptr) {
		delete[] _vm->_gMgr->_stageAnims;
		//TODO capire come mai
		_vm->_gMgr->_stageAnims = nullptr;
	}
}
void GameManager::hideText() {
	_vm->_txtMgr->hideText();
	_waitingForEnter = false;
}
void GameManager::resetWalkAnimation(Actor *actor) {
	actor->whereToWalk = 0;
	setActorOffsetWalk(actor, 0);
}
void GameManager::handleRoomsTransition(uint8 prevRoom, int8 prevPak) {
	uint16 direction = kUp;
	uint16 x1 = 160, x2 = 160;
	uint16 y1 = 190, y2 = 190;
	for (uint8 i = 0; i < _vm->_gameState.numExits; i++) {
		if (_vm->_gameState.exits[i].pak == prevPak && _vm->_gameState.exits[i].room == prevRoom) {
			direction = _oppositeDirections[_vm->_gameState.exits[i].direction];
			if (_mainActor->facingDirection == direction) {
				getExitCoords(i, x1, y1, x2, y2);
			}
		}
	}
	_mainActor->pivotX = x1 + ((x2 - x1) / 2);
	_mainActor->pivotY = y1 + ((y2 - y1) / 2);
	_mainActor->facingDirection = direction;
	actorPrepareAnimation(_mainActor, direction - 1);
	if (_roomZoomLevel == 0) {
		if (direction == kUp || direction == kDown) {
			_vm->_gMgr->setDrawWithEffect();
		} else {
			//TODO handle horizontal screens scroll
			_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->_backgroundBuffer, _vm->_gMgr->_videoBackbuffer);
			_vm->_gMgr->addStageElementsToDisplayList();
			_vm->_gMgr->addActorsToDisplayList();
			_vm->_gMgr->drawActorsAndStageElements();
			_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->_videoBackbuffer, _vm->_gMgr->_backgroundBuffer);
			if (direction == kRight) {
				_vm->_gMgr->scrollRoomLeft();
			}
			if (direction == kLeft) {
				_vm->_gMgr->scrollRoomRight();
			}
			_vm->_gMgr->loadStageBackground();
		}
	}
	if (_vm->_gMgr->getFadeStatus() == 0) {
		_vm->_gMgr->fadePalette(_vm->_gMgr->mainGamePalette, _vm->_gMgr->tmpPalette, _vm->_gameState.paletteFadeLevel, 256);
		_vm->_gMgr->uploadPalette(_vm->_gMgr->tmpPalette);
	}
}
Actor *GameManager::getActor(uint8 actorIdx) {
	return &_vm->_gameState.actors[actorIdx];
}
void GameManager::getExitCoords(uint8 exitIdx, uint16 &x1, uint16 &y1, uint16 &x2, uint16 &y2) {
	//TODO implement
	RoomExit *exit = &_vm->_gameState.exits[exitIdx];
	x1 = exit->x1;
	x2 = exit->x2;
	y1 = _vm->_gameState.topWalkAreaYValues[x1];
	if (x1 == x2) {
		y2 = _COMET_YRESOLUTION - 1;
	} else {
		if (exit->direction == 3) {
			y2 = _COMET_YRESOLUTION - 1;
			y1 = y2;
		} else {
			y2 = _vm->_gameState.topWalkAreaYValues[x2];
		}
	}
	if (y1 > y2) {
		uint16 t = y1;
		y1 = y2;
		y2 = t;
	}
}

void GameManager::getMainCharSightArea(Rectangle &rect, uint16 sightWidth, uint16 sightHeight) {
	int16 minX = _mainActor->pivotX - _mainActor->halfWidth - 8;
	int16 minY = _mainActor->pivotY - _mainActor->height - 8;
	int16 maxX = _mainActor->pivotX + _mainActor->halfWidth + 8;
	int16 maxY = _mainActor->pivotY + 8;
	if (_mainActor->facingDirection == kUp) {
		minY -= sightHeight;
		maxY -= 20;
		minX -= sightWidth;
		maxX += sightWidth;
	}
	if (_mainActor->facingDirection == kRight) {
		maxX += sightHeight; //sic
		minX += 25;
		minY -= 32;
	}
	if (_mainActor->facingDirection == kDown) {
		maxY += sightHeight;
		minY += 20;
	}
	if (_mainActor->facingDirection == kLeft) {
		minX -= sightHeight; //sic
		maxX -= 25;
		minY -= 32;
	}
	if (minX < 0) {
		minX = 0;
	}
	if (maxX > 319) {
		maxX = 319;
	}
	if (minY < 0) {
		minY = 0;
	}
	if (maxY > 199) {
		maxY = 199;
	}
	rect.left = minX;
	rect.top = minY;
	rect.right = maxX;
	rect.bottom = maxY;
}

void GameManager::getMainCharIntersectionWithHotspots(bool showName) {
	Rectangle area;
	const char *nameSrc = nullptr;
	_hotspotInSight = false;
	if (_vm->_gameState.movementMask == 0x0f) {
		return;
	}
	getMainCharSightArea(area, 0, 50);
	uint16 intersectingHotspot = findIntersectingHotspot(area);
	if (intersectingHotspot == 0) {
		return;
	}
	_hotspotInSight = true;
	intersectingHotspot &= 0x0FF;
	Hotspot *h = &_vm->_gameState.hotspots[intersectingHotspot];
	_hotspotDirection = _mainActor->facingDirection;
	_hotspotX = h->x;
	_hotspotY = h->y - 6;
	if (h->objType == 0) {
		nameSrc = _vm->_txtMgr->getObjNames();
	} else {
		nameSrc = _vm->_txtMgr->getMultiChoiceText();
	}
	if (showName) {
		if (!_waitingForEnter && !_vm->_txtMgr->isTextDisplayed()) {
			_vm->_txtMgr->sayDescription(h->objIdx, 20, nameSrc);
		}
	}
}
uint16 GameManager::findIntersectingHotspot(Rectangle area) {
	Rectangle hots;
	for (uint8 i = 0; i < 20; i++) {
		if (_vm->_gameState.hotspots[i].isActive) {
			hots.left = _vm->_gameState.hotspots[i].x - 8;
			hots.right = _vm->_gameState.hotspots[i].x + 8;
			hots.top = _vm->_gameState.hotspots[i].y - 8;
			hots.bottom = _vm->_gameState.hotspots[i].y + 8;
			if (rectanglesIntersect(area, hots)) {
				return i | 0x500;
			}
		}
	}
	return 0;
}
bool GameManager::rectanglesIntersect(const Rectangle &r1, const Rectangle &r2) const {
	if (r1.left <= r2.right && r1.top <= r2.bottom && r1.right >= r2.left && r1.bottom >= r2.top) {
		return true;
	}
	return false;
}

void GameManager::drawHotspotViewLine() {
	if (_hotspotInSight) {
		int16 pivotX = _mainActor->pivotX;
		int16 pivotY = _mainActor->pivotY - 23;
		if (_hotspotDirection == kUp) {
			pivotY--;
		}
		if (_hotspotDirection == kRight) {
			pivotX += 4;
		}
		if (_hotspotDirection == kDown) {
			pivotY++;
		}
		if (_hotspotDirection == kLeft) {
			pivotX -= 5;
		}
		_vm->_gMgr->drawDashedLine(pivotX, pivotY, _hotspotX + _rnd.getRandomNumber(3) - 1, _hotspotY + _rnd.getRandomNumber(3) - 1, 7);
	}
}
void GameManager::handleAnimations() {
	for (uint8 i = 0; i < 10; i++) {
		if (_vm->_gameState.actors[i].life != 0) {
			handleActorAnimation(&_vm->_gameState.actors[i]);
		}
	}
	handlePortraitAnimation(&_vm->_gameState.actors[10]);
}
void GameManager::handleActorAnimation(Actor *actor) {
	//if (actor->resourceIdx == 4 && actor->animationIdx == 0) {
	//	int b = 0;
	//}
	if (actor->animationType == 1) {
		actor->animationType = 0;
		int16 animIdx = actor->facingDirection + actor->offsetWalkAnimation - 1;
		actorPrepareAnimation(actor, animIdx);
	} else {
		if (actor->fixedAnimationFrame == -1) {
			uint8 *animPtr = _vm->_gMgr->getAnimData(actor->animationIdx, _vm->_gameState.resources[actor->resourceIdx].data) + 2;
			uint16 factor = READ_LE_UINT16(animPtr + ((actor->frameToDraw * 8) + 2));
			uint16 decodeType = factor >> 14;
			factor &= 0x3fff;
			if (decodeType == 1) {
				if (factor < 1) {
					factor = 1;
				}
				if (actor->currentAnimationFactor >= factor - 1) {
					actor->currentAnimationFactor = 0;
					actor->frameToDraw++;
				}
			} else {
				actor->frameToDraw++;
			}
			if (actor->frameToDraw >= actor->numFrames) {
				actor->frameToDraw = 0;
			}
		} else {
			actor->currentAnimationFactor = 0;
		}
	}
}
void GameManager::handlePortraitAnimation(Actor *actor) {
	//bug in the original game: it didn't check if portrait's resourceIdx is -1
	//in the original game it read data from random memory and everything happened to work fine,
	//but we can't do it here ;)
	if (actor->resourceIdx >= 0) {
		if (actor->fixedAnimationFrame == -1) {
			uint8 *animPtr = _vm->_gMgr->getAnimData(actor->animationIdx, _vm->_gameState.resources[actor->resourceIdx].data) + 2;
			uint16 factor = READ_LE_UINT16(animPtr + ((actor->frameToDraw * 8) + 2));
			uint16 decodeType = factor >> 14;
			factor &= 0x3fff;
			if (decodeType == 1) {
				if (factor < 1) {
					factor = 1;
				}
				if (actor->currentAnimationFactor > factor - 1) {
					actor->currentAnimationFactor = 0;
					actor->frameToDraw++;
				}
			} else {
				actor->frameToDraw++;
			}
			if (actor->frameToDraw >= actor->numFrames) {
				actor->frameToDraw = 0;
				if (actor->animationIdx < 4) {
					if (_portratiAnimationCounter == 0) {
						if (_vm->isCD() && _vm->_gameState.speechOptions != 0) {
							_portraitRandomAnimation = _rnd.getRandomNumber(3);
							if (!_vm->_spMgr->isSpeechActive()) {
								_portraitRandomAnimation = 0;
							}

						} else {
							_portraitRandomAnimation = _rnd.getRandomNumber(3);
							if (_portraitRandomAnimation == 0) {
								_portraitRandomAnimation = 1;
							}
						}
					} else {
						_portratiAnimationCounter++;
						if (_vm->isCD() && (_vm->_gameState.speechOptions == 1 || _vm->_gameState.speechOptions == 2)) {
							if (_portratiAnimationCounter == 1) {
								_portratiAnimationCounter = 0;
							}
						} else {
							if (_portratiAnimationCounter == 10) {
								_portratiAnimationCounter = 0;
							}
						}
					}
					actorPrepareAnimation(actor, _portraitRandomAnimation);
				}
			}
		} else {
			actor->currentAnimationFactor = 0;
		}
	}
	if (_vm->isCD()) {
		_vm->_gMgr->callCapFPS();
	}
}
uint8 GameManager::checkPointsCoincidence(uint16 x1, uint16 y1, uint16 x2, uint16 y2) {
	uint8 retval = 0;
	if (x1 == x2) {
		retval |= 0x01;
	}
	if (y1 == y2) {
		retval |= 0x02;
	}
	return retval;
}
void GameManager::actorStorePrevDestination(Actor *actor) {
	if (actor->whereToWalk & 0x03 && !(actor->whereToWalk & 0x04)) {
		actor->prevDestinationX = actor->destinationX;
		actor->prevDestinationY = actor->destinationY;
		actor->whereToWalk |= 0x04;
	}
}
void GameManager::setActorFacingDirection(Actor *actor, uint8 direction) {
	if (actor->facingDirection != direction && direction != 0 && actor->animationType != 2) {
		actor->facingDirection = direction;
		actor->animationType = 1;
	}
}

void GameManager::setFacingDirectionToDestination(Actor *actor) {
	uint8 whereToWalk = actor->whereToWalk & 0x03;
	int16 xDistance = actor->destinationX - actor->pivotX;
	int16 yDistance = actor->destinationY - actor->pivotY;
	uint8 direction = actor->facingDirection;
	if (whereToWalk == 1 || (whereToWalk == 0 && abs(xDistance) > abs(yDistance))) {
		if (xDistance > 0) {
			direction = 2;
		}
		if (xDistance < 0) {
			direction = 4;
		}
	}
	if (whereToWalk == 2 || (whereToWalk == 0 && abs(yDistance) > abs(xDistance))) {
		if (yDistance > 0) {
			direction = 3;
		}
		if (yDistance < 0) {
			direction = 1;
		}
	}
	setActorFacingDirection(actor, direction);
}

bool GameManager::setActorDestination(int8 actorIdx, int16 x, int16 y) {
	Actor *actor = &_vm->_gameState.actors[actorIdx];
	if (actor->intersectionType != 8) {
		constrainCoordsToWalkingArea(x, y, actor->halfWidth, actor->height);
	}
	uint8 coincidence = checkPointsCoincidence(actor->pivotX, actor->pivotY, x, y);
	if (coincidence == 0x03) {
		return false;
	}
	actorStorePrevDestination(actor);
	actor->destinationX = x;
	actor->destinationY = y;
	if (coincidence == 0) {
		if (!(actor->whereToWalk & 0x03)) {
			actor->whereToWalk |= _rnd.getRandomNumber(1) + 1;
		} else {
			actor->whereToWalk ^= 0x03;
		}

	} else {
		actor->whereToWalk &= 0xfffc;
		actor->whereToWalk |= coincidence ^ 0x03;
	}
	setActorOffsetWalk(actor, 4);

	setFacingDirectionToDestination(actor);
	return true;
}
uint16 GameManager::findIntersectingExit(Rectangle &r, uint8 direction) {
	bool isInRange = false;
	int16 left = r.left;
	int16 top = r.top;
	int16 right = r.right;
	uint16 x1, y1, x2, y2;
	for (uint8 i = 0; i < _vm->_gameState.numExits; i++) {
		if (_vm->_gameState.exits[i].direction == direction) {
			getExitCoords(i, x1, y1, x2, y2);
			if (
				((direction == 1 || direction == 3) && left >= x1 && right <= x2) || (direction == 2 && top >= y1 && top <= y2 && right >= x1) || (direction == 4 && top >= y1 && top <= y2 && left <= x2)) {
				isInRange = true;
			}
			if (isInRange) {
				return i | 0x400;
			}
		}
	}
	return 0;
}

void GameManager::constrainCoordsToWalkingArea(int16 &x, int16 &y, int16 hw, int16 h) {
	Rectangle r;
	if (x - hw < 0) {
		x = hw + 1;
	}
	if (x + hw > _COMET_XRESOLUTION - 1) {
		x = _COMET_XRESOLUTION - 1 - hw - 1;
	}
	r.left = x - hw;
	r.top = y - h;
	r.right = x + hw;
	r.bottom = y;
	if (findIntersectingExit(r, 1) != 0) {
		return;
	}
	if (findIntersectingExit(r, 2) != 0) {
		return;
	}
	if (findIntersectingExit(r, 3) != 0) {
		return;
	}
	if (findIntersectingExit(r, 4) != 0) {
		return;
	}
	if (y - h <= _vm->_gameState.topWalkAreaYValues[x - hw]) {
		y = h + 2 + _vm->_gameState.topWalkAreaYValues[x - hw];
	}
	if (y - h <= _vm->_gameState.topWalkAreaYValues[x + hw]) {
		y = h + 2 + _vm->_gameState.topWalkAreaYValues[x + hw];
	}
	if (y > _COMET_YRESOLUTION - 1) {
		y = _COMET_YRESOLUTION - 1;
	}
}
void GameManager::clipRectToWalkingArea(int16 &x, int16 &y, int16 hw, int16 h) {
	int16 newx = x;
	int16 newy = y;
	if (newx - hw < 0) {
		newx = hw + 1;
	}
	if (newx + hw > _COMET_XRESOLUTION - 1) {
		newx = _COMET_XRESOLUTION - 1 - hw - 1;
	}
	if (newy - h <= _vm->_gameState.topWalkAreaYValues[newx - hw]) {
		newy = _vm->_gameState.topWalkAreaYValues[newx - hw] + h + 2;
	}
	if (newy - h <= _vm->_gameState.topWalkAreaYValues[newx + hw]) {
		newy = _vm->_gameState.topWalkAreaYValues[newx + hw] + h + 2;
	}
	if (newy > 199) {
		newy = 199;
	}
	x = newx;
	y = newy;
}
uint8 GameManager::comparePoints(uint16 x1, uint16 y1, uint16 x2, uint16 y2) {
	int16 xDiff = x2 - x1;
	int16 yDiff = y2 - y1;
	if ((abs(xDiff) / 2) > (abs(yDiff) / 2)) {
		if (xDiff > 0) {
			return 2;
		} else {
			return 4;
		}
	} else {
		if (yDiff > 0) {
			return 3;
		} else {
			return 1;
		}
	}
}

void GameManager::handleWalk() {
	for (uint8 i = 0; i < 11; i++) {
		Actor *actor = &_vm->_gameState.actors[i];
		if (actor->life != 0) {
			bool didWalk = handleActorWalk(i);
			if (actor->whereToWalk & 0x03) {
				handleActorAutomaticWalk(actor, i, didWalk);
			}
		}
	}
}
bool GameManager::handleActorWalk(uint8 actorIdx) {
	Actor *actor = &_vm->_gameState.actors[actorIdx];
	if (actor->offsetWalkAnimation != 4) {
		return false;
	}
	int16 pivotX = actor->pivotX;
	int16 pivotY = actor->pivotY;
	uint8 *animData = _vm->_gMgr->getAnimData(actor->animationIdx, _vm->_gameState.resources[actor->resourceIdx].data) + 2;
	int16 frameX = READ_LE_INT16(animData + (actor->frameToDraw * 8) + 4);
	int16 frameY = READ_LE_INT16(animData + (actor->frameToDraw * 8) + 6);
	adjustWalkAnimCoords(actor->facingDirection, frameX, frameY);
	pivotX += frameX;
	pivotY += frameY;
	if (actor->whereToWalk & 0x03) {
		clipCoordsOnDestination(actor, pivotX, pivotY);
	}
	if (actor->intersectionType != 8) {
		uint16 resultIntersection = checkIntersections(actorIdx, pivotX, pivotY, actor->halfWidth, actor->height, actor->facingDirection);
		if (resultIntersection != 0) {
			resultIntersection = handleActorIntersections(actor, actorIdx, resultIntersection);
			if (resultIntersection == 0) {
				return false;
			}
		} else {
			actor->intersectionType = 0;
		}
	}
	actor->pivotX = pivotX;
	actor->pivotY = pivotY;
	return true;
}
void GameManager::adjustWalkAnimCoords(uint8 facingDir, int16 &x, int16 &y) {
	if (y >= 1) {
		y += _vm->_gameState.walkVerticalAdjustment;
		if (y < 1) {
			y = 1;
		}
	} else {
		if (y <= -1) {
			y -= _vm->_gameState.walkVerticalAdjustment;
			if (y > -1) {
				y = -1;
			}
		}
	}
	if (_vm->_gameState.currRoomNum == _vm->_gameState.newRoomNum) {
		if (facingDir == 1) {
			x += _vm->_gameState.walkHorizAdjustment;
		}
		if (facingDir == 2) {
			y += _vm->_gameState.walkVertAdjustment_HorizMovement;
		}
		if (facingDir == 3) {
			x -= _vm->_gameState.walkHorizAdjustment;
		}
		if (facingDir == 4) {
			y -= _vm->_gameState.walkVertAdjustment_HorizMovement;
		}
	}
}
void GameManager::clipCoordsOnDestination(Actor *actor, int16 &x, int16 &y) {
	switch (actor->facingDirection) {
	case 1:
		if (actor->destinationY > y) {
			y = actor->destinationY;
		}
		break;
	case 2:
		if (actor->destinationX < x) {
			x = actor->destinationX;
		}
		break;
	case 3:
		if (actor->destinationY < y) {
			y = actor->destinationY;
		}
		break;
	case 4:
		if (actor->destinationX > x) {
			x = actor->destinationX;
		}
		break;
	}
}
uint16 GameManager::checkIntersections(uint8 actorIdx, int16 x, int16 y, uint8 halfWidth, uint8 height, uint8 facingDir) {
	uint16 intersection = 0;
	uint16 exitIdx = 0;
	Rectangle r;
	r.left = x - halfWidth;
	r.top = y - height;
	r.right = x + halfWidth;
	r.bottom = y;
	intersection = getIntersectionwithWalkArea(r, facingDir);
	if (intersection != 0) {
		exitIdx = findIntersectingExit(r, facingDir);
		if (exitIdx != 0) {
			intersection = exitIdx;
		}
		return intersection;
	} else {
		intersection = checkIntersectionWithStageElements(r);
		if (intersection == 0) {
			intersection = checkIntersectionWithActors(actorIdx, r);
			if (intersection == 0) {
				return 0;
			}
		}
		return intersection;
	}
}
uint16 GameManager::getIntersectionwithWalkArea(Rectangle &r, uint8 facingDir) {
	Rectangle r2 = r;
	uint8 retval = 0;
	if (r2.left < 0) {
		r2.left = 0;
	}
	if (r2.right > _COMET_XRESOLUTION - 1) {
		r2.right = _COMET_XRESOLUTION - 1;
	}
	uint16 walkAreaYLeft = _vm->_gameState.topWalkAreaYValues[r2.left];
	uint16 walkAreaYRight = _vm->_gameState.topWalkAreaYValues[r2.right];
	if (facingDir == 1) {
		if ((r2.top <= walkAreaYLeft) || (r2.top <= walkAreaYRight)) {
			retval = 1;
		}
	}
	if (facingDir == 2) {
		if (walkAreaYRight >= r2.top || r2.right == _COMET_XRESOLUTION - 1) {
			retval = 1;
		}
	}
	if (facingDir == 4) {
		if (walkAreaYLeft >= r2.top || r2.left == 0) {
			retval = 1;
		}
	}
	if (r2.bottom > 199) {
		retval = 2;
	}
	return retval << 8;
}
uint16 GameManager::checkIntersectionWithStageElements(Rectangle &r) {
	uint8 numElements = _vm->_gameState.numStageElements;
	for (uint8 i = 0; i < numElements; i++) {
		StageElement *element = &_vm->_gameState.stageElements[i];
		int16 left = element->halfLeft * 2;
		_intersectingElementRectangle.left = left;
		_intersectingElementRectangle.top = element->top;
		int16 right = element->halfRight * 2;
		_intersectingElementRectangle.right = right;
		_intersectingElementRectangle.bottom = element->bottom;
		if (left != right) {
			if (rectanglesIntersect(r, _intersectingElementRectangle)) {
				return 0x300 | i;
			}
		}
	}
	return 0;
}
uint16 GameManager::checkIntersectionWithActors(uint8 actorIdx, Rectangle &r) {
	for (uint8 i = 0; i < 11; i++) {
		if (i != actorIdx && _vm->_gameState.actors[i].life != 0 && _vm->_gameState.actors[i].intersectionType != 8) {
			Actor *actor = &_vm->_gameState.actors[i];
			_intersectingElementRectangle.left = actor->pivotX - actor->halfWidth;
			_intersectingElementRectangle.top = actor->pivotY - actor->height;
			_intersectingElementRectangle.right = actor->pivotX + actor->halfWidth;
			_intersectingElementRectangle.bottom = actor->pivotY;
			if (rectanglesIntersect(_intersectingElementRectangle, r)) {
				return 0x600 | i;
			}
		}
	}
	return 0;
}
uint16 GameManager::handleActorIntersections(Actor *actor, uint8 actorIdx, uint16 resultIntersection) {
	uint16 retval = 0;
	actor->intersectionType = resultIntersection >> 8;
	actor->intersectionIdx = resultIntersection & 0x00ff;
	if (actorIdx == 0 && actor->intersectionType == 4) {
		//exit
		retval = setMainActorPositionForNextRoom(_vm->_gameState.exits[actor->intersectionIdx].pak, _vm->_gameState.exits[actor->intersectionIdx].room);
	}
	if (retval == 0) {
		setActorOffsetWalk(actor, 0);
		handleActorAnimation(actor);
	}
	return retval;
}
uint16 GameManager::setMainActorPositionForNextRoom(int8 pak, int8 room) {
	if (room == -1) {
		_vm->_gameState.newPakNum = -1;
		return 0;
	}
	_vm->_gameState.newPakNum = pak;
	_vm->_gameState.newRoomNum = room;
	Actor *actor = _mainActor;
	uint8 facingDir = actor->facingDirection;
	if (facingDir != 1 && facingDir != 3) {

		actor->lastIntersectionType = 4;
		uint16 x1, y1, x2, y2;
		getExitCoords(actor->intersectionIdx, x1, y1, x2, y2);
		if (x2 == _COMET_XRESOLUTION - 2) {
			x2 = _COMET_XRESOLUTION - 1;
		}
		actor->intersectionType = 8;
		if (facingDir == 2) {
			//facing right;
			actor->drawAreaMinX = 0;
			actor->drawAreaMaxX = x2;
			setActorDestination(0, _COMET_XRESOLUTION - 1, actor->pivotY);
		} else {
			//facing left;
			actor->drawAreaMinX = x1;
			actor->drawAreaMaxX = _COMET_XRESOLUTION - 1;
			setActorDestination(0, 0, actor->pivotY);
		}
		actor->whereToWalk &= ~0x0004;
	}
	return 1;
}

void GameManager::handleActorAutomaticWalk(Actor *actor, uint8 actorIdx, bool didMove) {
	if (!didMove) {
		clipMovement(actorIdx, actor);
	}
	int8 coincidence = checkPointsCoincidence(actor->pivotX, actor->pivotY, actor->destinationX, actor->destinationY);
	if (coincidence == 3 || (coincidence == 1 && (actor->whereToWalk & 0x08)) || (coincidence == 2 && (actor->whereToWalk & 0x10))) {
		if (actor->whereToWalk & 0x04) {
			setActorDestination(actorIdx, actor->prevDestinationX, actor->prevDestinationY);
			actor->whereToWalk &= ~0x0004;
		} else {
			resetWalkAnimation(actor);
		}
	} else {
		if ((actor->whereToWalk & 0x03) == coincidence) {
			actor->whereToWalk ^= 0x0003;
			setFacingDirectionToDestination(actor);
		}
	}
}
void GameManager::clipMovement(uint8 actorIdx, Actor *actor) {
	if (actor->intersectionType == 1 || actor->intersectionType == 2) {
		clipMovementToWalkArea(actorIdx, actor);

	} else if (actor->intersectionType == 6 && actor->lastIntersectionType == 6 && actor->intersectionIdx == 0) {
		actor->lastIntersectionType = 0;
		resetWalkAnimation(actor);
		if (actor->isKillable) {
			actorDamageLife(_mainActor, actor->life);
		}
	} else {
		clipMovementToObstacle(actorIdx, actor);
	}
}
void GameManager::clipMovementToWalkArea(uint8 actorIdx, Actor *actor) {
	int16 destX = actor->pivotX; // BUG??? should it be ->destinationX?
	int16 destY = actor->destinationY;
	switch (actor->facingDirection) {
	case 1:
	case 3:
		destX = actor->destinationX;
		break;
	case 2:
		if (actor->destinationY <= actor->pivotY) {
			destY = 2 + actor->height + getWalkAreaYObstacleRight(actor->halfWidth + destX, actor->pivotY - actor->height);
		}
		break;
	case 4:
		if (actor->destinationY <= actor->pivotY) {
			destY = 1 + actor->height + getWalkAreaYObstacleLeft(destX - actor->halfWidth, actor->pivotY - actor->height);
		}
		break;
	}
	setActorDestination(actorIdx, destX, destY);
}
uint16 GameManager::getWalkAreaYObstacleRight(int16 right, int16 top) {
	int16 x = 0, y = 0;
	for (uint16 i = 0; i < _vm->_gameState.currTopWalkAreaNumPoints; i++) {
		x = _topWalkAreaLineStrip.points[i].x;
		y = _topWalkAreaLineStrip.points[i].y;
		if (x > right && y >= top) {
			break;
		}
	}
	if (y >= _COMET_YRESOLUTION - 1) {
		y = 190;
	}
	return y;
}
uint16 GameManager::getWalkAreaYObstacleLeft(int16 left, int16 top) {
	int16 x = 0, y = 0;
	for (int16 i = _vm->_gameState.currTopWalkAreaNumPoints - 1; i >= 0; i--) {
		x = _topWalkAreaLineStrip.points[i].x;
		y = _topWalkAreaLineStrip.points[i].y;
		if (x < left && y >= top) {
			break;
		}
	}
	if (y >= _COMET_YRESOLUTION - 1) {
		y = 190;
	}
	return y;
}

void GameManager::actorDamageLife(Actor *actor, uint16 amount) {
	int16 life = actor->life;
	life -= amount;
	if (life < 0) {
		life = 0;
	}
	actor->life = life;
}
void GameManager::clipMovementToObstacle(uint8 actorIdx, Actor *actor) {
	int16 destX = actor->pivotX;
	int16 destY = actor->pivotY;
	switch (actor->facingDirection) {
	case 1:
	case 3:
		if (_rnd.getRandomNumber(1) == 0) {
			destX = _intersectingElementRectangle.left - (actor->halfWidth + 2);
		} else {
			destX = _intersectingElementRectangle.right + actor->halfWidth + 2;
		}
		break;
	case 2:
	case 4:
		if (_rnd.getRandomNumber(1) == 0) {
			destY = _intersectingElementRectangle.top - 2;
		} else {
			destY = _intersectingElementRectangle.bottom + actor->height + 2;
		}
		break;
	}
	setActorDestination(actorIdx, destX, destY);
}
void GameManager::waitForNoInput() {
	bool res;
	while (_mouseButtons != 0 || _lastPressedKey != 0 || _movementStatus != 0) {
		_vm->_gMgr->_hfCappedUpdate();
		res = updateInputStatus();
		if (!res) {
			return;
		}
	}
}

bool GameManager::copyProtection() {
	uint8 currSelectionBorderColor = 0x38;
	int8 currSelectionBorderColorStep = 1;
	uint8 randConstellationColumn = 0;
	uint8 randConstellationRow = 0;
	_copyProtectionTitleSentence = new char[50];
	_copyProtectionInstructionsSentence = new char[50];
	_vm->_txtMgr->getSentence(2, 0x27, _copyProtectionInstructionsSentence);
	_copyProtectionStarsGraphics = _vm->_archMgr->allocateAndGetFile("A05.PAK", 19);
	uint8 *tmp = _vm->_archMgr->allocateAndGetFile("A05.PAK", 18);
	//	uint16 size = 11 * 16 * sizeof(Constellation);
	memcpy(_copyProtectionsConstellationsData, tmp, 11 * 16 * sizeof(Constellation));
	delete[] tmp;
	//		_copyProtectionInstructionsSentence++;
	uint8 numRightAnswers = 0;
	uint8 numErrors = 0;
	while (numRightAnswers < 2 && numErrors != 3) {
		_vm->_txtMgr->getSentence(2, 0x25 + numErrors, _copyProtectionTitleSentence);
		//			_copyProtectionTitleSentence++;
		_copyProtectionRightConstellationColumn = getRandom(11);
		_copyProtectionRightConstellationRow = getRandom(16);
		uint8 copyProtectionRightX = getRandom(3);
		uint8 copyProtectionRightY = getRandom(3);

		for (uint8 x = 0; x < 3; x++) {
			for (uint8 y = 0; y < 3; y++) {
				do {
					randConstellationColumn = getRandom(11);
					randConstellationRow = getRandom(16);
					if (x == copyProtectionRightX && y == copyProtectionRightY) {
						_copyProtectionDisplayedConstellationsData[(y * 3) + x] = _copyProtectionsConstellationsData[(_copyProtectionRightConstellationRow * _copyProtectionNumColumns) + _copyProtectionRightConstellationColumn];
					} else {
						_copyProtectionDisplayedConstellationsData[(y * 3) + x] = _copyProtectionsConstellationsData[(randConstellationRow * _copyProtectionNumColumns) + randConstellationColumn];
					}
				} while (randConstellationColumn == _copyProtectionRightConstellationColumn && randConstellationRow == _copyProtectionRightConstellationRow);
			}
		}

		drawCopyProtection(numRightAnswers);
		_vm->_gMgr->setFadeSpeed(10);
		_vm->_gMgr->dealFadeIn();
		uint8 currY = 0;
		uint8 currX = 0;
		bool isEnterPressed = false;
		while (isEnterPressed == false) {
			drawCopyProtection(numRightAnswers);
			_vm->_gMgr->drawRectangleOutline((currX * 48) + 86, (currY * 48) + 10, (currX * 48) + 130, (currY * 48) + 54, currSelectionBorderColor);
			currSelectionBorderColor += currSelectionBorderColorStep;
			if (currSelectionBorderColor == 0x3f) {
				currSelectionBorderColorStep = -1;
			}
			if (currSelectionBorderColor == 0x38) {
				currSelectionBorderColorStep = 1;
			}
			_vm->_gMgr->paintBackbuffer_mouse();
			updateInputStatus();
			if (_vm->isQuitRequested()) {
				numErrors = 3;
				break;
			}
			if (_movementStatus & 1) {
				if (currY > 0) {
					currY--;
					waitForNoInput();
					if (_vm->isQuitRequested()) {
						numErrors = 3;
						break;
					}
				}
			}
			if (_movementStatus & 2) {
				if (currY < 2) {
					currY++;
					waitForNoInput();
					if (_vm->isQuitRequested()) {
						numErrors = 3;
						break;
					}
				}
			}
			if (_movementStatus & 4) {
				if (currX > 0) {
					currX--;
					waitForNoInput();
					if (_vm->isQuitRequested()) {
						numErrors = 3;
						break;
					}
				}
			}
			if (_movementStatus & 8) {
				if (currX < 2) {
					currX++;
					waitForNoInput();
					if (_vm->isQuitRequested()) {
						numErrors = 3;
						break;
					}
				}
			}
			if (_lastPressedKey == Common::KeyCode::KEYCODE_RETURN || _lastPressedKey == Common::KeyCode::KEYCODE_KP_ENTER) {
				isEnterPressed = true;
				waitForNoInput();
				if (_vm->isQuitRequested()) {
					numErrors = 3;
					break;
				}
			}
		}
		if (currX == copyProtectionRightX && currY == copyProtectionRightY) {
			numErrors = 0;
			numRightAnswers++;
		} else {
			numErrors++;
		}
	}
	delete[] _copyProtectionStarsGraphics;
	_copyProtectionStarsGraphics = nullptr;
	delete[] _copyProtectionInstructionsSentence;
	_copyProtectionInstructionsSentence = nullptr;
	delete[] _copyProtectionTitleSentence;
	_copyProtectionTitleSentence = nullptr;
	waitForNoInput();
	if (numErrors == 0) {
		return true;
	}
	return false;
}
void GameManager::drawCopyProtection(uint8 numRightAnswers) {
	char buf[4];
	uint16 xOffset;
	uint8 y;
	uint8 stringWidth;
	strncpy(buf, "A", 4);
	_vm->_gMgr->decodeFrame(0, 0, 0, _copyProtectionStarsGraphics);
	y = 160;
	stringWidth = _vm->_txtMgr->calcStringWidth(_copyProtectionTitleSentence + 1);
	xOffset = (_COMET_XRESOLUTION - stringWidth) / 2;
	_vm->_txtMgr->drawOutlinedString(xOffset, y, _copyProtectionTitleSentence + 1, 0xc1, 0x5d);
	y = 180;
	stringWidth = _vm->_txtMgr->calcStringWidth(_copyProtectionInstructionsSentence + 1);
	xOffset = (_COMET_XRESOLUTION - stringWidth - 20) / 2;
	buf[0] = _copyProtectionRightConstellationColumn + 0x41;
	_vm->_txtMgr->drawOutlinedString(xOffset, y, _copyProtectionInstructionsSentence + 1, 0xc1, 0x5d);
	_vm->_txtMgr->drawOutlinedString(xOffset + stringWidth + 10, y, buf, 0xc1, 0x5d);
	_vm->_txtMgr->drawOutlinedString(xOffset + stringWidth + 20, y, _vm->_txtMgr->printDecimalNumber(_copyProtectionRightConstellationRow + 1), 0xc1, 0x5d);
	for (uint8 x = 0; x < 3; x++) {
		for (uint8 _y = 0; _y < 3; _y++) {
			_vm->_gMgr->setDrawAreaX((x * 48) + 88, (x * 48) + 128);
			_vm->_gMgr->setDrawAreaY((_y * 48) + 12, (_y * 48) + 52);
			for (uint8 i = 0; i < _copyProtectionDisplayedConstellationsData[(_y * 3) + x].numStars; i++) {
				_vm->_gMgr->decodeFrame(_copyProtectionDisplayedConstellationsData[(_y * 3) + x].stars[i].frameIdx, _copyProtectionDisplayedConstellationsData[(_y * 3) + x].stars[i].x + (x * 48) + 88, _copyProtectionDisplayedConstellationsData[(_y * 3) + x].stars[i].y + (_y * 48) + 12, _copyProtectionStarsGraphics);
			}
		}
	}
	_vm->_gMgr->setDrawAreaX(0, _COMET_XRESOLUTION - 1);
	_vm->_gMgr->setDrawAreaY(0, _COMET_YRESOLUTION - 1);
}

void GameManager::tryTakingObject() {
	Rectangle r;
	getMainCharSightArea(r, 0, 50);
	uint16 hotspotIdx = findIntersectingHotspot(r);
	if (hotspotIdx != 0) {
		hotspotIdx &= 0x00ff;
		Hotspot *hotspot = &_vm->_gameState.hotspots[hotspotIdx];
		if (hotspot->objType == 0) {
			_vm->_gameState.objectsFlags[hotspot->objIdx] = 1;
			_lastTakenObjId = hotspot->objIdx;
			hotspot->isActive = false;
			_vm->_txtMgr->sayDescription(hotspot->objIdx, 10, _vm->_txtMgr->getObjNames());
			_vm->_gameState.numObjsTaken++;
		} else {
			_vm->_txtMgr->sayDescription(4, 10, _vm->_txtMgr->getSysMessages());
		}
	}
}
void GameManager::handleInputAndMenus() {
	_currentlyHandledKey = _lastPressedKey;
	if (_vm->isCD() && _vm->_moMgr->getRightBut()) {
		_currentlyHandledKey = Common::KEYCODE_TAB;
	}
	switch (_currentlyHandledKey) {
	case Common::KEYCODE_TAB:
		handleMenu();
		break;
	case Common::KEYCODE_t:
		_isTalk = true;
		break;
	case Common::KEYCODE_u:
		setActiveObject();
		break;
	case Common::KEYCODE_i:
		if (_vm->isCD()) {
			_vm->_moMgr->setMouseCursor(1, nullptr);
			_vm->_moMgr->setMouseVisibility(true);
		}
		handleDiary();
		break;
	case Common::KEYCODE_o:
		if (_vm->isCD()) {
			_vm->_moMgr->setMouseCursor(1, nullptr);
			_vm->_moMgr->setMouseVisibility(true);
		}
		chooseObject();
		break;
	case Common::KEYCODE_p:
		handlePause();
		break;
	case Common::KEYCODE_RETURN:
	case Common::KEYCODE_KP_ENTER:
		enterPressed();
		break;
	case Common::KEYCODE_d:
		if (_vm->isCD()) {
			_vm->_moMgr->setMouseCursor(1, nullptr);
			_vm->_moMgr->setMouseVisibility(true);
		}
		handleDiskMenu();
		break;
	case Common::KEYCODE_g:
		_isGet = true;
		break;
	case Common::KEYCODE_l:
		_isLook = true;
		break;
	case Common::KEYCODE_m:
		tryMap();
		break;
	default:
		return;
		break;
	}
	waitForNoInput();
}
void GameManager::tryMap() {
	if (_vm->_txtMgr->isTextDisplayed()) {
		return;
	}
	if (_vm->_gameState.movementMask != 0) {
		return;
	}
	if (waitingForEnter()) {
		return;
	}
	if (_vm->isCD() && _vm->_spMgr->isSpeechActive()) {
		return;
	}
	uint16_t gameVarA = 0x81;
	uint16_t gameVarB = 0x7a;
	if (_vm->isCD()) {
		gameVarA = 0x07;
		gameVarB = 0x08;
	}
	if (_vm->_gameState.gameVars[gameVarA] == 1) {
		return;
	}
	if (_vm->_gameState.gameVars[gameVarB] == 1 || _vm->_gameState.gameVars[gameVarB] == 2) {
		return;
	}
	if (_vm->_gameState.currPakNum == 0 &&
		((_vm->_gameState.currRoomNum >= 0 && _vm->_gameState.currRoomNum < 0x17) ||
		 (_vm->_gameState.currRoomNum >= 0x1e && _vm->_gameState.currRoomNum < 0x35) ||
		 (_vm->_gameState.currRoomNum >= 0x3c && _vm->_gameState.currRoomNum < 0x53))) {
		handleMap();
		_vm->_gameState.objectsFlags[0] = 1;
	}
	if (_vm->_gameState.currPakNum == 6 && _vm->_gameState.currRoomNum >= 0 && _vm->_gameState.currRoomNum < 0x17) {
		handleMap();
		_vm->_gameState.objectsFlags[0] = 1;
	}
}
void GameManager::handleMap() {
	startCoords startCoords[23];
	hotspotCoords hotspotsCoords[10];
	destIdxs destIdxs[10];
	uint16 retVal = 0;

	uint16 hotspotmask_varIdx = 2;
	uint16 isNight_varIdx = 0x59;
	uint16 v1_varIdx = 0x72;
	uint16 v2_varIdx = 0x3d;
	uint16 v3_varIdx = 0x5c;
	if (_vm->isCD()) {
		hotspotmask_varIdx = 2;
		isNight_varIdx = 3;
		v1_varIdx = 4;
		v2_varIdx = 5;
		v3_varIdx = 6;

	}

	memcpy(startCoords, mapRoomsStartCoords, sizeof(startCoords));
	memcpy(hotspotsCoords, mapHotspotsCoords, sizeof(hotspotsCoords));
	memcpy(destIdxs, mapDestsPakRoomIdxs, sizeof(destIdxs));
	uint16 minX = 0x40, maxX = 0x10d, minY = 0x41, maxY = 0xbb, xStep = 8, yStep = 8;
	if (_vm->isCD()) {
		_vm->_moMgr->setMouseCursor(1, nullptr);
		_vm->_moMgr->setMouseVisibility(true);
	}
	int16 activeHotspotsBitmaks = _vm->_gameState.gameVars[hotspotmask_varIdx];
	int16 isNight = _vm->_gameState.gameVars[isNight_varIdx];
	int16 var_u1 = _vm->_gameState.gameVars[v1_varIdx];
	char destNames[200];
	for (uint8 i = 0; i < 10; i++) {
		_vm->_txtMgr->getSentence(2, i + 40, &destNames[i * 20]);
	}
	uint16 mapCurrXPos = 160;
	uint16 mapCurrYPos = 160;
	if (_vm->isCD()) {
		_vm->_moMgr->setMouseVisibility(false);
	}
	uint16 startRoom = _vm->_gameState.currRoomNum;
	while (startRoom > 30) {
		startRoom -= 30;
	}
	mapCurrXPos = startCoords[startRoom].x;
	mapCurrYPos = startCoords[startRoom].y;
	_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->lockMainSurface(), _vm->_gMgr->getBackbuffer());
	_vm->_gMgr->unlockMainSurface();
	if (_vm->isCD()) {
		_vm->_moMgr->warpMouse(mapCurrXPos, mapCurrYPos);
	}
	while (retVal == 0) {
		//TODO
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return;
		}
		if (_vm->isCD()) {
			int16 mx = _vm->_moMgr->getCurrX();
			int16 my = _vm->_moMgr->getCurrY();
			if (mx > minX && mx < maxX) {
				mapCurrXPos = mx;
			}
			else if (mx >= maxX) {
				mapCurrXPos = maxX - 1;
			}
			else {
				mapCurrXPos = minX + 1;
			}
			if (my > minY && my < maxY) {
				mapCurrYPos = my;
			}
			else if (my >= maxY) {
				mapCurrYPos = maxY - 1;
			}
			else {
				mapCurrYPos = minY + 1;
			}
		}
		uint16 mov = _movementStatus;
		if (!_vm->isCD()) {
			if ((mov & kRightFlag) && mapCurrXPos < 268) {
				mapCurrXPos += 8;
			}
			if ((mov & kLeftFlag) && mapCurrXPos > 82) {
				mapCurrXPos -= 8;
			}
			if ((mov & kUpFlag) && mapCurrYPos > 68) {
				mapCurrYPos -= 8;
			}
			if ((mov & kDownFlag) && mapCurrYPos < 186) {
				mapCurrYPos += 8;
			}
		}
		else {
			if (mov & kRightFlag) {
				mapCurrXPos += xStep;
				if (mapCurrXPos >= maxX) {
					mapCurrXPos = maxX - 1;
				}
			}
			if (mov & kLeftFlag) {
				mapCurrXPos -= xStep;
				if (mapCurrXPos <= minX) {
					mapCurrXPos = minX + 1;
				}
			}
			if (mov & kUpFlag) {
				mapCurrYPos -= yStep;
				if (mapCurrYPos <= minY) {
					mapCurrYPos = minY + 1;
				}
			}
			if (mov & kDownFlag) {
				mapCurrYPos += yStep;
				if (mapCurrYPos >= maxY) {
					mapCurrYPos = maxY - 1;
				}
			}
			_vm->_moMgr->warpMouse(mapCurrXPos, mapCurrYPos);

		}
		_vm->_gMgr->decodeFrame(0x32, 0, 0, _vm->_gMgr->_iconsGraphics);
		if (_lastPressedKey == Common::KeyCode::KEYCODE_ESCAPE) {
			retVal = 1;
		}
		if (_vm->isCD() && _vm->_moMgr->getRightBut()) {
			retVal = 1;
		}
		uint16 isHotspot = 0;
		for (uint8 i = 0; i < 10; i++) {
			uint16 tmpMask = 1;
			for (uint8 j = 0; j < i; j++) {
				tmpMask <<= 1;
			}
			if (hotspotsCoords[i].left < mapCurrXPos &&
				hotspotsCoords[i].right > mapCurrXPos &&
				hotspotsCoords[i].top < mapCurrYPos &&
				hotspotsCoords[i].bottom > mapCurrYPos &&
				(tmpMask & activeHotspotsBitmaks) != 0) {
				uint16 x = 0;
				if (_vm->_txtMgr->calcStringWidth(&destNames[(i * 20) + 1]) + mapCurrXPos - 2 >= 283) {
					x = _vm->_txtMgr->calcStringWidth(&destNames[(i * 20) + 1]) + mapCurrXPos - 285;
				}
				_vm->_txtMgr->drawOutlinedString(mapCurrXPos - 2 - x, mapCurrYPos - 6, &destNames[(i * 20) + 1], 0x77, 0x78);
				isHotspot |= 1;
				if (_lastPressedKey == Common::KeyCode::KEYCODE_KP_ENTER || _lastPressedKey == Common::KeyCode::KEYCODE_RETURN ||
					(_vm->isCD() && _vm->_moMgr->getLeftBut())) {
					_vm->_gameState.newPakNum = destIdxs[i].pak;
					_vm->_gameState.newRoomNum = destIdxs[i].room;
					if (isNight == 1) {
						_vm->_gameState.newPakNum += 6;
					} else {
						if (var_u1 >= 2) {
							_vm->_gameState.newRoomNum += (var_u1 - 1) * 30;
						}
					}
					if (startRoom == 7 || startRoom == 8) {
						if (_vm->_gameState.gameVars[v2_varIdx] == 2 &&
							_vm->_gameState.gameVars[v3_varIdx] == 0 &&
							i != 6 && i != 7 && i != 4) {
							_vm->_gameState.newRoomNum = 36;
						}
					}
					retVal = 2;
				}
			}
		}
		if (isHotspot == 0) {
			_vm->_gMgr->decodeFrame(0x33, mapCurrXPos, mapCurrYPos, _vm->_gMgr->_iconsGraphics);
		}
		_vm->_gMgr->paintBackbuffer_mouse();
	}
	waitForNoInput();
	if (_vm->isCD()) {
		_vm->_moMgr->setMouseVisibility(true);
	}
}

Hotspot *GameManager::getFirstInactiveHotspot() {
	for (uint8 i = 0; i < 20; i++) {
		if (_vm->_gameState.hotspots[i].isActive == false) {
			return &_vm->_gameState.hotspots[i];
		}
	}
	return nullptr;
}
void GameManager::handleMenu() {
	uint8 maxIconNumber = 6;
	uint8 status = 0;
	uint8 frameIdx = 0;

	int16 mouseTarget = 0; //only for CD
	Common::KeyCode operation = Common::KEYCODE_INVALID; //only for CD

	_numMenusOnScreen++;
	waitForNoInput();
	if (_vm->isCD()) {
		_vm->_audioMgr->stopDigi();
		_vm->_moMgr->setMouseVisibility(false);
	}

	uint8 *video = _vm->_gMgr->lockMainSurface();
	_vm->_gMgr->copyVideoBuffer(video, _vm->_gMgr->getBackground());
	_vm->_gMgr->copyVideoBuffer(video, _vm->_gMgr->getBackbuffer());
	_vm->_gMgr->unlockMainSurface();
	if (_vm->isCD()) {
		_vm->_moMgr->setMouseCursor(1, nullptr);
		_vm->_moMgr->setMouseVisibility(true);
	}
	while (status == 0) {
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return;
		}

		frameIdx++;
		if (_vm->isCD()) {
			mouseTarget = _vm->_moMgr->getCurrentTarget(MouseManager::Targets::MAINMENU, maxIconNumber+1, -1);
			if (mouseTarget >= 0) {
				_vm->_gameState.selectedMenuItem = mouseTarget;
			}

		}

		_vm->_gMgr->drawIconFrame(_vm->_gameState.selectedMenuItem, frameIdx);
		_vm->_gMgr->paintBackbuffer_mouse();

		if (_vm->isCD()) {
			if (_movementStatus == 0 && (!_vm->_moMgr->getLeftBut()) && (!_vm->_moMgr->getRightBut()) && _lastPressedKey == Common::KEYCODE_INVALID) {
				continue;
			}
			operation = Common::KEYCODE_INVALID;
			if (_vm->_moMgr->getRightBut()) {
				operation = Common::KEYCODE_ESCAPE;
			}
			else if (_vm->_moMgr->getLeftBut()) {
				if (mouseTarget != -1) {
					switch (_vm->_gameState.selectedMenuItem) {
					case 0: operation = Common::KEYCODE_t; break;
					case 1: operation = Common::KEYCODE_g; break;
					case 2: operation = Common::KEYCODE_u; break;
					case 3: operation = Common::KEYCODE_l; break;
					case 4: operation = Common::KEYCODE_o; break;
					case 5: operation = Common::KEYCODE_SEMICOLON; break;
					case 6: operation = Common::KEYCODE_d; break;
					}
				}
			}
		}

		if (_movementStatus & 0x08) {
			if (_vm->_gameState.selectedMenuItem == maxIconNumber) {
				if (_vm->isCD() && mouseTarget == _vm->_gameState.selectedMenuItem) {
					_vm->_moMgr->warpMouseOffset(-(_vm->_moMgr->getTarget(MouseManager::Targets::MAINMENU, maxIconNumber).left - _vm->_moMgr->getTarget(MouseManager::Targets::MAINMENU, 0).left)
						,0
					);
				}

				_vm->_gameState.selectedMenuItem = 0;
			} else {
				if (_vm->isCD() && mouseTarget == _vm->_gameState.selectedMenuItem) {
					_vm->_moMgr->warpMouseOffset(
						_vm->_moMgr->getTarget(MouseManager::Targets::MAINMENU, _vm->_gameState.selectedMenuItem+1).left -
						_vm->_moMgr->getTarget(MouseManager::Targets::MAINMENU, _vm->_gameState.selectedMenuItem).left
						, 0
					);
				}
				_vm->_gameState.selectedMenuItem++;
			}
		}
		if (_movementStatus & 0x04) {
			if (_vm->_gameState.selectedMenuItem == 0) {
				if (_vm->isCD() && mouseTarget == _vm->_gameState.selectedMenuItem) {
					_vm->_moMgr->warpMouseOffset(_vm->_moMgr->getTarget(MouseManager::Targets::MAINMENU, maxIconNumber).left - _vm->_moMgr->getTarget(MouseManager::Targets::MAINMENU, 0).left
						, 0
					);
				}
				_vm->_gameState.selectedMenuItem = maxIconNumber;
			} else {
				if (_vm->isCD() && mouseTarget == _vm->_gameState.selectedMenuItem) {
					_vm->_moMgr->warpMouseOffset(-(
						_vm->_moMgr->getTarget(MouseManager::Targets::MAINMENU, _vm->_gameState.selectedMenuItem).left -
						_vm->_moMgr->getTarget(MouseManager::Targets::MAINMENU, _vm->_gameState.selectedMenuItem-1).left)
						, 0
					);
				}
				_vm->_gameState.selectedMenuItem--;
			}
		}
		if (operation == Common::KEYCODE_INVALID) {
			operation = _lastPressedKey;
		}

		switch (operation) {
		case Common::KeyCode::KEYCODE_o:
			_vm->_gameState.selectedMenuItem = 4;
			_vm->_gMgr->drawIconFrame(_vm->_gameState.selectedMenuItem, frameIdx);
			_vm->_gMgr->paintBackbuffer_mouse();
			status = chooseObject();
			break;
		case Common::KeyCode::KEYCODE_u://
			_vm->_gameState.selectedMenuItem = 2;
			_vm->_gMgr->drawIconFrame(_vm->_gameState.selectedMenuItem, frameIdx);
			_vm->_gMgr->paintBackbuffer_mouse();
			setActiveObject();
			status = 1;
			break;
		case Common::KeyCode::KEYCODE_i:
		case Common::KeyCode::KEYCODE_SEMICOLON:
			_vm->_gameState.selectedMenuItem = 5;
			_vm->_gMgr->drawIconFrame(_vm->_gameState.selectedMenuItem, frameIdx);
			_vm->_gMgr->paintBackbuffer_mouse();
			status = handleDiary();
			break;
		case Common::KeyCode::KEYCODE_ESCAPE:
			status = 1;
			break;
		case Common::KeyCode::KEYCODE_t://
			_isTalk = true;
			_vm->_gameState.selectedMenuItem = 0;
			_vm->_gMgr->drawIconFrame(_vm->_gameState.selectedMenuItem, frameIdx);
			_vm->_gMgr->paintBackbuffer_mouse();
			status = 1;
			break;
		case Common::KeyCode::KEYCODE_g://
			_isGet = true;
			_vm->_gameState.selectedMenuItem = 1;
			_vm->_gMgr->drawIconFrame(_vm->_gameState.selectedMenuItem, frameIdx);
			_vm->_gMgr->paintBackbuffer_mouse();
			status = 1;
			break;
		case Common::KeyCode::KEYCODE_l://
			_isLook = true;
			_vm->_gameState.selectedMenuItem = 3;
			_vm->_gMgr->drawIconFrame(_vm->_gameState.selectedMenuItem, frameIdx);
			_vm->_gMgr->paintBackbuffer_mouse();
			status = 1;
			break;
		case Common::KeyCode::KEYCODE_d://
			_vm->_gameState.selectedMenuItem = 6;
			_vm->_gMgr->drawIconFrame(_vm->_gameState.selectedMenuItem, frameIdx);
			_vm->_gMgr->paintBackbuffer_mouse();
			status = handleDiskMenu();
			break;
		case Common::KeyCode::KEYCODE_KP_ENTER:
		case Common::KeyCode::KEYCODE_RETURN:
			switch (_vm->_gameState.selectedMenuItem) {
			case 0:
				_isTalk = true;
				status = 1;
				break;
			case 1:
				_isGet = true;
				status = 1;
				break;
			case 2:
				setActiveObject();
				status = 1;
				break;
			case 3:
				_isLook = true;
				status = 1;
				break;
			case 4:
				status = chooseObject();
				break;
			case 5:
				status = handleDiary();
				break;
			case 6:
				status = handleDiskMenu();
				break;
			}
			break;
		}

		waitForNoInput();
	}
	waitForNoInput();
	_numMenusOnScreen--;
	_vm->_gMgr->loadStageBackground();
}
void GameManager::setActiveObject() {
	for (int16 i = 0; i < 256; i++) {
		if (_vm->_gameState.objectsFlags[i] == 2) {
			_vm->_gameState.objectsFlags[i] = 1;
		}
	}
	if (_vm->_gameState.activeObject != -1 && _vm->_gameState.objectsFlags[_vm->_gameState.activeObject] == 1) {
		_vm->_gameState.objectsFlags[_vm->_gameState.activeObject] = 2;
	}
}

uint8 GameManager::chooseObject() {
	uint16 buffer[0x200];
	uint16 objCount = 0;
	uint8 status = 0;
	uint8 numItemsPerPage = 10;
	uint16 selectedIdx = 0;
	uint16 firstVisibleIdx = 0;
	uint32 delayCounter = 0;
	uint16 frameIdx = 0;
	int16 mouseTarget = 0; //only for CD
	Common::KeyCode operation = Common::KEYCODE_INVALID; //only for CD

	_numMenusOnScreen++;
	if (_vm->isCD()) {
		_vm->_audioMgr->stopDigi();
		_vm->_moMgr->setMouseVisibility(false);
	}
	if (_numMenusOnScreen == 1) {
		_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->lockMainSurface(), _vm->_gMgr->getBackground());
		_vm->_gMgr->unlockMainSurface();
	}
	if (_vm->isCD()) {
		_vm->_moMgr->setMouseVisibility(true);
	}
	waitForNoInput();
	for (uint16 i = 0; i < 255; i++) {
		if (_lastTakenObjId != 0 && i == _lastTakenObjId) {
			this->_vm->_gameState.activeObject = i;
			_lastTakenObjId = 0;
		}
		if (_vm->_gameState.objectsFlags[i] >= 1) {
			buffer[(objCount * 2)] = _vm->_gameState.objectsFlags[i];
			buffer[(objCount * 2) + 1] = i;
			if (i == this->_vm->_gameState.activeObject) {
				if (objCount < 5) {
					firstVisibleIdx = 0;
				} else {
					firstVisibleIdx = objCount - 5;
				}
				selectedIdx = objCount;
			}
			objCount++;
		}
	}

	if (objCount == 0) {
		_vm->_gMgr->drawObjectsWindow(objCount, buffer, firstVisibleIdx, selectedIdx, 0);
		_vm->_gMgr->paintBackbuffer_mouse();
		while (_lastPressedKey != Common::KeyCode::KEYCODE_INVALID) {
			updateInputStatus();
			if (_vm->isQuitRequested()) {
				break;
			}
		}
		waitForNoInput();
		status = 0;
	} else {
		while (status == 0) {
			updateInputStatus();
			if (_vm->isQuitRequested()) {
				status = 2;
				break;
			}
			if (_vm->isCD()) {
				uint16 numTargets = (objCount - firstVisibleIdx) >= 10 ? 10 : objCount - firstVisibleIdx;
				mouseTarget = _vm->_moMgr->getCurrentTarget(MouseManager::Targets::OBJSMENU, numTargets, -1);
				if (mouseTarget >= 0) {
					selectedIdx = firstVisibleIdx + mouseTarget;
				}

			}
			_vm->_gMgr->drawObjectsWindow(objCount, buffer, firstVisibleIdx, selectedIdx, frameIdx);
			_vm->_gMgr->paintBackbuffer_mouse();
			if (_vm->isCD()) {
				if (_movementStatus == 0 && (!_vm->_moMgr->getLeftBut()) && (!_vm->_moMgr->getRightBut()) && _lastPressedKey == Common::KEYCODE_INVALID) {
					continue;
				}
				operation = Common::KEYCODE_INVALID;
				if (_vm->_moMgr->getRightBut()) {
					operation = Common::KEYCODE_ESCAPE;
				}
				else if (_vm->_moMgr->getLeftBut()) {
					switch (mouseTarget) {
					case -1:break;
					case -3:
						if (selectedIdx > firstVisibleIdx) {
							selectedIdx--;
						}
						else {
							if (firstVisibleIdx > 0) {
								firstVisibleIdx--;
								selectedIdx--;
							}
						}
						break;
					case -2:
						if ((selectedIdx - firstVisibleIdx) < (numItemsPerPage - 1) && (objCount - 1) > selectedIdx) {
							selectedIdx++;
						}
						else if((firstVisibleIdx+numItemsPerPage)<objCount){
							firstVisibleIdx++;
							selectedIdx++;

						}
						break;
					default: operation = Common::KEYCODE_u; break;
					}
				}
			}
			if (_movementStatus & kDownFlag) {
				if (selectedIdx - firstVisibleIdx >= numItemsPerPage - 1 || objCount - 1 <= selectedIdx) {
					if (firstVisibleIdx + numItemsPerPage < objCount) {
						firstVisibleIdx++;
						selectedIdx++;
					}
				} else {
					if (_vm->isCD() && mouseTarget >= 0 && (firstVisibleIdx + mouseTarget) != selectedIdx) {
						_vm->_moMgr->warpMouseOffset(0,
							_vm->_moMgr->getTarget(MouseManager::Targets::OBJSMENU, selectedIdx + 3).top - _vm->_moMgr->getTarget(MouseManager::Targets::OBJSMENU, selectedIdx+2).top
						);
					}
					selectedIdx++;
				}
			}
			if (_movementStatus & kUpFlag) {
				if (selectedIdx <= firstVisibleIdx) {
					if (firstVisibleIdx > 0) {
						firstVisibleIdx--;
						selectedIdx--;
					}
				} else {
					if (_vm->isCD() && mouseTarget >= 0 && (firstVisibleIdx + mouseTarget) != selectedIdx) {
						_vm->_moMgr->warpMouseOffset(0,
							-1*(_vm->_moMgr->getTarget(MouseManager::Targets::OBJSMENU, selectedIdx + 3).top - _vm->_moMgr->getTarget(MouseManager::Targets::OBJSMENU, selectedIdx + 2).top)
						);
					}
					selectedIdx--;
				}
			}
			if (operation == Common::KEYCODE_INVALID) {
				operation = _lastPressedKey;
			}
			if (operation == Common::KeyCode::KEYCODE_ESCAPE) {
				status = 2;
			}
			if (operation == Common::KeyCode::KEYCODE_KP_ENTER || operation == Common::KeyCode::KEYCODE_RETURN) {
				for (uint16 i = 0; i < 255; i++) {
					if (_vm->_gameState.objectsFlags[i] == 2) {
						_vm->_gameState.objectsFlags[i] = 1;
					}
					this->_vm->_gameState.activeObject = buffer[(selectedIdx * 2) + 1];
					status = 1;
				}
			}
			if (operation == Common::KeyCode::KEYCODE_u) {
				for (uint16 i = 0; i < 255; i++) {
					if (_vm->_gameState.objectsFlags[i] == 2) {
						_vm->_gameState.objectsFlags[i] = 1;
					}
				}
				this->_vm->_gameState.activeObject = buffer[(selectedIdx * 2) + 1];
				_vm->_gameState.objectsFlags[this->_vm->_gameState.activeObject] = 2;
				status = 1;
			}
			while ((_movementStatus & (kUpFlag | kDownFlag)) && (delayCounter < 400000)) { // <-this is a really unrealiable timing, it won't work on modern CPUs
				updateInputStatus();
				if (_vm->isQuitRequested()) {
					status = 2;
					break;
				}
				delayCounter++;
				//
			}
			if (delayCounter <= 399000) {
				waitForNoInput();
				delayCounter = 0;
			}
			if (_movementStatus == 0) {
				delayCounter = 0;
			}
			frameIdx++;
		}
		status = 2 - status;
	}
	if (status == 0) {
		removeSubmenus(4);
	} else {
		_vm->_gMgr->loadStageBackground();
	}
	_numMenusOnScreen--;
	return status;
}
/*
	void GameManager::setActiveObject() {

	}
	*/

uint8 GameManager::handleDiary() {
	int retVal = 0;
	uint16 realChapter = 0;
	int16 maxPage = _vm->_gameState.gameVars[1];
	int16 currPage = maxPage;
	int16 prevPage = -1; //CD only
	int16 mouseTarget = -1; //CD only
	Common::KeyCode operation = Common::KEYCODE_INVALID;
	_numMenusOnScreen++;
	_vm->_gMgr->diaryFade(false, currPage, maxPage);
	if (_vm->isCD()) {
		realChapter = _vm->_gameState.textChapterID;
		_vm->_gameState.textChapterID = 7;
		_vm->_spMgr->openNARarchive(7);
	}
	waitForNoInput();
	while (retVal == 0) {
		_vm->_gMgr->drawDiarySentences(currPage, maxPage, 64);
		_vm->_gMgr->paintBackbuffer_mouse();
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return 0;
		}
		if (_vm->isCD() && _vm->_gameState.speechOptions != 0) {
			if (currPage != prevPage) {
				if (currPage == 0) {
					_vm->_spMgr->stopSpeech();
				}
				else {
					_vm->_spMgr->startSpeech(currPage);
				}
			}
			else {
				prevPage = currPage;
				_vm->_spMgr->handleSpeech();
			}
		}
		operation = Common::KEYCODE_INVALID;
		if (_vm->isCD()) {
			mouseTarget = _vm->_moMgr->getCurrentTarget(MouseManager::Targets::DIARY, 2, -1);
			switch (mouseTarget) {
			case -1:
				_vm->_moMgr->setMouseCursor(1);
				break;
			case 0:
				_vm->_moMgr->setMouseCursor(0, _vm->_moMgr->loadedCursors[3]);
				break;
			case 1:
				_vm->_moMgr->setMouseCursor(0, _vm->_moMgr->loadedCursors[4]);
				break;
			}
			_vm->_moMgr->setMouseVisibility(true);
			if (_movementStatus == 0 && (!_vm->_moMgr->getLeftBut()) && (!_vm->_moMgr->getRightBut()) && _lastPressedKey == Common::KEYCODE_INVALID) {
				continue;
			}
			if (_vm->_moMgr->getRightBut()) {
				operation = Common::KEYCODE_ESCAPE;
			}
			else if (_vm->_moMgr->getLeftBut()) {
				if (mouseTarget != -1) {
					switch (mouseTarget) {
					case 0:
						if(currPage > 0) {
							_vm->_audioMgr->stopDigi();
							_vm->_gMgr->diaryFade(true, currPage, maxPage);
							_vm->_gMgr->drawDiaryPagesAnimation(false);
							currPage--;
						}
						break;
					case 1:
						if (currPage < maxPage) {
							_vm->_gMgr->diaryFade(true, currPage, maxPage);
							_vm->_gMgr->drawDiaryPagesAnimation(true);
							currPage++;
						}
						break;
					}
					_vm->_gMgr->diaryFade(false, currPage, maxPage);

				}

			}


		}
		if (operation == Common::KEYCODE_INVALID) {
			operation = _lastPressedKey;
		}
		if (operation == Common::KeyCode::KEYCODE_RETURN || operation == Common::KeyCode::KEYCODE_KP_ENTER) {
			retVal = 1;
		}
		if (operation == Common::KeyCode::KEYCODE_ESCAPE) {
			retVal = 2;
		}
		if (_movementStatus & kRightFlag ) {
			if (_vm->isCD() && mouseTarget == 0) {
				_vm->_moMgr->warpMouseOffset(-114, 0);
			}
			if (currPage < maxPage) {
				if (_vm->isCD()) { _vm->_audioMgr->stopDigi(); }
				_vm->_gMgr->diaryFade(true, currPage, maxPage);
				_vm->_gMgr->drawDiaryPagesAnimation(true);
				currPage++;
				_vm->_gMgr->diaryFade(false, currPage, maxPage);
			}
		}
		if (_movementStatus & kLeftFlag) {
			if (_vm->isCD() && mouseTarget == 1) {
				_vm->_moMgr->warpMouseOffset(114, 0);
			}
			if (currPage > 0) {
				if (_vm->isCD()) { _vm->_audioMgr->stopDigi(); }
				_vm->_gMgr->diaryFade(true, currPage, maxPage);
				_vm->_gMgr->drawDiaryPagesAnimation(false);
				currPage--;
				_vm->_gMgr->diaryFade(false, currPage, maxPage);
			}
		}
		waitForNoInput();
	}
	waitForNoInput();
	if (_vm->isCD()) {

		_vm->_gameState.textChapterID = realChapter;
		_vm->_spMgr->openNARarchive(realChapter);
	}

	retVal = 2 - retVal;
	if (retVal == 0) {
		removeSubmenus(5);
	} else {
		_vm->_gMgr->loadStageBackground();
	}
	_numMenusOnScreen--;
	return retVal;
}
uint8 GameManager::handleDiskMenu() {
	int16 selectedItem = 0;
	uint8 retVal = 0;
	int16 mouseTarget = -1;
	Common::KeyCode operation = Common::KEYCODE_INVALID;
	selectedItem = _lastDiskMenuSelectedItem;
	_numMenusOnScreen++;
	if (_vm->isCD()) {
		_vm->_moMgr->setMouseVisibility(false);
	}
	if (_numMenusOnScreen == 1) {
		_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->lockMainSurface(), _vm->_gMgr->getBackground());
		_vm->_gMgr->unlockMainSurface();
	}
	if (_vm->isCD()) {
		_vm->_moMgr->setMouseVisibility(true);
	}
	while (waitForNoInput(), retVal == 0) {
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return 0;
		}
		operation = Common::KEYCODE_INVALID;
		if (_vm->isCD()) {
			mouseTarget = _vm->_moMgr->getCurrentTarget(MouseManager::Targets::DISKMENU, 4, -1);
			if (mouseTarget != -1) {
				selectedItem = mouseTarget;
			}
		}
		_vm->_gMgr->drawDiskMenu(selectedItem);
		_vm->_gMgr->paintBackbuffer_mouse();
		if (_vm->isCD()) {
			if (_movementStatus == 0 && (!_vm->_moMgr->getLeftBut()) && (!_vm->_moMgr->getRightBut()) && _lastPressedKey == Common::KEYCODE_INVALID) {
				continue;
			}

			if (_vm->_moMgr->getRightBut()) {
				operation = Common::KEYCODE_ESCAPE;
			}
			else if (_vm->_moMgr->getLeftBut()) {
				switch (mouseTarget) {
				case -1:break;
				case 0: operation = Common::KEYCODE_s; break;
				case 1: operation = Common::KEYCODE_l; break;
				case 2: operation = Common::KEYCODE_t; break;
				case 3: operation = Common::KEYCODE_x; break;
				}
			}
		}
		if (_movementStatus & kDownFlag) {
			if (selectedItem < 3) {
				if (_vm->isCD() && mouseTarget == selectedItem) {
					_vm->_moMgr->warpMouseOffset(0,
						_vm->_moMgr->getTarget(MouseManager::Targets::DISKMENU, selectedItem+1).top - _vm->_moMgr->getTarget(MouseManager::Targets::DISKMENU, selectedItem).top
						);
				}
				selectedItem++;
			} else {
				if (_vm->isCD() && mouseTarget == selectedItem) {
					_vm->_moMgr->warpMouseOffset(0,
						-1*(_vm->_moMgr->getTarget(MouseManager::Targets::DISKMENU, selectedItem).top - _vm->_moMgr->getTarget(MouseManager::Targets::DISKMENU, 0).top)
					);
				}
				selectedItem = 0;
			}
		}
		if (_movementStatus & kUpFlag) {
			if (selectedItem == 0) {
				selectedItem = 3;
			} else {
				if (_vm->isCD() && mouseTarget == selectedItem) {
					_vm->_moMgr->warpMouseOffset(0,
						_vm->_moMgr->getTarget(MouseManager::Targets::DISKMENU, 3).top - _vm->_moMgr->getTarget(MouseManager::Targets::DISKMENU, 0).top
					);
				}
				selectedItem--;
			}
		}
		if (operation == Common::KEYCODE_INVALID) {
			operation = _lastPressedKey;
		}
		switch (operation) {
		case Common::KeyCode::KEYCODE_ESCAPE:
			retVal = 2;
			break;
		case Common::KeyCode::KEYCODE_RETURN:
		case Common::KeyCode::KEYCODE_KP_ENTER: {
			switch (selectedItem) {
			case 0:
				retVal = handleSave();
				break;
			case 1:
				retVal = handleLoad();
				break;
			case 2:
				retVal = handleSettings();
				break;
			case 3:
				retVal = (this->*handleQuit)();
				break;
			}
		} break;
		case Common::KeyCode::KEYCODE_s: {
			selectedItem = 0;
			_vm->_gMgr->drawDiskMenu(selectedItem);
			_vm->_gMgr->paintBackbuffer_mouse();
			retVal = handleSave();
		} break;
		case Common::KeyCode::KEYCODE_l: {
			selectedItem = 1;
			_vm->_gMgr->drawDiskMenu(selectedItem);
			_vm->_gMgr->paintBackbuffer_mouse();
			retVal = handleLoad();

		} break;
		case Common::KeyCode::KEYCODE_t: {
			selectedItem = 2;
			_vm->_gMgr->drawDiskMenu(selectedItem);
			_vm->_gMgr->paintBackbuffer_mouse();
			retVal = handleSettings();

		} break;
		case Common::KeyCode::KEYCODE_x: {
			selectedItem = 3;
			_vm->_gMgr->drawDiskMenu(selectedItem);
			_vm->_gMgr->paintBackbuffer_mouse();
			retVal = (this->*handleQuit)();

		} break;
		}
	}
	waitForNoInput();
	_lastDiskMenuSelectedItem = selectedItem;
	retVal = 2 - retVal;
	if (retVal == 0) {
		removeSubmenus(6);
	} else {
		_vm->_gMgr->loadStageBackground();
	}
	_numMenusOnScreen--;
	return retVal;
}
uint8 GameManager::handleSave() {
	Common::DumpFile f;
	uint8 retVal = 0;
	int16 mouseTarget = -1;
	_numMenusOnScreen++;
	waitForNoInput();
	if (loadSaveFilesDescriptions() == -1) {
		//TODO manage save error here
		showNoticeBox(_vm->_txtMgr->getSentencePtr(8, _vm->_txtMgr->getSysMessages()));
		if (_vm->_gmMgr->forceLoading != 0) {
			(this->*handleQuit)();
			if (_vm->isQuitRequested()) {
				return 0;
			}
		}
		removeSubmenus(7);
		_numMenusOnScreen--;
		return 0;
	}
	if (_vm->isCD()) {
		MouseManager::mouseTarget tmp = _vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, _vm->_gameState.selectedSaveLoadSlot);
		_vm->_moMgr->warpMouse((tmp.left + tmp.right) / 2, (tmp.top + tmp.bottom) / 2);
	}
	while (retVal == 0) {
		if (_vm->isCD()) {
			mouseTarget = _vm->_moMgr->getCurrentTarget(MouseManager::Targets::FILESMENU, 10, -1);
			if (mouseTarget != -1) {
				_vm->_gameState.selectedSaveLoadSlot = mouseTarget;
			}
		}
		_vm->_gMgr->drawSaveMenu(_vm->_gameState.selectedSaveLoadSlot, false, _saveFilesDescription);
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return 0;
		}
		_vm->_gMgr->paintBackbuffer_mouse();
		Common::KeyCode key = Common::KeyCode::KEYCODE_INVALID;
		if (_vm->isCD()) {
			if (_vm->_moMgr->getRightBut()) {
				key = Common::KeyCode::KEYCODE_ESCAPE;
			}else if(_vm->_moMgr->getLeftBut()) {
				if (mouseTarget >= 0) {
					key = Common::KeyCode::KEYCODE_RETURN;
				}
			}
		}
		if (key == Common::KeyCode::KEYCODE_INVALID) {
			key = _lastPressedKey;
		}
		for (uint8 i = 0; i < 26; i++) {
			if (_allowedKeys[i] == key) {
				if (handleSaveFilenameTyping(key, _vm->_gameState.selectedSaveLoadSlot)) {
					key = Common::KeyCode::KEYCODE_RETURN;
				}
			}
		}
		if ((key == Common::KeyCode::KEYCODE_RETURN || key == Common::KeyCode::KEYCODE_KP_ENTER) && _saveFilesDescription[_vm->_gameState.selectedSaveLoadSlot][0] != 0) {
			_vm->_txtMgr->hideText();
			_roomZoomLevel = 0;
			char filename[256];
			snprintf(filename, 256, "%s%02d.sav", _vm->_gameDescription->desc.gameId, _vm->_gameState.selectedSaveLoadSlot);
			Common::SaveFileManager *sfm = g_system->getSavefileManager();
			Common::OutSaveFile *osf = sfm->openForSaving(filename, false);
			//			if (!f.open(filename)) {
			if (!osf) {
				showNoticeBox(_vm->_txtMgr->getSentencePtr(8, _vm->_txtMgr->getSysMessages()));
			} else {
				//				CometXorSerializer s(0, &f);
				CometXorSerializer s(0, osf);
				if (_vm->isCD()) {
					// WHY?!!! Who is NED?
					char ned[] = { 0x4e,0x45,0x44,0x00 };
					s.syncBytes(reinterpret_cast<byte*>(ned), 4);

				}
				s.syncBytes(reinterpret_cast<byte *>(&_saveFilesDescription[_vm->_gameState.selectedSaveLoadSlot]), 30);
				_vm->_gameState.serialize(s, _vm->isCD());
				//				f.close();
				delete osf;
			}
			// test salvataggio per versione DOS
			
			snprintf(filename, 256, "CTU%01d.sav", _vm->_gameState.selectedSaveLoadSlot);
			osf = sfm->openForSaving(filename, false);
			//			if (!f.open(filename)) {
			if (!osf) {
				showNoticeBox(_vm->_txtMgr->getSentencePtr(8, _vm->_txtMgr->getSysMessages()));
			} else {
				//				CometXorSerializer s(0, &f);
				CometXorSerializer s(0, osf);
				if (_vm->isCD()) {
					// WHY?!!! Who is NED?
					char ned[] = { 0x4e,0x45,0x44,0x00 };
					s.syncBytes(reinterpret_cast<byte*>(ned), 4);

				}
				s.syncBytes(reinterpret_cast<byte *>(&_saveFilesDescription[_vm->_gameState.selectedSaveLoadSlot]), 30);
				_vm->_gameState.serialize(s, _vm->isCD());
				//				f.close();
				delete osf;
			}
			if (_vm->isCD()) {
				showNoticeBoxTimed(_vm->_txtMgr->getSentencePtr(19, _vm->_txtMgr->getSysMessages()));

			}
			retVal = 1;
		}
		if (key == Common::KeyCode::KEYCODE_ESCAPE) {
			retVal = 2;
		}
		if (_movementStatus & (kDownFlag)) {
			if (_vm->_gameState.selectedSaveLoadSlot < 9) {
				if (_vm->isCD() && _vm->_gameState.selectedSaveLoadSlot == mouseTarget) {
					_vm->_moMgr->warpMouseOffset(0,
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, mouseTarget +1).top-
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, mouseTarget).top
						);
				}
				_vm->_gameState.selectedSaveLoadSlot++;
			} else {
				if (_vm->isCD() && _vm->_gameState.selectedSaveLoadSlot == mouseTarget) {
					_vm->_moMgr->warpMouseOffset(0,-(
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU,9).top -
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, 0).top
					));
				}
				_vm->_gameState.selectedSaveLoadSlot = 0;
			}
		}
		if (_movementStatus & (kUpFlag)) {
			if (_vm->_gameState.selectedSaveLoadSlot == 0) {
				if (_vm->isCD() && _vm->_gameState.selectedSaveLoadSlot == mouseTarget) {
					_vm->_moMgr->warpMouseOffset(0,
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, 9).top -
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, 0).top
					);
				}
				_vm->_gameState.selectedSaveLoadSlot = 9;
			} else {
				if (_vm->isCD() && _vm->_gameState.selectedSaveLoadSlot == mouseTarget) {
					_vm->_moMgr->warpMouseOffset(0, -(
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, mouseTarget).top -
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, mouseTarget - 1).top
						));
				}
				_vm->_gameState.selectedSaveLoadSlot--;
			}
		}
		waitForNoInput();
	}
	waitForNoInput();
	retVal = 2 - retVal;
	if (retVal == 0) {
		removeSubmenus(7);
	} else {
		_vm->_gMgr->loadStageBackground();
	}
	_numMenusOnScreen--;
	return retVal;
}

void GameManager::showNoticeBox(const char *message) {
	uint16 stringWidth = _vm->_txtMgr->calcStringWidth(message);
	uint16 x1 = ((320 - stringWidth) / 2) - 10;
	uint16 x2 = x1 + stringWidth + 20;
	uint16 y1 = 85;
	uint16 y2 = 119;
	_vm->_gMgr->drawLine(x1, y1, x2, y1, 0x15);
	_vm->_gMgr->drawLine(x1, y1, x1, y2, 0x15);
	_vm->_gMgr->drawLine(x2, y1, x2, y2, 0x11);
	_vm->_gMgr->drawLine(x1, y2, x2, y2, 0x11);
	_vm->_gMgr->drawRectangleFilled(x1 + 1, y1 + 1, x2 - 1, y2 - 1, 0x13);
	updateFontDataAndColor(0x78);
	_vm->_txtMgr->drawString(x1 + 11, y1 + 11, _vm->_gMgr->getBackbuffer(), message);
	updateFontDataAndColor(0x77);
	_vm->_txtMgr->drawString(x1 + 10, y1 + 10, _vm->_gMgr->getBackbuffer(), message);
	waitForNoInput();
	do {
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return;
		}
	} while (_lastPressedKey == Common::KeyCode::KEYCODE_INVALID && _mouseButtons==0);
}
void GameManager::showNoticeBoxTimed(const char* message) {
	uint16 stringWidth = _vm->_txtMgr->calcStringWidth(message);
	uint16 x1 = ((320 - stringWidth) / 2) - 10;
	uint16 x2 = x1 + stringWidth + 20;
	uint16 y1 = 85;
	uint16 y2 = 119;
	_vm->_gMgr->drawLine(x1, y1, x2, y1, 0x15);
	_vm->_gMgr->drawLine(x1, y1, x1, y2, 0x15);
	_vm->_gMgr->drawLine(x2, y1, x2, y2, 0x11);
	_vm->_gMgr->drawLine(x1, y2, x2, y2, 0x11);
	_vm->_gMgr->drawRectangleFilled(x1 + 1, y1 + 1, x2 - 1, y2 - 1, 0x13);
	updateFontDataAndColor(0x78);
	_vm->_txtMgr->drawString(x1 + 11, y1 + 11, _vm->_gMgr->getBackbuffer(), message);
	updateFontDataAndColor(0x77);
	_vm->_txtMgr->drawString(x1 + 10, y1 + 10, _vm->_gMgr->getBackbuffer(), message);
	waitForNoInput();
	for (uint16_t j = 0; j < 100; j++) {
		_vm->_gMgr->waitVRetrace();
	}
}
uint8 GameManager::handleSaveFilenameTyping(::Common::KeyCode key, uint8 selectedItem) {
	char prevDescription[30];
	uint8 retVal = 0;
	uint8 numChars = 0;
	uint16 xOffs = 95;
	uint16 yOffs = 64;
	uint16 height = 12;
	memcpy(prevDescription, _saveFilesDescription[selectedItem], 30);
	_saveFilesDescription[selectedItem][0] = 0;
	do {
		_vm->_gMgr->drawRectangleFilled(94, (selectedItem * height) + 63, 232, (selectedItem * height) + 72, 0x13);
		updateFontDataAndColor(0x78);
		uint16 x1 = (135 - _vm->_txtMgr->calcStringWidth(_saveFilesDescription[selectedItem])) / 2;
		_vm->_txtMgr->drawString(x1 + xOffs + 1, yOffs + 1 + (height * selectedItem), _vm->_gMgr->getBackbuffer(), _saveFilesDescription[selectedItem]);
		_vm->_gmMgr->updateFontDataAndColor(0x77);
		_vm->_txtMgr->drawString(x1 + xOffs, yOffs + (height * selectedItem), _vm->_gMgr->getBackbuffer(), _saveFilesDescription[selectedItem]);
		_vm->_gMgr->paintBackbuffer_mouse();
		if (key == Common::KeyCode::KEYCODE_RETURN || key == Common::KeyCode::KEYCODE_KP_ENTER || _mouseButtons==2) {
			_saveFilesDescription[selectedItem][numChars] = 0;
			retVal = 1;
		}
		if (key == Common::KeyCode::KEYCODE_ESCAPE || _mouseButtons==1) {
			memcpy(_saveFilesDescription[selectedItem], prevDescription, 30);
			retVal = 2;
		}
		if (key == Common::KeyCode::KEYCODE_BACKSPACE && numChars > 0) {
			numChars--;
			_saveFilesDescription[selectedItem][numChars] = 0;
			waitForNoInput();
		}
		if ((_movementStatus & kConfirm) && _vm->_txtMgr->calcStringWidth(_saveFilesDescription[selectedItem]) < 130) {
			_saveFilesDescription[selectedItem][numChars++] = ' ';
			_saveFilesDescription[selectedItem][numChars] = 0;
			waitForNoInput();
		}
		for (uint8 i = 0; i < 36; i++) {
			if (key == _allowedKeys[i] && numChars < 29 && _vm->_txtMgr->calcStringWidth(_saveFilesDescription[selectedItem]) < 128) {
				if (i < 26) {
					//it's a letter
					if (_vm->isCD() && (i == 0x18 || i == 0x19) && _vm->_gameState.selectedLanguageID == 3) {
							if (i == 0x18) {
								// if Y, turns into Z
								_saveFilesDescription[selectedItem][numChars++] = 0x5A;
								continue;
							}
							else if (i == 0x19) {
								// if Z, turns into Y
								_saveFilesDescription[selectedItem][numChars++] = 0x59;
								continue;

							}
						}
					else {
						_saveFilesDescription[selectedItem][numChars] = i + 0x41;
						numChars++;
					}
				} else {
					//it's a number
					_saveFilesDescription[selectedItem][numChars] = i + 0x16;
					numChars++;
				}
				_saveFilesDescription[selectedItem][numChars] = 0;
				waitForNoInput();
			}
		}
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return 0;
		}
		key = _lastPressedKey;
	} while (retVal == 0);
	waitForNoInput();
	retVal = 2 - retVal;
	return retVal;
}

int32 GameManager::loadSaveFilesDescriptions() {
	/*
	uint32 retVal = 0;
	Common::File f;
	for (uint8 i = 0; i < 10; i++) {
		_saveFilesDescription[i][0] = 0;
		char filename[256];
		snprintf(filename, 256, "%s%02d.sav", _vm->_gameDescription->gameId, i);
		if (f.open(filename)) {
			f.read(_saveFilesDescription[i], 30);
			retVal = 1;
			f.close();
		}
	}
	return retVal;
	*/
	uint32 retVal = 0;
	//	Common::File f;
	Common::SaveFileManager *sfm = g_system->getSavefileManager();
	for (uint8 i = 0; i < 10; i++) {
		_saveFilesDescription[i][0] = 0;
		char filename[256];
		snprintf(filename, 256, "%s%02d.sav", _vm->_gameDescription->desc.gameId, i);
		Common::InSaveFile *isf = sfm->openForLoading(filename);
		if (isf) {
			if (_vm->isCD()) {
				char tmp[] = { 0,0,0,0 };
				isf->read(tmp, 4);
				if (memcmp(tmp, "NED", 4) != 0) {
					_saveFilesDescription[i][0] = 0;
					delete isf;
					continue;
				}
			}
			isf->read(_saveFilesDescription[i], 30);
			retVal = 1;
			delete isf;
		}
	}
	return retVal;
}
void GameManager::loadFileAndInit(CometXorSerializer &s) {

	_vm->_gMgr->clearBackbuffer();
	_vm->_gMgr->paintBackbuffer_mouse();
	_vm->_audioMgr->stopDigi();
	killAllNPCs();
	_vm->_gameState.actors[0].resourceIdx = -1;
	freeResourcesExceptMainActor();
	freeStageResources();
	_vm->_gameState.serialize(s,_vm->isCD());
	setPakNumber(_vm->_gameState.currPakNum, _vm->_gameState.currRoomNum);
	_vm->_scMgr->loadAndPrepareRoomScripts(_roomsFilename, _vm->_gameState.currRoomNum, true);
	_vm->_gMgr->loadStage(true);
	reloadResources();
	_vm->_gMgr->fadePalette(_vm->_gMgr->mainGamePalette, _vm->_gMgr->tmpPalette, _vm->_gameState.paletteFadeLevel, 256);
	_vm->_gMgr->uploadPalette(_vm->_gMgr->tmpPalette);
	if (_vm->_gameState.paletteRedTintFactor != 0) {
		_vm->_gMgr->setPaletteTint(_vm->_gameState.paletteRedTintFactor);
	}
	_vm->_audioMgr->setVoiceParams(_vm->_gameState.musicVolume, 0, 0x2000);
}

uint8 GameManager::handleLoad() {
	uint8 retVal = 0;
	int16 mouseTarget = -1;
	_numMenusOnScreen++;
	waitForNoInput();
	if (loadSaveFilesDescriptions() == -1) {
		showNoticeBox(_vm->_txtMgr->getSentencePtr(7, _vm->_txtMgr->getSysMessages()));
		if (forceLoading) {
			_numMenusOnScreen--;
			char filename[256];
			char filedescbuf[30];
			snprintf(filename, 256, "init.sav");
			Common::File f;
			if (!f.open(filename)) {
				return 0;
			}
			defaultLanguage = _vm->_gameState.selectedLanguageID;
			_vm->_txtMgr->hideText();
			_roomZoomLevel = 0;
			CometXorSerializer s(&f, 0);
			if (_vm->isCD()) {
				// we meet again, NED!
				char tmp[] = { 0,0,0,0 };
				s.syncBytes(reinterpret_cast<byte*>(tmp), 4);
				if (strncmp(tmp, "NED", 4)!=0) {
					f.close();
					showNoticeBox(_vm->_txtMgr->getSentencePtr(1, _vm->_txtMgr->getSysMessages()));
					removeSubmenus(8);
					_numMenusOnScreen--;
					return 0;
				}
			}
			s.syncBytes(reinterpret_cast<byte *>(filedescbuf), 30);
			loadFileAndInit(s);
			f.close();
			if (_vm->_gameState.isAlternatePaletteActive) {
				_vm->_gameState.isAlternatePaletteActive = false;
			}
			_vm->_txtMgr->loadSystemAndObjects();
			_vm->_audioMgr->stopDigi();
			if (_vm->isCD()) {
				_vm->_spMgr->stopSpeech();
				_vm->_spMgr->openNARarchive(_vm->_gameState.textChapterID);
			}
			return 1;
		} else {
			removeSubmenus(8);
			_numMenusOnScreen--;
			return 0;
		}
	}
	while (retVal == 0) {
		if (_vm->isCD()) {
			mouseTarget = _vm->_moMgr->getCurrentTarget(MouseManager::Targets::FILESMENU, 10, -1);
			if (mouseTarget != -1) {
				if (_saveFilesDescription[mouseTarget][0] != 0) {
					_vm->_gameState.selectedSaveLoadSlot = mouseTarget;
				}
			}
		}
		_vm->_gMgr->drawSaveMenu(_vm->_gameState.selectedSaveLoadSlot, true, _saveFilesDescription);
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return 0;
		}
		_vm->_gMgr->paintBackbuffer_mouse();
		if (_vm->isCD() && _mouseButtons == 0 && _lastPressedKey == Common::KeyCode::KEYCODE_INVALID && _movementStatus == 0) {
			continue;
		}

		Common::KeyCode key = Common::KeyCode::KEYCODE_INVALID;
		if (_vm->isCD()) {
			if (_vm->_moMgr->getRightBut()) {
				key = Common::KeyCode::KEYCODE_ESCAPE;
			}
			else if (_vm->_moMgr->getLeftBut()) {
				key = Common::KeyCode::KEYCODE_RETURN;
			}
		}
		if (key == Common::KeyCode::KEYCODE_INVALID) {
			key = _lastPressedKey;
		}
		if ((key == Common::KeyCode::KEYCODE_RETURN || key == Common::KeyCode::KEYCODE_KP_ENTER) && _saveFilesDescription[_vm->_gameState.selectedSaveLoadSlot][0] != 0) {
			_vm->_gMgr->clearBackbuffer();
			_vm->_gMgr->paintBackbuffer_mouse();
			if (_vm->_gameState.isAlternatePaletteActive) {
				memcpy(_vm->_gMgr->mainGamePalette, _vm->_gMgr->normalPalette, 0x300);
				memcpy(_vm->_gMgr->tmpPalette, _vm->_gMgr->normalPalette, 0x300);
				_vm->_gMgr->uploadPalette(_vm->_gMgr->mainGamePalette);
				_vm->_gameState.isAlternatePaletteActive = 0;
			}
			_vm->_txtMgr->hideText();
			if (_vm->isCD()) {
				_vm->_spMgr->stopSpeech();
			}
			_roomZoomLevel = 0;
			char filename[256];
			snprintf(filename, 256, "%s%02d.sav", _vm->_gameDescription->desc.gameId, _vm->_gameState.selectedSaveLoadSlot);
			Common::SaveFileManager *sfm = g_system->getSavefileManager();
			Common::InSaveFile *isf = sfm->openRawFile(filename); //openForLoading defaults to compressed stream
			//			if (f.open(filename)) {
			if (isf) {
				defaultLanguage = _vm->_gameState.selectedLanguageID;
				//				CometXorSerializer s(&f, 0);
				CometXorSerializer s(isf, 0);
				if (_vm->isCD()) {
					// we meet again, NED!
					char tmp[] = { 0,0,0,0 };
					s.syncBytes(reinterpret_cast<byte*>(tmp), 4);
					if (strncmp(tmp, "NED", 4)!=0) {
						delete isf;
						showNoticeBox(_vm->_txtMgr->getSentencePtr(1, _vm->_txtMgr->getSysMessages()));
						removeSubmenus(8);
						_numMenusOnScreen--;
						return 0;
					}
				}
				s.syncBytes(reinterpret_cast<byte *>(&_saveFilesDescription[_vm->_gameState.selectedSaveLoadSlot]), 30);
				loadFileAndInit(s);
				//				f.close();
				delete isf;
				if (_vm->_gameState.isAlternatePaletteActive) {
					_vm->_gameState.isAlternatePaletteActive = false;
				}
				_vm->_txtMgr->loadSystemAndObjects();
				_vm->_audioMgr->stopDigi();
				if (_vm->isCD()) {
					_vm->_spMgr->stopSpeech();
					_vm->_spMgr->openNARarchive(_vm->_gameState.textChapterID);
				}
			}
			retVal = 1;
		}
		if (key == Common::KeyCode::KEYCODE_ESCAPE) {
			if (forceLoading != 0) {
				(this->*handleQuit)();
			}
			retVal = 2;
		}
		if (_movementStatus & (kDownFlag)) {
			if (_vm->_gameState.selectedSaveLoadSlot < 9) {
				if (_vm->isCD() && _vm->_gameState.selectedSaveLoadSlot == mouseTarget) {
					_vm->_moMgr->warpMouseOffset(0,
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, mouseTarget + 1).top -
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, mouseTarget).top
					);
				}
				_vm->_gameState.selectedSaveLoadSlot++;
			} else {
				if (_vm->isCD() && _vm->_gameState.selectedSaveLoadSlot == mouseTarget) {
					_vm->_moMgr->warpMouseOffset(0, -(
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, 9).top -
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, 0).top
						));
				}
				_vm->_gameState.selectedSaveLoadSlot = 0;
			}
		}
		if (_movementStatus & (kUpFlag)) {
			if (_vm->_gameState.selectedSaveLoadSlot == 0) {
				if (_vm->isCD() && _vm->_gameState.selectedSaveLoadSlot == mouseTarget) {
					_vm->_moMgr->warpMouseOffset(0,
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, 9).top -
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, 0).top
					);
				}
				_vm->_gameState.selectedSaveLoadSlot = 9;
			} else {
				_vm->_gameState.selectedSaveLoadSlot--;
				if (_vm->isCD() && _vm->_gameState.selectedSaveLoadSlot == mouseTarget) {
					_vm->_moMgr->warpMouseOffset(0, -(
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, mouseTarget).top -
						_vm->_moMgr->getTarget(MouseManager::Targets::FILESMENU, mouseTarget - 1).top
						));
				}
			}
		}
		waitForNoInput();
	}
	waitForNoInput();
	retVal = 2 - retVal;
	if (retVal == 0) {
		removeSubmenus(8);
	} else {
		_vm->_gMgr->loadStageBackground();
	}
	_numMenusOnScreen--;
	return retVal;
}
uint8 GameManager::handleSettings() {
	uint16 retVal = 0;
	uint16 currSelection = _lastSettingsMenuSelectedItem, maxSelection = 5, framesCounter = 0;
	uint16 volume = _vm->_gameState.musicVolume / 8;
	uint16 digiVolume = _vm->_gameState.digiVolume / 8;
	uint16 textSpeed = 2 - _vm->_gameState.textDurationScale;
	uint16 gameSpeed = _vm->_gameState.targetFPS;
	uint16 diskNum = 3, prevDiskNum = diskNum;
	uint16 langId = _vm->_gameState.selectedLanguageID;
	int16 mouseTarget = -1;
	bool mustWaitNoInput = true;

	uint8_t musicVolIdx = 0;
	uint8_t digiVolIdx = 1;
	uint8_t textSpeedIdx = 2;
	uint8_t gameSpeedIdx = 3;
	uint8_t diskSelectIdx = 4;
	uint8_t langSelectIdx = 5;
	uint8_t confirmIdx = 255;
	uint8_t speechOptionsIdx = 255;
	if (_vm->isCD()) {
		textSpeedIdx = 255;
		diskSelectIdx = 255;
		speechOptionsIdx = 2;
		langSelectIdx = 4;
		confirmIdx = 6;
	}


	_numMenusOnScreen++;
	waitForNoInput();
	while (retVal == 0) {
		if (_vm->isCD()) {
			mouseTarget = _vm->_moMgr->getCurrentTarget(MouseManager::Targets::OPTIONSMENU, 13, -1);
			if (mouseTarget != -1) {
				currSelection = mouseTarget;
			}
			if (currSelection >= 30) {
				currSelection -= 30;
			}
		}
		_vm->_audioMgr->setVoiceParams(volume * 8, 0, 0x2000);
		framesCounter++;
		if (framesCounter == 0x20) {
			framesCounter = 0;
		}

		_vm->_gMgr->drawSettingsMenu(currSelection, volume, digiVolume, textSpeed, gameSpeed, _vm->isCD()?0:diskNum, langId, framesCounter);
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return 0;
		}
		_vm->_gMgr->paintBackbuffer_mouse();
		if (_vm->isCD() && _mouseButtons == 0 && _lastPressedKey == Common::KeyCode::KEYCODE_INVALID && _movementStatus == 0) {
			continue;
		}

		Common::KeyCode key = Common::KeyCode::KEYCODE_INVALID;
		if (_vm->isCD()) {
			if (_vm->_moMgr->getRightBut()) {
				key = Common::KeyCode::KEYCODE_ESCAPE;
			}
			else if (_vm->_moMgr->getLeftBut() && mouseTarget != -1) {
				int16 mx = MAX<int16>(_vm->_moMgr->getCurrX(), 127);
				mx = MIN<int16>(mx, 189);
				switch (mouseTarget) {
				case 0:
					mustWaitNoInput = false;
					volume = (mx - 127) / 4;
					break;
				case 1:
					mustWaitNoInput = false;
					digiVolume = (mx - 127) / 4;
					break;
				case 2:
				case 32:
					_vm->_gameState.speechOptions++;
					if (_vm->_gameState.speechOptions > 2) {
						_vm->_gameState.speechOptions = 0;
					}
					break;
				case 3:
					mustWaitNoInput = false;
					gameSpeed = (mx - 127) / 4;
					break;
				case 4:
				case 34:
					do {
						langId++;
						if (langId == 5) {
							langId = 0;
						}
					} while (!_vm->_txtMgr->hasLanguageFile(langId));
					break;
				case 6:
					key = Common::KeyCode::KEYCODE_RETURN;
					break;
				case 30:
					volume = 8;
					break;
				case 31:
					digiVolume = 8;
					break;
				case 33:
					gameSpeed = 8;
					break;

				}
			}
		}
		if (key == Common::KeyCode::KEYCODE_INVALID) {
			key = _lastPressedKey;
		}
		if (key == Common::KeyCode::KEYCODE_RETURN || key == Common::KeyCode::KEYCODE_KP_ENTER) {
			if (!_vm->isCD() || (currSelection == 5 || currSelection == 6)) {
				retVal = 1;
			}
		}
		if (key == Common::KeyCode::KEYCODE_ESCAPE) {
			retVal = 2;
		}
		if (_movementStatus & (kDownFlag)) {
			if (currSelection < maxSelection) {
				if (_vm->isCD()&&((currSelection == mouseTarget) || ((currSelection + 30) == mouseTarget))) {
					_vm->_moMgr->warpMouseOffset(0,
						_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection + 1).top -
						_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection).top
					);
					if (_vm->_moMgr->getCurrX() > _vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, 0).right) {
						_vm->_moMgr->warpMouse(_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, 0).right, -1);
					}
					if (_vm->_moMgr->getCurrX() <= _vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, 0).left) {
						_vm->_moMgr->warpMouse(_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, 0).left, -1);
					}
				}
				currSelection++;
			} else {
				if (_vm->isCD() && ((currSelection == mouseTarget) || ((currSelection + 30) == mouseTarget))) {
					_vm->_moMgr->warpMouseOffset(0,-(
						_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, maxSelection).top -
						_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, 0).top)
					);
					if (_vm->_moMgr->getCurrX() > _vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection + 1).right) {
						_vm->_moMgr->warpMouse(_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection + 1).right, -1);
					}
					if (_vm->_moMgr->getCurrX() < _vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection + maxSelection + 1).left) {
						_vm->_moMgr->warpMouse(_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection + maxSelection + 1).left, -1);
					}
				}
				currSelection = 0;
			}
		}
		if (_movementStatus & (kUpFlag)) {
			if (currSelection == 0) {
				if (_vm->isCD() && ((currSelection == mouseTarget) || ((currSelection + 30) == mouseTarget))) {
					_vm->_moMgr->warpMouseOffset(0, 
						_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, maxSelection).top -
						_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, 0).top
					);
					if (_vm->_moMgr->getCurrX() >= _vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, maxSelection).right) {
						_vm->_moMgr->warpMouse(_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, maxSelection).right, -1);
					}
					if (_vm->_moMgr->getCurrX() < _vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, maxSelection + maxSelection + 1).left) {
						_vm->_moMgr->warpMouse(_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, maxSelection + maxSelection + 1).left, -1);
					}
				}
				currSelection = maxSelection;
			} else {
				if (_vm->isCD() && ((currSelection == mouseTarget) || ((currSelection + 30) == mouseTarget))) {
					_vm->_moMgr->warpMouseOffset(0, -(
						_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection).top -
						_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection-1).top)
					);
					if (_vm->_moMgr->getCurrX() > _vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection -1).right) {
						_vm->_moMgr->warpMouse(_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection - 1).right, -1);
					}
					if (_vm->_moMgr->getCurrX() < _vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection + maxSelection).left) {
						_vm->_moMgr->warpMouse(_vm->_moMgr->getTarget(MouseManager::Targets::OPTIONSMENU, currSelection + maxSelection).left, -1);
					}
				}
				currSelection--;
			}
		}
		if ((_movementStatus & kRightFlag) && currSelection == musicVolIdx) {
			if (volume < 0x0f) {
				mustWaitNoInput = false;
				volume++;
			}
		}
		if ((_movementStatus & kLeftFlag) && currSelection == musicVolIdx) {
			if (volume > 0) {
				mustWaitNoInput = false;
				volume--;
			}
		}
		if ((_movementStatus & kRightFlag) && currSelection == digiVolIdx) {
			if (digiVolume < 0x0f) {
				mustWaitNoInput = false;
				digiVolume++;
			}
		}
		if ((_movementStatus & kLeftFlag) && currSelection == digiVolIdx) {
			if (digiVolume > 0) {
				mustWaitNoInput = false;
				digiVolume--;
			}
		}
		if ((_movementStatus & kRightFlag) && currSelection == textSpeedIdx) {
			if (textSpeed < 0x02) {
				mustWaitNoInput = false;
				textSpeed++;
			}
		}
		if ((_movementStatus & kLeftFlag) && currSelection == textSpeedIdx) {
			if (textSpeed > 0) {
				mustWaitNoInput = false;
				textSpeed--;
			}
		}
		if (currSelection == speechOptionsIdx && (_movementStatus & (kLeftFlag | kRightFlag))) {
			_vm->_gameState.speechOptions++;
			if (_vm->_gameState.speechOptions > 2) {
				_vm->_gameState.speechOptions = 0;
			}
		}
		if ((_movementStatus & kRightFlag) && currSelection == gameSpeedIdx) {
			if (gameSpeed < 0x0f) {
				mustWaitNoInput = false;
				gameSpeed++;
			}
		}
		if ((_movementStatus & kLeftFlag) && currSelection == gameSpeedIdx) {
			if (gameSpeed > 0) {
				mustWaitNoInput = false;
				gameSpeed--;
			}
		}
		if ((_movementStatus & kRightFlag) && currSelection == diskSelectIdx) {
			if (diskNum < 0x19) {
				diskNum++;
			}
		}
		if ((_movementStatus & kLeftFlag) && currSelection == diskSelectIdx) {
			if (diskNum > 0) {
				diskNum--;
			}
		}
		if ((_movementStatus & kRightFlag) && currSelection == langSelectIdx && langId < 4) {
			do {
				langId++;
				if (langId == 5) {
					langId = 0;
				}

			} while (!_vm->_txtMgr->hasLanguageFile(langId));
		}
		if ((_movementStatus & kLeftFlag) && currSelection == langSelectIdx && langId > 0) {
			do {
				if (langId == 0) {
					langId = 5;
				}
				langId--;

			} while (!_vm->_txtMgr->hasLanguageFile(langId));
		}
		if (!_vm->isCD() || mustWaitNoInput) {
			waitForNoInput();
		}
		else {
			mustWaitNoInput = true;
		}
	}
	waitForNoInput();
	retVal = 2 - retVal;
	_lastSettingsMenuSelectedItem = currSelection;
	if (retVal == 0) {
		removeSubmenus(9);
	} else {
		_vm->_gameState.musicVolume = volume * 8;
		_vm->_gameState.digiVolume = digiVolume * 8;
		_vm->_gameState.textDurationScale = 2 - textSpeed;
		_vm->_gameState.targetFPS = gameSpeed;

		_vm->_gameState.selectedLanguageID = langId;
		_vm->_txtMgr->loadSystemAndObjects();
		_vm->_audioMgr->setVoiceParams(_vm->_gameState.musicVolume, 0, 0x2000);
		_vm->_gMgr->loadStageBackground();
	}
	_numMenusOnScreen--;
	return retVal;
}

uint8 GameManager::handleQuit_floppy() {
	uint8 *prevBB = _vm->_gMgr->getBackbuffer();
	uint8 *vga = _vm->_gMgr->lockMainSurface();
	_vm->_gMgr->setBackbuffer(vga);
	_vm->_gMgr->drawSimpleBox(80, 90, 240, 110, 156);
	_vm->_gmMgr->updateFontDataAndColor(0x78);
	const char *sent = _vm->_txtMgr->getSentencePtr(0, _vm->_txtMgr->getSysMessages());
	uint16 horizOffset = (160 - _vm->_txtMgr->calcStringWidth(sent)) / 2;
	_vm->_txtMgr->drawString(horizOffset + 80, 98, vga, sent);
	_vm->_gmMgr->updateFontDataAndColor(0x77);
	_vm->_txtMgr->drawString(horizOffset + 79, 97, vga, sent);
	_vm->_gMgr->setBackbuffer(prevBB);
	_vm->_gMgr->unlockMainSurface();
	//	_vm->_system->updateScreen();
	_vm->_gMgr->_hfCappedUpdate();
	do {
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return 0;
		}
	} while (_lastPressedKey != 0);
	do {
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return 0;
		}
	} while (_lastPressedKey == 0);
	if ((_lastPressedKey == Common::KeyCode::KEYCODE_o && _vm->_gameState.selectedLanguageID == 0) ||
		(_lastPressedKey == Common::KeyCode::KEYCODE_y && _vm->_gameState.selectedLanguageID == 1) ||
		(_lastPressedKey == Common::KeyCode::KEYCODE_s && _vm->_gameState.selectedLanguageID == 2) ||
		(_lastPressedKey == Common::KeyCode::KEYCODE_j && _vm->_gameState.selectedLanguageID == 3) ||
		(_lastPressedKey == Common::KeyCode::KEYCODE_s && _vm->_gameState.selectedLanguageID == 4)) {
		this->quit();
	}
	return 0;
}

uint8 GameManager::handleQuit_CD() {
	bool mustWaitNoInput = false;
	Rectangle menuRect{87,82,229,126};
	int16 mouseTarget = -1;

	_numMenusOnScreen++;
	waitForNoInput();
	_vm->_moMgr->setMouseCursor(1, nullptr);
	_vm->_moMgr->setMouseVisibility(true);
	const char* sent = _vm->_txtMgr->getSentencePtr(0, _vm->_txtMgr->getSysMessages());
	uint16 horizOffset = (menuRect.right- menuRect.left - _vm->_txtMgr->calcStringWidth(sent)) / 2;
	uint16 vertOffset = (menuRect.bottom - menuRect.top - 8) / 2;
	int16 currentChoice = 1;
	while (true) {
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			return 0;
		}
		_vm->_gMgr->decodeFrame(_vm->_gameState.selectedLanguageID + 94, 0, 0,  _vm->_gMgr->_iconsGraphics);
		uint8* prevBB = _vm->_gMgr->getBackbuffer();
//		uint8* vga = _vm->_gMgr->lockMainSurface();
//		_vm->_gMgr->setBackbuffer(vga);
		_vm->_gmMgr->updateFontDataAndColor(0x78);
		_vm->_txtMgr->drawString(menuRect.left + horizOffset + 1, menuRect.top + vertOffset + 1, prevBB, sent);
		_vm->_gmMgr->updateFontDataAndColor(0x77);
		_vm->_txtMgr->drawString(menuRect.left + horizOffset, menuRect.top + vertOffset, prevBB, sent);
		_vm->_gMgr->setBackbuffer(prevBB);
//		_vm->_gMgr->unlockMainSurface();
		//	_vm->_system->updateScreen();
		_vm->_gMgr->_hfCappedUpdate();
		if (_vm->_moMgr->isInitialized()) {
			mouseTarget = _vm->_moMgr->getCurrentTarget(MouseManager::Targets::QUITMENU, 2, -1);
		}
		if (mouseTarget != -1) {
			currentChoice = mouseTarget;
		}
		MouseManager::mouseTarget tmp = _vm->_moMgr->getTarget(MouseManager::Targets::QUITMENU, currentChoice);
		//	Rectangle choiceRect{ tmp.left,tmp.top,tmp.right, tmp.bottom };
		_vm->_gMgr->drawRectangleOutline(tmp.left, tmp.top, tmp.right, tmp.bottom, 0x77);
		_vm->_gMgr->paintBackbuffer_mouse();
		if (mustWaitNoInput) {
			waitForNoInput();
			mustWaitNoInput = false;
		}
		if (_movementStatus == 0 && (!_vm->_moMgr->getLeftBut()) && (!_vm->_moMgr->getRightBut()) && _lastPressedKey == Common::KEYCODE_INVALID) {
			continue;
		}
		Common::KeyCode operation = Common::KEYCODE_INVALID;
		if (_vm->_moMgr->getRightBut()) {
			operation = Common::KEYCODE_ESCAPE;
		}
		else if (_vm->_moMgr->getLeftBut()) {
			if (currentChoice == 1) {
				operation = Common::KEYCODE_n;
			}
			else if (currentChoice == 0) {
				switch (_vm->_gameState.selectedLanguageID) {
				case 0:operation = Common::KEYCODE_o; break; //oui
				case 1:operation = Common::KEYCODE_y; break; //yes
				case 2:operation = Common::KEYCODE_s; break; //si
				case 3:operation = Common::KEYCODE_j; break; //ja
				case 4:operation = Common::KEYCODE_s; break; //si
				}
			}
		}
		if (operation == Common::KEYCODE_INVALID) {
			operation = _lastPressedKey;
		}
		if ((operation == Common::KeyCode::KEYCODE_o && _vm->_gameState.selectedLanguageID == 0) ||
			(operation == Common::KeyCode::KEYCODE_y && _vm->_gameState.selectedLanguageID == 1) ||
			(operation == Common::KeyCode::KEYCODE_s && _vm->_gameState.selectedLanguageID == 2) ||
			(operation == Common::KeyCode::KEYCODE_j && _vm->_gameState.selectedLanguageID == 3) ||
			(operation == Common::KeyCode::KEYCODE_s && _vm->_gameState.selectedLanguageID == 4) ||
			((operation == Common::KEYCODE_RETURN || operation==Common::KEYCODE_KP_ENTER)&&currentChoice==0)
			) {
			this->quit();
			return 0;
		}
		if (operation == Common::KEYCODE_n) {
			break;
		}
		if (operation == Common::KEYCODE_ESCAPE) {
			break;
		}
		if((operation == Common::KEYCODE_RETURN || operation == Common::KEYCODE_KP_ENTER) && currentChoice == 0){
			break;
		}
		if ((_movementStatus & 0x0f) == 0 && operation != Common::KEYCODE_TAB) {
			continue;
		}
		mustWaitNoInput = true;
		currentChoice = currentChoice == 0 ? 1 : 0;
		if (_vm->_moMgr->isInitialized() && mouseTarget != -1) {
			tmp = _vm->_moMgr->getTarget(MouseManager::Targets::QUITMENU, currentChoice);
			_vm->_moMgr->warpMouse((tmp.left + tmp.right) / 2, (tmp.top + tmp.bottom) / 2);
		}
	}
	removeSubmenus(10);
	_numMenusOnScreen -= 1;
	return 0;

}

void GameManager::quit() {
	ripScreenToBlack(0, 0);
	_vm->_audioMgr->stopDigi();
	_vm->_audioMgr->muteAllVoices();
	Common::Event evq; // = Common::EventType::EVENT_QUIT;
	evq.type = Common::EventType::EVENT_QUIT;
	_evtMgr->pushEvent(evq);
}
void GameManager::ripScreenToBlack(uint16 color, uint16 effect) {
	uint16 v0 = 0; // ???? useless but it's there in the original code
	uint8 *prevBB = _vm->_gMgr->getBackbuffer();
	// The "directrix" of the two mirrored parabolas is a line passing through the two points (pa_x,0) and (pb_x,200).
	// It's not a real directrix, in the mathematical sense. It's just a cheap way to "skew" the parabolas for a better visual effect
	// In order to avoid a line too steep, pa_x and pb_x are taken randomly in the range [70,250] (instead of the whole x range [0,320])
	int32 pa_x = 70 + getRandom(180);
	int32 pb_x = 70 + getRandom(180);
	for (uint8 i = 0; i < 30; i++) {
		uint8 *vga = _vm->_gMgr->lockMainSurface();
		_vm->_gMgr->setBackbuffer(vga);
		for (uint8 j = 0; j < 200; j++) {
			int16 y = j;

			// debug drawing to better understand the effect
			//			_vm->_gMgr->drawLine(pa_x, 0, pb_x, 200, 0x87);
			//			_vm->_gMgr->drawLine(0, 99, 320, 99, 0x87);
			//			_vm->_gMgr->drawLine(0, 100, 320, 100, 0x87);

			// using the equation of line through two points, we get the directrix x-coord for the current y-coord
			int32 directrix_x = ((pa_x * 200) + (y * (pb_x - pa_x))) / 200;
			// abs(j-100) convert the y coords so that the parabola vertex is on the y=100 instead of y=0
			// the 1/10 is the sqrt of the paramter "alpha", which indicates how wide the "cup" of the parabola is.
			// using simple integer division gives the parabola some sort of jagged edge
			int32 tmp_x = abs(j - 100) / 10;
			// (tmp_x*tmp_x) is the parabola value, (i*i) moves it quadratically further from the screen center every passing frame
			// the random(20) give the parabolas their peculiar "ripped" effect
			int32 parab_x = (tmp_x * tmp_x) + (i * i) + getRandom(20);
			//let's mirror/clamp the parabola left and right, and paint the space in between the two parabolas
			int32 x_left = directrix_x - (parab_x / 2);
			if (x_left < 0) {
				x_left = 0;
			}
			int x_right = directrix_x + (parab_x / 2);
			if (x_right > 320) {
				x_right = 320;
			}
			parab_x = x_right - x_left;
			if (parab_x > 0) {
				if (effect != 0) {
					// if effect is != 0, let's fill with rotating colours
					_vm->_gMgr->drawLine(x_left, y, x_right, y, (color + j) - ((j / effect) * effect));
				} else {
					// if effect is == 0, let's fill with a single color
					_vm->_gMgr->drawLine(x_left, y, x_right, y, color);
				}
				_vm->_gMgr->drawLine(x_left - 2, y, x_left - 1, y, 0x87);
				_vm->_gMgr->drawLine(x_right + 1, y, x_right, y, 0x87);
			}
		}
		_vm->_gMgr->setBackbuffer(prevBB);
		_vm->_gMgr->unlockMainSurface();
		_vm->_gMgr->clearBackbuffer();
		//		_vm->_system->updateScreen();
		_vm->_gMgr->_hfCappedUpdate();
	}
	_vm->_gMgr->clearBackbuffer();
	//		_vm->_system->updateScreen();
	_vm->_gMgr->_hfCappedUpdate();
}

void GameManager::removeSubmenus(uint8 idx) {
	uint8 dm = 0;
	switch (idx) {
	case 4:
	case 5:
	case 6:
	case 11:
		if (_numMenusOnScreen == 1) {
			_vm->_gMgr->loadStageBackground();
		}
		if (_numMenusOnScreen != 2) {
			return;
		}
		_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->getBackground(), _vm->_gMgr->getBackbuffer());
		break;
	case 7:
		dm = _vm->isCD() ? 0 : 1;
		if (_numMenusOnScreen == 3) {
			_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->getBackground(), _vm->_gMgr->getBackbuffer());
			_vm->_gMgr->drawIconFrame(6, 0);

			_vm->_gMgr->drawDiskMenu(dm);

		} else if (_numMenusOnScreen == 2) {
			_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->getBackground(), _vm->_gMgr->getBackbuffer());
			_vm->_gMgr->drawDiskMenu(dm);
		}
		break;
	case 8:
		dm = _vm->isCD() ? 1 : 2;
		if (_numMenusOnScreen == 3) {
			_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->getBackground(), _vm->_gMgr->getBackbuffer());
			_vm->_gMgr->drawIconFrame(6, 0);

			_vm->_gMgr->drawDiskMenu(dm);

		} else if (_numMenusOnScreen == 2) {
			_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->getBackground(), _vm->_gMgr->getBackbuffer());
			_vm->_gMgr->drawDiskMenu(dm);
		}
		break;
	case 9:
		dm = _vm->isCD() ? 2 : 3;
		if (_numMenusOnScreen == 3) {
			_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->getBackground(), _vm->_gMgr->getBackbuffer());
			_vm->_gMgr->drawIconFrame(6, 0);

			_vm->_gMgr->drawDiskMenu(dm);

		} else if (_numMenusOnScreen == 2) {
			_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->getBackground(), _vm->_gMgr->getBackbuffer());
			_vm->_gMgr->drawDiskMenu(dm);
		}
		break;
	case 10:
		if (_numMenusOnScreen == 3) {
			_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->getBackground(), _vm->_gMgr->getBackbuffer());
			_vm->_gMgr->drawIconFrame(6, 0);

			_vm->_gMgr->drawDiskMenu(3);

		}
		else if (_numMenusOnScreen == 2) {
			_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->getBackground(), _vm->_gMgr->getBackbuffer());
			_vm->_gMgr->drawDiskMenu(3);
		}
		break;
	}
}

//for encapsulation
/*
	int8 GameManager::getActiveObject() {
		return _activeObject;
	}
	void GameManager::resetActiveObject() {
		_activeObject = -1;
	}
	*/
/*
uint8 GameManager::getObjectFlag(int8 objIdx) {
	if (objIdx >= 0) {
		return _vm->_gameState.objectsFlags[objIdx];
	}
	return 0;
}
*/
bool GameManager::isHotspotInSight() {
	return _hotspotInSight;
}
void GameManager::getCurrHotspotInfo(uint16 &dir, uint16 &x, uint16 &y) {
	dir = _hotspotDirection;
	x = _hotspotX;
	y = _hotspotY;
}
/*
void GameManager::setStage(uint16 stage) {
	_stageIdx = stage;
}

uint16 GameManager::getStage() {
	return _stageIdx;
}
*/
const char *GameManager::getStagesFilename() {
	return _stagesFilename;
}
void GameManager::setFacingDirection(uint8 dir) {
	_facingDirection = dir;
}
/*
void GameManager::setMovementMask(uint16 mask) {
	_movementMask = mask;
}
uint16 GameManager::getMovementMask() {
	return _movementMask;
}
*/
void GameManager::freeResourceFromCharacter(Actor *act) {
	int8 resourceIdx = act->resourceIdx;
	if (resourceIdx != -1) {
		Resource *r = &_vm->_gameState.resources[resourceIdx];
		if (r->data != nullptr && r->type == 0) {
			if (!isResourceUsedByActor(resourceIdx)) {
				removeResourceFromActors(resourceIdx);
				delete[] r->data;
				r->data = nullptr;
			}
		}
	}
}
/*
	void GameManager::setNewRoom(int16 room) {
		_vm->_gameState.newRoomNum = room;
	}
	*/
uint16 GameManager::getStatusVar(uint8 idx) {
	return *_statusVars[idx];
}
void GameManager::setStatusVar(uint8 idx, uint16 val) {
	*_statusVars[idx] = val;
}
uint16 GameManager::getRandom(uint16 max) {
	return _rnd.getRandomNumber(max - 1);
}
int16 *GameManager::getVarPtr(uint16 idx) {
	if (idx < 1000) {
		return _statusVars[idx];
	}
	if (idx < 2000) {
		return &_vm->_gameState.gameVars[idx - 1000];
	}
	return &_vm->_gameState.objectsFlags[idx - 2000];
}
/*
	void GameManager::setNewPak(int16 pak) {
		_newPakNumber = pak;
	}
	*/
bool GameManager::isLook() {
	return _isLook;
}
bool GameManager::isGet() {
	return _isGet;
}
bool GameManager::isUse() {
	return _isUse;
}
bool GameManager::isTalk() {
	return _isTalk;
}
void GameManager::unsetLook() {
	_isLook = false;
}
bool GameManager::waitingForEnter() {
	return _waitingForEnter;
}
void GameManager::unsetWaitForEnter() {
	_waitingForEnter = false;
}
void GameManager::setWaitForEnter() {
	_waitingForEnter = true;
}
void GameManager::setWalkAreaTopLineNumPoints(uint8 numPoints) {
	_topWalkAreaLineStrip.numPoints = numPoints;
}
void GameManager::setWalkAreaTopLinePoint(uint8 pointIdx, uint16 x, uint16 y) {
	_topWalkAreaLineStrip.points[pointIdx].x = x;
	_topWalkAreaLineStrip.points[pointIdx].y = y;
}
/*
	void GameManager::setNumExits(uint8 numExits) {
		_numExits = numExits;
	}
	*/
void GameManager::setExit(uint8 exitIdx, uint8 direction, uint8 pak, uint8 room, uint16 x1, uint16 x2) {
	RoomExit *r = &_vm->_gameState.exits[exitIdx];
	r->direction = direction;
	r->pak = pak;
	r->room = room;
	r->x1 = x1;
	r->x2 = x2;
}
/*
	int16 GameManager::getCurrentPak() {
		return _currPakNumber;
	}
	*/
Actor *GameManager::getMainActor() {
	return _mainActor;
}

void GameManager::debugDraw() {
	char tmpstr[256];
#define _COMET_DEBUG_RED 0xF2
#define _COMET_DEBUG_BLUE 0xF3
#define _COMET_DEBUG_GREEN 0xBF
#define _COMET_DEBUG_PURPLE 0xAD
#define _COMET_DEBUG_YELLOW 0x77
	if (_lastPressedKey != Common::KeyCode::KEYCODE_COMMA) {
		return;
	}

	_vm->_gMgr->setPixelColor(_COMET_DEBUG_RED);
	for (uint16 i = 0; i < 320; i++) {
		uint16 y = _vm->_gameState.topWalkAreaYValues[i];
		_vm->_gMgr->putPixel(i, y);
	}
	for (uint8 i = 0; i < _vm->_gameState.numExits; i++) {
		uint16 x1, y1, x2, y2;
		getExitCoords(i, x1, y1, x2, y2);
		_vm->_gMgr->drawRectangleFilled(x1 - 1, y1 - 1, x2, y2, _COMET_DEBUG_BLUE);
	}
	for (uint8 i = 0; i < _vm->_gameState.numStageElements; i++) {
		StageElement *el = &_vm->_gameState.stageElements[i];

		_vm->_gMgr->drawRectangleFilled(el->halfLeft * 2, el->top, el->halfRight * 2, el->bottom, _COMET_DEBUG_RED);
		//		snprintf(tmpstr, 256, "{%d, %d}", el->halfLeft * 2, el->top);
		//		_vm->_txtMgr->drawOutlinedString(MIN((el->halfLeft * 2)-10, 250), MIN<uint16_t>(el->top-10, 184), tmpstr, _COMET_DEBUG_BLUE, _COMET_DEBUG_GREEN);
	}
	for (uint8 i = 0; i < 10; i++) {
		Actor *actor = &_vm->_gameState.actors[i];
		if (actor->isVisible && actor->life > 0) {
			_vm->_gMgr->drawRectangleFilled(actor->pivotX - 1, actor->pivotY - 1, actor->pivotX + 1, actor->pivotY + 1, _COMET_DEBUG_GREEN);
			_vm->_gMgr->drawRectangleOutline(actor->pivotX - actor->halfWidth, actor->pivotY - actor->height, actor->pivotX + actor->halfWidth, actor->pivotY, _COMET_DEBUG_GREEN);
		}
	}
	for (uint8 i = 0; i < 20; i++) {
		Hotspot *hot = &_vm->_gameState.hotspots[i];
		if (hot->isActive) {
			uint8 color = _COMET_DEBUG_PURPLE;
			if (hot->objType != 0) {
				color = _COMET_DEBUG_YELLOW;
			}
			_vm->_gMgr->drawRectangleFilled(hot->x - 2, hot->y - 2, hot->x + 2, hot->y + 2, color);
		}
	}

	snprintf(tmpstr, 256, "Pak %02x, Room %02x, FPS %02x, targetFPS %02x", _vm->_gameState.currPakNum, _vm->_gameState.currRoomNum, _vm->_gMgr->getFPS(), _vm->_gameState.targetFPS);
	_vm->_gmMgr->updateFontDataAndColor(_COMET_DEBUG_BLUE);
	_vm->_txtMgr->drawString(1, 1, _vm->_gMgr->getBackbuffer(), tmpstr);
	_vm->_gmMgr->updateFontDataAndColor(_COMET_DEBUG_RED);
	_vm->_txtMgr->drawString(0, 0, _vm->_gMgr->getBackbuffer(), tmpstr);
	snprintf(tmpstr, 256, "Mov %04x, Mouse %04x, Key %04x", _movementStatus, _mouseButtons, _lastPressedKey);
	_vm->_gmMgr->updateFontDataAndColor(_COMET_DEBUG_BLUE);
	_vm->_txtMgr->drawString(1, 21, _vm->_gMgr->getBackbuffer(), tmpstr);
	_vm->_gmMgr->updateFontDataAndColor(_COMET_DEBUG_RED);
	_vm->_txtMgr->drawString(0, 20, _vm->_gMgr->getBackbuffer(), tmpstr);
}
uint8 GameManager::getCurrentMovementFlags() {
	return _currentlyHandledMovementFlags;
}
/*
Hotspot *GameManager::getHotspot(uint8 idx) {
	return &_vm->_gameState.hotspots[idx];
}
*/
/*
	int8 GameManager::_getCurrRoom() {
		return _currRoom;
	}
	*/
void GameManager::unsetTalk() {
	_isTalk = false;
}
void GameManager::onUseObject() {
	if (_vm->_gameState.objectsFlags[_vm->_gameState.activeObject] < 1) {
		if (_vm->_gameState.activeObject > 1) {
			int16 i;
			for (i = _vm->_gameState.activeObject - 1; i != -1; i--) {
				if (_vm->_gameState.objectsFlags[i] >= 1) {
					break;
				}
			}
			_vm->_gameState.activeObject = i;
		} else {
			_vm->_gameState.activeObject = -1;
		}
	}
	if (_vm->_gameState.objectsFlags[0] == 2) {
		handleDiary();
		_vm->_gameState.objectsFlags[0] = 1;
	}
}
uint16 GameManager::handleSkullPuzzle() {
	uint16 done = 0;
	int16 skullMouseX = -1, skullMouseY = -1;
	int16 skullMouseTarget = 0;
	if (_vm->isCD()) {
		_vm->_moMgr->setMouseVisibility(false);
	}
	memcpy(skullTilesState, skullInitialTilesState, 6 * 6 * sizeof(uint16));
	skullPuzzleData = _vm->_archMgr->allocateAndGetFile("A07.PAK", 0x18);
	if (_vm->isCD()) {
		_vm->_audioMgr->stopDigi();
		_vm->_audioMgr->loadDigiSubfile(0x4b);
	}
	skullCursorX = 0;
	skullCursorY = 0;
	while (done == 0) {
		updateInputStatus();
		if (_vm->isQuitRequested()) {
			done = 1;
			break;
		}

		if (_vm->isCD()) {
			skullMouseX = _vm->_moMgr->getCurrX();
			skullMouseY = _vm->_moMgr->getCurrY();
			if (skullMouseX < 103) {
				skullMouseX = 103;
				_vm->_moMgr->warpMouse(103, -1);
			}
			if (skullMouseX > 231) {
				skullMouseX = 231;
				_vm->_moMgr->warpMouse(231, -1);
			}
			if (skullMouseY < 44) {
				skullMouseY = 44;
				_vm->_moMgr->warpMouse(-1, 44);
			}
			if (skullMouseY > 171) {
				skullMouseY = 171;
				_vm->_moMgr->warpMouse(-1, 171);
			}
			skullMouseTarget = _vm->_moMgr->getCurrentTarget(MouseManager::Targets::SKULLPUZZLE, 21, -1);
			if (skullMouseTarget >= 0) {
				switch (skullMouseTarget) {
				case 0x00: skullCursorX = 1; skullCursorY = 0; break;
				case 0x01: skullCursorX = 2; skullCursorY = 0; break;
				case 0x02: skullCursorX = 3; skullCursorY = 0; break;
				case 0x03: skullCursorX = 4; skullCursorY = 0; break;
				case 0x04: skullCursorX = 5; skullCursorY = 1; break;
				case 0x05: skullCursorX = 5; skullCursorY = 2; break;
				case 0x06: skullCursorX = 5; skullCursorY = 3; break;
				case 0x07: skullCursorX = 5; skullCursorY = 4; break;
				case 0x08: skullCursorX = 1; skullCursorY = 5; break;
				case 0x09: skullCursorX = 2; skullCursorY = 5; break;
				case 0x0a: skullCursorX = 3; skullCursorY = 5; break;
				case 0x0b: skullCursorX = 4; skullCursorY = 5; break;
				case 0x0c: skullCursorX = 0; skullCursorY = 1; break;
				case 0x0d: skullCursorX = 0; skullCursorY = 2; break;
				case 0x0e: skullCursorX = 0; skullCursorY = 3; break;
				case 0x0f: skullCursorX = 0; skullCursorY = 4; break;
				case 0x10: skullCursorX = 0; skullCursorY = 0; break;
				case 0x11: skullCursorX = 5; skullCursorY = 0; break;
				case 0x12: skullCursorX = 5; skullCursorY = 5; break;
				case 0x13: skullCursorX = 0; skullCursorY = 5; break;
				case 0x14:
					skullCursorX = ((skullMouseX - 119) / 24) + 1;
					skullCursorY = ((skullMouseY - 60) / 24) + 1;
					break;
				default: skullCursorX = 0; skullCursorY = 0; break;
				}
			}
		}
		drawSkullPuzzle(skullMouseX, skullMouseY);
		if (_vm->isCD()) {
			_vm->_gMgr->paintBackbuffer_mouse();
			if (_movementStatus == 0 && (!_vm->_moMgr->getLeftBut()) && (!_vm->_moMgr->getRightBut()) && _lastPressedKey == Common::KEYCODE_INVALID) {
				continue;
			}
		}

		if (skullPuzzleCheckSolution()) {
			done = 2;
		}
		uint16 operation = Common::KEYCODE_INVALID;
		if (_vm->isCD()) {
			if (_vm->_moMgr->getRightBut()) {
				operation = Common::KEYCODE_ESCAPE;
			}
			else if (_vm->_moMgr->getLeftBut()) {
				if (skullMouseTarget >= 0 and skullMouseTarget < 16) {
					operation = Common::KEYCODE_RETURN;
				}
			}
		}
		if (operation == Common::KEYCODE_INVALID) {
			operation = _lastPressedKey;
		}
		uint16 moveStatus = _movementStatus;
		bool movedByKeys = false;
		if ((moveStatus & kUpFlag) && skullCursorY > 0) {
			movedByKeys = true;
			skullCursorY--;
		}
		if ((moveStatus & kDownFlag) && skullCursorY < 5) {
			movedByKeys = true;
			skullCursorY++;
		}
		if ((moveStatus & kLeftFlag) && skullCursorX > 0) {
			movedByKeys = true;
			skullCursorX--;
		}
		if ((moveStatus & kRightFlag) && skullCursorX < 5) {
			movedByKeys = true;
			skullCursorX++;
		}
		if (_vm->isCD() && movedByKeys) {
			skullMouseTarget = 20;
			if (skullCursorX == 0 && skullCursorY == 0) {
				skullMouseTarget = 16;
			}
			if (skullCursorX == 5 && skullCursorY == 0) {
				skullMouseTarget = 17;
			}
			if (skullCursorX == 5 && skullCursorY == 5) {
				skullMouseTarget = 18;
			}
			if (skullCursorX == 0 && skullCursorY == 5) {
				skullMouseTarget = 19;
			}
			if (skullCursorX > 0 && skullCursorX < 5 && skullCursorY == 0) {
				skullMouseTarget = skullCursorX-1;
			}
			if (skullCursorX > 0 && skullCursorX < 5 && skullCursorY == 5) {
				skullMouseTarget = skullCursorX + 7;
			}
			if (skullCursorY > 0 && skullCursorY < 5 && skullCursorX == 0) {
				skullMouseTarget = skullCursorY + 11;
			}
			if (skullCursorY > 0 && skullCursorY < 5 && skullCursorX == 5) {
				skullMouseTarget = skullCursorY + 3;
			}
			if (skullMouseTarget == 20) {
				_vm->_moMgr->warpMouse(((skullCursorX - 1) * 24) + 130, ((skullCursorY - 1) * 24) + 71);
			}
			else {
				MouseManager::mouseTarget mt=_vm->_moMgr->getTarget(MouseManager::Targets::SKULLPUZZLE, skullMouseTarget);
				_vm->_moMgr->warpMouse((mt.left+mt.right)/2, (mt.top+mt.bottom)/2);
			}
		}
		if (operation == Common::KeyCode::KEYCODE_ESCAPE) {
			done = 1;
		}
		if (_lastPressedKey == Common::KeyCode::KEYCODE_RETURN || _lastPressedKey == Common::KeyCode::KEYCODE_KP_ENTER) {
			if (skullCursorX == 0 && skullCursorY > 0 && skullCursorY < 5) {
				if (_vm->isCD() && _vm->_gameState.digiVolume != 0) {
					_vm->_audioMgr->playDigiCurrent(1);
				}
				skullPuzzleMoveHorz(skullCursorY, -1);
			}
			if (skullCursorX == 5 && skullCursorY > 0 && skullCursorY < 5) {
				if (_vm->isCD() && _vm->_gameState.digiVolume != 0) {
					_vm->_audioMgr->playDigiCurrent(1);
				}
				skullPuzzleMoveHorz(skullCursorY, 1);
			}
			if (skullCursorY == 0 && skullCursorX > 0 && skullCursorX < 5) {
				if (_vm->isCD() && _vm->_gameState.digiVolume != 0) {
					_vm->_audioMgr->playDigiCurrent(1);
				}
				skullPuzzleMoveVert(skullCursorX, -1);
			}
			if (skullCursorY == 5 && skullCursorX > 0 && skullCursorX < 5) {
				if (_vm->isCD() && _vm->_gameState.digiVolume != 0) {
					_vm->_audioMgr->playDigiCurrent(1);
				}
				skullPuzzleMoveVert(skullCursorX, 1);
			}
			do {
				updateInputStatus();
				if (_vm->isQuitRequested()) {
					done = 1;
					break;
				}
			} while (_mouseButtons != 0);
		}
		waitForNoInput();
		_vm->_gMgr->paintBackbuffer_mouse();
	}
	delete[] skullPuzzleData;
	skullPuzzleData = nullptr;
	if (_vm->isCD()) {
		_vm->_moMgr->setMouseVisibility(true);
	}
	if (done == 1) {
		return 0;
	}
	if (done == 2) {
		return 2;
	}
	return 0;
}
void GameManager::skullPuzzleMoveHorz(uint16 row, int16 dir) {
	if (dir == -1) {
		skullTilesState[30 + row] = skullTilesState[6 + row];
		for (uint8 pix = 0; pix < 24; pix += 2) {
			for (uint8 rotCol = 1; rotCol < 6; rotCol++) {
				_vm->_gMgr->setDrawAreaX(120, 215);
				drawSkullPuzzleTile(rotCol, row, -pix, 0);
			}
			_vm->_gMgr->setDrawAreaX(0, 319);
			_vm->_gMgr->decodeFrame(18, 108 + (skullCursorX * 24), 48 + (skullCursorY * 24), skullPuzzleData);
			_vm->_gMgr->paintBackbuffer_mouse();
		}
		for (uint8 rotCol = 0; rotCol < 5; rotCol++) {
			skullTilesState[(rotCol * 6) + row] = skullTilesState[((rotCol + 1) * 6) + row];
		}
		skullTilesState[30 + row] = skullTilesState[6 + row];
	}
	if (dir == 1) {
		skullTilesState[row] = skullTilesState[24 + row];
		for (uint8 pix = 0; pix < 24; pix += 2) {
			for (uint8 rotCol = 0; rotCol < 5; rotCol++) {
				_vm->_gMgr->setDrawAreaX(120, 215);
				drawSkullPuzzleTile(rotCol, row, pix, 0);
			}
			_vm->_gMgr->setDrawAreaX(0, 319);
			_vm->_gMgr->decodeFrame(18, 108 + (skullCursorX * 24), 48 + (skullCursorY * 24), skullPuzzleData);
			_vm->_gMgr->paintBackbuffer_mouse();
		}
		for (uint8 rotCol = 5; rotCol > 0; rotCol--) {
			skullTilesState[(rotCol * 6) + row] = skullTilesState[((rotCol - 1) * 6) + row];
		}
		skullTilesState[row] = skullTilesState[24 + row];
	}
}
void GameManager::skullPuzzleMoveVert(uint16 col, int16 dir) {
	if (dir == -1) {
		skullTilesState[(col * 6) + 5] = skullTilesState[(col * 6) + 1];
		for (uint8 pix = 0; pix < 24; pix += 2) {
			for (uint8 rotRow = 1; rotRow < 6; rotRow++) {
				_vm->_gMgr->setDrawAreaY(60, 156);
				drawSkullPuzzleTile(col, rotRow, 0, -pix);
			}
			_vm->_gMgr->setDrawAreaY(0, 320); //Bug??
			_vm->_gMgr->decodeFrame(18, 108 + (skullCursorX * 24), 48 + (skullCursorY * 24), skullPuzzleData);
			_vm->_gMgr->paintBackbuffer_mouse();
		}
		for (uint8 rotRow = 0; rotRow < 5; rotRow++) {
			skullTilesState[(col * 6) + rotRow] = skullTilesState[(col * 6) + rotRow + 1];
		}
		skullTilesState[(col * 6) + 5] = skullTilesState[(col * 6) + 1];
	}
	if (dir == 1) {
		skullTilesState[(col * 6)] = skullTilesState[(col * 6) + 4];
		for (uint8 pix = 0; pix < 24; pix += 2) {
			for (uint8 rotRow = 0; rotRow < 5; rotRow++) {
				_vm->_gMgr->setDrawAreaY(60, 156);
				drawSkullPuzzleTile(col, rotRow, 0, pix);
			}
			_vm->_gMgr->setDrawAreaY(0, 320); //Bug??
			_vm->_gMgr->decodeFrame(18, 108 + (skullCursorX * 24), 48 + (skullCursorY * 24), skullPuzzleData);
			_vm->_gMgr->paintBackbuffer_mouse();
		}
		for (uint8 rotRow = 5; rotRow > 0; rotRow--) {
			skullTilesState[(col * 6) + rotRow] = skullTilesState[(col * 6) + rotRow - 1];
		}
		skullTilesState[(col * 6)] = skullTilesState[(col * 6) + 4];
	}
}

void GameManager::drawSkullPuzzle(int16 mouseX, int16 mouseY) {
	_vm->_gMgr->copyVideoBuffer(_vm->_gMgr->getBackground(), _vm->_gMgr->getBackbuffer());
	_vm->_gMgr->decodeFrame(0x11, 0, 0, skullPuzzleData);
	for (uint16 col = 1; col < 5; col++) {
		for (uint16 row = 1; row < 5; row++) {
			drawSkullPuzzleTile(col, row, 0, 0);
		}
	}
	if (mouseX == -1 || mouseY == -1) {
		_vm->_gMgr->decodeFrame(18, 108 + (skullCursorX * 24), 48 + (skullCursorY * 24), skullPuzzleData);
	}
	else {
		_vm->_gMgr->decodeFrame(18,mouseX, mouseY, skullPuzzleData);
	}
}
void GameManager::drawSkullPuzzleTile(uint16 col, uint16 row, uint16 xoffs, uint16 yoffs) {
	_vm->_gMgr->decodeFrame(
		skullTilesState[(col * 6) + row],
		119 + xoffs + ((col - 1) * 24),
		60 + yoffs + ((row - 1) * 24),
		skullPuzzleData);
}
bool GameManager::skullPuzzleCheckSolution() {
	uint8 correctlyPositioned = 0;
	for (uint8 col = 1; col < 5; col++) {
		for (uint8 row = 1; row < 5; row++) {
			if (skullTilesState[(col * 6) + row] == (((row - 1) * 4) + col - 1)) {
				correctlyPositioned++;
			}
		}
	}
	if (correctlyPositioned == 16) {
		return true;
	}
	return false;
}

uint16 const GameManager::skullInitialTilesState[36] = {0, 0, 0, 0, 0, 0,
														0, 0, 4, 10, 15, 0,
														0, 5, 11, 13, 12, 0,
														0, 8, 1, 2, 6, 0,
														0, 3, 7, 9, 14, 0,
														0, 0, 0, 0, 0, 0};
const GameManager::startCoords GameManager::mapRoomsStartCoords[23] = {
	{0xf8, 0x7e},
	{0xe2, 0x7e},
	{0xe0, 0x96},
	{0xcc, 0x9c},
	{0xb2, 0x9a},
	{0xb0, 0x8a},
	{0x98, 0x88},
	{0x7c, 0x86},
	{0x70, 0x94},
	{0x60, 0x84},
	{0x5c, 0x72},
	{0x92, 0x74},
	{0xb0, 0x6a},
	{0x8a, 0x64},
	{0x68, 0x5e},
	{0x52, 0x60},
	{0xac, 0x5e},
	{0x74, 0x50},
	{0x86, 0x50},
	{0x94, 0x56},
	{0xca, 0x76},
	{0xb2, 0x78},
	{0xbe, 0x5c},
};
const GameManager::hotspotCoords GameManager::mapHotspotsCoords[10] = {
	{0xf0, 0x74, 0x108, 0x86},
	{0xc0, 0x66, 0xd8, 0x7f},
	{0xa4, 0x52, 0xbd, 0x60},
	{0xa5, 0x62, 0xbd, 0x71},
	{0x6c, 0x6c, 0xa2, 0x7c},
	{0x8c, 0x80, 0xa6, 0x90},
	{0x55, 0x63, 0x68, 0x7a},
	{0x54, 0x7c, 0x6a, 0x8e},
	{0x7d, 0x5c, 0x9a, 0x6a},
	{0x68, 0x46, 0x80, 0x57},
};
const GameManager::destIdxs GameManager::mapDestsPakRoomIdxs[10] = {
	{0, 0},
	{0, 0x14},
	{0, 0x10},
	{0, 0x0c},
	{0, 0x0b},
	{0, 6},
	{0, 0x0a},
	{0, 9},
	{0, 0x0d},
	{0, 0x11},
};
Common::KeyCode GameManager::_allowedKeys[] = {
	Common::KeyCode::KEYCODE_a, Common::KeyCode::KEYCODE_b, Common::KeyCode::KEYCODE_c, Common::KeyCode::KEYCODE_d, Common::KeyCode::KEYCODE_e, Common::KeyCode::KEYCODE_f,
	Common::KeyCode::KEYCODE_g, Common::KeyCode::KEYCODE_h, Common::KeyCode::KEYCODE_i, Common::KeyCode::KEYCODE_j, Common::KeyCode::KEYCODE_k, Common::KeyCode::KEYCODE_l,
	Common::KeyCode::KEYCODE_m, Common::KeyCode::KEYCODE_n, Common::KeyCode::KEYCODE_o, Common::KeyCode::KEYCODE_p, Common::KeyCode::KEYCODE_q, Common::KeyCode::KEYCODE_r,
	Common::KeyCode::KEYCODE_s, Common::KeyCode::KEYCODE_t, Common::KeyCode::KEYCODE_u, Common::KeyCode::KEYCODE_v, Common::KeyCode::KEYCODE_w, Common::KeyCode::KEYCODE_x,
	Common::KeyCode::KEYCODE_y, Common::KeyCode::KEYCODE_z, Common::KeyCode::KEYCODE_0, Common::KeyCode::KEYCODE_1, Common::KeyCode::KEYCODE_2, Common::KeyCode::KEYCODE_3,
	Common::KeyCode::KEYCODE_4, Common::KeyCode::KEYCODE_5, Common::KeyCode::KEYCODE_6, Common::KeyCode::KEYCODE_7, Common::KeyCode::KEYCODE_8, Common::KeyCode::KEYCODE_9,
	Common::KeyCode::KEYCODE_1, Common::KeyCode::KEYCODE_INVALID};
} // namespace Cometengine
