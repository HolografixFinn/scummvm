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

#ifndef COMET_GRAPHICSMANAGER_H
#define COMET_GRAPHICSMANAGER_H
#include "common/mutex.h"
#include "common/scummsys.h"
#include "common/system.h"

#include "comet/comet_structs.h"

#define _COMET_XRESOLUTION 320
#define _COMET_YRESOLUTION 200

namespace Cometengine {
class CometEngine;
class GraphicsManager {
public:
	static const uint32 TimerRate;
	GraphicsManager(CometEngine *vm);
	virtual ~GraphicsManager();
	static void onFrameTimer(void *ref);
	void init();
	void uninit();
	void waitVRetrace();
	void initializePalette(char *_mainPalette, char *_flahsbackPalette, char *_cdintroPalette=nullptr, char *_cdPalette2=nullptr);
	void setBasicResources(char *speechbox, char *mainWalk, char *icons, char *objects);
	void initGameObjectsAndFlags();
	char *getResourceData(uint8 type);
	uint32 _rowsOffsets[_COMET_YRESOLUTION];
	char *lockMainSurface();
	void unlockMainSurface();
	char *_stageAnims;
	//palettes
	void fadePalette(char *palette, char *destPalette, uint8 fadeLevel, uint16 numColors);
	void uploadPalette(char *palette);
//	void uploadPalettePartial(char *palette, uint16 startIdx, uint16 count);
	char *mainGamePalette; // _primaryPaletteData;
	char *sepiaPalette;     //_secondaryPaletteData;
	char *normalPalette;
	char *tmpPalette;
	char *cdintroPalette;
	char *cd2Palette;
	void setPaletteTint(uint16 tint);
	void tintPalette(const char *palette, char *destPalette, uint16 factor);
	void scalePaletteBrightness(uint16 factor);
	void fadePaletteBlock(uint16 blockIdx, uint16 fadeLevel);

	//
	void copyVideoBuffer(char *src, char *dst);
	char *_videoBackbuffer;
	char *_backgroundBuffer;
	void addStageElementsToDisplayList();
	char *getAnimData(uint8 animationIdx, char *resourceData);
	char *getGraphicsData(uint8 type, uint8 animationIdx, char *resourceData);
	void addActorsToDisplayList();
	void drawActorsAndStageElements();
	void resetDrawArea();
	void setDrawAreaX(uint16 minX, uint16 maxX);
	void setDrawAreaY(uint16 minY, uint16 maxY);
	void setDrawArea(uint16 minX, uint16 minY, uint16 maxX, uint16 maxY);
	char *getTypeData(uint8 type, char *resourceData);
	void drawDashedLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 color);
	void scrollRoomLeft();
	void scrollRoomRight();
	void loadStageBackground();
	void loadStageAnims();
	void prepareStageElements();
	void loadStageElements();
	void loadStage(bool isLoading = false);
	void backgroundToBackbuffer();
	void setDrawWithEffect();
	void setFadeSpeed(uint8 speed);
	void dealFadeIn();
	void dealFadeOut();
	void actorSaySentenceWithAnimation(uint8 actor, uint16 sentence, uint8 animation);
	void drawSpeechBox(int16 left, int16 top, int16 right, int16 bottom);
	void clearBackbuffer();
	void setPixelColor(uint8 newColor);
	void putPixel(uint16 x, uint16 y);
	//for encapsulation
	void setFadeStatus(uint8 status);
	uint8 getFadeStatus();
	void setJustFadedOut(bool val);
	bool getJustFadedOut();
	void drawFrameToScreen();
	void setClearScreen();
	char *getBackbuffer();
	void setBackbuffer(char *bb);
	char *getBackground();
	char *getTmpBuffer();
	//			uint8 getNumStageElements();

	//			StageElement *getStageElement(uint8 idx);
	void addStageElement(uint8 x1, uint8 y1, uint8 x2, uint8 y2);

