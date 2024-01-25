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

#include "comet/graphicsManager.h"
#include "comet/comet.h"
#include "engines/util.h"

namespace Cometengine {
/*
	 int16 GraphicsManager::_sineCosine[] = {
		 0x0, 0x192, 0x323, 0x4B5, 0x645, 0x7D5, 0x963, 0x0AF0, 0x0C7C, 0x0E05, 0x0F8C, 0x1111, 0x1293, 0x1413, 0x158F, 0x1708,
0x187D, 0x19EF, 0x1B5C, 0x1CC6, 0x1E2B, 0x1F8B, 0x20E6, 0x223D, 0x238E, 0x24D9, 0x261F, 0x275F, 0x2899, 0x29CD, 0x2AFA, 0x2C21,
 0x2D41, 0x2E59, 0x2F6B, 0x3075, 0x3178, 0x3274, 0x3367, 0x3453, 0x3536, 0x3611, 0x36E4, 0x37AF, 0x3871, 0x392A, 0x39DA, 0x3A82,
 0x3B20, 0x3BB5, 0x3C42, 0x3CC4, 0x3D3E, 0x3DAE, 0x3E14, 0x3E71, 0x3EC5, 0x3F0E, 0x3F4E, 0x3F84, 0x3FB1, 0x3FD3, 0x3FEC, 0x3FFB,
 0x3FFF, 0x3FFB, 0x3FEC, 0x3FD3, 0x3FB1, 0x3F84, 0x3F4E, 0x3F0E, 0x3EC5, 0x3E72, 0x3E15, 0x3DAE, 0x3D3E, 0x3CC5, 0x3C42, 0x3BB6,
 0x3B21,  0x3A82, 0x39DB, 0x392A, 0x3871, 0x37AF, 0x36E5, 0x3612, 0x3537, 0x3453, 0x3368, 0x3274, 0x3179, 0x3076, 0x2F6C, 0x2E5A,
 0x2D41,  0x2C22, 0x2AFB, 0x29CE, 0x289A, 0x2760, 0x2620, 0x24DA, 0x238F, 0x223E, 0x20E7, 0x1F8C, 0x1E2C, 0x1CC7, 0x1B5D, 0x19F0,
 0x187E,  0x1709, 0x1590, 0x1414, 0x1295, 0x1112, 0x0F8E, 0x0E06, 0x0C7D, 0x0AF2, 0x965, 0x7D6, 0x647, 0x4B6, 0x325, 0x193,
 0x1,	 0xFE70, 0xFCDE, 0xFB4C, 0xF9BC, 0xF82C, 0xF69E, 0xF511, 0xF385, 0xF1FC, 0xF075, 0xEEF0, 0xED6E, 0xEBEE, 0xEA72, 0xE8F9,
 0xE784, 0xE612, 0xE4A5, 0xE33B, 0xE1D6, 0xE076, 0xDF1B, 0xDDC4, 0xDC73, 0xDB28, 0xD9E2, 0xD8A2, 0xD768, 0xD634, 0xD507, 0xD3E0,
 0xD2C0, 0xD1A7, 0xD096, 0xCF8B, 0xCE88, 0xCD8D, 0xCC9A, 0xCBAE, 0xCACB, 0xC9EF, 0xC91C, 0xC852, 0xC790, 0xC6D7, 0xC626, 0xC57F,
 0xC4E0, 0xC44B, 0xC3BF, 0xC33C, 0xC2C2, 0xC252, 0xC1EC, 0xC18F, 0xC13C, 0xC0F2, 0xC0B2, 0xC07C, 0xC050, 0xC02D, 0xC014, 0xC005,
 0xC001, 0xC005, 0xC014, 0xC02D, 0xC04F, 0xC07B, 0xC0B2, 0xC0F1, 0xC13B, 0xC18E, 0xC1EB, 0xC251, 0xC2C1, 0xC33B, 0xC3BE, 0xC44A,
 0xC4DF, 0xC57D, 0xC625, 0xC6D5, 0xC78E, 0xC850, 0xC91A, 0xC9ED, 0xCAC9, 0xCBAC, 0xCC98, 0xCD8B, 0xCE86, 0xCF89, 0xD093, 0xD1A5,
 0xD2BE, 0xD3DE, 0xD504, 0xD631, 0xD765, 0xD89F, 0xD9DF, 0xDB25, 0xDC70, 0xDDC1, 0xDF18, 0xE073, 0xE1D3, 0xE338, 0xE4A1, 0xE60F,
 0xE781, 0xE8F6, 0xEA6F, 0xEBEB, 0xED6A, 0xEEEC, 0xF071, 0xF1F8, 0xF382, 0xF50D, 0xF69A, 0xF829, 0xF9B8, 0xFB49, 0xFCDA, 0xFE6C,
 0x0 };
 */

int16 GraphicsManager::_sineCosine[] = {
	0, 402, 803, 1205, 1605, 2005, 2403, 2800, 3196, 3589, 3980, 4369, 4755, 5139, 5519, 5896,
	6269, 6639, 7004, 7366, 7723, 8075, 8422, 8765, 9102, 9433, 9759, 10079, 10393, 10701, 11002, 11297,
	11585, 11865, 12139, 12405, 12664, 12916, 13159, 13395, 13622, 13841, 14052, 14255, 14449, 14634, 14810, 14978,
	15136, 15285, 15426, 15556, 15678, 15790, 15892, 15985, 16069, 16142, 16206, 16260, 16305, 16339, 16364, 16379,
	16383, 16379, 16364, 16339, 16305, 16260, 16206, 16142, 16069, 15986, 15893, 15790, 15678, 15557, 15426, 15286,
	15137, 14978, 14811, 14634, 14449, 14255, 14053, 13842, 13623, 13395, 13160, 12916, 12665, 12406, 12140, 11866,
	11585, 11298, 11003, 10702, 10394, 10080, 9760, 9434, 9103, 8766, 8423, 8076, 7724, 7367, 7005, 6640,
	6270, 5897, 5520, 5140, 4757, 4370, 3982, 3590, 3197, 2802, 2405, 2006, 1607, 1206, 805, 403,
	1, -400, -802, -1204, -1604, -2004, -2402, -2799, -3195, -3588, -3979, -4368, -4754, -5138, -5518, -5895,
	-6268, -6638, -7003, -7365, -7722, -8074, -8421, -8764, -9101, -9432, -9758, -10078, -10392, -10700, -11001, -11296,
	-11584, -11865, -12138, -12405, -12664, -12915, -13158, -13394, -13621, -13841, -14052, -14254, -14448, -14633, -14810, -14977,
	-15136, -15285, -15425, -15556, -15678, -15790, -15892, -15985, -16068, -16142, -16206, -16260, -16304, -16339, -16364, -16379,
	-16383, -16379, -16364, -16339, -16305, -16261, -16206, -16143, -16069, -15986, -15893, -15791, -15679, -15557, -15426, -15286,
	-15137, -14979, -14811, -14635, -14450, -14256, -14054, -13843, -13623, -13396, -13160, -12917, -12666, -12407, -12141, -11867,
	-11586, -11298, -11004, -10703, -10395, -10081, -9761, -9435, -9104, -8767, -8424, -8077, -7725, -7368, -7007, -6641,
	-6271, -5898, -5521, -5141, -4758, -4372, -3983, -3592, -3198, -2803, -2406, -2007, -1608, -1207, -806, -404,
	0};
const uint32 GraphicsManager::TimerRate = 60;
GraphicsManager::GraphicsManager(CometEngine *vm) : _vm(vm), _currFPS(0), _FPS(0), _lastFrame(0), _verticalRetraceCount(0x24),
													_videoBackbuffer(nullptr), _videoBackbufferSize(0),
													_prePaintMouseCallback(nullptr), _postPaintMouseCallback(nullptr),
													_speechBoxGraphics(nullptr), _parkerWalkAnimation(nullptr), _stageAnims(nullptr),
													_type3Resource_unused(nullptr), _iconsGraphics(nullptr), _objectsGraphics(nullptr),
													mainGamePalette(nullptr), sepiaPalette(nullptr), cdintroPalette(nullptr), cd2Palette(nullptr),
													normalPalette(nullptr), tmpPalette(nullptr), //_displayList(), //_numElementsInDisplayList(0),
													_tmpBuffer(nullptr), _coordinates(),
													_decodingFlags_v1(0), _decodingFlags_v2(0), _isByteParams_v1(false), _isByteParams_v2(false), _isNegParam1_v1(false), _isNegParam2_v1(false), _isNegParam1_v2(false), _isNegParam2_v2(false),
													_tmpCoords1(), _tmpCoords2(), _tmpCoords(), _polygonXBuffer(), _polygonXBuffer1(), _polygonXBuffer2(), _currPixelColor(0), _rowBuffer(), _isPaintWithEffect(false), _fadeStatus(0), _fadeSpeedStep(0),
													_justFadedOut(false),                               //_desiredFPS(13),
													_prevFPS(0), _framesToWait(5), _clearScreen(false), //_stageElements(), //_numStageElements(0),
													_activeObjectBorderColor(0x50),
													randCoordsBaseX(0),
													randCoordsBaseY(0), randomDrawActive(0), numRandCoords(0), numRandLoops(0), randCoords(),
													randomStarsActive(0), randomStarsLeft(0), randomStarsRight(0), randomStarsTop(0), randomStarsBottom(0), numLightnings(0),
													pingpong(0x70), pingpongStep(1), pseudoRandSeed(0xacdc), pseudoRandVal(0x349a) // AC/DC fan? XD
													,
													stageColorReplace(0)

//, _isAlternatePaletteActive(0)
{
	_hfLast = _vm->_system->getMillis();
	_hfMaxFps = 50;
	_hfInterval = 1000 / _hfMaxFps;
	_hfFrames = 0;
	_hfLastSecond = _hfLast;
	_hfFps = 0;
	if (!_vm->isCD()) {
		//		_hfMaxFps = 50;
		capFPS = &GraphicsManager::capFPS_floppy;
		_vm->addTimedProc(new Common::Functor0Mem<void, GraphicsManager>(this, &GraphicsManager::onFrame));
	} else {
		_hfMaxFps = 35;
		_hfInterval = 1000 / _hfMaxFps;
		capFPS = &GraphicsManager::capFPS_CD;
		_vm->addTimedProc(new Common::Functor0Mem<void, GraphicsManager>(this, &GraphicsManager::capFPS_CD_timerProc));
	}
}
void GraphicsManager::_hfCappedUpdate(bool incFPS) {
	//	int32 minInterval = _currFPS > 19 ? 20 : 0;
	int32 t = _hfInterval - (_vm->_system->getMillis() - _hfLast);
	if (t > 0) {
		//		_vm->_system->delayMillis(MAX(t, minInterval));
		_vm->_system->delayMillis(t);
	}
	//	_hfLast += _hfInterval;
	_hfLast = _vm->_system->getMillis();
	_vm->_system->updateScreen();
	_hfFrames++;
	//	if (incFPS) {
	//		this->_currFPS++;
	//	}
	if (_hfLast - _hfLastSecond > 1000) {
		_hfFps = _hfFrames;
		_hfFrames = 0;
		_hfLastSecond = _hfLast;
		//		char strr[256];
		//		snprintf(strr, 256, "HF %d fps", _hfFps);
		//		debug(strr);
	}
}
GraphicsManager::~GraphicsManager() {
	uninit();
}
void GraphicsManager::callCapFPS() {
	(this->*capFPS)();
}

void GraphicsManager::init() {
	//	_vm->_system->getTimerManager()->installTimerProc(GraphicsManager::onFrameTimer, 1000000 / TimerRate, this, "FramesTimer");
	this->_videoBackbuffer = new uint8[_COMET_XRESOLUTION * _COMET_YRESOLUTION];
	this->_videoBackbufferSize = _COMET_XRESOLUTION * _COMET_YRESOLUTION;
	this->_backgroundBuffer = new uint8[(_COMET_XRESOLUTION * _COMET_YRESOLUTION) + 8000];
	_tmpBuffer = _backgroundBuffer + (_COMET_XRESOLUTION * _COMET_YRESOLUTION);
	uint32 offset = 0;
	for (uint32 i = 0; i < _COMET_YRESOLUTION; i++) {
		this->_rowsOffsets[i] = offset;
		offset += _COMET_XRESOLUTION;
	}
	initGraphics(320, 200);
}
void GraphicsManager::uninit() {
	_vm->_system->getTimerManager()->removeTimerProc(GraphicsManager::onFrameTimer);
	delete[] this->_videoBackbuffer;
	delete[] this->_backgroundBuffer;
	delete[] this->normalPalette;
	delete[] this->tmpPalette;
	if (_vm->isCD()) {

		delete[] this->mainGamePalette;
		delete[] this->sepiaPalette;
		delete[] this->cdintroPalette;
		delete[] this->cd2Palette;
		delete[] this->_speechBoxGraphics;
		delete[] this->_parkerWalkAnimation;
		delete[] this->_iconsGraphics;
		delete[] this->_objectsGraphics;
	}
}
void GraphicsManager::onFrameTimer(void *ref) {
	GraphicsManager *gm = reinterpret_cast<GraphicsManager *>(ref);
	gm->onFrame();
}
void GraphicsManager::onFrame() {
	{
		Common::StackLock lock(_mutex);
		uint32 curr = _vm->_system->getMillis();
		int32 diff = (1000 / TimerRate) - (curr - _lastFrame);
		if (diff > 0) {
			//			debug("Accelerato %d", diff);
			//			_vm->_system->delayMillis(diff);
			curr = _vm->_system->getMillis();
		}
		_lastFrame = curr;
		//		debug("Timer %d", _lastFrame);
		_verticalRetraceCount++;
		if (_verticalRetraceCount == TimerRate) {
			_verticalRetraceCount = 0;
			_FPS = _currFPS;
			_currFPS = 0;
			//			warning("Second Elapsed");
		}
	}
}
void GraphicsManager::waitVRetrace() {
	_hfCappedUpdate(false);
	return;
	//		_vm->_system->delayMillis(16);
	//		return;
	//	char warningString[256];
	//	uint32 next = 0;
	uint32 check = 0;
	//	uint32 curr = _vm->_system->getMillis();
	uint32 last = 0;
	//	uint32 frm = 1000 / TimerRate;
	//raw and probably unstable
	if (invretrace) {
		debug("Vretrace internal call");
	}
	invretrace = true;
	{
		Common::StackLock lock(_mutex);
		last = _lastFrame;
		//		buff = (curr / frm) * frm;
	}
	uint32 now = _vm->_system->getMillis();
	int32 t = (1000 / TimerRate) - (now - last);
	//	debug("pre %d -> %d = %d", last, now, t);
	if (t > 0) {
		_vm->_system->delayMillis(t + 2);
	}
	//	debug("post %0d", _vm->_system->getMillis());
	invretrace = false;
	/*
	while (true) {
		{
			Common::StackLock lock(_mutex);
			check = _lastFrame;
		}
		if (check != last) {
			break;
		}
		_vm->_system->delayMillis(1);
	}
	*/
}
void GraphicsManager::resetFullscreenViewport() {
	setDrawArea(0, 0, 319, 199);
}

void GraphicsManager::clearBackbuffer() {
	if (this->_videoBackbuffer != nullptr) {
		memset(this->_videoBackbuffer, 0, this->_videoBackbufferSize);
	}
}
void GraphicsManager::paintBackbuffer_mouse() {
	if (this->_prePaintMouseCallback != nullptr) {
		this->_prePaintMouseCallback();
	}
	//TODO: the following part with retrace probably needs to be recoded to consider the eventual vsync of the backend
	this->waitVRetrace();
	this->_vm->_system->copyRectToScreen(this->_videoBackbuffer, _COMET_XRESOLUTION, 0, 0, _COMET_XRESOLUTION, _COMET_YRESOLUTION);
	//	this->_vm->_system->updateScreen();
	this->_hfCappedUpdate();

	if (this->_postPaintMouseCallback != nullptr) {
		this->_postPaintMouseCallback();
	}
	//TEST
	this->_currFPS++;
}
void GraphicsManager::paintBackbuffer() {
	//TODO: the following part with retrace probably needs to be recoded to consider the eventual vsync of the backend
	this->waitVRetrace();
	this->_vm->_system->copyRectToScreen(this->_videoBackbuffer, _COMET_XRESOLUTION, 0, 0, _COMET_XRESOLUTION, _COMET_YRESOLUTION);
	//	this->_vm->_system->updateScreen();
	this->_hfCappedUpdate();
	//TEST
	this->_currFPS++;
}
void GraphicsManager::initializePalette(uint8 *_mainPalette, uint8 *_flahsbackPalette, uint8 *_cdintroPalette, uint8 *_cdPalette2) {
	mainGamePalette = _mainPalette;
	sepiaPalette = _flahsbackPalette;
	cdintroPalette = _cdintroPalette;
	cd2Palette = _cdPalette2;
	normalPalette = new uint8[0x300];
	tmpPalette = new uint8[0x300]; //TODO it isn't there?
	memcpy(normalPalette, mainGamePalette, 0x300);
	memcpy(tmpPalette, mainGamePalette, 0x300); // TODO it isn't there?
	uploadPalette(mainGamePalette);
}
void GraphicsManager::setBasicResources(uint8 *speechbox, uint8 *mainWalk, uint8 *icons, uint8 *objects) {
	_speechBoxGraphics = speechbox;
	_parkerWalkAnimation = mainWalk;
	_iconsGraphics = icons;
	_objectsGraphics = objects;
}

void GraphicsManager::uploadPalette(uint8 *palette) {
	//ok ma scalepalette non necessario?
	uint8 _tmpPalette[0x300];
	memcpy(_tmpPalette, palette, 0x300);
	//		scalePalette(tmpPalette);
	//	waitVRetrace();
	if (!_vm->isCD()) {
		setPaletteEntries(_tmpPalette, 0, 0x80);
		waitVRetrace();
		setPaletteEntries(_tmpPalette + (0x80 * 3), 0x80, 0x80);
	} else {
		setPaletteEntries(_tmpPalette, 0, 256);
		_vm->_system->updateScreen();
	}
}
/*
void GraphicsManager::uploadPalettePartial(char *palette, uint16 startIdx, uint16 count) {
	setPaletteEntries(palette, startIdx, count);
}
*/
void GraphicsManager::scalePalette(char *palette) {
	for (uint16 i = 0; i < 0x300; i++) {
		palette[i] = palette[i] >> 2;
	}
}
void GraphicsManager::scalePaletteBrightness(uint16 factor) {
	for (uint16 i = 0; i < 0x300; i++) {
		uint8 v = mainGamePalette[i];
		uint16 v2 = 0x100 - v;
		v2 = v2 / 16 * factor;
		tmpPalette[i] = v2 + v;
	}
	uploadPalette(tmpPalette);
}
void GraphicsManager::setPaletteEntries(const uint8 *palette, uint8 startEntry, uint16 numEntries) {
	this->_vm->_system->getPaletteManager()->setPalette(reinterpret_cast<const byte *>(palette), startEntry, numEntries);
}
uint8 *GraphicsManager::lockMainSurface() {
	Graphics::Surface *surf = this->_vm->_system->lockScreen();
	return (uint8 *)surf->getPixels();
}
void GraphicsManager::unlockMainSurface() {
	this->_vm->_system->unlockScreen();
}
void GraphicsManager::initGameObjectsAndFlags() {
	//ok
	for (uint16 i = 0; i < 256; i++) {
		_vm->_gameState.objectsFlags[i] = 0;
		_vm->_gameState.gameVars[i] = 0;
	}
}
uint8 *GraphicsManager::getResourceData(uint8 type) {
	uint8 *ptr = nullptr;
	switch (type) {
	case 1:
		ptr = _parkerWalkAnimation;
		break;
	case 2:
		ptr = _stageAnims;
		break;
	case 3:
		ptr = _type3Resource_unused;
		break;
	}
	return ptr;
}
void GraphicsManager::fadePalette(uint8 *palette, uint8 *destPalette, uint8 fadeLevel, uint16 numColors) {
	for (uint16 i = 0; i < numColors; i++) {
		uint8 b = *palette;
		uint16 val = (b * fadeLevel) >> 8;
		*destPalette = val;
		palette++;
		destPalette++;
		b = *palette;
		val = (b * fadeLevel) >> 8;
		*destPalette = val;
		palette++;
		destPalette++;
		b = *palette;
		val = (b * fadeLevel) >> 8;
		*destPalette = val;
		palette++;
		destPalette++;
	}
}
void GraphicsManager::copyVideoBuffer(uint8 *src, uint8 *dst) {
	memcpy(dst, src, _COMET_XRESOLUTION * _COMET_YRESOLUTION);
}
uint8 *GraphicsManager::getAnimData(uint8 animationIdx, uint8 *resourceData) {
	return getGraphicsData(2, animationIdx, resourceData);
}
uint8 *GraphicsManager::getGraphicsData(uint8 type, uint8 animationIdx, uint8 *resourceData) {
	resourceData = resourceData + READ_LE_UINT32(resourceData + (type * 4));
	resourceData = resourceData + READ_LE_UINT16(resourceData + (animationIdx * 4));
	return resourceData;
}

void GraphicsManager::addStageElementsToDisplayList() {
	uint8 *elementsData = getGraphicsData(0, 0, _stageAnims);
	uint16 num = *(elementsData + 1);
	elementsData += 8;
	for (uint16 i = 0; i < num; i++) {
		_vm->_gameState.displayList[i].yPos = *elementsData;
		_vm->_gameState.displayList[i].idx = 0x10;
		elementsData += 8;
	}
	_vm->_gameState.numElementsInDisplayList = num;
}
void GraphicsManager::addActorsToDisplayList() {
	for (uint8 i = 0; i < 11; i++) {
		if (_vm->_gameState.actors[i].life > 0 && _vm->_gameState.actors[i].isVisible) {
			insertInDisplayListSorted(_vm->_gameState.actors[i].pivotY, i);
		}
	}
}

void GraphicsManager::insertInDisplayListSorted(uint8 yPos, uint16 idx) {
	uint16 num = _vm->_gameState.numElementsInDisplayList;
	uint16 listIdx = 0;
	for (listIdx = 0; listIdx < num; listIdx++) {
		if (_vm->_gameState.displayList[listIdx].yPos > yPos) {
			break;
		}
	}
	for (uint16 i = num; i > listIdx; i--) {
		_vm->_gameState.displayList[i] = _vm->_gameState.displayList[i - 1];
	}
	_vm->_gameState.displayList[listIdx].yPos = yPos;
	_vm->_gameState.displayList[listIdx].idx = idx;
	_vm->_gameState.numElementsInDisplayList++;
}

void GraphicsManager::drawActorsAndStageElements() {
	resetDrawArea();
	uint16 numElements = _vm->_gameState.numElementsInDisplayList;
	uint8 *stageElements = getGraphicsData(0, 0, _stageAnims);
	uint8 *frames = getTypeData(1, _stageAnims);
	stageElements += 2;
	for (uint16 i = 0; i < numElements; i++) {
		char actorData = _vm->_gameState.displayList[i].idx;
		uint8 actorIdx = actorData & 0x0f;
		actorData = actorData & 0xf0;
		if (actorData == 0) {
			drawActorAnimationFrame(actorIdx);
		}
		if (actorData == 0x10) {
			uint8 type = *stageElements;
			stageElements += 2;
			uint16 idxAndFlags = READ_LE_UINT16(stageElements);
			stageElements += 2;
			int16 x = READ_LE_INT16(stageElements);
			stageElements += 2;
			int16 y = READ_LE_INT16(stageElements);
			stageElements += 2;
			if (type == 1) {
				decodeFrameType1(idxAndFlags, x, y, _videoBackbuffer, frames);
			} else {
				decodeFrame(idxAndFlags, x, y, _stageAnims);
			}
		}
	}
	if (_vm->_gmMgr->isHotspotInSight() && _vm->_gmMgr->getActor(0)->facingDirection != GameManager::kUp) {
		_vm->_gmMgr->drawHotspotViewLine();
	}
}
void GraphicsManager::resetDrawArea() {
	setDrawAreaX(0, 319);
	setDrawAreaY(0, 199);
}
void GraphicsManager::setDrawAreaX(uint16 minX, uint16 maxX) {
	_drawAreaMinX = minX;
	_drawAreaMaxX = maxX;
}
void GraphicsManager::setDrawAreaY(uint16 minY, uint16 maxY) {
	_drawAreaMinY = minY;
	_drawAreaMaxY = maxY;
}
void GraphicsManager::setDrawArea(uint16 minX, uint16 minY, uint16 maxX, uint16 maxY) {
	setDrawAreaX(minX, maxX);
	setDrawAreaY(minY, maxY);
}

uint8 *GraphicsManager::getTypeData(uint8 type, uint8 *resourceData) {
	return resourceData + READ_LE_UINT32(resourceData + (type * 4));
}
void GraphicsManager::drawActorAnimationFrame(uint8 actorIdx) {
	Actor *actor = &_vm->_gameState.actors[actorIdx];

	uint16 x = actor->pivotX;
	uint16 y = actor->pivotY;
	uint16 halfWidth = actor->halfWidth;
	uint16 height = actor->height;
	uint8 *resource = _vm->_gameState.resources[actor->resourceIdx].data;
	uint8 *animation = getAnimData(actor->animationIdx, resource) + 2;
	setDrawArea(actor->drawAreaMinX, actor->drawAreaMinY, actor->drawAreaMaxX, actor->drawAreaMaxY);
	if (actor->animationType == 2) {
		actor->currentAnimationFactor = drawAnimationFrame(resource, animation, actor->frameToDraw, actor->currentAnimationFactor, x, y, actor->numFrames);
	} else {
		if (actorIdx == 0 && actor->facingDirection == 1 && _vm->_gmMgr->isHotspotInSight()) {
			_vm->_gmMgr->drawHotspotViewLine();
		}
		uint16 idxAndFlags = READ_LE_UINT16(animation + (actor->frameToDraw * 8));
		decodeFrame(idxAndFlags, x, y, resource);
	}
	resetDrawArea();
}
uint16 GraphicsManager::drawAnimationFrame(uint8 *resource, uint8 *animation, uint16 frameIdx, uint16 factor_, int16 x, int16 y, uint16 numFrames) {
	uint8 *mergeBuffer = _tmpBuffer;
	uint8 *animation_ = animation;
	uint16 frameInfo = READ_LE_UINT16(animation_ + (frameIdx * 8));
	uint16 factor = READ_LE_UINT16(animation_ + ((frameIdx * 8) + 2));
	uint8 decodeType = factor >> 14;
	factor = factor & 0x3fff;
	uint8 *currAnimData = animation_ + 4;
	int16 currX = x;
	int16 currY = y;
	for (uint16 i = 0; i <= frameIdx; i++) {
		currX += READ_LE_INT16(currAnimData);
		currY += READ_LE_INT16(currAnimData + 2);
		currAnimData += 8;
	}
	if (decodeType == 1) {
		uint16 frameInfo2;
		if (frameIdx == numFrames - 1) {
			frameInfo2 = frameInfo;
		} else {
			frameInfo2 = READ_LE_UINT16(animation_ + (frameIdx * 8) + 8);
		}
		if (factor == 0) {
			factor = 1;
		}
		mergeFrames(frameInfo, 0, 0, resource, frameInfo2, 0, 0, resource, (int16 *)mergeBuffer);
		decodeMergedFrame(0, currX, currY, mergeBuffer, (factor_ * 256) / factor);
		factor_++;
		uint16 destFactor = READ_LE_UINT16(animation_ + (frameIdx * 8) + 2) & 0x3fff;
		if (factor_ >= destFactor) {
			factor_ = 0;
		}
		return factor_;
	} else {
		if (decodeType == 0) {
			decodeFrame(frameInfo, currX, currY, resource);
		}
		if (decodeType == 2) {
			decodeFrame_scaled(frameInfo, currX, currY, resource, factor);
		}
		if (decodeType == 3) {
			decodeFrame_rotated(frameInfo, currX, currY, resource, factor);
		}
	}
	return 0;
}
void GraphicsManager::mergeFrames(uint16 idx1, int16 x1, int16 y1, uint8 *data1, uint16 idx2, int16 x2, int16 y2, uint8 *data2, int16 *buffer) {
	int16 b1[100];
	int16 b2[100];
	int16 *pA, *pB, *pBuf;
	*buffer = 4;
	buffer++;
	*buffer = 0;
	buffer++;
	*buffer = 4;
	buffer++;
	*buffer = 0;
	buffer++;
	pBuf = buffer;
	*buffer = 0;
	buffer++;

	data1 = data1 + READ_LE_UINT32(data1);
	data2 = data2 + READ_LE_UINT32(data2);
	data1 = data1 + READ_LE_UINT32(data1 + (idx1 * 4));
	data2 = data2 + READ_LE_UINT32(data2 + (idx2 * 4));

	uint8 flags1 = *data1++;
	flags1 &= 0x01;
	uint8 flags2 = *data2++;
	flags2 &= 0x01;
	uint8 numOps1 = *data1++;
	uint8 numOps2 = *data2++;
	uint8 numOpsMin = 0, numOpsMax = 0;
	if (numOps1 < numOps2) {
		numOpsMin = numOps1;
		numOpsMax = numOps2;
	} else {
		numOpsMin = numOps2;
		numOpsMax = numOps1;
	}
	uint8 numOpsInBuffer = 0;
	for (uint8 i = 0; i < numOpsMax; i++) {
		numOpsInBuffer++;
		pA = b1;
		pB = b2;
		*((uint32 *)pA) = READ_LE_UINT32(data1);
		pA += 2;
		*((uint32 *)pB) = READ_LE_UINT32(data2);
		pB += 2;
		uint8 op1 = *data1++;
		uint8 op2 = *data2++;
		uint8 numCoords1 = *data1++;
		data1 += 2;
		uint8 numCoords2 = *data2++;
		data2 += 2;
		if (i < numOps1) {
			for (uint8 coordIdx = 0; coordIdx < numCoords1; coordIdx++) {
				int16 tmpX;
				int16 tmpY;
				if (flags1) {
					int8 tmpByte = *((int8 *)data1);
					data1++;
					tmpX = tmpByte + x1;
					tmpByte = *((int8 *)data1);
					data1++;
					tmpY = tmpByte + y1;
				} else {
					tmpX = x1 + READ_LE_INT16(data1);
					data1 += 2;
					tmpY = y1 + READ_LE_INT16(data1);
					data1 += 2;
				}
				*pA = tmpX;
				pA++;
				*pA = tmpY;
				pA++;
			}
		}
		if (i < numOps2) {
			for (uint8 coordIdx = 0; coordIdx < numCoords2; coordIdx++) {
				int16 tmpX;
				int16 tmpY;
				if (flags2) {
					int8 tmpByte = *((int8 *)data2);
					data2++;
					tmpX = tmpByte + x2;
					tmpByte = *((int8 *)data2);
					data2++;
					tmpY = tmpByte + y2;
				} else {
					tmpX = x2 + READ_LE_INT16(data2);
					data2 += 2;
					tmpY = y2 + READ_LE_INT16(data2);
					data2 += 2;
				}
				*pB = tmpX;
				pB++;
				*pB = tmpY;
				pB++;
			}
		}
		pA = b1;
		pB = b2;
		uint8 numCoordsMin = 0, numCoordsMax = 0;
		if (numCoords1 < numCoords2) {
			numCoordsMin = numCoords1;
			numCoordsMax = numCoords2;
		} else {
			numCoordsMin = numCoords2;
			numCoordsMax = numCoords1;
		}
		if (i < numOpsMin) {
			if (op1 == op2) {
				*buffer++ = *pA++;
				*buffer++ = *pA++;
				pB += 2;
				*((uint8 *)(((uint8 *)buffer) - 3)) = numCoordsMax;
				*buffer++ = *(pB - 1);
				for (uint8 coordIdx = 0; coordIdx < numCoordsMax; coordIdx++) {
					if (coordIdx < numCoordsMin || numCoords1 == numCoordsMin) {
						if (coordIdx < numCoordsMin) {
							*buffer++ = *pA++;
							*buffer++ = *pA++;
						} else {
							*buffer++ = *(pA - 2);
							*buffer++ = *(pA - 1);
						}
						*buffer++ = *pB++;
						*buffer++ = *pB++;

					} else {
						*buffer++ = *pA++;
						*buffer++ = *pA++;
						*buffer++ = *(pB - 2);
						*buffer++ = *(pB - 1);
					}
				}
			} else {
				*buffer++ = *pA++;
				*buffer++ = *pA++;
				*buffer++ = *(pA - 1);
				int16 *coordsStart = pA;
				for (uint8 coordIdx = 0; coordIdx < numCoords1; coordIdx++) {
					*buffer++ = *pA++;
					*buffer++ = *pA++;
					*buffer++ = *(pB + 2);
					*buffer++ = *(pB + 3);
				}
				*buffer++ = *pB++;
				*buffer++ = *pB++;
				*buffer++ = *(pB - 1);
				for (uint8 coordIdx = 0; coordIdx < numCoords2; coordIdx++) {
					*buffer++ = *coordsStart;
					*buffer++ = *(coordsStart + 1);
					*buffer++ = *pB++;
					*buffer++ = *pB++;
				}
				numOpsInBuffer++;
			}

		} else if (numOps1 == numOpsMin) {
			*buffer++ = *pB++;
			*buffer++ = *pB++;
			*buffer++ = *(pB - 1);
			int16 *coordsStart = pB;
			for (uint8 coordIdx = 0; coordIdx < numCoords2; coordIdx++) {
				*buffer++ = *coordsStart;
				*buffer++ = *(coordsStart + 1);
				*buffer++ = *pB++;
				*buffer++ = *pB++;
			}
		} else {
			*buffer++ = *pA++;
			*buffer++ = *pA++;
			*buffer++ = *(pA - 1);
			int16 *coordsStart = pA;
			for (uint8 coordIdx = 0; coordIdx < numCoords1; coordIdx++) {
				*buffer++ = *pA++;
				*buffer++ = *pA++;
				*buffer++ = *coordsStart;
				*buffer++ = *(coordsStart + 1);
			}
		}
	}
	//		*pBuf = (numOpsInBuffer << 8);
	*(((uint8 *)pBuf) + 1) = numOpsInBuffer;
}
void GraphicsManager::decodeMergedFrame(uint16 info, uint16 x, uint16 y, uint8 *mergeBuffer, uint16 ratio) {
	uint8 *ptr = mergeBuffer;
	ptr += READ_LE_UINT32(ptr);
	ptr += READ_LE_UINT32(ptr);
	uint16 value = READ_LE_UINT16(ptr);
	ptr += 2;
	uint16 numOperations = value >> 8;
	uint16 treshold = (ratio * numOperations) >> 8;

	for (uint8 i = 0; i < numOperations; i++) {
		uint8 operation = *ptr++;
		uint8 numCoordinates = *ptr++;
		uint16 extra1 = READ_LE_UINT16(ptr);
		ptr += 2;
		uint16 extra2 = READ_LE_UINT16(ptr);
		ptr += 2;
		if (i >= treshold) {
			uint16 tmp = extra2;
			extra2 = extra1;
			extra1 = tmp;
		}
		uint8 extraParamLO = extra2 & 0x00FF;
		uint8 extraParamHI = extra2 >> 8;
		uint8 coordIdx = 0;
		for (coordIdx = 0; coordIdx < numCoordinates; coordIdx++) {
			int16 x1 = READ_LE_INT16(ptr);
			ptr += 2;
			int16 y1 = READ_LE_INT16(ptr);
			ptr += 2;
			int16 x2 = READ_LE_INT16(ptr);
			ptr += 2;
			int16 y2 = READ_LE_INT16(ptr);
			ptr += 2;
			this->_coordinates[coordIdx].x = x + (x1 + (((x2 - x1) * ratio) / 256));
			this->_coordinates[coordIdx].y = y + (y1 + (((y2 - y1) * ratio) / 256));
		}
		switch (operation) {
		case 0: {
			uint8 flagsBkp = _decodingFlags_v1;
			this->decodeFrame(extraParamLO + ((extraParamHI | _decodingFlags_v1) << 8), this->_coordinates[0].x, this->_coordinates[0].y, mergeBuffer);
			configureDecodingParameters_v1(flagsBkp);
		} break;
		case 1: {
			uint8 *dataPtr2 = mergeBuffer + READ_LE_UINT32(mergeBuffer + 4);
			uint16 idx2 = extraParamLO + ((extraParamHI ^ _decodingFlags_v1) << 8);
			if (_decodingFlags_v1 & 0x80) {
				this->_coordinates[0].x -= getWidthOfType1Image(idx2, dataPtr2);
			}
			if (_decodingFlags_v1 & 0x20) {
				this->_coordinates[0].y += getHeightOfType1Image(idx2, dataPtr2);
			}
			this->decodeFrameType1(idx2, this->_coordinates[0].x, this->_coordinates[0].y, _videoBackbuffer, dataPtr2);
		} break;
		case 2:
		case 3: {
			uint16 skip = (256 - numCoordinates) * 4;
			ptr += skip;
		} break;
		case 4: {
			this->drawPolygon(0, 0, this->_coordinates, numCoordinates, extraParamHI);
			if (extraParamLO != 0xff) {
				this->_coordinates[numCoordinates] = this->_coordinates[0];
				for (uint8 j = 0; j < numCoordinates; j++) {
					this->drawLine(this->_coordinates[j].x, this->_coordinates[j].y, this->_coordinates[j + 1].x, this->_coordinates[j + 1].y, extraParamLO);
				}
			}
		}
		//TODO: implement drawPolygon
		break;
		case 5: {
			uint16 x1 = this->_coordinates[0].x;
			uint16 y1 = this->_coordinates[0].y;
			uint16 x2 = this->_coordinates[1].x;
			uint16 y2 = this->_coordinates[1].y;
			if (x1 > x2) {
				uint16 tmp = x1;
				x1 = x2;
				x2 = tmp;
			}
			if (y1 > y2) {
				uint16 tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			this->drawRectangleFilled(x1, y1, x2, y2, extraParamHI);
			if (extraParamLO != 0xff) {
				this->drawRectangleOutline(x1, y1, x2, y2, extraParamLO);
			}
		} break;
		case 6:
		case 8:
		case 9: {
			for (uint8 j = 0; j < numCoordinates - 1; j++) {
				this->drawLine(this->_coordinates[j].x, this->_coordinates[j].y, this->_coordinates[j + 1].x, this->_coordinates[j + 1].y, extraParamHI);
			}
		} break;
		case 7: {
			this->setPixelColor(extraParamHI);
			for (uint8 j = 0; j < numCoordinates; j++) {
				this->putPixel(this->_coordinates[j].x, this->_coordinates[j].y);
			}
		} break;
		case 10: {
			uint16 numFrame = extraParamLO + (extraParamHI * 256);
			uint16 offsType1Block = READ_LE_UINT32(mergeBuffer + 4);
			uint8 *tmpPtr = mergeBuffer + offsType1Block;
			uint16 offsFrame = READ_LE_UINT32(tmpPtr + (numFrame * 4));
			tmpPtr += offsFrame;
			uint8 height = *(tmpPtr + 1);
			height--;
			tmpPtr += 2;
			this->_coordinates[0].y -= height;
			//TODO just for testing
			/*
				if (this->_coordinates[0].y < 0) {
					this->_coordinates[0].y = 0;
				}
				if (this->_coordinates[0].x < 0) {
					this->_coordinates[0].x = 0;
				}
				//
				*/
			this->decodeFrameType1_special(this->_coordinates[0].x, this->_coordinates[0].y, tmpPtr);

		} break;
		}
	}
}
void GraphicsManager::configureDecodingParameters_v1(uint8 flags) {
	_isByteParams_v1 = false;
	_isNegParam1_v1 = false;
	_isNegParam2_v1 = false;
	if (flags & 0x10) {
		_isByteParams_v1 = true;
	}
	if (flags & 0x80) {
		_isNegParam1_v1 = true;
	}
	if (flags & 0x20) {
		_isNegParam2_v1 = true;
	}
	_decodingFlags_v1 = flags;
}
void GraphicsManager::configureDecodingParameters_v2(uint8 flags) {
	_isByteParams_v2 = false;
	_isNegParam1_v2 = false;
	_isNegParam2_v2 = false;
	if (flags & 0x10) {
		_isByteParams_v2 = true;
	}
	if (flags & 0x80) {
		_isNegParam1_v2 = true;
	}
	if (flags & 0x20) {
		_isNegParam2_v2 = true;
	}
	_decodingFlags_v2 = flags;
}
uint16 GraphicsManager::getWidthOfType1Image(uint16 idx, uint8 *dataPtr) {
	idx = idx & 0x0fff;
	dataPtr += READ_LE_UINT32(dataPtr + (idx * 4));
	uint8 size = *dataPtr;
	return size * 16;
}
uint16 GraphicsManager::getHeightOfType1Image(uint16 idx, uint8 *dataPtr) {
	idx = idx & 0x0fff;
	dataPtr += READ_LE_UINT32(dataPtr + (idx * 4));
	dataPtr++;
	uint8 size = *dataPtr;
	return size;
}
void GraphicsManager::drawRectangleFilled(int16 left, int16 top, int16 right, int16 bottom, uint8 color) {
	_currentFillColor = color;
	if (left < this->_drawAreaMinX) {
		left = this->_drawAreaMinX;
	}
	if (right > this->_drawAreaMaxX) {
		right = this->_drawAreaMaxX;
	}
	if (top < this->_drawAreaMinY) {
		top = this->_drawAreaMinY;
	}
	if (bottom > this->_drawAreaMaxY) {
		bottom = this->_drawAreaMaxY;
	}
	if (right <= left) {
		return;
	}
	if (bottom <= top) {
		return;
	}
	uint16 width = right - left + 1;
	uint16 height = bottom - top + 1;
	uint8 *dest = this->_videoBackbuffer;
	dest += (this->_rowsOffsets[top] + left);
	uint16 toNextLine = _COMET_XRESOLUTION - width;
	for (uint16 i = 0; i < height; i++) {
		memset(dest, _currentFillColor, width);
		dest += width;
		dest += toNextLine;
	}
}
void GraphicsManager::drawRectangleOutline(int16 left, int16 top, int16 right, int16 bottom, uint8 color) {
	this->drawHorizontalLine(left, top, right, color);
	this->drawHorizontalLine(left, bottom, right, color);
	this->drawLine(left, top, left, bottom, color);
	this->drawLine(right, top, right, bottom, color);
}
void GraphicsManager::drawSimpleBox(int16 left, int16 top, int16 right, int16 bottom, uint8 color_unused) {
	drawLine(left, top, right, top, 0x15);
	drawLine(left, top, left, bottom, 0x15);
	drawLine(right, top, right, bottom, 0x11);
	drawLine(left, bottom, right, bottom, 0x11);
	drawRectangleFilled(left + 1, top + 1, right - 1, bottom - 1, 0x13);
}

void GraphicsManager::drawHorizontalLine(int16 x1, int16 y1, int16 x2, int8 color) {
	if (x1 > this->_drawAreaMaxX) {
		return;
	}
	if (x2 < this->_drawAreaMinX) {
		return;
	}
	if (x1 < this->_drawAreaMinX) {
		x1 = this->_drawAreaMinX;
	}
	if (x2 > this->_drawAreaMaxX) {
		x2 = this->_drawAreaMaxX;
	}
	if (y1 < this->_drawAreaMinY) {
		return;
	}
	if (y1 > this->_drawAreaMaxY) {
		return;
	}
	int16 width = x2 - x1 + 1;
	uint8 *dest = this->_videoBackbuffer;
	dest += this->_rowsOffsets[y1] + x1;
	memset(dest, color, width);
}
void GraphicsManager::drawLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 color) {
	//NOTE: right now I'm trying to mimic exactly what the original ASM code does. Probably I will optimize something later
	if (x1 > x2) {
		SWAP(x1, x2);
		SWAP(y1, y2);
		//uint16 tmp = x1;
		//x1 = x2;
		//x2 = tmp;
		//tmp = y1;
		//y1 = y2;
		//y2 = tmp;
	}
	if (y1 == y2) {
		//horizontal line: the game here duplicates the horizontal line drawing code. I won't!
		this->drawHorizontalLine(x1, y1, x2, color);
	} else {
		uint8 pointA_clippingFlags = 0;
		uint8 pointB_clippingFlags = 0;
		if (x1 < this->_drawAreaMinX) {
			pointA_clippingFlags |= 1;
		}
		if (x1 > this->_drawAreaMaxX) {
			pointA_clippingFlags |= 2;
		}
		if (y1 < this->_drawAreaMinY) {
			pointA_clippingFlags |= 4;
		}
		if (y1 > this->_drawAreaMaxY) {
			pointA_clippingFlags |= 8;
		}

		if (x2 < this->_drawAreaMinX) {
			pointB_clippingFlags |= 1;
		}
		if (x2 > this->_drawAreaMaxX) {
			pointB_clippingFlags |= 2;
		}
		if (y2 < this->_drawAreaMinY) {
			pointB_clippingFlags |= 4;
		}
		if (y2 > this->_drawAreaMaxY) {
			pointB_clippingFlags |= 8;
		}
		if ((pointA_clippingFlags | pointB_clippingFlags) != 0) {
			//we need to do some clipping before we start drawing!
			do {
				if ((pointA_clippingFlags & pointB_clippingFlags) != 0) {
					//if both points have one clipping situation in common, the line is automatically out of the viewport
					return;
				}
				int16 clippedValue = 0;
				int16 clippedX = 0;
				int16 clippedY = 0;

				uint8 tmpClippingFlags = pointA_clippingFlags;
				if (tmpClippingFlags == 0) {
					tmpClippingFlags = pointB_clippingFlags;
					if (tmpClippingFlags == 0) {
						// sic
						tmpClippingFlags = pointA_clippingFlags;
					}
				}
				uint8 currentClippingProblem = 8;
				clippedValue = this->_drawAreaMaxY;
				if (!(currentClippingProblem & tmpClippingFlags)) {
					currentClippingProblem = currentClippingProblem >> 1;
					clippedValue = this->_drawAreaMinY;
					if (!(currentClippingProblem & tmpClippingFlags)) {
						currentClippingProblem = currentClippingProblem >> 1;
						clippedValue = this->_drawAreaMaxX;
						if (!(currentClippingProblem & tmpClippingFlags)) {
							currentClippingProblem = currentClippingProblem >> 1;
							clippedValue = this->_drawAreaMinX;
						}
					}
				}

				int16 xDiff = x2 - x1;
				int16 yDiff = y2 - y1;
				if (currentClippingProblem & 0x0C) {
					clippedY = clippedValue;
					clippedX = x1 + ((clippedY - y1) * xDiff / yDiff);
				} else {
					clippedX = clippedValue;
					clippedY = y1 + ((clippedX - x1) * yDiff / xDiff);
				}
				uint8 newClippingFlags = 0;
				if (clippedX < this->_drawAreaMinX) {
					newClippingFlags |= 1;
				}
				if (clippedX > this->_drawAreaMaxX) {
					newClippingFlags |= 2;
				}
				if (clippedY < this->_drawAreaMinY) {
					newClippingFlags |= 4;
				}
				if (clippedY > this->_drawAreaMaxY) {
					newClippingFlags |= 8;
				}
				if (tmpClippingFlags == pointA_clippingFlags) {
					x1 = clippedX;
					y1 = clippedY;
					pointA_clippingFlags = newClippingFlags;
				} else {
					x2 = clippedX;
					y2 = clippedY;
					pointB_clippingFlags = newClippingFlags;
				}
			} while ((pointA_clippingFlags | pointB_clippingFlags) != 0);
		}
		//ok everything is correctly clipped here
		int16 xDiff = x2 - x1;
		int16 yDiff = y2 - y1;
		if (xDiff == 0 && yDiff == 0) {
			return;
		}
		uint8 *dest = this->_videoBackbuffer + x1 + (this->_rowsOffsets[y1]);
		*dest = color;
		int16 offset = 320;
		if (yDiff < 0) {
			yDiff = -yDiff;
			offset = -offset;
		}
		if (xDiff > yDiff) {
			uint16 counter = xDiff >> 1;
			for (int16 i = 0; i < xDiff; i++) {
				dest++;
				counter += yDiff;
				if (counter >= xDiff) {
					counter -= xDiff;
					dest += offset;
				}
				*dest = color;
			}
		} else {
			uint16 counter = yDiff >> 1;
			for (int16 i = 0; i < yDiff; i++) {
				dest += offset;
				counter += xDiff;
				if (counter >= yDiff) {
					counter -= yDiff;
					dest++;
				}
				*dest = color;
			}
		}
	}
}

void GraphicsManager::drawDashedLine(int16 x1, int16 y1, int16 x2, int16 y2, uint8 color) {
	//NOTE: this is exactly the same as drawline except for a small difference in the final part, when plotting pixels
	//NOTE: right now I'm trying to mimic exactly what the original ASM code does. Probably I will optimize something later
	if (x1 > x2) {
		SWAP(x1, x2);
		SWAP(y1, y2);
		//uint16 tmp = x1;
		//x1 = x2;
		//x2 = tmp;
		//tmp = y1;
		//y1 = y2;
		//y2 = tmp;
	}
	uint8 pointA_clippingFlags = 0;
	uint8 pointB_clippingFlags = 0;
	if (x1 < this->_drawAreaMinX) {
		pointA_clippingFlags |= 1;
	}
	if (x1 > this->_drawAreaMaxX) {
		pointA_clippingFlags |= 2;
	}
	if (y1 < this->_drawAreaMinY) {
		pointA_clippingFlags |= 4;
	}
	if (y1 > this->_drawAreaMaxY) {
		pointA_clippingFlags |= 8;
	}

	if (x2 < this->_drawAreaMinX) {
		pointB_clippingFlags |= 1;
	}
	if (x2 > this->_drawAreaMaxX) {
		pointB_clippingFlags |= 2;
	}
	if (y2 < this->_drawAreaMinY) {
		pointB_clippingFlags |= 4;
	}
	if (y2 > this->_drawAreaMaxY) {
		pointB_clippingFlags |= 8;
	}
	if ((pointA_clippingFlags | pointB_clippingFlags) != 0) {
		//we need to do some clipping before we start drawing!
		do {
			if ((pointA_clippingFlags & pointB_clippingFlags) != 0) {
				//if both points have one clipping situation in common, the line is automatically out of the viewport
				return;
			}
			uint16 clippedValue = 0;
			uint16 clippedX = 0;
			uint16 clippedY = 0;

			uint8 tmpClippingFlags = pointA_clippingFlags;
			if (tmpClippingFlags == 0) {
				tmpClippingFlags = pointB_clippingFlags;
				if (tmpClippingFlags == 0) {
					// sic
					tmpClippingFlags = pointA_clippingFlags;
				}
			}
			uint8 currentClippingProblem = 8;
			clippedValue = this->_drawAreaMaxY;
			if (!(currentClippingProblem & tmpClippingFlags)) {
				currentClippingProblem = currentClippingProblem >> 1;
				clippedValue = this->_drawAreaMinY;
				if (!(currentClippingProblem & tmpClippingFlags)) {
					currentClippingProblem = currentClippingProblem >> 1;
					clippedValue = this->_drawAreaMaxX;
					if (!(currentClippingProblem & tmpClippingFlags)) {
						currentClippingProblem = currentClippingProblem >> 1;
						clippedValue = this->_drawAreaMinX;
					}
				}
			}

			int16 xDiff = x2 - x1;
			int16 yDiff = y2 - y1;
			if (currentClippingProblem & 0xC0) {
				clippedY = clippedValue;
				clippedX = x1 + ((clippedY - y1) * xDiff / yDiff);
			} else {
				clippedX = clippedValue;
				clippedY = y1 + ((clippedX - x1) * yDiff / xDiff);
			}
			uint8 newClippingFlags = 0;
			if (clippedX < this->_drawAreaMinX) {
				newClippingFlags |= 1;
			}
			if (clippedX > this->_drawAreaMaxX) {
				newClippingFlags |= 2;
			}
			if (clippedY < this->_drawAreaMinY) {
				newClippingFlags |= 4;
			}
			if (clippedY > this->_drawAreaMaxY) {
				newClippingFlags |= 8;
			}
			if (tmpClippingFlags == pointA_clippingFlags) {
				x1 = clippedX;
				y1 = clippedY;
				pointA_clippingFlags = newClippingFlags;
			} else {
				x2 = clippedX;
				y2 = clippedY;
				pointB_clippingFlags = newClippingFlags;
			}
		} while ((pointA_clippingFlags | pointB_clippingFlags) != 0);
	}
	//ok everything is correctly clipped here
	int16 xDiff = x2 - x1;
	int16 yDiff = y2 - y1;
	if (xDiff == 0 && yDiff == 0) {
		return;
	}
	uint8 *dest = this->_videoBackbuffer + x1 + (this->_rowsOffsets[y1]);
	*dest = color;
	int16 offset = _COMET_XRESOLUTION;
	if (yDiff < 0) {
		yDiff = -yDiff;
		offset = -offset;
	}
	uint8 dashCounter = 2;
	if (xDiff > yDiff) {
		uint16 counter = xDiff >> 1;
		for (uint16 i = 0; i < xDiff; i++) {
			dest++;
			counter += yDiff;
			if (counter >= xDiff) {
				counter -= xDiff;
				dest += offset;
			}
			dashCounter++;
			if (dashCounter & 0x02) {
				*dest = color;
			}
		}
	} else {
		uint16 counter = yDiff >> 1;
		for (uint16 i = 0; i < yDiff; i++) {
			dest += offset;
			counter += xDiff;
			if (counter >= yDiff) {
				counter -= yDiff;
				dest++;
			}
			dashCounter++;
			if (dashCounter & 0x02) {
				*dest = color;
			}
		}
	}
}

uint16 GraphicsManager::polygonClipLeft(uint16 currentNumCoords) {
	uint16 clippedParamsNum = 0;
	this->_polygonHasBeenClipped = true;
	uint16 idx = 0;
	int16 x1, y1, x2, y2;
	x1 = this->_tmpCoords[0][idx].x;
	y1 = this->_tmpCoords[0][idx].y;
	idx++;
	do {
		bool clamp1 = false, clamp2 = false;
		x2 = this->_tmpCoords[0][idx].x;
		y2 = this->_tmpCoords[0][idx].y;
		idx++;
		if (x1 < this->_drawAreaMinX) {
			clamp1 = true;
		}
		if (x2 < this->_drawAreaMinX) {
			clamp2 = true;
		}
		if (!clamp1) {
			this->_tmpCoords[1][clippedParamsNum].x = x1;
			this->_tmpCoords[1][clippedParamsNum].y = y1;
			clippedParamsNum++;
		}
		if (clamp1 != clamp2) {
			int16 tmpX = x2;
			int16 tmpY = y2;

			if (x2 <= x1) {
				int16 tmp = x1;
				x1 = x2;
				x2 = tmp;

				tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			int16 xDiff = x2 - x1;
			int16 yDiff = y2 - y1;
			int16 newY = -(x1 - this->_drawAreaMinX);
			newY = newY * yDiff / xDiff;
			newY += y1;
			this->_tmpCoords[1][clippedParamsNum].x = this->_drawAreaMinX;
			this->_tmpCoords[1][clippedParamsNum].y = newY;
			clippedParamsNum++;
			x1 = tmpX;
			y1 = tmpY;
			currentNumCoords--;
		} else {
			x1 = x2;
			y1 = y2;
			currentNumCoords--;
		}
	} while (currentNumCoords > 0);
	this->_tmpCoords[1][clippedParamsNum] = this->_tmpCoords[1][0];
	Coordinate *tmp = this->_tmpCoords[0];
	this->_tmpCoords[0] = this->_tmpCoords[1];
	this->_tmpCoords[1] = tmp;
	return clippedParamsNum;
}
uint16 GraphicsManager::polygonClipRight(uint16 currentNumCoords) {
	uint16 clippedParamsNum = 0;
	this->_polygonHasBeenClipped = true;
	uint16 idx = 0;
	int16 x1, y1, x2, y2;
	x1 = this->_tmpCoords[0][idx].x;
	y1 = this->_tmpCoords[0][idx].y;
	idx++;
	do {
		bool clamp1 = false, clamp2 = false;
		x2 = this->_tmpCoords[0][idx].x;
		y2 = this->_tmpCoords[0][idx].y;
		idx++;
		if (x1 > this->_drawAreaMaxX) {
			clamp1 = true;
		}
		if (x2 > this->_drawAreaMaxX) {
			clamp2 = true;
		}
		if (!clamp1) {
			this->_tmpCoords[1][clippedParamsNum].x = x1;
			this->_tmpCoords[1][clippedParamsNum].y = y1;
			clippedParamsNum++;
		}
		if (clamp1 != clamp2) {
			int16 tmpX = x2;
			int16 tmpY = y2;
			if (x2 <= x1) {
				int16 tmp = x1;
				x1 = x2;
				x2 = tmp;

				tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			int16 xDiff = x2 - x1;
			int16 yDiff = y2 - y1;
			int16 newY = -(x1 - this->_drawAreaMaxX);
			newY = newY * yDiff / xDiff;
			newY += y1;
			this->_tmpCoords[1][clippedParamsNum].x = this->_drawAreaMaxX;
			this->_tmpCoords[1][clippedParamsNum].y = newY;
			clippedParamsNum++;
			x1 = tmpX;
			y1 = tmpY;
			currentNumCoords--;
		} else {
			x1 = x2;
			y1 = y2;
			currentNumCoords--;
		}
	} while (currentNumCoords > 0);
	this->_tmpCoords[1][clippedParamsNum] = this->_tmpCoords[1][0];
	Coordinate *tmp = this->_tmpCoords[0];
	this->_tmpCoords[0] = this->_tmpCoords[1];
	this->_tmpCoords[1] = tmp;
	return clippedParamsNum;
}
uint16 GraphicsManager::polygonClipTop(uint16 currentNumCoords) {
	uint16 clippedParamsNum = 0;
	this->_polygonHasBeenClipped = true;
	uint16 idx = 0;
	int16 x1, y1, x2, y2;
	x1 = this->_tmpCoords[0][idx].x;
	y1 = this->_tmpCoords[0][idx].y;
	idx++;
	do {
		bool clamp1 = false, clamp2 = false;
		x2 = this->_tmpCoords[0][idx].x;
		y2 = this->_tmpCoords[0][idx].y;
		idx++;
		if (y1 < this->_drawAreaMinY) {
			clamp1 = true;
		}
		if (y2 < this->_drawAreaMinY) {
			clamp2 = true;
		}
		if (!clamp1) {
			this->_tmpCoords[1][clippedParamsNum].x = x1;
			this->_tmpCoords[1][clippedParamsNum].y = y1;
			clippedParamsNum++;
		}
		if (clamp1 != clamp2) {
			int16 tmpX = x2;
			int16 tmpY = y2;

			int16 xDiff = x2 - x1;
			int16 yDiff = y2 - y1;
			int16 newX = -(y1 - this->_drawAreaMinY);
			newX = newX * xDiff / yDiff;
			newX += x1;
			this->_tmpCoords[1][clippedParamsNum].x = newX;
			this->_tmpCoords[1][clippedParamsNum].y = this->_drawAreaMinY;
			clippedParamsNum++;
			x1 = tmpX;
			y1 = tmpY;
			currentNumCoords--;
		} else {
			x1 = x2;
			y1 = y2;
			currentNumCoords--;
		}
	} while (currentNumCoords > 0);
	this->_tmpCoords[1][clippedParamsNum] = this->_tmpCoords[1][0];
	Coordinate *tmp = this->_tmpCoords[0];
	this->_tmpCoords[0] = this->_tmpCoords[1];
	this->_tmpCoords[1] = tmp;
	return clippedParamsNum;
}
uint16 GraphicsManager::polygonClipBottom(uint16 currentNumCoords) {
	uint16 clippedParamsNum = 0;
	this->_polygonHasBeenClipped = true;
	uint16 idx = 0;
	int16 x1, y1, x2, y2;
	x1 = this->_tmpCoords[0][idx].x;
	y1 = this->_tmpCoords[0][idx].y;
	idx++;
	do {
		bool clamp1 = false, clamp2 = false;
		x2 = this->_tmpCoords[0][idx].x;
		y2 = this->_tmpCoords[0][idx].y;
		idx++;
		if (y1 > this->_drawAreaMaxY) {
			clamp1 = true;
		}
		if (y2 > this->_drawAreaMaxY) {
			clamp2 = true;
		}
		if (!clamp1) {
			this->_tmpCoords[1][clippedParamsNum].x = x1;
			this->_tmpCoords[1][clippedParamsNum].y = y1;
			clippedParamsNum++;
		}
		if (clamp1 != clamp2) {
			int16 tmpX = x2;
			int16 tmpY = y2;

			int16 xDiff = x2 - x1;
			int16 yDiff = y2 - y1;
			int16 newX = -(y1 - this->_drawAreaMaxY);
			newX = newX * xDiff / yDiff;
			newX += x1;
			this->_tmpCoords[1][clippedParamsNum].x = newX;
			this->_tmpCoords[1][clippedParamsNum].y = this->_drawAreaMaxY;
			clippedParamsNum++;
			x1 = tmpX;
			y1 = tmpY;
			currentNumCoords--;
		} else {
			x1 = x2;
			y1 = y2;
			currentNumCoords--;
		}
	} while (currentNumCoords > 0);
	this->_tmpCoords[1][clippedParamsNum] = this->_tmpCoords[1][0];
	Coordinate *tmp = this->_tmpCoords[0];
	this->_tmpCoords[0] = this->_tmpCoords[1];
	this->_tmpCoords[1] = tmp;
	return clippedParamsNum;
}
#define _DRAWING false
void GraphicsManager::drawPolygon(int16 x, int16 y, Coordinate *coords, uint16 numCoords, uint8 color) {
	this->_tmpCoords[0] = this->_tmpCoords1;
	this->_tmpCoords[1] = this->_tmpCoords2;
	if (_DRAWING) {
		int16 tmpMinX = 32767, tmpMinY = 32767;
		for (uint16 i = 0; i < numCoords; i++) {
			int16 tmp = coords[i].x;
			if (tmp < tmpMinX) {
				tmpMinX = tmp;
			}

			tmp = coords[i].y;
			if (tmp < tmpMinY) {
				tmpMinY = tmp;
			}
		}
		x = -tmpMinX;
		y = -tmpMinY;
	}
	uint16 startNumCoords = numCoords;
	uint16 clippedNumCoords = numCoords;
	uint16 currentNumCoords = numCoords;
	this->_polygonHasBeenClipped = false;
	int16 minX = 32767, maxX = -32768, minY = 32767, maxY = -32768;
	for (uint16 i = 0; i < currentNumCoords; i++) {
		int16 tmp = coords[i].x + x;
		if (tmp > maxX) {
			maxX = tmp;
		}
		if (tmp < minX) {
			minX = tmp;
		}
		_tmpCoords1[i].x = tmp;

		tmp = coords[i].y + y;
		if (tmp > maxY) {
			maxY = tmp;
		}
		if (tmp < minY) {
			minY = tmp;
		}
		_tmpCoords1[i].y = tmp;
	}
	_tmpCoords1[currentNumCoords] = _tmpCoords1[0];
	if (maxY == minY) {
		return;
	}

	if (minX < this->_drawAreaMinX) {
		clippedNumCoords = this->polygonClipLeft(currentNumCoords);
		if (clippedNumCoords <= 2) {
			return;
		}
		currentNumCoords = clippedNumCoords;
	}
	if (maxX > this->_drawAreaMaxX) {
		clippedNumCoords = this->polygonClipRight(currentNumCoords);
		if (clippedNumCoords <= 2) {
			return;
		}
		currentNumCoords = clippedNumCoords;
	}
	if (minY < this->_drawAreaMinY) {
		clippedNumCoords = this->polygonClipTop(currentNumCoords);
		if (clippedNumCoords <= 2) {
			return;
		}
		currentNumCoords = clippedNumCoords;
	}
	if (maxY > this->_drawAreaMaxY) {
		clippedNumCoords = this->polygonClipBottom(currentNumCoords);
		if (clippedNumCoords <= 2) {
			return;
		}
		currentNumCoords = clippedNumCoords;
	}
	if (this->_polygonHasBeenClipped) {
		// clipping occurred, so we have to recalculate min and max Y
		minY = 32767;
		maxY = -32768;
		for (uint8 i = 0; i < currentNumCoords; i++) {
			if (this->_tmpCoords[0][i].y < minY) {
				minY = this->_tmpCoords[0][i].y;
			}
			if (this->_tmpCoords[0][i].y > maxY) {
				maxY = this->_tmpCoords[0][i].y;
			}
		}
		if (minY == maxY) {
			return;
		}
	}
	this->_polygonXBuffer[0] = this->_polygonXBuffer1;
	this->_polygonXBuffer[1] = this->_polygonXBuffer2;
	uint8 coordIdx = 0;
	int16 x1 = this->_tmpCoords[0][coordIdx].x;
	int16 y1 = this->_tmpCoords[0][coordIdx].y;
	coordIdx++;
	do {
		int16 x2 = this->_tmpCoords[0][coordIdx].x;
		int16 y2 = this->_tmpCoords[0][coordIdx].y;
		coordIdx++;
		int16 tmpX = x2;
		int16 tmpY = y2;
		if (y2 != y1) {
			if (y1 == maxY || y1 == minY) {
				int16 *tmp = this->_polygonXBuffer[0];
				this->_polygonXBuffer[0] = this->_polygonXBuffer[1];
				this->_polygonXBuffer[1] = tmp;
			}
			if (x1 >= x2) {
				int16 tmp = x1;
				x1 = x2;
				x2 = tmp;
				tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			int16 destCoordIdx = y1;
			int8 destCoordIdxOffs = 1;
			if (y1 >= y2) {
				destCoordIdxOffs = -destCoordIdxOffs;
				int16 tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			int16 yDiff = y2 - y1;
			int16 xDiff = x2 - x1;
			this->_polygonXBuffer[0][destCoordIdx] = x1;
			destCoordIdx += destCoordIdxOffs;
			int16 tmpCounter = yDiff / 2;
			for (uint8 i = 0; i < yDiff; i++) {
				tmpCounter += xDiff;
				while (tmpCounter >= yDiff) {
					tmpCounter -= yDiff;
					x1++;
				}
				this->_polygonXBuffer[0][destCoordIdx] = x1;
				destCoordIdx += destCoordIdxOffs;
			}
		}
		x1 = tmpX;
		y1 = tmpY;
		currentNumCoords--;
	} while (currentNumCoords != 0);
	//here we actually start drawing the polygon
	int16 currY = minY;
	int16 height = (maxY - minY) + 1;
	uint8 *currLine = this->_videoBackbuffer + (this->_rowsOffsets[currY]);
	do {
		int16 _x1 = this->_polygonXBuffer[0][currY];
		int16 _x2 = this->_polygonXBuffer[1][currY];
		currY++;
		if (_x1 >= _x2) {
			int16 tmp = _x1;
			_x1 = _x2;
			_x2 = tmp;
		}
		int16 width = (_x2 - _x1) + 1;
		if (width > _COMET_XRESOLUTION) {
			int error = 1;
		}
		memset(currLine + _x1, color, width);
		currLine += _COMET_XRESOLUTION;
		height--;
	} while (height != 0);
}

void GraphicsManager::setPixelColor(uint8 newColor) {
	this->_currPixelColor = newColor;
}
void GraphicsManager::putPixel(uint16 x, uint16 y) {
	if (x < this->_drawAreaMinX) {
		return;
	}
	if (x > this->_drawAreaMaxX) {
		return;
	}
	if (y < this->_drawAreaMinY) {
		return;
	}
	if (y > this->_drawAreaMaxY) {
		return;
	}
	*(this->_videoBackbuffer + x + (this->_rowsOffsets[y])) = this->_currPixelColor;
}
void GraphicsManager::decodeFrameType1_special(int16 x, int16 y, uint8 *ptr) {
	// this procedure is pure EVIL :)
	// It's a different way of decoding type1 graphics,
	// it's basically a series of different pre-defined row patterns to be filled with a single color

	uint16 special = (this->_rowsOffsets[y] + x);
	for (uint8 i = 0; i < _COMET_YRESOLUTION; i++) {
		this->specialLinesOffsets[i] = special;
		special += _COMET_XRESOLUTION;
	}
	uint16 yOffset = 0;

	do {
		uint8 baseSubprocNum = *ptr++;
		uint8 count = *ptr++;
		if (baseSubprocNum & 0x80) {
			unsigned char bb = *ptr++;
			yOffset = 0 + bb;
			//				yOffset = yOffset << 1;
		}
		uint8 color = *ptr++;
		baseSubprocNum &= 0x3e;
		uint8 basePixelCount = 1;
		do {
			uint8 variablePixelCount = *ptr++;
			uint8 subprocNum = baseSubprocNum | (variablePixelCount & 0xc0);
			switch (subprocNum / 2) {
				//we're in for a long long journey...
			case 0:
			case 8:
			case 0x16: {
				//checked2
				for (uint8 i = 0; i < 6; i++) {
					uint16 destOffs = specialLinesOffsets[yOffset + i];
					uint8 count1 = basePixelCount + (variablePixelCount & 0x01);
					variablePixelCount = variablePixelCount >> 1;
					memset(this->_videoBackbuffer + destOffs, color, count1);
					specialLinesOffsets[yOffset + i] = destOffs + count1;
					if (i == 5) {
						basePixelCount = count1;
					}
				}
				yOffset += 6;
			} break;
			case 1:
			case 9:
			case 0x17:

			{
				//checked2
				for (uint8 i = 0; i < 6; i++) {
					uint8 count1 = basePixelCount + (variablePixelCount & 0x01);
					variablePixelCount = variablePixelCount >> 1;
					specialLinesOffsets[yOffset + i] += count1;
					if (i == 5) {
						basePixelCount = count1;
					}
				}
				yOffset += 6;
			} break;
			case 2:
			case 4:
			case 6:
			case 0x18:
			case 0x1a:
			case 0x2a: {
				//checked
				uint8 count1 = basePixelCount;
				for (uint8 i = 0; i < 6; i++) {
					uint16 destOffs = specialLinesOffsets[yOffset + i];
					count1 += (variablePixelCount & 0x01);
					variablePixelCount = variablePixelCount >> 1;
					memset(this->_videoBackbuffer + destOffs, color, count1);
					specialLinesOffsets[yOffset + i] = destOffs + count1;
					if (i == 5) {
						basePixelCount = count1;
					}
				}
				yOffset += 6;
			} break;
			case 3:
			case 5:
			case 7:
			case 0x19:
			case 0x1b:
			case 0x2b: {
				//checked2
				uint8 count1 = basePixelCount;
				for (uint8 i = 0; i < 6; i++) {
					uint16 destOffs = specialLinesOffsets[yOffset + i];
					count1 += (variablePixelCount & 0x01);
					variablePixelCount = variablePixelCount >> 1;
					specialLinesOffsets[yOffset + i] += count1;
					if (i == 5) {
						basePixelCount = count1;
					}
				}
				yOffset += 6;

			} break;
			case 0x0a:
			case 0x0c:
			case 0x0e:
			case 0x22:
			case 0x38:

			{
				//checked2
				uint8 count1 = basePixelCount;
				for (uint8 i = 0; i < 6; i++) {
					uint16 destOffs = specialLinesOffsets[yOffset + i];
					count1 -= (variablePixelCount & 0x01);
					variablePixelCount = variablePixelCount >> 1;
					memset(this->_videoBackbuffer + destOffs, color, count1);
					specialLinesOffsets[yOffset + i] = destOffs + count1;
					if (i == 5) {
						basePixelCount = count1;
					}
				}
				yOffset += 6;

			} break;
			case 0x0b:
			case 0x0d:
			case 0x0f:
			case 0x23:
			case 0x39: {
				//checked2
				uint8 count1 = basePixelCount;
				for (uint8 i = 0; i < 6; i++) {
					uint16 destOffs = specialLinesOffsets[yOffset + i];
					count1 -= (variablePixelCount & 0x01);
					variablePixelCount = variablePixelCount >> 1;

					specialLinesOffsets[yOffset + i] += count1;
					if (i == 5) {
						basePixelCount = count1;
					}
				}
				yOffset += 6;

			} break;
			case 0x10:
			case 0x12:
			case 0x14: {
				//checked2
				uint8 count1 = basePixelCount;
				count1 += (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				uint8 count2 = basePixelCount;
				count2 += (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				uint8 count3 = basePixelCount;
				count3 += (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				int8 numLoops = variablePixelCount & 0x07;

				do {
					uint16 destOffs = specialLinesOffsets[yOffset];
					memset(this->_videoBackbuffer + destOffs, color, count1);
					specialLinesOffsets[yOffset] = destOffs + count1;

					destOffs = specialLinesOffsets[yOffset + 1];
					memset(this->_videoBackbuffer + destOffs, color, count2);
					specialLinesOffsets[yOffset + 1] = destOffs + count2;

					destOffs = specialLinesOffsets[yOffset + 2];
					memset(this->_videoBackbuffer + destOffs, color, count3);
					specialLinesOffsets[yOffset + 2] = destOffs + count3;

					yOffset += 3;
					numLoops--;
				} while (numLoops >= 0);
				basePixelCount = count3;
			} break;
			case 0x11:
			case 0x13:
			case 0x15: {
				//checked2
				uint8 count1 = basePixelCount;
				count1 += (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				uint8 count2 = basePixelCount;
				count2 += (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				uint8 count3 = basePixelCount;
				count3 += (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				int8 numLoops = variablePixelCount & 0x07;

				do {
					uint16 destOffs = specialLinesOffsets[yOffset];
					specialLinesOffsets[yOffset] = destOffs + count1;

					destOffs = specialLinesOffsets[yOffset + 1];
					specialLinesOffsets[yOffset + 1] = destOffs + count2;

					destOffs = specialLinesOffsets[yOffset + 2];
					specialLinesOffsets[yOffset + 2] = destOffs + count3;

					yOffset += 3;
					numLoops--;
				} while (numLoops >= 0);
				basePixelCount = count3;
			} break;
			case 0x1c:
			case 0x3c:
			case 0x5c:
			case 0x7c: {
				//checked2
				uint8 count1 = count & 0x07;
				memset(this->_videoBackbuffer + specialLinesOffsets[yOffset], color, count1);
				specialLinesOffsets[yOffset] += count1;
				count = count >> 3;
				yOffset++;

				count1 = count & 0x07;
				if (count1 > 0) {
					memset(this->_videoBackbuffer + specialLinesOffsets[yOffset], color, count1);
					specialLinesOffsets[yOffset] += count1;
					count = count >> 3;
					yOffset++;
					count1 = count & 0x07;
					if (count1 > 0) {
						memset(this->_videoBackbuffer + specialLinesOffsets[yOffset], color, count1);
						specialLinesOffsets[yOffset] += count1;
						count = count >> 3;
						yOffset++;
					}
				}
				ptr--;
				count = 1;
			} break;
			case 0x1d:
			case 0x3d:
			case 0x5d:
			case 0x7d: {
				//checked2
				uint8 count1 = count & 0x07;
				specialLinesOffsets[yOffset] += count1;
				count = count >> 3;
				yOffset++;
				count1 = count & 0x07;
				if (count1 != 0) {
					specialLinesOffsets[yOffset] += count1;
					count = count >> 3;
					yOffset++;
					count1 = count & 0x07;
					if (count1 != 0) {
						specialLinesOffsets[yOffset] += count1;
						count = count >> 3;
						yOffset++;
					}
				}
				ptr--;
				count = 1;
			} break;
			case 0x1e:
			case 0x24:
			case 0x26:
			case 0x40:
			case 0x42:
			case 0x4e:
			case 0x50: {
				//checked
				uint8 count1 = basePixelCount + (variablePixelCount & 0x03);
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset], color, count1);
				this->specialLinesOffsets[yOffset] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 += variablePixelCount & 0x03;
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset + 1], color, count1);
				this->specialLinesOffsets[yOffset + 1] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 += variablePixelCount & 0x03;
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset + 2], color, count1);
				this->specialLinesOffsets[yOffset + 2] += count1;
				basePixelCount = count1;

				yOffset += 3;

			} break;
			case 0x1f:
			case 0x25:
			case 0x27:
			case 0x41:
			case 0x43:
			case 0x4f:
			case 0x51: {
				//checked
				uint8 count1 = basePixelCount + (variablePixelCount & 0x03);
				this->specialLinesOffsets[yOffset] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 += variablePixelCount & 0x03;
				this->specialLinesOffsets[yOffset + 1] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 += variablePixelCount & 0x03;
				this->specialLinesOffsets[yOffset + 2] += count1;
				basePixelCount = count1;

				yOffset += 3;

			} break;
			case 0x20:
			case 0x28:
			case 0x36: {
				//checked2
				uint8 count1 = 0;
				for (uint8 i = 0; i < 6; i++) {
					uint16 destOffs = specialLinesOffsets[yOffset + i];
					count1 = basePixelCount - (variablePixelCount & 0x01);
					variablePixelCount = variablePixelCount >> 1;
					memset(this->_videoBackbuffer + destOffs, color, count1);
					specialLinesOffsets[yOffset + i] = destOffs + count1;
					if (i == 5) {
						basePixelCount = count1;
					}
				}
				yOffset += 6;

			} break;
			case 0x21:
			case 0x29:
			case 0x37: {
				//checked2
				uint8 count1 = 0;
				for (uint8 i = 0; i < 6; i++) {
					uint16 destOffs = specialLinesOffsets[yOffset + i];
					count1 = basePixelCount - (variablePixelCount & 0x01);
					variablePixelCount = variablePixelCount >> 1;
					specialLinesOffsets[yOffset + i] = destOffs + count1;
					if (i == 5) {
						basePixelCount = count1;
					}
				}
				yOffset += 6;

			} break;
			case 0x2c:
			case 0x2e:
			case 0x46:
			case 0x48:
			case 0x4a:
			case 0x52: {
				//checked2
				uint8 count1 = basePixelCount - (variablePixelCount & 0x03);
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset], color, count1);
				this->specialLinesOffsets[yOffset] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 -= variablePixelCount & 0x03;
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset + 1], color, count1);
				this->specialLinesOffsets[yOffset + 1] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 -= variablePixelCount & 0x03;
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset + 2], color, count1);
				this->specialLinesOffsets[yOffset + 2] += count1;
				basePixelCount = count1;

				yOffset += 3;

			} break;
			case 0x2d:
			case 0x2f:
			case 0x47:
			case 0x49:
			case 0x4b:
			case 0x53: {
				//checked2
				uint8 count1 = basePixelCount - (variablePixelCount & 0x03);
				this->specialLinesOffsets[yOffset] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 -= variablePixelCount & 0x03;
				this->specialLinesOffsets[yOffset + 1] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 -= variablePixelCount & 0x03;
				this->specialLinesOffsets[yOffset + 2] += count1;
				basePixelCount = count1;

				yOffset += 3;

			} break;
			case 0x30:
			case 0x32:
			case 0x34: {
				//checked2
				uint8 count1 = basePixelCount;
				count1 -= (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				uint8 count2 = basePixelCount;
				count2 -= (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				uint8 count3 = basePixelCount;
				count3 -= (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				int8 numLoops = variablePixelCount & 0x07;

				do {
					uint16 destOffs = specialLinesOffsets[yOffset];
					memset(this->_videoBackbuffer + destOffs, color, count1);
					specialLinesOffsets[yOffset] = destOffs + count1;

					destOffs = specialLinesOffsets[yOffset + 1];
					memset(this->_videoBackbuffer + destOffs, color, count2);
					specialLinesOffsets[yOffset + 1] = destOffs + count2;

					destOffs = specialLinesOffsets[yOffset + 2];
					memset(this->_videoBackbuffer + destOffs, color, count3);
					specialLinesOffsets[yOffset + 2] = destOffs + count3;

					yOffset += 3;
					numLoops--;
				} while (numLoops >= 0);
				basePixelCount = count3;
			} break;
			case 0x31:
			case 0x33:
			case 0x35: {
				//checked2
				uint8 count1 = basePixelCount;
				count1 -= (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				uint8 count2 = basePixelCount;
				count2 -= (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				uint8 count3 = basePixelCount;
				count3 -= (variablePixelCount & 0x01);
				variablePixelCount = variablePixelCount >> 1;
				int8 numLoops = variablePixelCount & 0x07;

				do {
					uint16 destOffs = specialLinesOffsets[yOffset];
					specialLinesOffsets[yOffset] = destOffs + count1;

					destOffs = specialLinesOffsets[yOffset + 1];
					specialLinesOffsets[yOffset + 1] = destOffs + count2;

					destOffs = specialLinesOffsets[yOffset + 2];
					specialLinesOffsets[yOffset + 2] = destOffs + count3;

					yOffset += 3;
					numLoops--;
				} while (numLoops >= 0);
				basePixelCount = count3;
			} break;
			case 0x3a:
			case 0x56:
			case 0x58: {
				//checked2
				uint8 count1 = basePixelCount + (variablePixelCount & 0x03) - 1;
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset], color, count1);
				this->specialLinesOffsets[yOffset] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 += (variablePixelCount & 0x03) - 1;
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset + 1], color, count1);
				this->specialLinesOffsets[yOffset + 1] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 += (variablePixelCount & 0x03) - 1;
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset + 2], color, count1);
				this->specialLinesOffsets[yOffset + 2] += count1;
				basePixelCount = count1;

				yOffset += 3;

			} break;
			case 0x3b:
			case 0x57:
			case 0x59: {
				//checked2
				uint8 count1 = basePixelCount + (variablePixelCount & 0x03) - 1;
				this->specialLinesOffsets[yOffset] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 += (variablePixelCount & 0x03) - 1;
				this->specialLinesOffsets[yOffset + 1] += count1;
				variablePixelCount = variablePixelCount >> 2;

				count1 += (variablePixelCount & 0x03) - 1;
				this->specialLinesOffsets[yOffset + 2] += count1;
				basePixelCount = count1;

				yOffset += 3;

			} break;
			case 0x3e:
			case 0x44:
			case 0x5a: {
				//checked
				uint8 count1 = basePixelCount - (variablePixelCount & 0x07);
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset], color, count1);
				this->specialLinesOffsets[yOffset] += count1;
				variablePixelCount = variablePixelCount >> 3;

