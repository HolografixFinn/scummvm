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

#include "comet/speechManager.h"
#include "comet/comet.h"

namespace Cometengine {
SpeechManager::SpeechManager(CometEngine *vm, bool isCD = false) : _vm(vm), _isCD(isCD), _speechBuffer(nullptr), _f(), _offsetsTable(nullptr), _numOffsets(-1),
																   _isDescription(false), _isMultiFiles(false), _vocRemainingSegments(0), _numVocSegments(0),
																   _vocData(nullptr), _isSpeechPlaying(false),
																   _lipSyncTimerRestartValue(0x34bab), _lipSyncTimerCurrValue(0), _lipSyncCounter(0), _lipSyncMutex() {
	if (_isCD) {
		_speechBuffer = new uint8[20 * 1024 * 16]; //in EMS that's 20 pages, each one 16K
		memset(_speechBuffer, 0x80, 20 * 1024 * 16);
		_vocData = _speechBuffer;
		_lipSyncTimerCurrValue = _lipSyncTimerRestartValue;
		_vm->addTimedProc(new Common::Functor0Mem<void, SpeechManager>(this, &SpeechManager::lipSync_proc));
	}
}
void SpeechManager::lipSync_proc() {
	uint32 diff = 0;
	// _lipSyncTimerCurrValue  is a 16.16 fixed point divider used to run this proc at 18.222Hz insted of 60Hz
	if (_lipSyncTimerCurrValue >= 0x10000) {
		_lipSyncTimerCurrValue -= 0x10000;
	} else {
		diff = 0x10000 - _lipSyncTimerCurrValue;
		_lipSyncTimerCurrValue = 0;
	}
	if (_lipSyncTimerCurrValue == 0) {
		_lipSyncTimerCurrValue = _lipSyncTimerRestartValue - diff;
		{
			Common::StackLock lock(_lipSyncMutex);
			_lipSyncCounter += 0x366;
			if (_lipSyncCounter > 0x28000) {
				_lipSyncCounter -= 0x28000;
			}
		}
	}
}
void SpeechManager::resetLipSyncCounter() {
	Common::StackLock lock(_lipSyncMutex);
	_lipSyncCounter = 0;
}

SpeechManager::~SpeechManager() {
	closeNARarchive();
	if (_speechBuffer != nullptr) {
		delete[] _speechBuffer;
		_speechBuffer = nullptr;
	}
	if (_offsetsTable != nullptr) {
		delete[] _offsetsTable;
		_offsetsTable = nullptr;
	}
}
void SpeechManager::openNARarchive(uint8 textChapter) {
	uint32 size = 0;
	if (_f.isOpen()) {
		closeNARarchive();
	}
	_numOffsets = -1;
	if (!_f.open(_narFilenames[textChapter])) {
		//Error
		return;
	}
	do {
		size = _f.readUint32LE();
	} while (size == 0);
	_numOffsets = size / 4;
	_f.seek(0, 0);
	_offsetsTable = new uint32[_numOffsets];
	for (uint32 i = 0; i < _numOffsets; i++) {
		_offsetsTable[i] = _f.readUint32LE();
	}
}
void SpeechManager::closeNARarchive() {
	if (_f.isOpen()) {
		_f.close();
	}
	if (_offsetsTable != nullptr) {
		delete[] _offsetsTable;
		_offsetsTable = nullptr;
	}
	_numOffsets = -1;
}
void SpeechManager::startSpeech(uint16 sentenceIdx) {
	_vm->_audioMgr->stopDigi();
	uint32 sentenceOffset = _offsetsTable[sentenceIdx];
	if (sentenceOffset == 0) {
		//TODO handle case no speech
	} else {
		_f.seek(sentenceOffset, 0);
		_vocRemainingSegments = 0;
		_vm->_txtMgr->showText();
		_isSpeechPlaying = true;
		_vocRemainingSegments = 0;
		_isMultiFiles = false;
		_numVocSegments = 0;
		_vocData = &_speechBuffer[0];
		playNar();
	}
}

void SpeechManager::stopSpeech() {
	_vm->_audioMgr->stopDigi();
	_vocRemainingSegments = 0;
	_isMultiFiles = false;
	if (_vm->_gameState.speechOptions == 2 && _isDescription) {
		_vm->_txtMgr->hideDialogue();
		
	}
	_isSpeechPlaying = false;
	_numVocSegments = 0;
}
void SpeechManager::nextSpeech() {
	_vm->_audioMgr->stopDigi();
	uint32 playedSegments = _numVocSegments - _vocRemainingSegments;
	_vocData = &_speechBuffer[playedSegments * 64 * 1024];
	_isSpeechPlaying = true;
	playNar();
}
uint8 SpeechManager::getCurrentAudioSample(uint8 *buffer) {
	Common::StackLock lock(_lipSyncMutex);
	return buffer[_lipSyncCounter];
}
void SpeechManager::handleSpeech() {
	Actor *portrait = _vm->_gmMgr->getActor(10);
	if (!_vm->_audioMgr->getDigiPlayingStatus()) {
		if (_isMultiFiles || _vocRemainingSegments != 0) {
			nextSpeech();
		} else {
			stopSpeech();
		}
	}
	uint8 sample = getCurrentAudioSample(_vocData);
	if (sample >= 0x7e && sample <= 0x80) {
		_vm->_gmMgr->actorPrepareAnimation(portrait, 0);
	}
}

void SpeechManager::playNar() {
	uint32 totalBytesRead = 0;
	uint32 i = 0;
	uint8 vocHeader[0x20];
	//	_vm->_audioMgr->playDigiSubfile(-1);
	uint32 bRead = 0;
	if (_vocRemainingSegments == 0) {
		_numVocSegments = 0;
		_vocData = &_speechBuffer[0];
		totalBytesRead = _f.read(vocHeader, 0x20);
		uint32 vocFirstBlockSize = vocHeader[27] + (vocHeader[28] << 8) + (vocHeader[29] << 16);
		uint32 totalVocSize = vocFirstBlockSize + 31;
		if (vocHeader[0] == 0) {
			_isMultiFiles = false;
		} else {
			_isMultiFiles = true;
		}
		//we're not limited to 64k windows like in real EMS, so we just load (and play) the whole VOC
		//in order to avoid occasional hiccups during playback
		/*
		if (totalVocSize > 0x10000) {
			if (totalVocSize <= 0x1ffdf) {
				vocHeader[0] = 2;
			} else if (totalVocSize <= 0x2ffbe) {
				vocHeader[0] = 3;
			} else {
				vocHeader[0] = 4;
			}
		}
		*/
		uint8 numSegs = vocHeader[0];
		_numVocSegments = numSegs;
		// same logic as above, no need to split in 64k windows
		/*
		for (i = 0; i < numSegs || i == 0; i++) {
			if (totalBytesRead + 0x00000ffdf < totalVocSize - 0x1f) {
				bRead = _f.read(_vocData + 0x20, 0xffdf);
				totalBytesRead += 0xffdf;
			} else {
				bRead = _f.read(_vocData + 0x20, totalVocSize - 0x1f - totalBytesRead);
				bRead--;
			}
			adjustVocHeader(vocHeader, bRead);
			_vocRemainingSegments++;
			_vocData = _vocData + (4 * 16 * 1024);
		}
		*/
		_f.read(_vocData + 0x20, totalVocSize - 0x20);
		adjustVocHeader(vocHeader, vocFirstBlockSize-2);
		_vocRemainingSegments++;
		_numVocSegments = _vocRemainingSegments;
	}
	if (_numVocSegments == _vocRemainingSegments) {
		_vocData = _speechBuffer;
	}
	resetLipSyncCounter();
//	uint32 firstPlaySize = _vocData[27] + (_vocData[28] << 8) + (_vocData[29] << 16) +0x1f;
	uint32 firstPlaySize = (_vocData[27] + (_vocData[28] << 8) + (_vocData[29] << 16)) - 2 + 0x20 + 1;
	_vm->_audioMgr->playVOC(_vocData, firstPlaySize, 0);
	_vocRemainingSegments--;
}
void SpeechManager::adjustVocHeader(uint8 *vocHeader, uint32 numBytes) {
	numBytes += 2;
	vocHeader[27] = numBytes & 0x000000ff;
	vocHeader[28] = (numBytes & 0x0000ff00) >> 8;
	vocHeader[29] = (numBytes & 0x00ff0000) >> 16;
	memcpy(_vocData, vocHeader, 0x20);
	// we fix the first byte, so ScummVM VocStream accepts it
	_vocData[0] = 'C';
}

const char *SpeechManager::_narFilenames[8] = {
	"D00.NAR",
	"D01.NAR",
	"D02.NAR",
	"D03.NAR",
	"D04.NAR",
	"D05.NAR",
	"D06.NAR",
	"D07.NAR",
};

} // namespace Cometengine