	void drawRectangleOutline(int16 left, int16 top, int16 right, int16 bottom, uint8 color);
	void drawSimpleBox(int16 left, int16 top, int16 right, int16 bottom, uint8 color_unused);
	void paintBackbuffer_mouse();
	void paintBackbuffer();
	void decodeFrame(uint16 idxAndFlags, uint16 x, uint16 y, char *dataPtr);
	void drawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 color);
	void drawRectangleFilled(int16 left, int16 top, int16 right, int16 bottom, uint8 color);
	void drawIconFrame(uint8 selected, uint8 idx);
	void drawObjectsWindow(uint8 objectsCount, uint16 *objBuffer, uint8 firstVisibleIdx, uint8 selectedObjIdx, uint16 frameIdx);
	void drawDiskMenu(uint8 selectedItem);
	void drawSaveMenu(uint8 selectedItem, bool isLoad, char descs[10][30]);
	void drawSettingsMenu(uint16 currSelection, uint16 volume, uint16 digiVolume, uint16 textSpeed, uint16 gameSpeed, uint16 diskNum, uint16 langId, uint16 framesCounter);
	//			uint16 getIsAlternatePaletteActive();
	//			void setIsAlternatePaletteActive(uint16 val);
	void diaryFade(bool isFadeOut, int16 currS, int16 maxS);
	void drawDiarySentences(int16 currS, int16 maxS, int16 fontC);
	void drawDiaryPagesAnimation(bool forward);
	void resetFullscreenViewport();
	uint16 drawAnimationFrame(char *resource, char *animation, uint16 frameIdx, uint16 factor, int16 x, int16 y, uint16 numFrames);
	void callCapFPS();
	void _hfCappedUpdate(bool incFPS=true);
	//stage color replace?
	uint16 stageColorReplace;
	//randomGraphics
	uint16 randCoordsBaseX;
	uint16 randCoordsBaseY;
	uint16 randomDrawActive;
	uint16 numRandCoords;
	uint16 numRandLoops;
	uint16 randCoords[200];

	uint16 randomStarsActive;
	uint16 randomStarsLeft;
	uint16 randomStarsRight;
	uint16 randomStarsTop;
	uint16 randomStarsBottom;

	uint16 numLightnings;

	void generateRandCoords(uint16 baseX, uint16 baseY, uint16 numCoords, uint16 numLoops);
	void handleRandomStars();

	//
	char *_iconsGraphics;

	//TODO this function down here is useful only for debug draw
	uint32 getFPS() { return _FPS; }


protected:
	uint8 _fadeStatus;
	uint8 _fadeSpeedStep;
	bool _justFadedOut;
	//		uint16 _isAlternatePaletteActive;