				count1 -= (variablePixelCount & 0x07);
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset + 1], color, count1);
				this->specialLinesOffsets[yOffset + 1] += count1;
				basePixelCount = count1;

				yOffset += 2;

			} break;
			case 0x3f:
			case 0x45:
			case 0x5b: {
				//checked
				uint8 count1 = basePixelCount - (variablePixelCount & 0x07);
				this->specialLinesOffsets[yOffset] += count1;
				variablePixelCount = variablePixelCount >> 3;

				count1 -= (variablePixelCount & 0x07);
				this->specialLinesOffsets[yOffset + 1] += count1;
				basePixelCount = count1;

				yOffset += 2;

			} break;
			case 0x4c:
			case 0x54:
			case 0x5e: {
				//checked2
				uint8 count1 = basePixelCount + (variablePixelCount & 0x07);
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset], color, count1);
				this->specialLinesOffsets[yOffset] += count1;
				variablePixelCount = variablePixelCount >> 3;

				count1 += (variablePixelCount & 0x07);
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset + 1], color, count1);
				this->specialLinesOffsets[yOffset + 1] += count1;
				basePixelCount = count1;

				yOffset += 2;

			} break;
			case 0x4d:
			case 0x55:
			case 0x5f: {
				////checked
				uint8 count1 = basePixelCount + (variablePixelCount & 0x07);
				this->specialLinesOffsets[yOffset] += count1;
				variablePixelCount = variablePixelCount >> 3;

				count1 += (variablePixelCount & 0x07);
				this->specialLinesOffsets[yOffset + 1] += count1;
				basePixelCount = count1;

				yOffset += 2;

			} break;
			case 0x60:
			case 0x62:
			case 0x64:
			case 0x66:
			case 0x68:
			case 0x6a:
			case 0x6c:
			case 0x6e:
			case 0x70:
			case 0x72:
			case 0x74:
			case 0x76:
			case 0x78:
			case 0x7a:
			case 0x7e: {
				//checked
				uint8 count1 = (variablePixelCount & 0x3f);
				basePixelCount = count1;
				memset(this->_videoBackbuffer + this->specialLinesOffsets[yOffset], color, count1);
				this->specialLinesOffsets[yOffset] += count1;

				yOffset++;

			} break;
			case 0x61:
			case 0x63:
			case 0x65:
			case 0x67:
			case 0x69:
			case 0x6b:
			case 0x6d:
			case 0x6f:
			case 0x71:
			case 0x73:
			case 0x75:
			case 0x77:
			case 0x79:
			case 0x7b:
			case 0x7f:

			{
				//checked
				uint8 count1 = (variablePixelCount & 0x3f);
				basePixelCount = count1;
				this->specialLinesOffsets[yOffset] += count1;

				yOffset++;

			} break;
			default:
				warning("type1_special unknown value");
				break;
			}
			count--;
		} while (count > 0);
	} while (*ptr != 0);
	//TODO:
}
void GraphicsManager::decodeFrameType1(uint16 idxAndFlags, uint16 left, uint16 bottom, uint8 *surface, uint8 *dataPtr) {
	uint8 *ptr = dataPtr;
	uint16 drawX = left;
	int16 drawY = bottom;
	uint16 idx = idxAndFlags & 0xFFF;
	uint16 flags = idxAndFlags;
	uint16 leftClamp = 0;
	int16 tmp = 0;
	uint16 tmpI = 0, tmpJ = 0, tmpK = 0;
	uint16 numBytes = 0, bytesUsed = 0;
	uint8 numElements = 0;

	ptr += idx * 4;
	ptr = dataPtr + (READ_LE_UINT32(ptr) - 2);
	ptr += 2;
	int16 width = *ptr * 16;
	uint32 toNextLine = _COMET_XRESOLUTION - width;
	int16 height = (unsigned char)*(ptr + 1);
	ptr += 2;
	drawY = drawY - height + 1;

	//only for test drawing
	/*
		if (drawY < 0) {
			drawY = 0;
		}
	*/

	uint8 *destPtr = surface + (drawY * _COMET_XRESOLUTION);
	bool isHorizontallyClamped = false;
	flags ^= READ_LE_UINT16(ptr - 4);
	if (flags & 0x8000) { //TODO check for endianness
		//is x mirrored
		uint8 *tmpSrc = ptr;
		tmp = width;
		WRITE_LE_UINT16(ptr - 4, READ_LE_UINT16(ptr - 4) ^ 0x8000); //TODO check for endianness
		char *bPtr = this->_rowBuffer + 398;
		for (tmpI = 0; tmpI < height; tmpI++) {
			numElements = *ptr;
			ptr++;
			bytesUsed = 1;
			*bPtr++ = *ptr++;
			for (tmpJ = 0; tmpJ < numElements; tmpJ++) {
				bPtr -= 2;
				numBytes = READ_LE_UINT16(ptr);
				ptr += 2;
				if (numBytes != 0) {
					tmp = numBytes;
					numBytes = ((numBytes % 256) * 4) + (numBytes / 256);
					bytesUsed += numBytes + 3;
					for (tmpK = 0; tmpK < numBytes; tmpK++) {
						*bPtr++ = *ptr++;
						bPtr -= 2;
					}
					bPtr -= 2;
				}
				WRITE_LE_UINT16(bPtr + 1, tmp);
				*bPtr++ = *ptr++;
			}
			ptr -= bytesUsed;
			bPtr--;
			for (tmpK = 0; tmpK < bytesUsed; tmpK++) {
				*ptr++ = *bPtr++;
			}
			bPtr--;
		}
		ptr = tmpSrc;
	}
	//let's check if the image is bottom-clamped by the viewport
	tmp = drawY + height - this->_drawAreaMaxY - 1;
	if (tmp > 0) {
		//is bottom clamped
		height -= tmp;
		if (height <= 0) {
			//nothing to draw
			return;
		}
	}
	//let's check if the image is top-clamped by the viewport
	tmp = drawY - this->_drawAreaMinY;
	if (tmp < 0) {
		//it is top-clamped, let's subtract the clamped lines from the height
		height += tmp; //remember, tmp is negative here
		if (height < 0) {
			//nothing to draw
			return;
		}
		//we should now skip the clamped lines from the input data
		tmp = -tmp; //tmp is now the positive number or lines to skip
		for (tmpI = 0; tmpI < tmp; tmpI++) {
			numElements = *ptr++;
			for (tmpJ = 0; tmpJ < numElements; tmpJ++) {
				numBytes = READ_LE_UINT16(ptr + 1);
				numBytes = ((numBytes % 256) * 4) + (numBytes / 256);
				ptr += numBytes + 3;
			}
			ptr++;
			destPtr += _COMET_XRESOLUTION;
		}
	}
	//let's check if the image is left-clamped by the viewport
	tmp = this->_drawAreaMinX - drawX;
	if (tmp > 0) {
		//is left clamped;
		isHorizontallyClamped = true;
		width -= tmp;
		drawX += tmp;
		leftClamp = tmp;
		toNextLine += tmp;
	}
	//let's check if the image is right-clamped by the viewport
	tmp = drawX + width - this->_drawAreaMaxX - 1;
	if (tmp > 0) {
		//is right-clamped
		isHorizontallyClamped = true;
		width -= tmp;
		toNextLine += tmp;
	}
	if (width <= 0) {
		//nothing to draw
		return;
	}
	//ok, draw it
	destPtr += drawX;
	if (!(flags & 0x4000)) {
		//draw with transparency
		if (!isHorizontallyClamped) {
			//transparency, no Horizontal clamp
			for (tmpI = 0; tmpI < height; tmpI++) {
				numElements = *ptr++;
				if (numElements > width) {
					//hack for some invalid images?
					return;
				}
				for (tmpJ = 0; tmpJ < numElements; tmpJ++) {
					uint8 preSkip = *ptr++;
					destPtr += preSkip;
					if (preSkip > width) {
						//hack for some invalid images?
						return;
					}
					numBytes = READ_LE_UINT16(ptr);
					ptr += 2;
					numBytes = ((numBytes % 256) * 4) + (numBytes / 256);
					if (numBytes > width) {
						//hack for some invalid images?
						return;
					}
					for (tmpK = 0; tmpK < numBytes; tmpK++) {
						*destPtr++ = *ptr++;
					}
				}
				uint8 postSkip = *ptr++;
				if (postSkip > width) {
					//hack for some invalid images?
					return;
				}

				destPtr += postSkip;
				destPtr += toNextLine;
			}
		} else {
			//transparenct, horizontal clamp
			for (tmpI = 0; tmpI < height; tmpI++) {
				char *bPtr = this->_rowBuffer;
				numElements = *ptr++;
				for (tmpJ = 0; tmpJ < numElements; tmpJ++) {
					uint8 preSkip = *ptr++;
					for (tmpK = 0; tmpK < preSkip; tmpK++) {
						*bPtr++ = 0;
					}
					numBytes = READ_LE_UINT16(ptr);
					ptr += 2;
					numBytes = ((numBytes % 256) * 4) + (numBytes / 256);
					for (tmpK = 0; tmpK < numBytes; tmpK++) {
						*bPtr++ = *ptr++;
					}
				}
				uint8 postSkip = *ptr++;
				for (tmpK = 0; tmpK < postSkip; tmpK++) {
					*bPtr++ = 0;
				}
				bPtr = this->_rowBuffer + leftClamp;
				for (tmpK = 0; tmpK < width; tmpK++) {
					uint8 pixel = *bPtr++;
					if (pixel != 0) {
						*destPtr = pixel;
					}
					destPtr++;
				}
				destPtr += toNextLine;
			}
		}
	} else {
		// draw opaque
		if (!isHorizontallyClamped) {
			//opaque, no Horizontal clamp
			for (tmpI = 0; tmpI < height; tmpI++) {
				numElements = *ptr++;
				for (tmpJ = 0; tmpJ < numElements; tmpJ++) {
					uint8 preSkip = *ptr++;
					for (tmpK = 0; tmpK < preSkip; tmpK++) {
						*destPtr++ = 0;
					}
					numBytes = READ_LE_UINT16(ptr);
					ptr += 2;
					numBytes = ((numBytes % 256) * 4) + (numBytes / 256);
					for (tmpK = 0; tmpK < numBytes; tmpK++) {
						*destPtr++ = *ptr++;
					}
				}
				uint8 postSkip = *ptr++;
				for (tmpK = 0; tmpK < postSkip; tmpK++) {
					*destPtr++ = 0;
				}
				destPtr += toNextLine;
			}

		} else {
			//opaque, horizontal clamp
			for (tmpI = 0; tmpI < height; tmpI++) {
				char *bPtr = this->_rowBuffer;
				numElements = *ptr++;
				for (tmpJ = 0; tmpJ < numElements; tmpJ++) {
					uint8 preSkip = *ptr++;
					for (tmpK = 0; tmpK < preSkip; tmpK++) {
						*bPtr++ = 0;
					}
					numBytes = READ_LE_UINT16(ptr);
					ptr += 2;
					numBytes = ((numBytes % 256) * 4) + (numBytes / 256);
					for (tmpK = 0; tmpK < numBytes; tmpK++) {
						*bPtr++ = *ptr++;
					}
				}
				uint8 postSkip = *ptr++;
				for (tmpK = 0; tmpK < postSkip; tmpK++) {
					*bPtr++ = 0;
				}
				bPtr = this->_rowBuffer + leftClamp;
				for (tmpK = 0; tmpK < width; tmpK++) {
					*destPtr++ = *bPtr++;
				}
				destPtr += toNextLine;
			}
		}
	}
}

