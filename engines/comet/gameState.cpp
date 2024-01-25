#include "comet/gameState.h"
namespace Cometengine {
gameState::gameState() : activeObject(-1), selectedSaveLoadSlot(0), selectedMenuItem(0), selectedLanguageID(0), isAlternatePaletteActive(0), speechOptions(0),
						 prevPakNum(-1), currPakNum(-1), newPakNum(0), prevRoomNum(-1), currRoomNum(-1), newRoomNum(0),
						 numExits(0), numObjsTaken(0), numScriptsInRoom(0), currTopWalkAreaNumPoints(0), paletteFadeLevel(255), audiocardID(0),
						 currentMusicFile(0), musicVolume(127), digiVolume(127), targetFPS(13), stageIndex(0), textChapterID(0), textDurationScale(1),
						 lifeRegenCounter(0), roomZoomX(160), roomZoomY(100), stageColorReplace(0), walkHorizAdjustment(0), walkVertAdjustment_HorizMovement(0), movementMask(0),
						 walkVerticalAdjustment(0), paletteRedTintFactor(0), numElementsInDisplayList(0), numStageElements(0) {
	memset(&actors[0], 0, sizeof(actors));
	memset(&exits[0], 0, sizeof(exits));
	memset(&scripts[0], 0, sizeof(scripts));
	memset(&resources[0], 0, sizeof(resources));
	memset(&hotspots[0], 0, sizeof(hotspots));
	memset(&topWalkAreaYValues[0], 0, sizeof(topWalkAreaYValues));
	memset(&displayList[0], 0, sizeof(displayList));
	memset(&stageElements[0], 0, sizeof(stageElements));
	memset(&objectsFlags[0], 0, sizeof(objectsFlags));
	memset(&gameVars[0], 0, sizeof(gameVars));
}
void gameState::serialize(CometXorSerializer &s) {
	uint16 wzero = 0;
	byte bzero = 0;
	s.stopXor();
	s.syncAsUint16LE(wzero);
	s.syncAsSint16LE(activeObject);
	s.syncAsUint16LE(selectedSaveLoadSlot);
	s.syncAsUint16LE(selectedMenuItem);
	s.syncAsUint16LE(selectedLanguageID);
	s.syncAsUint16LE(isAlternatePaletteActive);
	s.syncAsByte(speechOptions);
	for (uint8 i = 0; i < 17; i++) {
		s.syncAsByte(bzero);
	}
	s.startXor(0x05, 0x05);
	//la parte xor deve essere 3900 byte
	//0
	s.syncAsUint16LE(wzero);
	s.syncAsSint16LE(prevPakNum);
	s.syncAsSint16LE(currPakNum);
	s.syncAsSint16LE(newPakNum);
	s.syncAsSint16LE(prevRoomNum);
	s.syncAsSint16LE(currRoomNum);
	s.syncAsSint16LE(newRoomNum);
	s.syncAsSint16LE(numExits);
	//16
	s.syncAsUint16LE(numObjsTaken);
	s.syncAsUint16LE(wzero);
	s.syncAsUint16LE(numScriptsInRoom);
	s.syncAsUint16LE(currTopWalkAreaNumPoints);
	s.syncAsByte(paletteFadeLevel);
	s.syncAsByte(bzero);
	s.syncAsUint16LE(wzero);

	s.syncAsUint16LE(audiocardID);
	s.syncAsUint16LE(currentMusicFile);
	// 32
	s.syncAsUint16LE(musicVolume);
	s.syncAsUint16LE(digiVolume);
	s.syncAsUint16LE(targetFPS);
	s.syncAsUint16LE(stageIndex);
	s.syncAsUint16LE(textChapterID);
	s.syncAsUint16LE(textDurationScale);
	s.syncAsSint32LE(lifeRegenCounter);
	// 48
	s.syncAsUint16LE(roomZoomX);
	s.syncAsUint16LE(roomZoomY);
	s.syncAsUint16LE(stageColorReplace);
	s.syncAsUint16LE(walkHorizAdjustment);
	s.syncAsUint16LE(walkVertAdjustment_HorizMovement);
	s.syncAsUint16LE(movementMask);
	s.syncAsByte(walkVerticalAdjustment);
	s.syncAsByte(bzero);
	s.syncAsUint16LE(paletteRedTintFactor);
	// 64
	for (uint8 i = 0; i < 19; i++) {
		s.syncAsUint16LE(wzero);
	}
	//102
	for (uint8 i = 0; i < 11; i++) {
		// 82 134 186
		serializeActor(actors[i], s);
	}

	//674
	for (uint8 i = 0; i < 16; i++) {
		serializeRoomExit(exits[i], s);
	}
	// 802
	for (uint8 i = 0; i < 0x11; i++) {
		serializeRoomScript(scripts[i], s);
	}
	// 1176
	for (uint8 i = 0; i < 20; i++) {
		serializeResource(resources[i], s);
	}
	// 1296
	for (uint8 i = 0; i < 20; i++) {
		serializeHotspot(hotspots[i], s);
	}
	//1456
	for (uint16 i = 0; i < 320; i++) {
		s.syncAsByte(topWalkAreaYValues[i]);
	}
	// 1776

	s.syncAsByte(numElementsInDisplayList);
	s.syncAsByte(bzero);
	// 1778
	for (uint8 i = 0; i < 49; i++) {
		serializeDisplayListElement(displayList[i], s);
	}
	//1876
	s.syncAsByte(numStageElements);
	s.syncAsByte(bzero);
	//1878
	for (uint8 i = 0; i < 249; i++) {
		serializeStageElement(stageElements[i], s);
	}
	s.syncAsByte(bzero);
	s.syncAsByte(bzero);
	for (uint16 i = 0; i < 256; i++) {
		s.syncAsSint16LE(objectsFlags[i]);
	}
	for (uint16 i = 0; i < 256; i++) {
		s.syncAsSint16LE(gameVars[i]);
	}
	s.stopXor();
}
void gameState::serializeStageElement(StageElement &elem, CometXorSerializer &s) {
	s.syncAsByte(elem.halfLeft);
	s.syncAsByte(elem.top);
	s.syncAsByte(elem.halfRight);
	s.syncAsByte(elem.bottom);
}

void gameState::serializeDisplayListElement(DisplayListElement &elem, CometXorSerializer &s) {
	s.syncAsByte(elem.yPos);
	s.syncAsByte(elem.idx);
}

void gameState::serializeHotspot(Hotspot &hotspot, CometXorSerializer &s) {
	s.syncAsUint16LE(hotspot.objIdx);
	/*
	byte _tmp = hotspot.isActive;
	s.syncAsByte(_tmp);
	hotspot.isActive = _tmp;
	*/
	syncBool(hotspot.isActive, s);
	s.syncAsByte(hotspot.objType);
	s.syncAsUint16LE(hotspot.x);
	s.syncAsUint16LE(hotspot.y);
}
void gameState::serializeResource(Resource &resource, CometXorSerializer &s) {
	uint32 dzero = 0;
	uint32 _tmp = 0;
	s.syncAsByte(resource.type);
	s.syncAsByte(resource.fileIdx);
	/*
	_tmp = 1;
	if (resource.data == nullptr) {
		_tmp = 0;
	}
//	_tmp = reinterpret_cast<uint32>(resource.data);
	s.syncAsUint32LE(_tmp); 
	resource.data = reinterpret_cast<char *>(_tmp);
	*/
	syncPointer<uint8>(resource.data, s);
}
void gameState::serializeRoomScript(RoomScript &script, CometXorSerializer &s) {
	uint32 dzero = 0;
	uint32 _tmp = 0;
	/*
	_tmp = 1;
	if (script.start == nullptr) {
		_tmp = 0;
	}
	s.syncAsUint32LE(_tmp); //start ptr
	script.start = reinterpret_cast<const char *>(_tmp);
	*/
	syncPointer<const char>(script.start, s);
	/*
	_tmp = 1;
	if (script.current == nullptr) {
		_tmp = 0;
	}
	s.syncAsUint32LE(_tmp); //start ptr
	script.current = reinterpret_cast<const char *>(_tmp);
	*/
	syncPointer<const char>(script.current, s, script.current-script.start+1);
	s.syncAsSByte(script.actorIdx);
	s.syncAsByte(script.flags);

	s.syncAsUint16LE(script.syncCounter);
	s.syncAsUint16LE(script.loopCounter);
	s.syncAsUint16LE(script.left);
	s.syncAsUint16LE(script.top);
	s.syncAsUint16LE(script.right);
	s.syncAsUint16LE(script.bottom);
}

void gameState::serializeRoomExit(RoomExit &exit, CometXorSerializer &s) {
	s.syncAsByte(exit.direction);
	s.syncAsByte(exit.pak);
	s.syncAsByte(exit.room);
	s.syncAsByte(exit.unk);
	s.syncAsUint16LE(exit.x1);
	s.syncAsUint16LE(exit.x2);
}
void gameState::syncBool(bool &v, CometXorSerializer &s) {
	byte _tmp = v;
	s.syncAsByte(_tmp);
	v = _tmp;
};
void gameState::serializeActor(Actor &actor, CometXorSerializer &s) {
	s.syncAsUint16LE(actor.pivotX);
	s.syncAsUint16LE(actor.pivotY);
	s.syncAsUint16LE(actor.offsetWalkAnimation);
	s.syncAsUint16LE(actor.animationType);
	s.syncAsUint16LE(actor.facingDirection);
	syncBool(actor.isKillable, s);
	/*
	byte _tmp = actor.isKillable; //workaround for bool
	s.syncAsByte(_tmp);
	actor.isKillable = _tmp;
	*/
	s.syncAsSByte(actor.resourceIdx);
	s.syncAsUint16LE(actor.animationIdx);
	s.syncAsUint16LE(actor.frameToDraw);
	s.syncAsUint16LE(actor.currentAnimationFactor);
	s.syncAsUint16LE(actor.numFrames);
	s.syncAsSByte(actor.fixedAnimationFrame);
	s.syncAsByte(actor.halfWidth);
	s.syncAsByte(actor.height);
	s.syncAsByte(actor.intersectionType);
	s.syncAsByte(actor.intersectionIdx);
	s.syncAsByte(actor.lastIntersectionType);
	s.syncAsUint16LE(actor.life);
	s.syncAsUint16LE(actor.speechColor);
	s.syncAsSint16LE(actor.textPivotX);
	s.syncAsSint16LE(actor.textPivotY);
	s.syncAsUint16LE(actor.whereToWalk);
	s.syncAsUint16LE(actor.destinationX);
	s.syncAsUint16LE(actor.destinationY);
	s.syncAsUint16LE(actor.prevDestinationX);
	s.syncAsUint16LE(actor.prevDestinationY);
	s.syncAsUint16LE(actor.drawAreaMinX);
	s.syncAsUint16LE(actor.drawAreaMaxX);
	s.syncAsByte(actor.drawAreaMinY);
	s.syncAsByte(actor.drawAreaMaxY);
	s.syncAsByte(actor.isVisible);
	s.syncAsByte(actor._dummy);
}

} // namespace Cometengine
