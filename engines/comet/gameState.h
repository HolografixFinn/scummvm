#ifndef __COMETGAMESTATE__
#define __COMETGAMESTATE__
#include "comet/comet_structs.h"
#include "common/scummsys.h"
#include"comet/CometXorSerializer.h"
namespace Cometengine {
struct gameState {
	gameState();
	void serialize(CometXorSerializer &s);
	void serializeActor(Actor &actor, CometXorSerializer &s);
	void serializeRoomExit(RoomExit &actor, CometXorSerializer &s);
	void serializeRoomScript(RoomScript &actor, CometXorSerializer &s);
	void serializeResource(Resource &resource, CometXorSerializer &s);
	void serializeHotspot(Hotspot &hotspot, CometXorSerializer &s);
	void serializeDisplayListElement(DisplayListElement &elem, CometXorSerializer &s);
	void serializeStageElement(StageElement &elem, CometXorSerializer &s);
	void syncBool(bool &v, CometXorSerializer &s);
	template<typename T>
	void syncPointer(T*& ptr, CometXorSerializer& s, uint32 dummyval=1) {
		// I know I can't store pointers this way, but I only need to store if the ptr was null or not (or just the difference between two pointers). If it's not null, it will be reloaded with correct data
		uint32 _tmp = dummyval;
		if (ptr == nullptr) {
			_tmp = 0;
		}
		s.syncAsUint32LE(_tmp);
		if (_tmp == 0) {
			ptr = nullptr;
		} else {
			// pointers are reloaded on loading, but setting them to 1 on saving would corrupt the current game
			if (s.isLoading()) {
				ptr = reinterpret_cast<T *>(_tmp);
			}
		}
	}
	// dw empty
	int16 activeObject;
	uint16 selectedSaveLoadSlot;
	uint16 selectedMenuItem;
	uint16 selectedLanguageID;
	uint16 isAlternatePaletteActive;
	uint8 speechOptions;
	// 17 db empty
	//here crypt
	// dw empty
	int16 prevPakNum;
	int16 currPakNum;
	int16 newPakNum;
	int16 prevRoomNum;
	int16 currRoomNum;
	int16 newRoomNum;
	int16 numExits;
	uint16 numObjsTaken;
	// dw empty
	uint16 numScriptsInRoom;
	uint16 currTopWalkAreaNumPoints;
	uint8 paletteFadeLevel;
	// db empty
	// dw empty
	uint16 audiocardID;
	uint16 currentMusicFile;
	uint16 musicVolume;
	uint16 digiVolume;
	uint16 targetFPS;
	uint16 stageIndex;
	uint16 textChapterID;
	uint16 textDurationScale;
	int32 lifeRegenCounter;
	uint16 roomZoomX;
	uint16 roomZoomY;
	uint16 stageColorReplace;
	uint16 walkHorizAdjustment;
	uint16 walkVertAdjustment_HorizMovement;
	uint16 movementMask;
	uint8 walkVerticalAdjustment;
	//db empty
	uint16 paletteRedTintFactor;
	// 38 bytes fix
	Actor actors[11];
	RoomExit exits[16];
	RoomScript scripts[0x11];
	Resource resources[20];
	Hotspot hotspots[20];
	uint8 topWalkAreaYValues[320]; //TODO turn width to parameter ???
	uint8 numElementsInDisplayList;
	// db empty
	DisplayListElement displayList[49];
	uint8 numStageElements;
	// db empty
	StageElement stageElements[249];
	int16 objectsFlags[256];
	int16 gameVars[256];
};

} // namespace CometEngine
#endif
