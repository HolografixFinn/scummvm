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

#include "comet/audioManager.h"
#include "audio/decoders/raw.h"
#include "audio/decoders/voc.h"
#include "comet\comet.h"
#include "common/memstream.h"
#include "common/textconsole.h"
#include "common/endian.h"

namespace Cometengine {

AudioManager::AudioManager(CometEngine *vm) : _vm(vm), _musicData(nullptr), _digiSampleData(nullptr),
											  //_currentMusicFile(0),
											  _adlib(nullptr), _isAudioInited(false), _currenDigiFile(0xff), _digiDataSize(0) {
}
void AudioManager::timerProc() {
	if (this->_isAudioInited) {
		this->_adlib->updateCallback();
	}
}
void AudioManager::init() {
	this->_adlib = new AdlibMusicDriver();
	this->_isAudioInited = true;
	this->_adlib->resetVoicesAndChannels();
	this->_sbdigi = new SBDigiDriver(_vm->_mixer);
//	_vm->addTimedProc(new Common::Functor0Mem<void, AudioManager::AdlibMusicDriver>(this->_adlib, &AudioManager::AdlibMusicDriver::updateCallback));
}
void AudioManager::initResources() {
	//ok ma musSubbfileData buffer[0x0a] ???
	_musicData = new uint8[13000];
	_digiSampleData = new uint8[65000];
	_vm->_gameState.currentMusicFile = 0;
}

AudioManager::~AudioManager() {
	if (_musicData != nullptr) {
		delete[] _musicData;
		_musicData = nullptr;
	}
	if (_digiSampleData != nullptr) {
		delete[] _digiSampleData;
		_digiSampleData = nullptr;
	}
	if (_adlib != nullptr) {
		delete _adlib;
		_adlib = nullptr;
	}
	if (_sbdigi != nullptr) {
		delete _sbdigi;
		_sbdigi = nullptr;
	}
}
void AudioManager::playMusicSubfile(uint16 subfile) {
	int16 tmp = this->_adlib->voiceParameter(0, 0x10, 0);
	if (tmp == -1) {
		this->muteAllVoices();
		/*
			Common::File f;
			if (!f.open("shadow7.add")) {
			// handle failure to find/open file
			return;
			}
			*/
		_vm->_archMgr->getFile("MUS.PAK", subfile, _musicData);
		_vm->_gameState.currentMusicFile = subfile;
		this->musicInitSong(_musicData);
		this->_adlib->voiceParameter(0, 0x80, 0);
		this->_adlib->voiceParameter(0, 0x2000, _vm->_gameState.musicVolume);
	}
}
void AudioManager::stopDigi() {
	_sbdigi->stopPlaying();
}
void AudioManager::loadDigiSubfile(uint16 subfile) {
	if (subfile != _currenDigiFile) {
		_vm->_archMgr->getFile("SMP.PAK", subfile, _digiSampleData, &_digiDataSize);
		_currenDigiFile = subfile;
	}
}
uint8 AudioManager::getDigiPlayingStatus() {
	return _sbdigi->getPlayStatus();
}

void AudioManager::playDigiSubfile(uint16 subfile, uint8 numLoops) {
	if (subfile == 0xff) {
		_sbdigi->stopPlaying();
	} else {
		if (_sbdigi->getPlayStatus() == 0) {
			if (_vm->isCD() && _vm->_spMgr->isSpeechActive()) {
				return;
			}
			loadDigiSubfile(subfile);
			if (_vm->_gameState.digiVolume != 0) {
				_sbdigi->playVOC(_digiSampleData, _digiDataSize, numLoops);
			}
		}
	}
}
void AudioManager::playDigiCurrent(uint8 numLoops) {
			if (_vm->_gameState.digiVolume != 0) {
				_sbdigi->playVOC(_digiSampleData, _digiDataSize, numLoops);
			}
}
void AudioManager::playVOC(const uint8* vocData, uint32 dataSize, uint8 numLoops) {
	_sbdigi->stopPlaying();
	_currenDigiFile = 0xff;
	_sbdigi->playVOC(vocData, dataSize, 0);
}

void AudioManager::setVoiceParams(uint16 value, uint16 channel, uint16 command) {
	if (this->_isAudioInited) {
		this->_adlib->voiceParameter(channel, command, value);
	}
}
void AudioManager::setSecondaryVoiceParams(uint16 value, uint16 channel, uint16 command) {
	if (this->_isAudioInited) {
		uint8 *ptr = _musicData;
		uint16 offs = READ_LE_UINT16(ptr + 0x0e);
		ptr += offs;
		offs = READ_LE_UINT16(ptr + (channel * 2));
		ptr += offs;
		uint16 mask = 1;
		uint16 dataMask = READ_LE_UINT16(ptr);
		ptr += 2;
		for (uint8 i = 0; i < 11; i++) {
			if (mask & dataMask) {
				uint16 val = *ptr++;
				this->_adlib->secondaryVoiceParameter(mask, command, val);
			}
			mask <<= 1;
		}
	}
	
}
void AudioManager::muteAllVoices() {
	this->_adlib->voiceParameter(0, 0x40, 0);
}
void AudioManager::musicInitSong(void *data) {
	if (this->_isAudioInited) {
		this->_adlib->initVoices(reinterpret_cast<uint8 *>(data));
		this->_adlib->resetCard();
	}
}
const uint8 AudioManager::AdlibMusicDriver::_registersOffsetsNoDrums[] = {
	0x03, 0x00,
	0x04, 0x01,
	0x05, 0x02,
	0x0b, 0x08,
	0x0c, 0x09,
	0x0d, 0x0a,
	0x13, 0x10,
	0x14, 0x11,
	0x15, 0x12,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff};
const uint8 AudioManager::AdlibMusicDriver::_registersOffsetsDrums[] = {
	0x03,
	0x00,
	0x04,
	0x01,
	0x05,
	0x02,
	0x0b,
	0x08,
	0x0c,
	0x09,
	0x0d,
	0x0a,
	0x13,
	0x10,
	0xff,
	0x14,
	0xff,
	0x12,
	0xff,
	0x15,
	0xff,
	0x11,
};
const uint8 AudioManager::AdlibMusicDriver::_registerOffsetToChannel[] = {
	0x00, 0x01, 0x02, 0x0ff, 0x0ff, 0x0ff, 0x0ff, 0x0ff,
	0x03, 0x04, 0x05, 0x0ff, 0x0ff, 0x0ff, 0x0ff, 0x0ff,
	0x06, 0x07, 0x08, 0x0ff, 0x0ff, 0x0ff};
const uint8 AudioManager::AdlibMusicDriver::_percussionsFlags[] = {
	0x10, // bass drum
	0x08, // snare
	0x04, // tom-tom
	0x02, // cymbal
	0x01  // hi-hat
};

const uint16 *AudioManager::AdlibMusicDriver::_noteValues[] = {
	&AdlibMusicDriver::_freqValues[0],   // C
	&AdlibMusicDriver::_freqValues[25],  // C#
	&AdlibMusicDriver::_freqValues[50],  // D
	&AdlibMusicDriver::_freqValues[75],  // D#
	&AdlibMusicDriver::_freqValues[100], // E
	&AdlibMusicDriver::_freqValues[125], // F
	&AdlibMusicDriver::_freqValues[150], // F#
	&AdlibMusicDriver::_freqValues[175], // G
	&AdlibMusicDriver::_freqValues[200], // G#
	&AdlibMusicDriver::_freqValues[225], // A
	&AdlibMusicDriver::_freqValues[250], // A#
	&AdlibMusicDriver::_freqValues[275]  // B
};
const uint16 AudioManager::AdlibMusicDriver::_freqValues[] = {
	0x0157, 0x0158, 0x0159, 0x015A, 0x015A, 0x015B, 0x015C, 0x015D, 0x015E, 0x015F, 0x015F, 0x0160,         // C
	0x0161, 0x0162, 0x0163, 0x0164, 0x0164, 0x0165, 0x0166, 0x0167, 0x0168, 0x0168, 0x0169, 0x016A, 0x016B, // C
	0x016C, 0x016D, 0x016D, 0x016E, 0x016F, 0x0170, 0x0171, 0x0172, 0x0173, 0x0174, 0x0174, 0x0175,         // C#
	0x0176, 0x0177, 0x0178, 0x0179, 0x017A, 0x017B, 0x017B, 0x017C, 0x017D, 0x017E, 0x017F, 0x0180, 0x0181, // C#
	0x0181, 0x0183, 0x0183, 0x0184, 0x0185, 0x0186, 0x0187, 0x0188, 0x0189, 0x018A, 0x018B, 0x018C,         // D
	0x018D, 0x018E, 0x018E, 0x018F, 0x0190, 0x0191, 0x0192, 0x0193, 0x0194, 0x0195, 0x0196, 0x0197, 0x0198, // D
	0x0198, 0x019A, 0x019A, 0x019B, 0x019C, 0x019D, 0x019F, 0x019F, 0x01A0, 0x01A1, 0x01A2, 0x01A3,         // D#
	0x01A4, 0x01A5, 0x01A6, 0x01A7, 0x01A8, 0x01A9, 0x01AA, 0x01AB, 0x01AC, 0x01AD, 0x01AE, 0x01AF, 0x01B0, // D#
	0x01B1, 0x01B2, 0x01B3, 0x01B4, 0x01B5, 0x01B6, 0x01B7, 0x01B8, 0x01B9, 0x01BA, 0x01BB, 0x01BC,         // E
	0x01BD, 0x01BF, 0x01C0, 0x01C1, 0x01C2, 0x01C3, 0x01C4, 0x01C5, 0x01C6, 0x01C7, 0x01C8, 0x01C9, 0x01CA, // E
	0x01CB, 0x01CC, 0x01CD, 0x01CE, 0x01CF, 0x01D0, 0x01D2, 0x01D3, 0x01D4, 0x01D5, 0x01D6, 0x01D7,         // F
	0x01D8, 0x01D9, 0x01DA, 0x01DB, 0x01DD, 0x01DE, 0x01DF, 0x01E0, 0x01E1, 0x01E2, 0x01E3, 0x01E4, 0x01E5, // F
	0x01E6, 0x01E8, 0x01E9, 0x01EA, 0x01EB, 0x01EC, 0x01ED, 0x01EF, 0x01F0, 0x01F1, 0x01F2, 0x01F3,         // F#
	0x01F4, 0x01F6, 0x01F7, 0x01F8, 0x01F9, 0x01FA, 0x01FB, 0x01FD, 0x01FE, 0x01FF, 0x0200, 0x0201, 0x0202, // F#
	0x0203, 0x0205, 0x0206, 0x0207, 0x0208, 0x020A, 0x020B, 0x020C, 0x020D, 0x020F, 0x0210, 0x0211,         // G
	0x0212, 0x0214, 0x0215, 0x0216, 0x0217, 0x0219, 0x021A, 0x021B, 0x021C, 0x021D, 0x021F, 0x0220, 0x0221, // G
	0x0222, 0x0224, 0x0225, 0x0226, 0x0227, 0x0229, 0x022A, 0x022C, 0x022D, 0x022E, 0x022F, 0x0231,         // G#
	0x0232, 0x0234, 0x0235, 0x0236, 0x0237, 0x0239, 0x023A, 0x023B, 0x023C, 0x023E, 0x023F, 0x0241, 0x0242, // G#
	0x0243, 0x0245, 0x0246, 0x0247, 0x0248, 0x024A, 0x024B, 0x024D, 0x024E, 0x0250, 0x0251, 0x0252,         // A
	0x0254, 0x0255, 0x0257, 0x0258, 0x0259, 0x025B, 0x025C, 0x025E, 0x025F, 0x0260, 0x0262, 0x0263, 0x0264, // A
	0x0266, 0x0267, 0x0269, 0x026A, 0x026B, 0x026D, 0x026E, 0x0270, 0x0271, 0x0273, 0x0274, 0x0276,         // A#
	0x0277, 0x0279, 0x027A, 0x027C, 0x027D, 0x027F, 0x0280, 0x0282, 0x0283, 0x0285, 0x0286, 0x0288, 0x0289, // A#
	0x028A, 0x028C, 0x028D, 0x028F, 0x0291, 0x0292, 0x0294, 0x0295, 0x0297, 0x0299, 0x029A, 0x029C,         // B
	0x029D, 0x029F, 0x02A0, 0x02A2, 0x02A3, 0x02A5, 0x02A7, 0x02A8, 0x02AA, 0x02AB, 0x02AD, 0x02AF, 0x02B0  // B
};
AudioManager::AdlibMusicDriver::~AdlibMusicDriver() {
	_opl->stop();
	delete _opl;
}

AudioManager::BaseMusicDriver::DriverType AudioManager::AdlibMusicDriver::getDriverType() {
	DriverType type;
	type.id[0] = 'A';
	type.id[1] = 'D';
	type.version = 1;
	return type;
}
AudioManager::AdlibMusicDriver::AdlibMusicDriver() : _isMusicInited(false), _pauseStatus(false), _callback(nullptr), _secondaryDataPtr(nullptr), _instruments(nullptr), _secondaryInstrumentsPtr(nullptr) {
	for (int i = 0; i < 11; i++) {
		memset(&_voicesOp1[i], 0, sizeof(VoiceOperatorStatus));
		_voicesOp1[i].adlibChannel = i;
		_voicesOp1[i].relativeOperator = &_voicesOp2[i];
		_voicesOp1[i].flags = 0x40;
		_voicesOp1[i].destVolume = 0x7f;
		_voicesOp1[i].currVolume = 0x7f;
		_voicesOp1[i].volumeFadeCounter = 1;
		_voicesOp1[i].volumeFadeStepDuration = 1;

		_voicesOp2[i].adlibChannel = i;
		_voicesOp2[i].relativeOperator = &_voicesOp1[i];
		_voicesOp2[i].flags = 0x8040;
		_voicesOp2[i].destVolume = 0x7f;
		_voicesOp2[i].currVolume = 0x7f;
		_voicesOp2[i].volumeFadeCounter = 1;
		_voicesOp2[i].volumeFadeStepDuration = 1;
	}
	for (int i = 0; i < 11; i++) {
		memset(&_channels[i], 0, sizeof(AdlibChannelStatus));
		_channels[i].note = 0xffff;
		_channels[i].playStatus = 0x40;
		_channels[i].instrument = 0xff;
		_channels[i].volume = 0xff;
		_channels[i].bend = 0x9c;
		_channels[i].flag8000 = 0xffff;
	}
	_opl = OPL::Config::create();
	if (!_opl || !_opl->init()) {
		error("Failed to create OPL");
	}

	_opl->start(new Common::Functor0Mem<void, AdlibMusicDriver>(this, &AdlibMusicDriver::updateCallback), 60);
//	_opl->start(nullptr, 60); // we use the 60Hz general timer in CometEngine in order to make the audio driver tick
}
void AudioManager::AdlibMusicDriver::resetAdlibChannelStatus(uint8 channelIdx) {
	_channels[channelIdx].volume = 0xff;
	_channels[channelIdx].instrument = 0xff;
	_channels[channelIdx].playStatus = 0x40;
	_channels[channelIdx].note = 0xffff;
	_channels[channelIdx].bend = 0x9c;
	_channels[channelIdx].flag8000 = 0xffff;
}
void AudioManager::AdlibMusicDriver::setOperatorVolumeScaled(uint8 *instrumentData, uint8 registerOffset, uint8 level) {
	if (registerOffset == 0xff) {
		return;
	}
	uint8 vol = *instrumentData;
	uint16 scaled = 0;
	scaled = ((((vol & 0x3f) * level) * 2) + 0x7f) / 0xfe;
	scaled = 0x3f - scaled;
	scaled = (vol & 0xc0) | (scaled & 0xff);
	this->_opl->writeReg(0x40 + registerOffset, scaled);
}
void AudioManager::AdlibMusicDriver::setNoteData(uint8 channel, uint8 noteBits, uint8 bend, uint8 flags, uint8 keyOnOff) {
	int8 signedBend = static_cast<int8>(bend);
	if (!(flags & 0x80)) {
		_opl->writeReg(0xb0 + channel, 0);
	}
	const uint16 *noteArray = this->_noteValues[noteBits & 0x0f];
	if (signedBend < 0 && (noteBits & 0x0f) == 0) {
		noteArray = this->_noteValues[11] + 25;
		if ((noteBits & 0x70) != 0) {
			noteBits -= 0x10;
		}
	}
	if (noteBits & 0x80) {
		keyOnOff = 0x40;
	}
	uint16 fValue = *(noteArray + signedBend);
	this->_opl->writeReg(0xa0 + channel, fValue & 0x00ff);
	uint8 freqHI = ((noteBits & 0x70) >> 2) + (fValue >> 8);
	if (!(keyOnOff & 0x40)) {
		freqHI |= 0x20;
	}
	_opl->writeReg(0xb0 + channel, freqHI);
}
void AudioManager::AdlibMusicDriver::setNote(uint8 channel, uint8 noteBits, uint8 bend, uint8 flags, uint8 keyOnOff) {
	if (this->_needs9MelodicVoices) {
		this->setNoteData(channel, noteBits, bend, flags, keyOnOff);
		return;
	}
	if (channel < 6) {
		this->setNoteData(channel, noteBits, bend, flags, keyOnOff);
		return;
	}
	if (channel == 6) {
		this->setNoteData(channel, noteBits, bend, flags, 0x40);

	} else if (channel == 8 && !(noteBits & 0x80)) {
		this->setNoteData(8, noteBits, bend, flags, 0x40);
		uint8 nb = noteBits;
		uint8 oct = noteBits & 0x70;
		nb = nb & 0x0f;
		nb += 7;
		if (nb >= 12) {
			nb -= 12;
			if (oct != 0x70) {
				oct += 0x10;
			}
		}
		nb |= oct;
		this->setNoteData(7, nb, bend, flags, 0x40);
	}
	uint8 drumByte = AdlibMusicDriver::_percussionsFlags[channel - 6];
	uint8 drumByte2 = (~drumByte) & this->_currentRythmRegister;
	this->_opl->writeReg(0xbd, drumByte2);
	if (!(keyOnOff & 0x40) && !(noteBits & 0x80)) {
		drumByte2 |= drumByte;
		this->_opl->writeReg(0xbd, drumByte2);
	}
	this->_currentRythmRegister = drumByte2;
}

void AudioManager::AdlibMusicDriver::loadOperatorInstrument(OperatorInstrumentData data, uint8 operatorRegisterOffset) {
	uint8 channel = AdlibMusicDriver::_registerOffsetToChannel[operatorRegisterOffset];
	if (channel != 0xff) {
		this->_opl->writeReg(channel + 0xc0, data.feedbackAlgo);
	}
	this->_opl->writeReg(operatorRegisterOffset + 0x60, data.AD);
	this->_opl->writeReg(operatorRegisterOffset + 0x80, data.SR);
	this->_opl->writeReg(operatorRegisterOffset + 0x20, data.AmpVib);
	this->_opl->writeReg(operatorRegisterOffset + 0xe0, data.Waveform);
}

void AudioManager::AdlibMusicDriver::silenceChannel(uint8 channel) {
	this->_opl->writeReg(channel + 0xa0, 0);
	this->_opl->writeReg(channel + 0xb0, 0);
}
void AudioManager::AdlibMusicDriver::resetCard() {
	this->_needs9MelodicVoices = false;
	this->_opl->writeReg(0xbd, this->_currentRythmRegister);
	this->_opl->writeReg(0x60, 4);
	this->_opl->writeReg(0x80, 4);
	this->_opl->writeReg(0x08, 0);
	this->_opl->writeReg(0x01, 0x20);
	for (int i = 0; i < 9; i++) {
		silenceChannel(i);
	}
	for (int i = 0; i < 11; i++) {
		this->resetAdlibChannelStatus(i);
	}
	silenceChannel(6);
	silenceChannel(7);
	silenceChannel(8);
	setNoteData(8, 0, 0, 0, 0x40);
	setNoteData(7, 7, 0, 0, 0x40);
}
void AudioManager::AdlibMusicDriver::updateVoiceOperator(VoiceOperatorStatus &voice, AdlibChannelStatus &channel) {
	if (voice.flags & 0x40) {
		return;
	}
	if (voice.flags & 0x02) {
		voice.currTrackData = voice.trackData;
		voice.flags &= 0xfffd;
		voice.notesPlayed = 0;

	} else {
		if (voice.destVolume != voice.currVolume) {
			voice.volumeFadeCounter--;
			if (voice.volumeFadeCounter & 0x80) {
				voice.volumeFadeCounter = voice.volumeFadeStepDuration;
				int16 step = 1;
				if (voice.destVolume < voice.currVolume) {
					step = -1;
				}
				if (voice.currVolume + step < 0) {
					voice.currVolume = 0;
				} else if (voice.currVolume + step > 0x7f) {
					voice.currVolume = 0x7f;
				} else {
					voice.currVolume = voice.currVolume + step;
				}
			}
		}
		voice.currNoteCounter--;
		if (voice.currNoteCounter != 0) {
			return;
		}
		voice.currNoteCounter = voice.reloadNoteCounter;
	}
	do {
		uint8 operation = *voice.currTrackData++;
		uint8 param1 = *voice.currTrackData++;
		uint8 param2 = 0;
		switch (operation & 0x7f) {
		case 0: //set track as loopable
			voice.flags |= 0x02;
			if (!(voice.flags & 0x20)) {
				voice.flags |= 0x40;
				if (voice.flags & 0x8000) {
					voice.relativeOperator->flags &= 0xfffb;
				}
			}
			break;
		case 1: //change note duration
			param2 = *voice.currTrackData++;
			voice.reloadNoteCounter = voice.baseNoteDuration + ((param2 << 8) + param1);
			voice.currNoteCounter = voice.baseNoteDuration + ((param2 << 8) + param1);
			break;
		case 2: //change note
			voice.notesPlayed++;
			voice.note = param1;

			break;
		case 3: //change instrument
			voice.instrument = param1;
			break;
		case 4: //set volume attenuation
			voice.volumeAttenuation = param1;
			break;
		case 5: // set pitch bend
			voice.noteBend = param1;
			break;
		case 6: // callback?
			this->_callback(voice.adlibChannel);
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		default:
			error("Unexpected music opcode");
			break;
		}
		if (operation & 0x80) {
			break;
		}
	} while (true);
}
void AudioManager::AdlibMusicDriver::updateAdlibChannel(VoiceOperatorStatus &voice, AdlibChannelStatus &channel) {
	uint16 tmp = voice.flags & 0x40;
	if (tmp != channel.playStatus) {
		channel.playStatus = tmp;
		if (tmp & 0x40) {
			setNote(voice.adlibChannel, voice.note | 0x80, voice.noteBend, 0, 0x40);
			resetAdlibChannelStatus(voice.adlibChannel);
			return;
		}
	}
	if (tmp & 0x40) {
		return;
	}
	tmp = voice.flags & 0x8000;
	if ((channel.flag8000 & 0x01) || (channel.flag8000 != tmp)) {
		channel.flag8000 = tmp;
		channel.volume = 0xff;
		channel.instrument = 0xff;
		channel.note = 0xffff;
		channel.bend = 0x9c;
	}
	uint8 regOffsetOp2 = this->_registerOffsetsTable[voice.adlibChannel * 2];
	uint8 regOffsetOp1 = this->_registerOffsetsTable[(voice.adlibChannel * 2) + 1];
	if (regOffsetOp1 == 0xff && regOffsetOp2 == 0xff) {
		return;
	}
	if (voice.instrument != channel.instrument) {
		channel.instrument = voice.instrument;
		InstrumentData *inst = &this->_instruments[voice.instrument];
		loadOperatorInstrument(inst->op1, regOffsetOp1);
		if (regOffsetOp2 != 0xff) {
			loadOperatorInstrument(inst->op2, regOffsetOp2);
		}
		channel.volume = 0xff;
	}
	if (voice.currVolume - voice.volumeAttenuation <= 0) {
		tmp = 0;
	} else {
		tmp = voice.currVolume - voice.volumeAttenuation;
	}
	if (channel.volume != tmp) {
		channel.volume = tmp;
		uint16 vol = tmp;
		InstrumentData *inst = &this->_instruments[channel.instrument];
		setOperatorVolumeScaled(&inst->op1.volume, regOffsetOp1, regOffsetOp2 == 0xff ? tmp : voice.currVolume);
		setOperatorVolumeScaled(&inst->op2.volume, regOffsetOp2, tmp);
	}
	tmp = voice.noteBend;
	if (voice.noteBend != channel.bend) {
		channel.bend = voice.noteBend;
		if (channel.note == voice.note) {
			tmp |= 0x8000;
		}
	} else {
		if (voice.note == channel.note) {
			return;
		}
	}
	channel.note = voice.note | 0x8000;
	voice.note = voice.note | 0x8000;
	setNote(voice.adlibChannel, voice.note & 0x00ff, tmp & 0x00ff, tmp >> 8, voice.flags);
}
void AudioManager::AdlibMusicDriver::updateCallback() {
	//		if (!this->_isMusicInited) {
	//			return;
	//		}
	if (this->_pauseStatus) {
		return;
	}
	for (int i = 0; i < 11; i++) {
		updateVoiceOperator(this->_voicesOp1[i], this->_channels[i]);
		if (this->_voicesOp1[i].flags & 0x04) {
			updateVoiceOperator(*this->_voicesOp1[i].relativeOperator, this->_channels[i]);
		}
		updateAdlibChannel(this->_voicesOp1[i], this->_channels[i]);
	}
}
AudioManager::BaseMusicDriver::MusicCallback AudioManager::AdlibMusicDriver::getCallback() {
	return this->_callback;
}
void AudioManager::AdlibMusicDriver::setBaseRegister(uint16 newRegister) {
	//should change opl base register, I don't think it's needed here
}
int16 AudioManager::AdlibMusicDriver::voiceParameter(uint16 channelFlags, uint16 operation, uint16 value) {
	int16 retval = -1;
	if (channelFlags == 0) {
		channelFlags = 0x7ff;
	}
	uint16 mask = 1;
	VoiceOperatorStatus *voice = &this->_voicesOp1[0];
	for (int i = 0; i < 11; i++) {
		if (channelFlags & mask && voice->trackData != nullptr) {
			if (operation & 0x100) {
				if (value == 0) {
					this->_pauseStatus = true;
					resetCard();
				} else {
					this->_pauseStatus = false;
				}
				if (operation & 0x400) {
					voice->baseNoteDuration = value;
				}
			}
			if (operation & 0x40) {
				if (!(voice->flags & 0x40)) {
					voice->flags |= 0x40;
				}
			}
			if (operation & 0x80) {
				voice->flags = 0x40;
				voice->currVolume = 0x7f;
				voice->destVolume = 0x7f;
				voice->volumeAttenuation = 0;
				resetAdlibChannelStatus(voice->adlibChannel);
				voice->flags = 0x02;
			}
			if (operation & 0x20) {
				voice->flags |= 0x20;
			}
			if (operation & 0x2000 && !(operation & 0x10)) {
				voice->currVolume = value & 0x7f;
				voice->destVolume = value & 0x7f;
			}
			if (operation & 0x8000) {
				voice->destVolume = value & 0x7f;
			}
			if (operation & 0x1000) {
				voice->volumeFadeStepDuration = value;
				voice->volumeFadeCounter = value;
			}
			if (operation & 0x10) {
				if (!(operation & 0x2000)) {
					if (!(voice->flags & 0x40)) {
						if (retval <= voice->notesPlayed) {
							retval = voice->notesPlayed;
						}
					}
				} else {
					if (voice->currVolume != value) {
						retval = 0;
					}
				}
			}
		}
		mask = mask << 1;
		voice++;
	}
	if (operation & 0x200) {
		//we should return the callback, but not do it for now
		//return this->getCallback();
	}
	return retval;
}
int16 AudioManager::AdlibMusicDriver::secondaryVoiceParameter(uint16 channelFlags, uint16 operation, uint16 value) {
	int16 retval = -1;
	if (channelFlags == 0) {
		channelFlags = 0x7ff;
	}
	uint16 mask = 1;
	VoiceOperatorStatus *voice = &this->_voicesOp2[0];
	for (int i = 0; i < 11; i++) {
		if (channelFlags & mask) {
			if (operation & 0x400) {
				voice->baseNoteDuration = value;
			}
			if (operation & 0x40) {
				voice->flags |= 0x40;
			}
			if ((operation & 0x80) && !(operation & 0x4000) && (voice->flags & 0x40)) {
				uint8 *ptr = this->_secondaryDataPtr;
				ptr += 0x12;
				ptr += (value * 4);
				ptr = this->_secondaryDataPtr + (*((uint16 *)ptr));
				voice->trackData = ptr;
				voice->flags = 0x8002;
				voice->volumeAttenuation = 0;
				voice->relativeOperator->flags |= 0x04;
				resetAdlibChannelStatus(voice->adlibChannel);
			}
			if (operation & 0x20) {
				voice->flags |= 0x20;
			}
			if ((operation & 0x2000) && !(operation & 0x10)) {
				voice->currVolume = value & 0x7f;
				voice->destVolume = value & 0x7f;
			}
			if (operation & 0x8000) {
				voice->destVolume = value & 0x7f;
			}
			if (operation & 0x10) {
				if (!(operation & 0x2000)) {
					if (!(voice->flags & 0x40)) {
						if (retval <= voice->notesPlayed) {
							retval = voice->notesPlayed;
						}
					}
				} else {
					if (voice->currVolume != value) {
						retval = 0;
					}
				}
			}
		}
		mask = mask << 1;
		voice++;
	}
	return retval;
}

void AudioManager::AdlibMusicDriver::initVoices(uint8 *const data) {
	this->_registerOffsetsTable = AdlibMusicDriver::_registersOffsetsNoDrums;
	uint8 tmp = data[0x3c];
	if (data[0x3d] == 0) {
		this->_needs9MelodicVoices = false;
		tmp |= 0x20;
		this->_registerOffsetsTable = AdlibMusicDriver::_registersOffsetsDrums;
	} else {
		this->_needs9MelodicVoices = true;
	}
	this->_currentRythmRegister = tmp;
	for (int i = 0; i < 11; i++) {
		uint32 offset = *((uint32 *)(data + 8 + (i * 4)));

		if (i > 11) {
			offset = 0;
		}

		if (offset == 0) {
			this->_voicesOp1[i].trackData = nullptr;
		} else {
			this->_voicesOp1[i].trackData = data + offset;
		}
		this->_voicesOp1[i].flags |= 0x40;
	}
	uint32 offset = *((uint32 *)(data + 0x34));
	this->_instruments = reinterpret_cast<InstrumentData *>(data + offset);
}
void AudioManager::AdlibMusicDriver::initSecondaryVoices(uint8 *const data) {
	this->_secondaryDataPtr = data;
	uint16 offset = *((uint16 *)data + 0x0a);
	this->_secondaryInstrumentsPtr = reinterpret_cast<InstrumentData *>(data + offset);
	for (int i = 0; i < 11; i++) {
		this->_voicesOp2[i].flags |= 0x40;
	}
}
void AudioManager::AdlibMusicDriver::removeDriver() {
}
void AudioManager::AdlibMusicDriver::resetVoicesAndChannels() {
	for (int i = 0; i < 11; i++) {
		this->_voicesOp1[i].flags |= 0x40;
		this->_voicesOp1[i].relativeOperator->flags |= 0x40;
		resetAdlibChannelStatus(i);
	}
	resetCard();
}
void AudioManager::AdlibMusicDriver::setNoteBending(uint8 channel, uint8 note, uint8 bend, uint8 flags) {
	setNote(channel, note, bend, flags, 0);
}

void AudioManager::AdlibMusicDriver::setDrumsMode(uint8 needs9MelodicChannels) {
	this->_registerOffsetsTable = AdlibMusicDriver::_registersOffsetsNoDrums;
	uint8 tmp = 0;
	if (needs9MelodicChannels == 0) {
		tmp |= 0x20;
		this->_needs9MelodicVoices = false;
		this->_registerOffsetsTable = AdlibMusicDriver::_registersOffsetsDrums;
	} else {
		this->_needs9MelodicVoices = true;
	}
	this->_currentRythmRegister = tmp;
}
void AudioManager::AdlibMusicDriver::setInstrumentData(uint8 *const data, uint16 voice, uint8 flags, uint8 volume) {
	uint8 registerOffsetOp2 = this->_registerOffsetsTable[voice * 2];
	uint8 registerOffsetOp1 = this->_registerOffsetsTable[(voice * 2) + 1];
	if (registerOffsetOp1 == 0xff && registerOffsetOp2 == 0xff) {
		return;
	}
	InstrumentData *ptr = reinterpret_cast<InstrumentData *>(data);
	if (!flags) {
		this->loadOperatorInstrument(ptr->op1, registerOffsetOp1);
		if (registerOffsetOp2 != 0xff) {
			this->loadOperatorInstrument(ptr->op2, registerOffsetOp2);
		}
	}
	this->setOperatorVolumeScaled(&ptr->op1.volume, registerOffsetOp1, volume);
}

AudioManager::SBDigiDriver::SBDigiDriver(Audio::Mixer *_mx) : _mixer(_mx), _sfxId(1), _speechId(2), _handleSfx(), _handleSpeech(),
															  _mstr(nullptr), _astr(nullptr), _lastLoopCount(0), _lastVocData(nullptr) {
}
AudioManager::SBDigiDriver::~SBDigiDriver() {
	//streams should auto-dispose after _mixer->stopAll
	/*
		if (_mstr != nullptr) {
			delete _mstr;
			_mstr = nullptr;
		}
		*/
	_mixer->stopAll();
	/*
		if (_astr != nullptr) {
			delete _astr;
			_astr = nullptr;
		}
		*/
}
void AudioManager::SBDigiDriver::initCard() { //AH=00
}
void AudioManager::SBDigiDriver::uninstallDriver() { //AH=02
}
void AudioManager::SBDigiDriver::restoreOriginalIRQ() { //AH=04
}
void AudioManager::SBDigiDriver::playVOC(const uint8 *vocData, uint32 dataSize, uint8 numLoops) { //AH=06
	_mstr = new Common::MemoryReadStream(reinterpret_cast<const byte *>(vocData), dataSize);
	Audio::SeekableAudioStream *sastr = Audio::makeVOCStream(_mstr, Audio::FLAG_UNSIGNED, DisposeAfterUse::YES);
	if (numLoops > 1) {
		_astr = new Audio::LoopingAudioStream(sastr, numLoops, DisposeAfterUse::YES);
	} else {
		_astr = sastr;
	}

	_mixer->playStream(Audio::Mixer::SoundType::kSFXSoundType, &_handleSfx, _astr, _sfxId);
	_playState = 1;
}
void AudioManager::SBDigiDriver::stopPlaying() { //AH=08
	_mixer->stopAll();
	_playState = 0;
}
void AudioManager::SBDigiDriver::setFreqOffset(uint16 offs) { //AH=0a
															  //TODO find best way to implement this
}
void AudioManager::SBDigiDriver::nop() { //AH=0c
}
uint8 AudioManager::SBDigiDriver::getPlayStatus() { //AH=0e
	if (_mixer->isSoundIDActive(_sfxId) || _mixer->isSoundIDActive(_speechId)) {
		_playState = 1;
	} else {
		_playState = 0;
	}
	return _playState;
}
const uint8 *AudioManager::SBDigiDriver::getPtrPlayStatus() { //AH=10
	return &_playState;
}
} // namespace Cometengine
