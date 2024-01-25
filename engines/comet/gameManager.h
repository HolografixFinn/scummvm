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

#ifndef COMET_GAMEMANAGER_H
#define COMET_GAMEMANAGER_H
#include "common/array.h"
#include "common/events.h"
#include "common/file.h"
#include "common/keyboard.h"
#include "common/random.h"
#include "common/scummsys.h"
#include "common/mutex.h"
#include "comet/CometXorSerializer.h"
#include "comet/comet_structs.h"
namespace Cometengine {
//	class CometEngine;
#include "common/pack-start.h"
struct Rectangle {
	int16 left;
	int16 top;
	int16 right;
	int16 bottom;
} PACKED_STRUCT;
#include "common/pack-end.h"

/*
#include "common/pack-start.h"
	struct CometGameStatus {
		uint16 _dummy1;
		int16 prevPakNumber;
		int16 currPakNumber;
		int16 newPakNumber;
		int16 prevRoom;
		int16 currRoom;
		int16 newRoom;
		int16 numExits;
		int16 numObjsTaken;
		int16 numScriptsInRoom;
		int16 walkTopAreaNumPoints;
		int16 paletteFadeLevel;

		int16 audioCardId;
		int16 lastMusSubfile;
		int16 musicVolume;
		int16 digiAudioFlags;

		int16 desiredFPS;
		int16 stageIndex;
		int16 textBlock;
		int16 textDurationScale;
		uint32 lifeRegenerationCounter;
		int16 roomZoomX;
		int16 roomZoonY;

		int16 replaceColors;
		int16 walkHorizAdjustment;
		int16 walkVerticalAdjustment_HorizMovement;
		uint16 movementMask;
		int16 walkVerticalAdjustment;
		uint16 paletteRedTintFactor;
		uint8 dummy2[38];

//		Actor actors[11];
//		RoomExit exits[16];
//		RoomScript scripts[20];
	} PACKED_STRUCT;
	*/
#include "common/pack-end.h"

class CometEngine;
class GameManager{
public:
	GameManager(CometEngine *vm);
	~GameManager();

	bool updateInputStatus();
//	void readInputStatus();
	Common::KeyCode _lastPressedKey;
	uint32 _specialKeys;
	uint32 _mouseButtons;
	uint16 _movementStatus;
	uint16 _movementStatus2; //unused in Comet but maybe used in other games?
	Common::Mutex _inpMutex;

	void init();
	void loadResPak();

	static const uint32 kAlt = 1;
	static const uint32 kShift = 2;
	static const uint32 kCtrl = 4;

	static const uint32 kUpFlag = 1;
	static const uint32 kDownFlag = 2;
	static const uint32 kLeftFlag = 4;
	static const uint32 kRightFlag = 8;

	static const uint8 kNoDir = 0;
	static const uint8 kUp = 1;
	static const uint8 kDown = 3;
	static const uint8 kLeft = 4;
	static const uint8 kRight = 2;

	static const uint32 kClickLeft = 1;
	static const uint32 kClickRight = 2;
	static const uint32 kConfirm = 0x80;

	static const uint32 kSpecialKey = 0x2000;
	static const uint32 kClickKey = 0x8000;
	static const uint32 kMoveKey = 0x4000;

	static const uint32 kVolatileKey = 0x1000;

	char *_tempBuffer;

	//			Actor _actors[11];
	Actor *_mainActor;
	Actor *_NPCActors;

	void enterPressed();
	void handlePause();
	void updateFontDataAndColor(uint8 textColor);
	void gameFrame();
	void resetHotspotsAndOtherValues();
	void killAllNPCs();
	void actorPrepareAnimation(Actor *actor, uint8 animationIdx);
	Actor *getActor(uint8 actorIdx);

	//			Resource _resources[20];
	void getMainCharSightArea(Rectangle &rect, uint16 sightWidth, uint16 sightHeight);
	void getMainCharIntersectionWithHotspots(bool showName);
	void resetWalkAnimation(Actor *actor);
	void createActor(uint8 actorIdx, int8 resourceIdx);
	void drawHotspotViewLine();
	void freeResourceFromCharacter(Actor *act);
	uint8 getResourceIdx(uint8 type, uint8 fileIdx);
	void setActorPosition(uint8 actorIdx, uint16 x, uint16 y);
	int16 *getVarPtr(uint16 idx);
	bool setActorDestination(int8 actorIdx, int16 x, int16 y);
	void setActorFacingDirection(Actor *actor, uint8 direction);
	bool rectanglesIntersect(const Rectangle &r1, const Rectangle &r2) const;
	void waitForNoInput();
	bool copyProtection();
	void loadWalkArea();
	void unlockMainActor();
	void handleInputAndMenus();

