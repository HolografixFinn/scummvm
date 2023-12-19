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

#ifndef COMET_SPEECH_H
#define COMET_SPEECH_H
#include "common/scummsys.h"
#include"common/file.h"
#include"common/mutex.h"
//#include "audio/audiostream.h"
//#include "audio/fmopl.h"
//#include "audio/mixer.h"
//#include "common/memstream.h"

namespace Cometengine {
class CometEngine;
class SpeechManager {
public:
	SpeechManager(CometEngine *vm, bool isCD);
	void openNARarchive(uint8 textChapter);
	void closeNARarchive();
	void setDescription(bool val) { _isDescription = val; }
	bool isSayingDescription() { return _isDescription; }
	bool isSpeechActive() {return _isSpeechPlaying; }
	void startSpeech(uint16 sentenceIdx);
	void nextSpeech();
	void stopSpeech();
	void handleSpeech();
	~SpeechManager();

private:
	bool _isCD;
	CometEngine *_vm;
	uint8 *_speechBuffer;
	uint32 *_offsetsTable;
	int16_t _numOffsets;
	static const char *_narFilenames[8];
	Common::File _f;
	bool _isDescription;
	bool _isMultiFiles;
	uint16 _vocRemainingSegments;
	uint16 _numVocSegments;
	bool _isSpeechPlaying;
	uint8 *_vocData;
	uint32 _lipSyncTimerRestartValue;
	uint32 _lipSyncTimerCurrValue;
	uint32 _lipSyncCounter;
	void playNar();
	void adjustVocHeader(uint8 *vocHeader, uint32 numBytes);
	Common::Mutex _lipSyncMutex;
	void lipSync_proc();
	void resetLipSyncCounter();
	uint8 getCurrentAudioSample(uint8 *buffer);
};
} // namespace Cometengine
#endif