void GraphicsManager::decodeFrame(uint16 idxAndFlags, uint16 x, uint16 y, uint8 *dataPtr) {

	uint8 *ptr = dataPtr;
	ptr += READ_LE_UINT16(ptr);
	uint16 idx = idxAndFlags & 0x0fff; //TODO check endianness
	uint8 flags = (idxAndFlags >> 8) & 0xa0;
	ptr += READ_LE_UINT16(ptr + (idx * 4));
	uint8 flagsFromFile = *ptr++;
	uint8 numOperations = *ptr++;

	configureDecodingParameters_v1(flags | flagsFromFile);
	for (uint8 i = 0; i < numOperations; i++) {
		uint8 operation = *ptr++;
		uint8 numCoordinates = *ptr++;
		uint8 extraParamLO = *ptr++;
		uint8 extraParamHI = *ptr++;
		uint8 coordIdx = 0;
		for (coordIdx = 0; coordIdx < numCoordinates; coordIdx++) {

			int16 _tX = 0, _tY = 0;
			if (_isByteParams_v1) {
				_tX = *ptr++;
				_tY = *ptr++;
			} else {
				_tX = READ_LE_INT16(ptr);
				ptr += 2;
				_tY = READ_LE_INT16(ptr);
				ptr += 2;
			}
			if (_isNegParam1_v1) {
				_tX = -_tX;
			}
			if (_isNegParam2_v1) {
				_tY = -_tY;
			}
			this->_coordinates[coordIdx].x = x + _tX;
			this->_coordinates[coordIdx].y = y + _tY;
		}
		switch (operation) {
		case 0: {
			uint8 flagsBkp = _decodingFlags_v1;
			this->decodeFrame(extraParamLO + ((extraParamHI | _decodingFlags_v1) << 8), this->_coordinates[0].x, this->_coordinates[0].y, dataPtr);
			configureDecodingParameters_v1(flagsBkp);
		} break;
		case 1: {
			uint8 *dataPtr2 = dataPtr + READ_LE_UINT32(dataPtr + 4);
			uint16 idx2 = extraParamLO + ((extraParamHI ^ _decodingFlags_v1) << 8);
			if (_decodingFlags_v1 & 0x80) {
				this->_coordinates[0].x -= getWidthOfType1Image(idx2, dataPtr2);
			}
			if (_decodingFlags_v1 & 0x20) {
				this->_coordinates[0].y += getHeightOfType1Image(idx2, dataPtr2);
			}
			this->decodeFrameType1(idx2, this->_coordinates[0].x, this->_coordinates[0].y, _videoBackbuffer, dataPtr2);
		} break;
		case 2:
		case 3: {
			uint16 skip = (256 - numCoordinates) * 4;
			ptr += skip;
		} break;
		case 4: {
			this->drawPolygon(0, 0, this->_coordinates, numCoordinates, extraParamHI);
			if (extraParamLO != 0xff) {
				this->_coordinates[numCoordinates] = this->_coordinates[0];
				for (uint8 j = 0; j < numCoordinates; j++) {
					this->drawLine(this->_coordinates[j].x, this->_coordinates[j].y, this->_coordinates[j + 1].x, this->_coordinates[j + 1].y, extraParamLO);
				}
			}
		} break;
		case 5: {
			int16 x1 = this->_coordinates[0].x;
			int16 y1 = this->_coordinates[0].y;
			int16 x2 = this->_coordinates[1].x;
			int16 y2 = this->_coordinates[1].y;
			if (x1 > x2) {
				int16 tmp = x1;
				x1 = x2;
				x2 = tmp;
			}
			if (y1 > y2) {
				int16 tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			this->drawRectangleFilled(x1, y1, x2, y2, extraParamHI);
			if (extraParamLO != 0xff) {
				this->drawRectangleOutline(x1, y1, x2, y2, extraParamLO);
			}
		} break;
		case 6:
		case 8:
		case 9: {
			for (uint8 j = 0; j < numCoordinates - 1; j++) {
				this->drawLine(this->_coordinates[j].x, this->_coordinates[j].y, this->_coordinates[j + 1].x, this->_coordinates[j + 1].y, extraParamHI);
			}
		} break;
		case 7: {
			this->setPixelColor(extraParamHI);
			for (uint8 j = 0; j < numCoordinates; j++) {
				this->putPixel(this->_coordinates[j].x, this->_coordinates[j].y);
			}
		} break;
		case 10: {
			uint16 numFrame = (extraParamLO + (extraParamHI * 256)) * 4;
			uint16 offsType1Block = READ_LE_UINT32(dataPtr + 4);
			uint8 *tmpPtr = dataPtr + offsType1Block;
			uint16 offsFrame = READ_LE_UINT32(tmpPtr + (numFrame));
			tmpPtr += offsFrame;
			uint8 height = *(tmpPtr + 1);
			height--;
			tmpPtr += 2;
			this->_coordinates[0].y -= height;
			//TODO just for testing
			/*
				if (this->_coordinates[0].y < 0) {
					this->_coordinates[0].y = 0;
				}
				if (this->_coordinates[0].x < 0) {
					this->_coordinates[0].x = 0;
				}
				*/
			//

			this->decodeFrameType1_special(this->_coordinates[0].x, this->_coordinates[0].y, tmpPtr);

		} break;
		}
	}
}
void GraphicsManager::decodeFrame_scaled(uint16 idxAndFlags, uint16 x, uint16 y, uint8 *dataPtr, uint16 scale) {
	uint8 *ptr = dataPtr;
	ptr += READ_LE_UINT32(ptr);
	uint16 idx = idxAndFlags & 0x0fff; //TODO check endianness
	uint8 flags = (idxAndFlags >> 8) & 0xa0;
	ptr += READ_LE_UINT32(ptr + (idx * 4));
	uint8 flagsFromFile = *ptr++;
	uint8 numOperations = *ptr++;

	configureDecodingParameters_v2(flags | flagsFromFile);
	for (uint8 i = 0; i < numOperations; i++) {
		uint8 operation = *ptr++;
		uint8 numCoordinates = *ptr++;
		uint8 extraParamLO = *ptr++;
		uint8 extraParamHI = *ptr++;
		uint8 coordIdx = 0;
		for (coordIdx = 0; coordIdx < numCoordinates; coordIdx++) {
			int16 _tX = 0, _tY = 0;
			if (_isByteParams_v2) {
				_tX = *ptr++;
				_tY = *ptr++;
			} else {
				_tX = READ_LE_INT16(ptr);
				ptr += 2;
				_tY = READ_LE_INT16(ptr);
				ptr += 2;
			}
			if (_isNegParam1_v2) {
				_tX = -_tX;
			}
			if (_isNegParam2_v2) {
				_tY = -_tY;
			}
			this->_coordinates[coordIdx].x = x + ((_tX * scale) / 256);
			this->_coordinates[coordIdx].y = y + ((_tY * scale) / 256);
		}
		switch (operation) {
		case 0: {
			uint8 flagsBkp = _decodingFlags_v1;
			this->decodeFrame(extraParamLO + ((extraParamHI | _decodingFlags_v1) << 8), this->_coordinates[0].x, this->_coordinates[0].y, dataPtr);
			configureDecodingParameters_v1(flagsBkp);
		} break;
		case 1: {
			uint8 *dataPtr2 = dataPtr + READ_LE_UINT32(dataPtr + 4);
			uint16 idx2 = extraParamLO + ((extraParamHI ^ _decodingFlags_v1) << 8);
			if (_decodingFlags_v1 & 0x80) {
				this->_coordinates[0].x -= getWidthOfType1Image(idx2, dataPtr2);
			}
			if (_decodingFlags_v1 & 0x20) {
				this->_coordinates[0].y += getHeightOfType1Image(idx2, dataPtr2);
			}
			this->decodeFrameType1(idx2, this->_coordinates[0].x, this->_coordinates[0].y, _videoBackbuffer, dataPtr2);
		} break;
		case 2:
		case 3: {
			uint16 skip = (256 - numCoordinates) * 4;
			ptr += skip;
		} break;
		case 4: {
			this->drawPolygon(0, 0, this->_coordinates, numCoordinates, extraParamHI);
			if (extraParamLO != 0xff) {
				this->_coordinates[numCoordinates] = this->_coordinates[0];
				for (uint8 j = 0; j < numCoordinates; j++) {
					this->drawLine(this->_coordinates[j].x, this->_coordinates[j].y, this->_coordinates[j + 1].x, this->_coordinates[j + 1].y, extraParamLO);
				}
			}
		} break;
		case 5: {
			uint16 x1 = this->_coordinates[0].x;
			uint16 y1 = this->_coordinates[0].y;
			uint16 x2 = this->_coordinates[1].x;
			uint16 y2 = this->_coordinates[1].y;
			if (x1 > x2) {
				uint16 tmp = x1;
				x1 = x2;
				x2 = tmp;
			}
			if (y1 > y2) {
				uint16 tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			this->drawRectangleFilled(x1, y1, x2, y2, extraParamHI);
			if (extraParamLO != 0xff) {
				this->drawRectangleOutline(x1, y1, x2, y2, extraParamLO);
			}
		} break;
		case 6:
		case 8:
		case 9: {
			for (uint8 j = 0; j < numCoordinates - 1; j++) {
				this->drawLine(this->_coordinates[j].x, this->_coordinates[j].y, this->_coordinates[j + 1].x, this->_coordinates[j + 1].y, extraParamHI);
			}
		} break;
		case 7: {
			this->setPixelColor(extraParamHI);
			for (uint8 j = 0; j < numCoordinates; j++) {
				this->putPixel(this->_coordinates[j].x, this->_coordinates[j].y);
			}
		} break;
		case 10: {
			uint16 numFrame = extraParamLO + (extraParamHI * 256);
			uint16 offsType1Block = READ_LE_UINT32(dataPtr + 4);
			uint8 *tmpPtr = dataPtr + offsType1Block;
			uint16 offsFrame = READ_LE_UINT32(tmpPtr + (numFrame * 4));
			tmpPtr += offsFrame;
			uint8 height = *(tmpPtr + 1);
			height--;
			tmpPtr += 2;
			this->_coordinates[0].y -= height;
			//TODO just for testing
			/*
				if (this->_coordinates[0].y < 0) {
					this->_coordinates[0].y = 0;
				}
				if (this->_coordinates[0].x < 0) {
					this->_coordinates[0].x = 0;
				}
				//
				*/
			this->decodeFrameType1_special(this->_coordinates[0].x, this->_coordinates[0].y, tmpPtr);

		} break;
		}
	}
}
void GraphicsManager::decodeFrame_rotated(uint16 idxAndFlags, uint16 x, uint16 y, uint8 *dataPtr, uint16 angle) {
	int16 sine = _sineCosine[angle];
	int16 cosine = _sineCosine[angle + 0x40];

	uint8 *ptr = dataPtr;
	ptr += READ_LE_UINT32(ptr);
	uint16 idx = idxAndFlags & 0x0fff; //TODO check endianness
	uint8 flags = (idxAndFlags >> 8) & 0xa0;
	ptr += READ_LE_UINT32(ptr + (idx * 4));
	uint8 flagsFromFile = *ptr++;
	uint8 numOperations = *ptr++;

	configureDecodingParameters_v2(flags | flagsFromFile);
	for (uint8 i = 0; i < numOperations; i++) {
		uint8 operation = *ptr++;
		uint8 numCoordinates = *ptr++;
		uint8 extraParamLO = *ptr++;
		uint8 extraParamHI = *ptr++;
		uint8 coordIdx = 0;
		for (coordIdx = 0; coordIdx < numCoordinates; coordIdx++) {
			int16 unrotatedX = READ_LE_INT16(ptr);
			ptr += 2;
			int16 unrotatedY = READ_LE_INT16(ptr);
			ptr += 2;
			this->_coordinates[coordIdx].x = x + (((unrotatedX * cosine) - (unrotatedY * sine)) >> 6);
			this->_coordinates[coordIdx].y = y + (((unrotatedX * sine) + (unrotatedY * cosine)) >> 6);
		}
		switch (operation) {
		case 0: {
			uint8 flagsBkp = _decodingFlags_v1;
			this->decodeFrame(extraParamLO + ((extraParamHI | _decodingFlags_v1) << 8), this->_coordinates[0].x, this->_coordinates[0].y, dataPtr);
			configureDecodingParameters_v1(flagsBkp);
		} break;
		case 1: {
			uint8 *dataPtr2 = dataPtr + READ_LE_UINT32(dataPtr + 4);
			uint16 idx2 = extraParamLO + ((extraParamHI ^ _decodingFlags_v1) << 8);
			if (_decodingFlags_v1 & 0x80) {
				this->_coordinates[0].x -= getWidthOfType1Image(idx2, dataPtr2);
			}
			if (_decodingFlags_v1 & 0x20) {
				this->_coordinates[0].y += getHeightOfType1Image(idx2, dataPtr2);
			}
			this->decodeFrameType1(idx2, this->_coordinates[0].x, this->_coordinates[0].y, _videoBackbuffer, dataPtr2);
		} break;
		case 2:
		case 3: {
			uint16 skip = (256 - numCoordinates) * 4;
			ptr += skip;
		} break;
		case 4: {
			this->drawPolygon(0, 0, this->_coordinates, numCoordinates, extraParamHI);
			if (extraParamLO != 0xff) {
				this->_coordinates[numCoordinates] = this->_coordinates[0];
				for (uint8 j = 0; j < numCoordinates; j++) {
					this->drawLine(this->_coordinates[j].x, this->_coordinates[j].y, this->_coordinates[j + 1].x, this->_coordinates[j + 1].y, extraParamLO);
				}
			}
		} break;
		case 5: {
			uint16 x1 = this->_coordinates[0].x;
			uint16 y1 = this->_coordinates[0].y;
			uint16 x2 = this->_coordinates[1].x;
			uint16 y2 = this->_coordinates[1].y;
			if (x1 > x2) {
				uint16 tmp = x1;
				x1 = x2;
				x2 = tmp;
			}
			if (y1 > y2) {
				uint16 tmp = y1;
				y1 = y2;
				y2 = tmp;
			}
			this->drawRectangleFilled(x1, y1, x2, y2, extraParamHI);
			if (extraParamLO != 0xff) {
				this->drawRectangleOutline(x1, y1, x2, y2, extraParamLO);
			}
		} break;
		case 6:
		case 8:
		case 9: {
			for (uint8 j = 0; j < numCoordinates - 1; j++) {
				this->drawLine(this->_coordinates[j].x, this->_coordinates[j].y, this->_coordinates[j + 1].x, this->_coordinates[j + 1].y, extraParamHI);
			}
		} break;
		case 7: {
			this->setPixelColor(extraParamHI);
			for (uint8 j = 0; j < numCoordinates; j++) {
				this->putPixel(this->_coordinates[j].x, this->_coordinates[j].y);
			}
		} break;
		case 10: {
			uint16 numFrame = extraParamLO + (extraParamHI * 256);
			uint16 offsType1Block = READ_LE_UINT32(dataPtr + 4);
			uint8 *tmpPtr = dataPtr + offsType1Block;
			uint16 offsFrame = READ_LE_UINT32(tmpPtr + (numFrame * 4));
			tmpPtr += offsFrame;
			uint8 height = *(tmpPtr + 1);
			height--;
			tmpPtr += 2;
			this->_coordinates[0].y -= height;
			//TODO just for testing
			/*
				if (this->_coordinates[0].y < 0) {
					this->_coordinates[0].y = 0;
				}
				if (this->_coordinates[0].x < 0) {
					this->_coordinates[0].x = 0;
				}
				//
				*/
			this->decodeFrameType1_special(this->_coordinates[0].x, this->_coordinates[0].y, tmpPtr);

		} break;
		}
	}
}

void GraphicsManager::scrollRoomLeft() {
	scrollRoomHoriz(-1);
}
void GraphicsManager::scrollRoomRight() {
	scrollRoomHoriz(1);
}
void GraphicsManager::loadStageBackground() {
	_vm->_archMgr->getFile(_vm->_gmMgr->getStagesFilename(), _vm->_gameState.stageIndex, _backgroundBuffer);
}
void GraphicsManager::scrollRoomHoriz(int16 direction) {
	copyVideoBuffer(lockMainSurface(), _videoBackbuffer);
	unlockMainSurface();
	for (uint16 i = 0; i < 320; i += 0x28) {
		if (direction == -1) {
			scrollBackbufferLeft(40);
			copyBackgroundSlice_left(40, i);
		} else {
			scrollBackbufferRight(40);
			copyBackgroundSlice_right(40, i);
		}
		paintBackbuffer();
		_vm->_gmMgr->handlePause();
	}
}
void GraphicsManager::scrollBackbufferLeft(uint16 numPixels) {
	uint8 *src = _videoBackbuffer + _rowsOffsets[0] + numPixels;
	uint8 *dst = _videoBackbuffer + _rowsOffsets[0];
	uint16 size = _COMET_XRESOLUTION - numPixels;
	for (uint16 i = 0; i < _COMET_YRESOLUTION; i++) {
		memmove(dst, src, size);
		dst += size + numPixels;
		src += size + numPixels;
	}
}
void GraphicsManager::copyBackgroundSlice_left(uint16 width, uint16 offsetLeft) {
	uint8 *src = _backgroundBuffer + _rowsOffsets[0] + offsetLeft;
	uint8 *dst = _videoBackbuffer + _rowsOffsets[0] + _COMET_XRESOLUTION - width;
	uint16 remain = _COMET_XRESOLUTION - width;
	for (uint16 i = 0; i < _COMET_YRESOLUTION; i++) {
		memmove(dst, src, width);
		dst += width + remain;
		src += width + remain;
	}
}
void GraphicsManager::scrollBackbufferRight(uint16 numPixels) {
	uint8 *src = _videoBackbuffer + (_COMET_XRESOLUTION * _COMET_YRESOLUTION) - 1 - numPixels;
	uint8 *dst = _videoBackbuffer + (_COMET_XRESOLUTION * _COMET_YRESOLUTION) - 1;
	uint16 size = _COMET_XRESOLUTION - numPixels;
	for (uint16 i = 0; i < _COMET_YRESOLUTION; i++) {
		for (uint16 j = 0; j < size; j++) {
			*dst-- = *src--;
		}
		dst -= numPixels;
		src -= numPixels;
	}
}
void GraphicsManager::copyBackgroundSlice_right(uint16 width, uint16 offsetRight) {
	uint8 *src = _backgroundBuffer + _rowsOffsets[0] + _COMET_XRESOLUTION - (width + offsetRight);
	uint8 *dst = _videoBackbuffer + _rowsOffsets[0];
	uint16 remain = _COMET_XRESOLUTION - width;
	for (uint16 i = 0; i < _COMET_YRESOLUTION; i++) {
		memmove(dst, src, width);
		dst += width + remain;
		src += width + remain;
	}
}
void GraphicsManager::backgroundToBackbuffer() {
	memcpy(_videoBackbuffer, _backgroundBuffer, _COMET_XRESOLUTION * _COMET_YRESOLUTION);
}
void GraphicsManager::setDrawWithEffect() {
	_isPaintWithEffect = true;
}
void GraphicsManager::paintBackbuffer_withEffect() {
	_isPaintWithEffect = false;
	uint32 baseOffs = 0;
	for (uint8 i = 0; i < 7; i++) {
		//		for (uint8 jj = 0; jj < 20; jj++) {
		waitVRetrace();
		//		waitVRetrace();
		//		}
		uint8 *src = _videoBackbuffer + baseOffs;
		uint8 *dst = lockMainSurface() + baseOffs;
		baseOffs++;
		for (uint16 j = 0; j < 9142; j++) {
			*dst++ = *src++;
			dst += 6;
			src += 6;
		}
		unlockMainSurface();
		//		_vm->_system->updateScreen();
		this->_hfCappedUpdate();
	}
}
void GraphicsManager::setFadeSpeed(uint8 speed) {
	_fadeSpeedStep = speed;
}

void GraphicsManager::dealFadeIn() {
	fadePalette(mainGamePalette, tmpPalette, 0, 256);
	uploadPalette(tmpPalette);
	this->_vm->_system->copyRectToScreen(this->_videoBackbuffer, _COMET_XRESOLUTION, 0, 0, _COMET_XRESOLUTION, _COMET_YRESOLUTION);
	//	this->_vm->_system->updateScreen();
	this->_hfCappedUpdate();
	for (uint16 i = 0; i < 256; i += _fadeSpeedStep * 2) {
		fadePalette(mainGamePalette, tmpPalette, i, 256);
		uploadPalette(tmpPalette);
		//		this->_vm->_system->updateScreen();
		//		this->_hfCappedUpdate();
	}
	fadePalette(mainGamePalette, tmpPalette, 255, 256);
	uploadPalette(tmpPalette);
	//	this->_vm->_system->updateScreen();
	//	this->_hfCappedUpdate();
	_fadeStatus = 0;
	_justFadedOut = false;
}
void GraphicsManager::dealFadeOut() {
	for (int16 i = 255; i > 0; i -= _fadeSpeedStep * 2) {
		fadePalette(mainGamePalette, tmpPalette, i, 256);
		uploadPalette(tmpPalette);
		//		this->_vm->_system->updateScreen();
		//		this->_hfCappedUpdate();
	}
	fadePalette(mainGamePalette, tmpPalette, 0, 256);
	uploadPalette(tmpPalette);
	//	this->_vm->_system->updateScreen();
	//	this->_hfCappedUpdate();
	_fadeStatus = 0;
	_justFadedOut = true;
}
void GraphicsManager::setPaletteTint(uint16 tint) {
	_vm->_gameState.paletteRedTintFactor = tint;
	tintPalette(mainGamePalette, tmpPalette, tint);
	uploadPalette(tmpPalette);
}
void GraphicsManager::tintPalette(const uint8 *palette, uint8 *destPalette, uint16 factor) {
	for (uint16 i = 0; i < 256; i++) {
		uint8 r = palette[0];
		uint8 g = palette[1];
		uint8 b = palette[2];
		uint16 newRed = (r + g + b + 100) / 4;
		newRed = (newRed - r) * factor;
		newRed = (newRed >> 4) + r;

		destPalette[0] = newRed;
		destPalette[1] = g * (16 - factor) / 16;
		destPalette[2] = b * (16 - factor) / 16;
		palette += 3;
		destPalette += 3;
	}
}
void GraphicsManager::capFPS_CD() {
	uint8 check = 35 / (_vm->_gameState.targetFPS + 8);
	uint32 vcount = 0;
	char str[256];
	//	snprintf(str, 256, "REQ %02x -> CHK %02X\n", _vm->_gameState.targetFPS, check);
	//	debug(str);
	while (true) {
		{
			Common::StackLock l(_mutex);
			vcount = _verticalRetraceCount;
		}
		//		snprintf(str, 256, "vc %02x\n", vcount);
		//		debug(str);
		if (check <= vcount) {
			break;
		}
		//		_vm->_system->delayMillis(10);
		waitVRetrace();
	}
	{
		Common::StackLock l(_mutex);
		_verticalRetraceCount = 0;
	}
}
void GraphicsManager::capFPS_CD_timerProc() {
	Common::StackLock lock(_mutex);
	_verticalRetraceCount++;
}

void GraphicsManager::capFPS_floppy() {

	uint32 FPS = 0;
	{
		Common::StackLock lock(_mutex);
		FPS = _FPS;
	}
	if (abs((long)(FPS - _vm->_gameState.targetFPS)) > 1) {
		if (FPS != _prevFPS) {
			_prevFPS = FPS;
			if (FPS < _vm->_gameState.targetFPS) {
				if (_framesToWait > 1) { // limiting it to 1 instead of 0 to regulate speed peaks
					_framesToWait--;
				}
			} else {
				if (FPS > _vm->_gameState.targetFPS) {
					_framesToWait++;
				}
			}
		}
	}

	for (uint16 i = 0; i < _framesToWait; i++) {
		waitVRetrace();
	}
}
void GraphicsManager::clearScreen() {
	drawRectangleFilled(0, 0, _COMET_XRESOLUTION - 1, _COMET_YRESOLUTION - 1, 0);
}
void GraphicsManager::drawFrameToScreen() {
	if (randomDrawActive != 0) {
		numRandLoops--;
		if (numRandLoops > 0) {
			drawRandomLinesOrDots();
		} else {
			randomDrawActive = 0;
		}
	}
	if (numLightnings != 0) {
		drawLightnings();
	}
	if (randomStarsActive != 0) {
		handleRandomStars();
	}
	if (_clearScreen) {
		_clearScreen = false;
		clearScreen();
	}
	if (_vm->isCD()) {
		if (_vm->_gameState.currPakNum == 9 && _vm->_gameState.currRoomNum == 0 && _vm->_gameState.isAlternatePaletteActive == 0) {
			memcpy(normalPalette, mainGamePalette, 768);
			memcpy(mainGamePalette, cd2Palette, 768);
			memcpy(tmpPalette, cd2Palette, 768);
			clearBackbuffer();
			paintBackbuffer_mouse();
			uploadPalette(mainGamePalette);
			_vm->_gameState.isAlternatePaletteActive = 3;
		}
		if (_vm->_gameState.currPakNum == 9 && _vm->_gameState.currRoomNum == 1 && _vm->_gameState.isAlternatePaletteActive == 3) {
			memcpy(mainGamePalette, cdintroPalette, 768);
			memcpy(tmpPalette, cdintroPalette, 768);
			clearBackbuffer();
			paintBackbuffer_mouse();
			uploadPalette(mainGamePalette);
			_vm->_gameState.isAlternatePaletteActive = 2;
		}
		if (_vm->_gameState.currPakNum == 5 && _vm->_gameState.currRoomNum == 0 && (_vm->_gameState.isAlternatePaletteActive == 3 || _vm->_gameState.isAlternatePaletteActive == 2)) {
			memcpy(mainGamePalette, normalPalette, 768);
			memcpy(tmpPalette, normalPalette, 768);
			clearBackbuffer();
			paintBackbuffer_mouse();
			uploadPalette(mainGamePalette);
			_vm->_gameState.isAlternatePaletteActive = 0;
		}
		if (_vm->_gameState.currPakNum == 0 && _vm->_gameState.currRoomNum == 0 && _vm->_gameState.isAlternatePaletteActive != 0) {
			memcpy(mainGamePalette, normalPalette, 768);
			memcpy(tmpPalette, normalPalette, 768);
			uploadPalette(mainGamePalette);
			_vm->_gameState.isAlternatePaletteActive = 0;
		}
	}
	if (_isPaintWithEffect && _fadeStatus == 0 && _vm->_gmMgr->_roomZoomLevel == 0) {
		paintBackbuffer_withEffect();
		//		warning("Effect");
	}
	if (_vm->isCD()) {
		callCapFPS();
	}
	if (_fadeStatus == 1) {
		dealFadeIn();
		return;
	}
	if (_fadeStatus == 2) {
		dealFadeOut();
		return;
	}
	if (_vm->_gmMgr->_roomZoomLevel == 0) {
		paintBackbuffer();
	} else if (_vm->_gmMgr->_roomZoomLevel == 1) {
		drawRoomZoomed(_vm->_gameState.roomZoomX, _vm->_gameState.roomZoomY);
	} else if (_vm->_gmMgr->_roomZoomLevel == 2) {
		drawRoomZoomed_x3(_vm->_gameState.roomZoomX, _vm->_gameState.roomZoomY);

	} else if (_vm->_gmMgr->_roomZoomLevel == 3) {
		drawRoomZoomed_x4(_vm->_gameState.roomZoomX, _vm->_gameState.roomZoomY);
	}
}
void GraphicsManager::drawRoomZoomed(int16 centerx, int16 centery) {
	int16 x = centerx - 80;
	if (x < 0) {
		x = 0;
	}
	if (x > 159) {
		x = 159;
	}
	int16 y = centery - 80;
	if (y < 0) {
		y = 0;
	}
	if (y > 99) {
		y = 99;
	}
	uint32 srcOffs = _rowsOffsets[y] + x;
	uint32 dstOffs = 0;
	uint8 *dstPtr = lockMainSurface();
	uint8 *srcPtr = _videoBackbuffer;
	for (uint8 j = 0; j < 100; j++) {
		for (uint8_t i = 0; i < 160; i++) {
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs++];
		}
		srcOffs -= 160;
		for (uint8_t i = 0; i < 160; i++) {
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs++];
		}
		srcOffs += 160;
	}
	unlockMainSurface();
	//	g_system->updateScreen();
	this->_hfCappedUpdate();
}
void GraphicsManager::drawRoomZoomed_x3(int16 centerx, int16 centery) {
	int16 x = centerx - 53;
	if (x < 0) {
		x = 0;
	}
	if (x > 213) {
		x = 213;
	}
	int16 y = centery - 50;
	if (y < 0) {
		y = 0;
	}
	if (y > 133) {
		y = 133;
	}
	uint32 srcOffs = _rowsOffsets[y] + x;
	uint32 dstOffs = 0;
	uint8 *dstPtr = lockMainSurface();
	uint8 *srcPtr = _videoBackbuffer;
	for (uint8 j = 0; j < 66; j++) {
		for (uint8_t i = 0; i < 106; i++) {
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs++];
		}
		srcOffs -= 106;
		for (uint8_t i = 0; i < 160; i++) {
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs++];
		}
		srcOffs -= 106;
		for (uint8_t i = 0; i < 160; i++) {
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs++];
		}
		srcOffs += 214;
	}
	for (uint16 i = 0; i < 640; i++) {
		dstPtr[dstOffs++] = 0;
	}
	unlockMainSurface();
	//	g_system->updateScreen();
	this->_hfCappedUpdate();
}
void GraphicsManager::drawRoomZoomed_x4(int16 centerx, int16 centery) {
	int16 x = centerx - 40;
	if (x < 0) {
		x = 0;
	}
	if (x > 239) {
		x = 239;
	}
	int16 y = centery - 44;
	if (y < 0) {
		y = 0;
	}
	if (y > 149) {
		y = 149;
	}
	uint32 srcOffs = _rowsOffsets[y] + x;
	uint32 dstOffs = 0;
	uint8 *dstPtr = lockMainSurface();
	uint8 *srcPtr = _videoBackbuffer;
	for (uint8 j = 0; j < 50; j++) {
		for (uint8_t i = 0; i < 80; i++) {
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs++];
			dstPtr += 316;
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs++];
			dstPtr += 316;
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs++];
			dstPtr += 316;
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs];
			dstPtr[dstOffs++] = srcPtr[srcOffs++];
			dstPtr -= 960;
		}
		dstPtr += 960;
		srcPtr += 240;
	}
	unlockMainSurface();
	//	g_system->updateScreen();
	this->_hfCappedUpdate();
}