	Hotspot *getFirstInactiveHotspot();
	void removeSubmenus(uint8 idx);

	//for encapsulation
	void getCurrHotspotInfo(uint16 &dir, uint16 &x, uint16 &y);
	bool isHotspotInSight();
	//			void setStage(uint16 stage);
	//			uint16 getStage();
	const char *getStagesFilename();
	void setFacingDirection(uint8 dir);
	//			void setMovementMask(uint16 mask);
	//			uint16 getMovementMask();
	//			void setNewRoom(int16 room);
	//			void setNewPak(int16 pak);
	//			int16 getCurrentPak();
	Actor *getMainActor();
	uint16 getStatusVar(uint8 idx);
	void setStatusVar(uint8 idx, uint16 val);
	uint16 getRandom(uint16 max);
	bool isLook();
	bool isGet();
	bool isUse();
	bool isTalk();
	void unsetLook();
	bool waitingForEnter();
	void setWaitForEnter();
	void unsetWaitForEnter();
	void setWalkAreaTopLineNumPoints(uint8 numPoints);
	void setWalkAreaTopLinePoint(uint8 pointIdx, uint16 x, uint16 y);
	//void setNumExits(uint8 numExits);
	void setExit(uint8 exitIdx, uint8 direction, uint8 pak, uint8 room, uint16 x1, uint16 x2);
	uint8 getCurrentMovementFlags();
	//int8 getActiveObject();
	//void resetActiveObject();
	//			uint8 getObjectFlag(int8 objIdx);
	//			Hotspot *getHotspot(uint8 idx);
	//			int8 _getCurrRoom();
	void unsetTalk();
	void onUseObject();
	void clipRectToWalkingArea(int16 &x, int16 &y, int16 hw, int16 h);
	uint8 comparePoints(uint16 x1, uint16 y1, uint16 x2, uint16 y2);
	uint8 _roomZoomLevel;
	bool _zoomRestored;
	uint8 *priorityAllocateAndGetFile(const char *archiveName, uint16 fileNum);
	void showLoopedAnimation(uint8 subfile, uint8 anim, int16 frame_flags, uint8 numLoops, uint8 numDigiFX, const char *cues);
	uint16 handleSkullPuzzle();
	uint16 forceLoading;
	uint8 handleLoad();
	Common::RandomSource _rnd;
	uint16 defaultLanguage;

protected:
private:
	//for map
	typedef struct _startCoords {
		uint16 x;
		uint16 y;
	} startCoords;
	typedef struct _hotspotCoords {
		uint16 left;
		uint16 top;
		uint16 right;
		uint16 bottom;
	} hotspotCoords;
	typedef struct _destIdxs {
		uint16 pak;
		uint16 room;
	} destIdxs;
	static const startCoords mapRoomsStartCoords[23];
	static const hotspotCoords mapHotspotsCoords[10];
	static const destIdxs mapDestsPakRoomIdxs[10];
	void tryMap();
	void handleMap();



	//
	void setActiveObject();
	void loadFileAndInit(CometXorSerializer &s);
	void freeUnusedAnimResources();
	void reloadResources();
	//int16 _prevPakNumber;
	//int16 _currPakNumber;
	//int16 _newPakNumber;
	//int16 _prevRoom;
	//int16 _currRoom;
	//int16 _newRoom;
	//uint8 _numExits;
	//			GameStatus _status;
	uint8 _facingDirection;

	//			uint16 _stageIdx;
	//			uint32 _lifeRegenCounter;

	uint8 _startPakNumber;
	uint8 _startRoom;

