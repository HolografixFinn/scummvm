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

#include "common/scummsys.h"

#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/debug.h"
#include "common/error.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/cursorman.h"
#include "graphics/font.h"
#include "graphics/fontman.h"
#include "graphics/palette.h"
#include "graphics/screen.h"
#include "graphics/surface.h"

#include "comet/comet.h"

namespace Cometengine {

CometEngine::CometEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst), _gameDescription(gameDesc),
																			 _archMgr(nullptr), _gmMgr(nullptr), _gMgr(nullptr), _scMgr(nullptr), _txtMgr(nullptr), _audioMgr(nullptr), _spMgr(nullptr), _moMgr(nullptr),
																			 _isEscPressed(false), _isScummVMQuit(false), _isGameQuit(false),
																			 _isCDVersion(false), _isComet(false), _isMuseum(false), _isDemo(false), _gameState(), _console(nullptr) {
	_image = nullptr;
	_console = nullptr;
	_timerInstalled = false;
	_showScoreFl = false;
	_setDurationFl = false;
	_leftButtonDownFl = false;
	_endGameFl = false;
	_curSceneIdx = -1;
	_prvSceneIdx = -1;
	_curBitmapIdx = -1;
	_curChoice = 0;
	_totScene = -1;
	_totScore = 0;

	DebugMan.addDebugChannel(kDebugGeneral, "general", "General debug level");
	if (_gameDescription->flags & ADGF_CD) {
		_isCDVersion = true;
	}
	if (_isCDVersion) {
		_gameState.speechOptions = 1;
	}
	if (scumm_strnicmp(_gameDescription->gameId, "comet", 5) == 0) {
		_isComet = true;
		if (_isCDVersion) {
			Common::FSNode gamePath(ConfMan.get("path"));
			Common::FSNode shadowPath = gamePath.getChild("SHADOW");
			SearchMan.addDirectory("SHADOW", shadowPath, 0, 1);
		}
		if (scumm_strnicmp(_gameDescription->extra, "Demo", 4) == 0) {
			_isDemo = true;
		}
	} else if (scumm_strnicmp(_gameDescription->gameId, "museum", 6) == 0) {
		_isMuseum = true;
		if (_isCDVersion) {
			Common::FSNode gamePath(ConfMan.get("path"));
			Common::FSNode museePath = gamePath.getChild("MUSEE");
			SearchMan.addDirectory("MUSEE", museePath, 0, 1);
		}
	}
	switch (this->_gameDescription->language) {
	case Common::Language::FR_FRA:
		_gameState.selectedLanguageID = 0;
		break;
	case Common::Language::EN_ANY:
		_gameState.selectedLanguageID = 1;
		break;
	case Common::Language::ES_ESP:
		_gameState.selectedLanguageID = 2;
		break;
	case Common::Language::DE_DEU:
		_gameState.selectedLanguageID = 3;
		break;
	case Common::Language::IT_ITA:
		_gameState.selectedLanguageID = 4;
		break;
	}
	for (uint8 i = 0; i < 6; i++) {
		timedprocs[i] = nullptr;
	}
	_system->getTimerManager()->installTimerProc(CometEngine::staticTimerProc60, 1000000 / 60, this, "GeneralTimer");
}
void CometEngine::staticTimerProc60(void *param) {
	CometEngine *ce = reinterpret_cast<CometEngine *>(param);
	ce->timerProc60();
}
void CometEngine::timerProc60() {
	for (uint8 i = 0; i < 6; i++) {
		if (timedprocs[i] != nullptr) {
			(*timedprocs[i])();
		}
	}
}
int8 CometEngine::addTimedProc(Common::Functor0<void> *func) {
	for (uint8 i = 0; i < 6; i++) {
		if (timedprocs[i] == nullptr) {
			timedprocs[i] = func;
			return i;
		}
	}
	return -1;
}
void CometEngine::removeTimedProc(int8 idx) {
	if (idx >= 0 && idx < 6) {
		delete timedprocs[idx];
		timedprocs[idx] = nullptr;
	}
}

CometEngine::~CometEngine() {
	_system->getTimerManager()->removeTimerProc(CometEngine::staticTimerProc60);
	for (uint8 i = 0; i < 6; i++) {
		if (timedprocs[i] == nullptr) {
			delete timedprocs[i];
			timedprocs[i] = nullptr;
		}
	}

	delete _image;
	//	delete _console;	// this is automatically done by the engine
	delete _archMgr;
	delete _audioMgr;
	delete _gmMgr;
	delete _gMgr;
	delete _scMgr;
	delete _txtMgr;
	delete _spMgr;
	delete _moMgr;
}

void CometEngine::initManagers() {
	_archMgr = new ArchivesManager(this);
	_gmMgr = new GameManager(this);
	_gmMgr->init();
	_audioMgr = new AudioManager(this);
	_audioMgr->init();
	_gMgr = new GraphicsManager(this);
	_gMgr->init();
	//	uint16 currentLang = 0;

	_txtMgr = new TextManager(this); //, currentLang);
	_scMgr = new ScriptsManager(this);
	_spMgr = new SpeechManager(this, isCD());
	_moMgr = new MouseManager(this);
	_gmMgr->loadResPak();
}
void CometEngine::initBasicResources() {
}