void GraphicsManager::loadStage(bool isLoading) {
	//TODO complete implementation
	clearBackbuffer();
	loadStageBackground();
	loadStageAnims();
	copyVideoBuffer(_backgroundBuffer, _videoBackbuffer);
	prepareStageElements();
	copyVideoBuffer(_videoBackbuffer, _backgroundBuffer);
	if (stageColorReplace != 0) {
		replaceStageColors();
	}
	if (!isLoading) {
		loadStageElements();
	}
}
void GraphicsManager::loadStageAnims() {
	if (_stageAnims != nullptr) {
		delete[] _stageAnims;
	}
	_stageAnims = _vm->_archMgr->allocateAndGetFile(_vm->_gmMgr->getStagesFilename(), _vm->_gameState.stageIndex + 1);
}
void GraphicsManager::prepareStageElements() {
	uint8 *data = getGraphicsData(0, 0, _stageAnims);
	if (*(data + 1) != 0) {
		decodeFrame(0, 0, 0, _stageAnims);
	}
}
void GraphicsManager::loadStageElements() {
	uint8 *data = getGraphicsData(0, 0, _stageAnims);
	uint16 nElements = *(data + 1);
	uint16 num = nElements;
	StageElement *stg = &_vm->_gameState.stageElements[0];
	data += 2;
	for (uint16 i = 0; i < num; data += 2, i++) {
		uint8 type = *data;
		data += 2;
		uint16 elemIdx = READ_LE_UINT16(data);
		data += 2;
		uint16 elemPivotX = READ_LE_UINT16(data) / 2;
		data += 2;
		uint16 elemPivotY = READ_LE_UINT16(data);
		uint8 *elem = getGraphicsData(type, elemIdx, _stageAnims) - 2;
		uint8 elemHalfWidth = (*elem) / 2;
		elem++;
		if (elemHalfWidth == 0) {
			nElements--;
		} else {
			if (type == 0) {
				stg->halfLeft = elemPivotX - elemHalfWidth;
			} else {
				stg->halfLeft = elemPivotX;
			}
			uint8 elemHeight = (*elem);
			elemHeight = (elemHeight >> 4) & 0x03;
			elemHeight = (elemHeight + 1) << 2;
			stg->top = elemPivotY - elemHeight;
			stg->halfRight = elemPivotX + elemHalfWidth;
			stg->bottom = elemPivotY;
			stg++;
		}
	}
	_vm->_gameState.numStageElements = nElements;
}
void GraphicsManager::actorSaySentenceWithAnimation(uint8 actorIdx, uint16 sentenceIdx, uint8 animation) {
	Actor *actor = _vm->_gmMgr->getActor(actorIdx);
	_vm->_txtMgr->actorSaySentence(actorIdx, sentenceIdx, actor->speechColor);
	if (animation != 0xff) {
		_vm->_txtMgr->setPrevAnimationOrActor(actor->animationIdx);
		_vm->_txtMgr->setPrevFrameToDraw(actor->frameToDraw);
		_vm->_txtMgr->setPrevFixedFrame(actor->fixedAnimationFrame);
		_vm->_gmMgr->actorPrepareAnimation(actor, animation);
		actor->animationType = 2;
	} else {
		_vm->_txtMgr->setPrevAnimationOrActor(-1);
	}
	_vm->_scMgr->setWait(ScriptsManager::ScriptFlags::WAIT_TEXT);
}
void GraphicsManager::drawSpeechBox(int16 left, int16 top, int16 right, int16 bottom) {
	int16 xAddedPixels = (((right - left + 8) / 8) * 8) - (right - left);
	int16 yAddedPixels = (((bottom - top + 8) / 8) * 8) - (bottom - top);
	right += xAddedPixels;
	bottom += yAddedPixels;
	if (left < 0) {
		right -= left;
		left = 0;
	}
	if (right > 319) {
		left -= (right - 319);
		right = 319;
	}
	if (top < 0) {
		yAddedPixels = bottom - top;
		top = 0;
		bottom = top + yAddedPixels;
	}
	if (bottom > 199) {
		yAddedPixels = bottom - top;
		bottom = 199;
		top = bottom - yAddedPixels;
	}
	for (int16 drawY = top + 8; drawY <= bottom - 8; drawY += 8) {
		for (int16 drawX = left + 8; drawX < right - 8; drawX += 8) {
			decodeFrame(8, drawX, drawY, _speechBoxGraphics);
		}
	}
	for (int16 drawX = left + 8; drawX < right - 8; drawX += 8) {
		decodeFrame(3, drawX, top + 8, _speechBoxGraphics);
		decodeFrame(4, drawX, bottom, _speechBoxGraphics);
	}
	for (int16 drawY = top + 16; drawY < bottom; drawY += 8) {
		decodeFrame(1, left, drawY, _speechBoxGraphics);
		decodeFrame(6, right - 16, drawY, _speechBoxGraphics);
	}
	decodeFrame(0, left, top + 8, _speechBoxGraphics);
	decodeFrame(5, right - 16, top + 8, _speechBoxGraphics);
	decodeFrame(2, left, bottom, _speechBoxGraphics);
	decodeFrame(7, right - 16, bottom, _speechBoxGraphics);
}
void GraphicsManager::drawDiskMenu(uint8 selectedItem) {
	uint16 x = 0x89;
	uint16 yOffs = 0x41;
	uint16 height = 0x17;
	decodeFrame(10, 0, 0, _iconsGraphics);
	decodeFrame(11, x, yOffs + (selectedItem * height), _iconsGraphics);
}