	struct keySpecialFunc {
		Common::KeyCode _code;
		uint32 _flags;
		keySpecialFunc() : _code(Common::KEYCODE_INVALID), _flags(0) {}
		keySpecialFunc(Common::KeyCode code, uint32 flags) : _code(code), _flags(flags) {}
	};
	CometEngine *_vm;
//	uint16 _lastMovementStatus;
//	uint16 _unk3;
	uint16 _randomNumber;

	Common::EventManager *_evtMgr;
	void handleSpecialKey(uint32 idx, bool pressed);
	void handleClickKey(uint32 idx, bool pressed);
	void handleMovementKey(uint32 idx, bool pressed, uint32 type = 0);
	uint32 getKeySpecialFunction(Common::KeyCode code);
	Common::Array<keySpecialFunc> _keysFlags;
	void resetDefaultKeyFlags();
	void initAllResources();

	uint8 *_resPakData;

	uint8 *_fontData;
	//			uint16 _movementMask;

	struct Point {
		uint16 x;
		uint16 y;
	};
	struct WalkAreaLineStrip {
		uint16 numPoints;
		Point points[24];
	};
	WalkAreaLineStrip _topWalkAreaLineStrip;
	//uint16 _currTopWalkAreaNumPoints;
	//			uint16 _topWalkAreaYValues[320]; //TODO turn width to parameter
	void calcTopWalkArea(WalkAreaLineStrip strip, uint8 *values);

	bool _isRandomPixelsAndLinesActive;
	//			uint16 _walkHorizAdjustment;
	//			uint16 _walkVertAdjustment;
	//			uint16 _walkVertAdjustment_HorizMovement;
	//			uint8 _paletteFadeLevel;
	bool _isGet;
	bool _isLook;
	bool _isUse;
	bool _isTalk;
	void deactivateRandomPixelsAndLines();

	//			Hotspot _hotspots[20];

	int8 findResource(uint8 type, uint8 fileIdx);
	int8 findFreeResourceIdx();
	void clearUnusedResources();
	char _animsFilename[16];
	char _stagesFilename[16];
	char _roomsFilename[16];

	bool isResourceUsedByActor(uint8 resourceIdx);
	void removeResourceFromActors(uint8 resourceIdx);
	void setActorOffsetWalk(Actor *actor, uint8 offs);

	void setPakNumber(uint8 pak, uint8 room);
	void setFileNames(uint8 pakNum);

	//			uint16 _textOnScreenRemainingFrames;
	//			uint16 _textOnScreenDuration;
	void changeCurrentPak();
	void freeResourcesExceptMainActor();
	void freeStageResources();
	void loadAndExecuteNewRoom();
	void hideTextAndFreeStageResources();
	void hideText();
	void handleRoomsTransition(uint8 prevRoom, int8 prevPak);
	bool _isLoadingSavedGame;

	//			RoomExit _exits[16];
	uint16 _oppositeDirections[5];
	void getExitCoords(uint8 exitIdx, uint16 &x1, uint16 &y1, uint16 &x2, uint16 &y2);
	uint16 findIntersectingHotspot(Rectangle area);
	uint16 _hotspotDirection;
	int16 _hotspotX;
	int16 _hotspotY;
	bool _hotspotInSight;
	void handleAnimations();
	void handleActorAnimation(Actor *actor);
	void handlePortraitAnimation(Actor *actor);

	int16 *_statusVars[256];
	//			int16 _objectsFlags[256];
	//			int16 _gameFlags[256];