private:


	bool invretrace{false};

	//stage color replace
	void replaceStageColors();
	//randomGraphics
	void drawRandomStars(uint16 left, uint16 top, uint16 right, uint16 bottom);
	void generateRandCoord(uint16 *ptr);
	void drawRandomLinesOrDots();
	uint16 pingpong;
	int16 pingpongStep;
	uint16 pseudoRandSeed;
	uint16 pseudoRandVal;
	uint16 pseudoRand();
	void drawSingleLightning(uint16 startX, uint16 startY, uint16 maxX, uint16 maxY);
	void drawLightnings();
	//
	// for my capped framerate
	uint32 _hfLast;
	uint32 _hfMaxFps;
	uint32 _hfInterval;
	uint32 _hfFrames;
	uint32 _hfLastSecond;
	uint32 _hfFps;
	//
	void drawRoomZoomed(int16 centerx, int16 centery);
	void drawRoomZoomed_x3(int16 centerx, int16 centery);
	void drawRoomZoomed_x4(int16 centerx, int16 centery);

	Common::Mutex _mutex;
	typedef void (*mouseCallback)();
	void onFrame();
	uint32 _lastFrame;
	uint32 _currFPS;
	//			uint32 _desiredFPS;
	uint32 _prevFPS;
	uint32 _framesToWait;
	uint32 _verticalRetraceCount;
	void capFPS_floppy();
	void (GraphicsManager::*capFPS)();
	uint32 _FPS;
	void capFPS_CD();
	void capFPS_CD_timerProc();

	bool _clearScreen;
	CometEngine *_vm;
	uint32 _videoBackbufferSize;

	mouseCallback _prePaintMouseCallback;
	mouseCallback _postPaintMouseCallback;

	void paintBackbuffer_withEffect();
	bool _isPaintWithEffect;

	//resources
	char *_speechBoxGraphics;
	char *_parkerWalkAnimation;
	char *_type3Resource_unused;
	char *_objectsGraphics;
	void scalePalette(char *palette);
	void setPaletteEntries(const char *palette, uint8 startEntry, uint16 numEntries);

	//			uint16 _gameObjects[256];
	//			uint16 _gameFlags[256];

	//			DisplayListElement _displayList[49];
	//			uint8 _numElementsInDisplayList;
	void insertInDisplayListSorted(uint8 yPos, uint16 idx);

	uint16 _drawAreaMinX;
	uint16 _drawAreaMaxX;
	uint16 _drawAreaMinY;
	uint16 _drawAreaMaxY;

	void drawActorAnimationFrame(uint8 actorIdx);
	char *_tmpBuffer;
	void mergeFrames(uint16 idx1, int16 x1, int16 y1, char *data1, uint16 idx2, int16 x2, int16 y2, char *data2, int16 *buffer);
	void decodeMergedFrame(uint16 info, uint16 x, uint16 y, char *buffer, uint16 ratio);
	struct Coordinate {
		int16 x;
		int16 y;
	};
	Coordinate _coordinates[256];

	uint8 _decodingFlags_v1;
	uint8 _decodingFlags_v2;
	bool _isByteParams_v1;
	bool _isNegParam1_v1;
	bool _isNegParam2_v1;
	bool _isByteParams_v2;
	bool _isNegParam1_v2;
	bool _isNegParam2_v2;

	uint8 _currentFillColor;

	void configureDecodingParameters_v1(uint8 flags);
	void configureDecodingParameters_v2(uint8 flags);
	uint16 getWidthOfType1Image(uint16 idx, char *dataPtr);
	uint16 getHeightOfType1Image(uint16 idx, char *dataPtr);

	void drawPolygon(int16 x, int16 y, Coordinate *coords, uint16 numCoords, uint8 color);
	Coordinate _tmpCoords1[100];
	Coordinate _tmpCoords2[100];
	Coordinate *_tmpCoords[2];

	int16 _polygonXBuffer1[200];
	int16 _polygonXBuffer2[200];
	int16 *_polygonXBuffer[2];

	bool _polygonHasBeenClipped;
	uint16 polygonClipLeft(uint16 currentNumCoords);
	uint16 polygonClipRight(uint16 currentNumCoords);
	uint16 polygonClipTop(uint16 currentNumCoords);
	uint16 polygonClipBottom(uint16 currentNumCoords);

	uint8 _currPixelColor;

	void decodeFrameType1_special(int16 x, int16 y, char *ptr);
	uint16 specialLinesOffsets[_COMET_YRESOLUTION];

	//			void decodeFrame_0(uint16 idxAndFlags, uint16 x, uint16 y, char *dataPtr);
	void drawHorizontalLine(int16 x1, int16 y1, int16 x2, int8 color);
	void decodeFrameType1(uint16 idxAndFlags, uint16 x, uint16 y, char *surface, char *dataPtr);
	void decodeFrame_scaled(uint16 idxAndFlags, uint16 x, uint16 y, char *dataPtr, uint16 scale);
	void decodeFrame_rotated(uint16 idxAndFlags, uint16 x, uint16 y, char *dataPtr, uint16 angle);
	char _rowBuffer[400];

	static int16 _sineCosine[];
	void scrollRoomHoriz(int16 direction);

	void scrollBackbufferLeft(uint16 numPixels);
	void copyBackgroundSlice_left(uint16 width, uint16 offsetLeft);
	void scrollBackbufferRight(uint16 numPixels);
	void copyBackgroundSlice_right(uint16 width, uint16 offsetRight);
	void clearScreen();

	//			StageElement _stageElements[249];
	//			uint8 _numStageElements;

	void drawSingleAnimFrame(uint8 animIdx, int16 x, int16 y, uint16 frameIdx, char *data);

	uint8 _activeObjectBorderColor;
};
} // namespace Cometengine
#endif