//for encapsulation
void GraphicsManager::setFadeStatus(uint8 status) {
	_fadeStatus = status;
}
uint8 GraphicsManager::getFadeStatus() {
	return _fadeStatus;
}

void GraphicsManager::setJustFadedOut(bool val) {
	_justFadedOut = val;
}
bool GraphicsManager::getJustFadedOut() {
	return _justFadedOut;
}
void GraphicsManager::setClearScreen() {
	_clearScreen = true;
}

uint8 *GraphicsManager::getBackbuffer() {
	return _videoBackbuffer;
}
void GraphicsManager::setBackbuffer(uint8 *bb) {
	_videoBackbuffer = bb;
}
uint8 *GraphicsManager::getBackground() {
	return _backgroundBuffer;
}
/*
	uint8 GraphicsManager::getNumStageElements() {
		return _vm->_gameState.numStageElements;
	}
	*/
/*
	StageElement *GraphicsManager::getStageElement(uint8 idx) {
		return &_vm->_gameState.stageElements[idx];
	}
	*/
void GraphicsManager::addStageElement(uint8 x1, uint8 y1, uint8 x2, uint8 y2) {
	StageElement *el = &_vm->_gameState.stageElements[_vm->_gameState.numStageElements];
	el->halfLeft = x1;
	el->top = y1;
	el->halfRight = x2;
	el->bottom = y2;
	_vm->_gameState.numStageElements++;
}
void GraphicsManager::drawIconFrame(uint8 selected, uint8 idx) {
	uint16 x = 196;
	uint16 y = 14;
	decodeFrame(0, 0, 0, _iconsGraphics);
	decodeFrame(selected + 1, 0, 0, _iconsGraphics);
	if (_vm->_gameState.objectsFlags[_vm->_gameState.activeObject] < 1) {
		_vm->_gameState.activeObject = -1;
	}
	if (_vm->_gameState.activeObject != -1) {
		drawSingleAnimFrame(_vm->_gameState.activeObject, x, y, idx, _objectsGraphics);
	}
}
void GraphicsManager::drawSingleAnimFrame(uint8 animIdx, int16 x, int16 y, uint16 frameIdx, uint8 *data) {
	uint8 *anim = getGraphicsData(2, animIdx, data);
	uint8 numFrames = *(anim + 1);
	anim += 2;
	int16 tmpX = x;
	int16 tmpY = y;
	uint16 idxAndFlags = 0;
	if (numFrames > 1) {
		uint16 tmpIdx = frameIdx;
		while (tmpIdx > numFrames - 1) {
			tmpIdx -= numFrames;
		}
		idxAndFlags = READ_LE_UINT16(anim + (tmpIdx * 8));
		uint8 *tmp = anim + 4;
		for (uint8 i = 0; i <= tmpIdx; i++) {
			tmpX += READ_LE_INT16(tmp);
			tmp += 2;
			tmpY += READ_LE_INT16(tmp);
			tmp += 6;
		}
	} else {
		idxAndFlags = READ_LE_UINT16(anim);
	}
	decodeFrame(idxAndFlags, tmpX, tmpY, data);
}
void GraphicsManager::drawObjectsWindow(uint8 objectsCount, uint16 *objBuffer, uint8 firstVisibleIdx, uint8 selectedObjIdx, uint16 frameIdx) {
	uint16 left = 0x4a;
	uint16 top = 0x40;
	uint8 itemHeight = 12;
	uint8 numItemsPerPage = 10;
	uint16 x = 0;
	uint16 y = 0;
	uint16 objIdx = 0;
	decodeFrame(16, 0, 0, _iconsGraphics);
	if (firstVisibleIdx > 0) {
		decodeFrame(0x34, 0, 0, _iconsGraphics);
	}
	if (firstVisibleIdx + numItemsPerPage < objectsCount) {
		decodeFrame(0x35, 0, 0, _iconsGraphics);
	}
	for (uint16 i = 0; i < numItemsPerPage && i + firstVisibleIdx < objectsCount; i++) {
		const char *objectName = _vm->_txtMgr->getSentencePtr(objBuffer[((i + firstVisibleIdx) * 2) + 1], _vm->_txtMgr->getObjNames());
		x = left + 0x15;
		y = top + (i * itemHeight) + 1;
		_vm->_gmMgr->updateFontDataAndColor(0x78);
		_vm->_txtMgr->drawString(x + 1, y + 1, _videoBackbuffer, objectName);
		_vm->_gmMgr->updateFontDataAndColor(0x77);
		_vm->_txtMgr->drawString(x, y, _videoBackbuffer, objectName);
		x = left;
		y = top + (i * itemHeight) - 3;
		objIdx = objBuffer[((i + firstVisibleIdx) * 2) + 1];
		if (objIdx == 73) {
			uint16 breakki = 1;
		}
		drawSingleAnimFrame(objIdx, x, y, frameIdx, _objectsGraphics);
	}
	if (objectsCount != 0) {
		x = left + 16;
		y = top + (itemHeight * (selectedObjIdx - firstVisibleIdx)) - 1;
		drawRectangleOutline(x, y, 253, y + itemHeight - 1, _activeObjectBorderColor);
		_activeObjectBorderColor++;
		if (_activeObjectBorderColor >= 0x60) {
			_activeObjectBorderColor = 0x50;
		}
	}
}