void CometEngine::setEscPressed() {
	_isEscPressed = true;
}
bool CometEngine::isQuitRequested() {
	return this->shouldQuit() || _isScummVMQuit || _isGameQuit;
}

Common::Error CometEngine::run() {
	_console = new Console();
	setDebugger(_console);
	if (_system->hasFeature(OSystem::Feature::kFeatureVSync)) {
		_system->setFeatureState(OSystem::Feature::kFeatureVSync, true);
	}
	initManagers();

	while (!_isEscPressed) {
		_gmMgr->updateInputStatus();
		if (isQuitRequested()) {
			break;
		}
		if (_gmMgr->_lastPressedKey == Common::KeyCode::KEYCODE_ESCAPE) {
			_isEscPressed = true;
		}

		if (_gmMgr->_lastPressedKey == Common::KeyCode::KEYCODE_RETURN || _gmMgr->_lastPressedKey == Common::KeyCode::KEYCODE_KP_ENTER) {
			_gmMgr->enterPressed();
		}
		if (_gmMgr->_lastPressedKey == Common::KeyCode::KEYCODE_p) {
			_gmMgr->handlePause();
		}
		if (isCD() && _gameState.currPakNum == 0 && _gameState.currRoomNum == 0) {
			_isEscPressed = true;
		}
		_gmMgr->gameFrame();
	}
	if (isQuitRequested()) {
		return Common::kNoError;
	}
	_gmMgr->waitForNoInput();
	if (isQuitRequested()) {
		return Common::kNoError;
	}
	
	if (!isCD()) {

		bool protectionResult = _gmMgr->copyProtection();
		if (isQuitRequested() || (!protectionResult)) {
			return Common::kNoError;
		}
	}
	_gmMgr->waitForNoInput();
	if (!isCD()) {

		_gameState.newRoomNum = 2;
	} else {
		if (isComet()) {
			if (_gameState.currPakNum == 5) {
				_gameState.newRoomNum = 2;
			}
			if (_gameState.currPakNum == 9) {
				_gameState.newRoomNum = 9;
			}
		}
	}
	_gMgr->clearBackbuffer();
	_gMgr->paintBackbuffer_mouse();

	while (true) {
		_gmMgr->updateInputStatus();
		if (isQuitRequested()) {
			break;
		}
		if (_gameState.currPakNum == 7 && _gameState.currRoomNum == 1 && _gameState.isAlternatePaletteActive == 0) {
			memcpy(_gMgr->normalPalette, _gMgr->mainGamePalette, 256 * 3);
			memcpy(_gMgr->mainGamePalette, _gMgr->sepiaPalette, 256 * 3);
			memcpy(_gMgr->tmpPalette, _gMgr->sepiaPalette, 256 * 3);
			_gMgr->uploadPalette(_gMgr->mainGamePalette);
			_gameState.isAlternatePaletteActive = 1;
		}
		if (_gameState.currPakNum == 2 && _gameState.currRoomNum == 22 && _gameState.isAlternatePaletteActive == 1) {
			memcpy(_gMgr->mainGamePalette, _gMgr->normalPalette, 256 * 3);
			memcpy(_gMgr->tmpPalette, _gMgr->normalPalette, 256 * 3);
			_gMgr->uploadPalette(_gMgr->mainGamePalette);
			_gameState.isAlternatePaletteActive = 0;
		}

		if (_gameState.gameVars[0x9d] == 1) {
			_gMgr->copyVideoBuffer(_gMgr->lockMainSurface(), _gMgr->getBackground());
			_gMgr->unlockMainSurface();
			_gameState.gameVars[0x9d] = _gmMgr->handleSkullPuzzle();
			_gMgr->loadStageBackground();
			if (isQuitRequested()) {
				break;
			}
		}
		if (_gmMgr->_lastPressedKey == Common::KeyCode::KEYCODE_RETURN || _gmMgr->_lastPressedKey == Common::KeyCode::KEYCODE_KP_ENTER) {
			_gmMgr->enterPressed();
		}
		if (_gmMgr->_lastPressedKey == Common::KeyCode::KEYCODE_p) {
			_gmMgr->handlePause();
		}
		if (!_gmMgr->waitingForEnter() && _gameState.currPakNum != 3 && _gmMgr->getMainActor()->lastIntersectionType != 4 && !_gMgr->getJustFadedOut() && !_txtMgr->isTextDisplayed()) {
			_gmMgr->handleInputAndMenus();
		}
		if (isQuitRequested()) {
			break;
		}
		_gmMgr->gameFrame();
		if (_gmMgr->forceLoading != 0) {
			while (!_gmMgr->handleLoad()) {
				if (isQuitRequested()) {
					break;
				}
			}
			_audioMgr->muteAllVoices();
			_gmMgr->forceLoading = 0;
		}
		_gmMgr->onUseObject();
		if (isQuitRequested()) {
			break;
		}
	}
	return Common::kNoError;
}
void CometEngine::setScummVMQuit() {
	_isScummVMQuit = true;
}
void CometEngine::setGameQuit() {
	_isGameQuit = true;
}

} // namespace Cometengine