	void constrainCoordsToWalkingArea(int16 &x, int16 &y, int16 hw, int16 h);
	uint16 findIntersectingExit(Rectangle &r, uint8 direction);
	uint8 checkPointsCoincidence(uint16 x1, uint16 y1, uint16 x2, uint16 y2);
	void actorStorePrevDestination(Actor *actor);
	void setFacingDirectionToDestination(Actor *actor);
	bool _waitingForEnter;
	uint16 _portratiAnimationCounter;
	uint16 _portraitRandomAnimation;
	void handleWalk();
	bool handleActorWalk(uint8 actorIdx);
	void handleActorAutomaticWalk(Actor *actor, uint8 actorIdx, bool didMove);
	void adjustWalkAnimCoords(uint8 facingDir, int16 &x, int16 &y);
	void clipCoordsOnDestination(Actor *actor, int16 &x, int16 &y);
	uint16 checkIntersections(uint8 actorIdx, int16 x, int16 y, uint8 halfWidth, uint8 height, uint8 facingDir);
	uint16 handleActorIntersections(Actor *actor, uint8 actorIdx, uint16 resultIntersection);
	uint16 getIntersectionwithWalkArea(Rectangle &r, uint8 facingDir);
	uint16 checkIntersectionWithStageElements(Rectangle &r);
	Rectangle _intersectingElementRectangle;
	uint16 checkIntersectionWithActors(uint8 actorIdx, Rectangle &r);
	uint16 setMainActorPositionForNextRoom(int8 pak, int8 room);
	void clipMovement(uint8 actorIdx, Actor *actor);
	void clipMovementToWalkArea(uint8 actorIdx, Actor *actor);
	void actorDamageLife(Actor *actor, uint16 amount);
	void clipMovementToObstacle(uint8 actorIdx, Actor *actor);
	uint16 getWalkAreaYObstacleRight(int16 right, int16 top);
	uint16 getWalkAreaYObstacleLeft(int16 left, int16 top);

	//copy protection stuff
#include "common/pack-start.h"
	struct ConstellationStar {
		uint16 x;
		uint16 y;
		uint16 frameIdx;
	} PACKED_STRUCT;
#include "common/pack-end.h"
#include "common/pack-start.h"
	struct Constellation {
		uint16 numStars;
		ConstellationStar stars[7];
	} PACKED_STRUCT;
#include "common/pack-end.h"
	void drawCopyProtection(uint8 numRightAnswers);
	char *_copyProtectionTitleSentence;
	char *_copyProtectionInstructionsSentence;
	Constellation _copyProtectionsConstellationsData[11 * 16];
	uint8 *_copyProtectionStarsGraphics;
	uint8 _copyProtectionRightConstellationRow;
	uint8 _copyProtectionRightConstellationColumn;
	Constellation _copyProtectionDisplayedConstellationsData[9];
	uint8 _copyProtectionNumColumns;
	uint8 _copyProtectionNumRows;
	//

	//skull puzzle stuff
	static uint16 const skullInitialTilesState[36];
	uint16 skullTilesState[36];
	uint16 skullCursorX;
	uint16 skullCursorY;
	uint8 *skullPuzzleData;
	void drawSkullPuzzle();
	void drawSkullPuzzleTile(uint16 col, uint16 row, uint16 xoffs, uint16 yoffs);
	bool skullPuzzleCheckSolution();
	void skullPuzzleMoveHorz(uint16 row, int16 dir);
	void skullPuzzleMoveVert(uint16 col, int16 dir);
	//

	void drawIllsmouthTitle();
	char _illsmouthTitle[256];
	void tryTakingObject();
	uint8 _lastTakenObjId;
	//uint8 _numObjsTaken;

	void regenMainCharLife();
	void drawBottomExits();

	Common::KeyCode _currentlyHandledKey;
	uint8 _currentlyHandledMovementFlags;
	uint8 _isConfirmKeyFlag;
	bool _isConfirmKeyOrEnter;
	void moveMainActor();
	uint8 _moveFlagsToFacingDir[16];
	uint8 _facingDirChangeMatrix[5][5];
	uint8 _requestedFacingDir;

	uint8 _numMenusOnScreen;
	//uint8 _selectedMenuItem;
	void handleMenu();

	//			int8 _activeObject;

	uint8 chooseObject();
	//void setActiveObject();
	uint8 handleDiary();
	uint8 handleDiskMenu();
	uint8 _lastDiskMenuSelectedItem;
	uint8 _lastSettingsMenuSelectedItem;
	int32 loadSaveFilesDescriptions();
	char _saveFilesDescription[10][30];
	//uint8 _selectedSaveLoadSlot;

	uint8 handleSave();
	uint8 handleSettings();
	uint8 handleQuit();

	void debugDraw();

	//::Common::KeyCode _allowedKeys[38];
	static ::Common::KeyCode _allowedKeys[];
	uint8 handleSaveFilenameTyping(::Common::KeyCode key, uint8 selectedItem);
	void showNoticeBox(const char *message);
	//			void saveEncryptedData(Common::DumpFile &f);
	void quit();
	void ripScreenToBlack(uint16 color, uint16 effect);

};
} // namespace Cometengine
#endif