void GraphicsManager::drawSaveMenu(uint8 selectedItem, bool isLoad, char descs[10][30]) {
	uint16 xOffs = 95;
	uint16 yOffs = 64;
	uint16 height = 12;
	if (!isLoad) {
		decodeFrame(14, 0, 0, _iconsGraphics);
	} else {
		decodeFrame(15, 0, 0, _iconsGraphics);
	}
	for (uint8 i = 0; i < 10; i++) {
		_vm->_gmMgr->updateFontDataAndColor(0x78);
		uint16 x1 = (135 - _vm->_txtMgr->calcStringWidth(descs[i])) / 2;
		_vm->_txtMgr->drawString(x1 + xOffs + 1, yOffs + 1 + (height * i), _videoBackbuffer, descs[i]);
		_vm->_gmMgr->updateFontDataAndColor(0x77);
		_vm->_txtMgr->drawString(x1 + xOffs, yOffs + (height * i), _videoBackbuffer, descs[i]);
	}
	drawRectangleOutline(xOffs - 2, (height * selectedItem) + yOffs - 2, xOffs + 138, (height * selectedItem) + yOffs + 9, 0x77);
}
void GraphicsManager::drawSettingsMenu(uint16 currSelection, uint16 volume, uint16 digiVolume, uint16 textSpeed, uint16 gameSpeed, uint16 diskNum, uint16 langId, uint16 framesCounter) {
	uint16 baseX = 107;
	uint16 baseY = 65;
	uint16 rowHeight = 20;
	uint16 scroll_baseX = 128;
	uint16 scroll_baseY = 70;
	char diskName[2] = "A";
	decodeFrame(0x19, 0, 0, _iconsGraphics);
	decodeFrame(0x1c, baseX, baseY + (currSelection * rowHeight), _iconsGraphics);
	decodeFrame(0x1b, scroll_baseX + (volume * 4), scroll_baseY, _iconsGraphics);
	decodeFrame(0x1b, scroll_baseX + (digiVolume * 4), scroll_baseY + rowHeight, _iconsGraphics);
	decodeFrame(0x1b, scroll_baseX + (textSpeed * 30), scroll_baseY + (rowHeight * 2), _iconsGraphics);
	decodeFrame(0x1b, scroll_baseX + (gameSpeed * 4), scroll_baseY + (rowHeight * 3), _iconsGraphics);
	diskName[0] = 0x41 + diskNum;
	_vm->_gmMgr->updateFontDataAndColor(0x78);
	_vm->_txtMgr->drawString(133, 149, _videoBackbuffer, diskName);
	_vm->_gmMgr->updateFontDataAndColor(0x77);
	_vm->_txtMgr->drawString(132, 148, _videoBackbuffer, diskName);
	decodeFrame(0x20 + langId, 129, 177, _iconsGraphics);
	if (volume == 0) {
		decodeFrame(0x37, 0, 0, _iconsGraphics);
	}
	if (volume > 0 && volume < 8) {
		if (framesCounter < 0x10) {
			decodeFrame(0x47, 0, 0, _iconsGraphics);
		} else {
			decodeFrame(0x48, 0, 0, _iconsGraphics);
		}
	}
	if (volume >= 8) {
		if (framesCounter < 0x10) {
			decodeFrame(0x49, 0, 0, _iconsGraphics);
		} else {
			decodeFrame(0x4a, 0, 0, _iconsGraphics);
		}
	}
	if (digiVolume == 0) {
		drawSingleAnimFrame(1, 0, 0, framesCounter, _iconsGraphics);
	} else if (digiVolume < 7) {
		drawSingleAnimFrame(2, 0, 0, framesCounter, _iconsGraphics);
	} else {
		drawSingleAnimFrame(3, 0, 0, framesCounter, _iconsGraphics);
	}
	if (textSpeed == 0 and framesCounter > 6) {
		decodeFrame(0x46, 0, 0, _iconsGraphics);
	}
	if (textSpeed == 1 and framesCounter > 16) {
		decodeFrame(0x46, 0, 0, _iconsGraphics);
	}
	if (textSpeed == 2 and framesCounter > 24) {
		decodeFrame(0x46, 0, 0, _iconsGraphics);
	}
	if (gameSpeed < 5) {
		decodeFrame(0x4b, 0, 0, _iconsGraphics);
	}
	if (gameSpeed >= 5 && gameSpeed < 10) {
		decodeFrame(0x4c, 0, 0, _iconsGraphics);
	}
	if (gameSpeed >= 10) {
		decodeFrame(0x4d, 0, 0, _iconsGraphics);
	}
}
void GraphicsManager::drawDiaryPagesAnimation(bool forward) {
	_vm->_audioMgr->stopDigi();
	_vm->_audioMgr->playDigiSubfile(10, 1);
	if (forward) {
		for (uint8 i = 0x26; i < 0x31; i++) {
			decodeFrame(30, 0, 0, _iconsGraphics);
			decodeFrame(i, 0, 0, _iconsGraphics);
			paintBackbuffer_mouse();
			// here the game uses a small delay loop, which is quite useless on modern machines
			for (uint8 j = 0; j < 50; j++) {
			}
		}
	} else {
		for (uint8 i = 0x31; i > 0x26; i--) {
			decodeFrame(30, 0, 0, _iconsGraphics);
			decodeFrame(i, 0, 0, _iconsGraphics);
			paintBackbuffer_mouse();
			// here the game uses a small delay loop, which is quite useless on modern machines
			for (uint8 j = 0; j < 50; j++) {
			}
		}
	}
}

void GraphicsManager::drawDiarySentences(int16 currPage, int16 maxPage, int16 fontCol) {
	char stringBuffer[2000];
	char *stringPtr;
	uint16 linesDrawn = 0;
	int16 x, x_offs = 58, y_offs = 48;
	char pageFooter[10];

	memset(pageFooter, 0, 10);
	_vm->_txtMgr->getSentence(2, currPage, stringBuffer);
	stringPtr = stringBuffer + 1;
	decodeFrame(30, 0, 0, _iconsGraphics);
	if (currPage < maxPage) {
		decodeFrame(37, 0, 0, _iconsGraphics);
	}
	_vm->_gmMgr->updateFontDataAndColor(0x3a);
	Common::strlcat(pageFooter, "- ", 10);
	Common::strlcat(pageFooter, _vm->_txtMgr->printDecimalNumber((currPage * 2) + 1), 8);
	Common::strlcat(pageFooter, " -", 6);
	x = ((106 - _vm->_txtMgr->calcStringWidth(pageFooter)) / 2) + x_offs;
	_vm->_txtMgr->drawString(x, 180, _videoBackbuffer, pageFooter);
	pageFooter[0] = 0;
	Common::strlcat(pageFooter, "- ", 10);
	Common::strlcat(pageFooter, _vm->_txtMgr->printDecimalNumber((currPage * 2) + 2), 8);
	Common::strlcat(pageFooter, " -", 6);
	x = ((106 - _vm->_txtMgr->calcStringWidth(pageFooter)) / 2) + x_offs;
	_vm->_txtMgr->drawString(x + 115, 180, _videoBackbuffer, pageFooter);
	_vm->_gmMgr->updateFontDataAndColor(fontCol);
	while (*stringPtr != '*') {
		x = ((106 - _vm->_txtMgr->calcStringWidth(stringPtr)) / 2) + x_offs;
		if (x < 0) {
			x = 0;
		}
		_vm->_txtMgr->drawString(x, (linesDrawn * 10) + y_offs, _videoBackbuffer, stringPtr);
		linesDrawn++;
		if (linesDrawn == 13) {
			x_offs += 115;
			y_offs -= 130;
		}
		while (*stringPtr != 0 && *stringPtr != '*') {
			stringPtr++;
		}
		stringPtr++;
	}
}

void GraphicsManager::diaryFade(bool isFadeOut, int16 currS, int16 maxS) {
	int16 fontColor;
	if (isFadeOut) {
		for (fontColor = 0x40; fontColor < 0x48; fontColor += 2) {
			drawDiarySentences(currS, maxS, fontColor);
			_vm->_gMgr->paintBackbuffer_mouse();
		}
	} else {
		for (fontColor = 0x48; fontColor > 0x48; fontColor -= 2) {
			drawDiarySentences(currS, maxS, fontColor);
			_vm->_gMgr->paintBackbuffer_mouse();
		}
	}
}
/*
	uint16 GraphicsManager::getIsAlternatePaletteActive() {
		return _isAlternatePaletteActive;
	}
	void GraphicsManager::setIsAlternatePaletteActive(uint16 val) {
		_isAlternatePaletteActive = val;
	}
	*/
uint8 *GraphicsManager::getTmpBuffer() {
	return _tmpBuffer;
}

void GraphicsManager::generateRandCoord(uint16 *ptr) {
	*ptr++ = randCoordsBaseX + _vm->_gmMgr->getRandom(11) - 5;
	*ptr++ = randCoordsBaseY + _vm->_gmMgr->getRandom(6) - 3;
	*ptr++ = _vm->_gmMgr->getRandom(3) - 1;
	*ptr++ = -(_vm->_gmMgr->getRandom(7) + 1);
}
void GraphicsManager::generateRandCoords(uint16 baseX, uint16 baseY, uint16 numCoords, uint16 numLoops) {
	randCoordsBaseX = baseX;
	randCoordsBaseY = baseY;
	numRandCoords = numCoords;
	uint16 *ptr = randCoords;
	for (uint16 i = 0; i < numCoords; i++) {
		generateRandCoord(ptr);
		ptr += 4;
	}
	numRandLoops = numLoops;
}
void GraphicsManager::drawRandomStars(uint16 left, uint16 top, uint16 right, uint16 bottom) {
	if (left >= right) {
		return;
	}
	if (top >= bottom) {
		return;
	}
	uint16 xrange = right - left;
	uint16 yrange = bottom - top;
	uint16 numStars = (xrange >> 2) + yrange;
	_vm->_gMgr->setPixelColor(1);
	for (uint16 i = 0; i < numStars; i++) {
		uint16 x = left + _vm->_gmMgr->getRandom(xrange);
		uint16 y = top + _vm->_gmMgr->getRandom(yrange);
		_vm->_gMgr->putPixel(x, y);
		if (_vm->_gmMgr->getRandom(2) != 0) {
			_vm->_gMgr->putPixel(x, y - 1);
			_vm->_gMgr->putPixel(x, y + 1);
			_vm->_gMgr->putPixel(x - 1, y);
			_vm->_gMgr->putPixel(x + 1, y);
		}
	}
}
void GraphicsManager::handleRandomStars() {
	randomStarsActive = 0;
	drawRandomStars(randomStarsLeft, randomStarsTop, randomStarsRight, randomStarsBottom);
}
void GraphicsManager::drawRandomLinesOrDots() {
	uint16 *ptr = randCoords;
	setPixelColor(0x37);
	for (uint8 i = 0; i < numRandCoords; i++) {
		uint16 x = ptr[0];
		uint16 y = ptr[1];
		ptr[0] += ptr[3];
		ptr[1] += ptr[2];
		ptr[2]++;
		if ((140 + _vm->_gmMgr->getRandom(10)) < ptr[1]) {
			uint8 *tmp = _videoBackbuffer;
			_videoBackbuffer = _backgroundBuffer;
			putPixel(ptr[0], ptr[1]);
			_videoBackbuffer = tmp;
			generateRandCoord(ptr);
			x = randCoordsBaseX;
			y = randCoordsBaseY;
			if (_vm->_gmMgr->getRandom(2) == 0) {
				drawLine(ptr[0], ptr[1], x, y, 0x37);
			} else {
				putPixel(ptr[0], ptr[1]);
			}
		}
		ptr += 4;
	}
}
uint16 GraphicsManager::pseudoRand() {
	uint8 c_in = 0, c_out = 0;
	for (uint8 i = 0; i < 16; i++) {
		uint16 tmp = (pseudoRandSeed << 1) ^ pseudoRandSeed;
		tmp <<= 1;
		c_out = tmp >> 15;
		tmp <<= 1;
		c_in = c_out;
		c_out = pseudoRandVal >> 15;
		pseudoRandVal = (pseudoRandVal << 1) | c_in;
		c_in = c_out;
		c_out = pseudoRandSeed >> 15;
		pseudoRandSeed = (pseudoRandSeed << 1) | c_in;
		c_in = c_out;
	}
	return pseudoRandVal;
}
void GraphicsManager::drawSingleLightning(uint16 startX, uint16 startY, uint16 maxX, uint16 maxY) {
	uint16 x1 = startX;
	uint16 y1 = startY;
	for (uint8 i = 1; i <= 10; i++) {
		uint16 rand1 = (pseudoRand() % 10) + 1;
		uint16 color = 0x90 + (pseudoRand() % 7);
		int16 tmp = pseudoRand() & 0x03;
		int16 tmp2 = (maxX - startX) * i;
		tmp2 = (tmp2 / 10) + startX - 2;
		int16 x2 = tmp + tmp2;
		tmp = pseudoRand() & 0x03;
		tmp2 = (maxY - startY) * i;
		tmp2 = (tmp2 / 10) + startY - 2;
		uint16 y2 = tmp + tmp2;
		drawLine(x1, y1, x2, y2, color);
		x1 = x2;
		y1 = y2;
	}
}
void GraphicsManager::drawLightnings() {
	uint16 *coordsPtr = randCoords;
	for (uint16 i = 0; i < numLightnings; i++) {
		drawSingleLightning(coordsPtr[0], coordsPtr[1], coordsPtr[3], coordsPtr[2]);
		coordsPtr += 4;
	}
	numLightnings = 0;
	pingpong += pingpongStep;
	if (pingpong > 0x77) {
		pingpongStep = -1;
	}
	if (pingpong < 0x70) {
		pingpongStep = 1;
	}
}

void GraphicsManager::replaceStageColors() {
	//this implementation using goto is horrible, but is the quickest translation from ASM
	//TODO make it better
	uint8 *ptr = _backgroundBuffer + (320 + 172) - 1;
	uint16 outer = 86;
	uint16 val = 0xff;
	int16 inner = 0;
	char v = 0;
	do {
		for (inner = 320 * 2; inner > 0; inner--) {
			v = *ptr--;
			if (v == 0x0f)
				goto found;
		}
		outer--;

	} while (outer != 0);
	return;
	do {
		for (inner = 320 * 2; inner > 0; inner--) {
			v = *ptr--;
			if (v == 0x0f) {
			found:
				*(ptr + 1) = val;
			}
		}
		if (val != 0xe0) {
			val--;
		}
		outer--;

	} while (outer != 0);
}
void GraphicsManager::fadePaletteBlock(uint16 blockIdx, uint16 fadeLevel) {
	uint16 paletteStartIdx = blockIdx * 16;
	uint16 paletteOffset = paletteStartIdx * 3;
	fadePalette(mainGamePalette + paletteOffset, tmpPalette + paletteOffset, fadeLevel, 16);
	waitVRetrace();
	setPaletteEntries(tmpPalette + paletteOffset, paletteStartIdx, 16);
}

} // namespace Cometengine
