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

#ifndef COMET_SOUND_H
#define COMET_SOUND_H
#include "common/scummsys.h"
#include "common/memstream.h"
#include "audio/audiostream.h"
#include "audio/fmopl.h"
#include "audio/mixer.h"

namespace Cometengine {
class CometEngine;
	class AudioManager {
	public:
		AudioManager(CometEngine *vm);
		virtual ~AudioManager();
		void init();
		void initResources();
		void playMusicSubfile(uint16 subfile);
		void playDigiSubfile(uint16 subfile,uint8 numLoops=1);
		void setVoiceParams(uint16 value, uint16 channel, uint16 command);
		void setSecondaryVoiceParams(uint16 value, uint16 channel, uint16 command);
		void muteAllVoices();
		void musicInitSong(void *data);
		void stopDigi();
		uint8 getSampleIdx() { return _currenDigiFile; }
		void loadDigiSubfile(uint16 subfile);
		void playDigiCurrent(uint8 numLoops);
		void playVOC(const char *vocData, uint32 dataSize, uint8 numLoops);
		uint8 getDigiPlayingStatus();
	private:
		void timerProc();
		CometEngine *_vm;
		char *_musicData;
		char *_digiSampleData;
//		uint8 _currentMusicFile;
		uint8 _currenDigiFile;
		uint32 _digiDataSize;

		class BaseMusicDriver {
		public:
			struct DriverType {
				char id[2];
				uint16 version;
				/*
				DriverType(char c1, char c2, uint16 _version) :version(_version) {
					id[0] = c1;
					id[1] = c2;
				}
				*/
			};
			typedef void(*MusicCallback)(uint16);

			//		BaseMusicDriver();
			//		virtual ~BaseMusicDriver();

			virtual void updateCallback() = 0; //AH=00
			virtual void resetVoicesAndChannels() = 0; //AH=02
			virtual void resetCard() = 0; //AH=04
			virtual void initVoices(uint8 * const data) = 0; //AH=06
			virtual void initSecondaryVoices(uint8 * const data) = 0; //AH=08
			virtual int16 voiceParameter(uint16 channelFlags, uint16 operation, uint16 value) = 0; //AH=0A
			virtual int16 secondaryVoiceParameter(uint16 channelFlags, uint16 operation, uint16 value) = 0; //AH=0C
			virtual void removeDriver() = 0; //AH=0E
			virtual DriverType getDriverType() = 0; //AH=10
			virtual MusicCallback getCallback() = 0; //AH=12
			virtual void setBaseRegister(uint16 newRegister) = 0; //AH=14
			virtual void setNoteBending(uint8 channel, uint8 note, uint8 bend, uint8 flags) = 0; //AH=16
			virtual void setInstrumentData(uint8 * const data, uint16 voice, uint8 flags, uint8 volume) = 0; //AH=18
			virtual void setDrumsMode(uint8 needs9MelodicChannels) = 0; //AH=1A


		};

		class AdlibMusicDriver : public BaseMusicDriver {
		public:
			AdlibMusicDriver();
			virtual ~AdlibMusicDriver();

			virtual void updateCallback(); //AH=00
			virtual void resetVoicesAndChannels(); //AH=02
			virtual void resetCard(); //AH=04
			virtual void initVoices(uint8 * const data); //AH=06
			virtual void initSecondaryVoices(uint8 * const data); //AH=08
			virtual int16 voiceParameter(uint16 channelFlags, uint16 operation, uint16 value); //AH=0A
			virtual int16 secondaryVoiceParameter(uint16 channelFlags, uint16 operation, uint16 value); //AH=0C
			virtual void removeDriver(); //AH=0E
			virtual DriverType getDriverType(); //AH=10
			virtual MusicCallback getCallback(); //AH=12
			virtual void setBaseRegister(uint16 newRegister); //AH=14
			virtual void setNoteBending(uint8 channel, uint8 note, uint8 bend, uint8 flags); //AH=16
			virtual void setInstrumentData(uint8 * const data, uint16 voice, uint8 flags, uint8 volume); //AH=18
			virtual void setDrumsMode(uint8 needs9MelodicChannels); //AH=1A

#include "common/pack-start.h"	// START STRUCT PACKING
			struct OperatorInstrumentData {
				uint8 volume;
				uint8 AmpVib;
				uint8 feedbackAlgo;
				uint8 Waveform;
				uint8 AD;
				uint8 SR;

			} PACKED_STRUCT;
#include "common/pack-end.h"	// END STRUCT PACKING

#include "common/pack-start.h"	// START STRUCT PACKING
			struct InstrumentData {
				uint8 id;
				OperatorInstrumentData op1;
				OperatorInstrumentData op2;
			} PACKED_STRUCT;
#include "common/pack-end.h"	// END STRUCT PACKING
		protected:
			OPL::OPL *_opl;

		private:
			bool _isMusicInited;

			struct VoiceOperatorStatus {
				uint16 adlibChannel;
				VoiceOperatorStatus *relativeOperator;
				uint16 flags;
				uint8 *trackData;
				uint8 *currTrackData;
				uint16 currNoteCounter;
				uint16 reloadNoteCounter;
				uint16 instrument;
				uint16 baseNoteDuration;
				uint16 note;
				uint16 noteBend;
				uint16 notesPlayed;
				uint16 destVolume;
				uint16 volumeFadeCounter;
				uint16 volumeFadeStepDuration;
				uint16 currVolume;
				uint16 volumeAttenuation;

			};
			VoiceOperatorStatus _voicesOp1[11];
			VoiceOperatorStatus _voicesOp2[11];

			struct AdlibChannelStatus {
				uint16 note;
				uint16 playStatus;
				uint16 instrument;
				uint16 volume;
				uint16 bend;
				uint16 flag8000;
			};
			AdlibChannelStatus _channels[11];



			uint16 _baseRegister;
			bool _pauseStatus;
			bool _needs9MelodicVoices;
			uint8 _currentRythmRegister;

			InstrumentData *_instruments;
			MusicCallback _callback;
			const uint8 *_registerOffsetsTable;
			uint8 *_secondaryDataPtr;
			InstrumentData *_secondaryInstrumentsPtr;

			static const uint8 _registersOffsetsNoDrums[];
			static const uint8 _registersOffsetsDrums[];
			static const uint16 _freqValues[];
			static const uint16 *_noteValues[];
			static const uint8 _registerOffsetToChannel[];
			static const uint8 _percussionsFlags[];

			void resetAdlibChannelStatus(uint8 channelIdx);
			void setOperatorVolumeScaled(uint8 *instrumentData, uint8 registerOffset, uint8 level);
			void setNote(uint8 channel, uint8 noteBits, uint8 bend, uint8 flags, uint8 keyOnOff);
			void setNoteData(uint8 channel, uint8 noteBits, uint8 bend, uint8 flags, uint8 keyOnOff);
			void loadOperatorInstrument(OperatorInstrumentData data, uint8 operatorRegisterOffset);
			void silenceChannel(uint8 channel);
			void updateVoiceOperator(VoiceOperatorStatus &voice, AdlibChannelStatus &channel);
			void updateAdlibChannel(VoiceOperatorStatus &voice, AdlibChannelStatus &channel);
		};

		class BaseDigiDriver {
			virtual void initCard() = 0; //AH=00
			virtual void uninstallDriver() = 0; //AH=02
			virtual void restoreOriginalIRQ() = 0; //AH=04
			virtual void playVOC(const char *vocData, uint32 dataSize, uint8 numLoops) = 0; //AH=06
			virtual void stopPlaying() = 0; //AH=08
			virtual void setFreqOffset(uint16 offs) = 0; //AH=0a
			virtual void nop() = 0; //AH=0c
			virtual uint8 getPlayStatus() = 0; //AH=0e
			virtual const uint8 *getPtrPlayStatus() = 0; //AH=10


		};

		class SBDigiDriver : public BaseDigiDriver {
		public:
			SBDigiDriver(Audio::Mixer *_mx);
			virtual ~SBDigiDriver();
			virtual void initCard(); //AH=00
			virtual void uninstallDriver(); //AH=02
			virtual void restoreOriginalIRQ(); //AH=04
			virtual void playVOC(const char *vocData, uint32 dataSize, uint8 numLoops); //AH=06
			virtual void stopPlaying(); //AH=08
			virtual void setFreqOffset(uint16 offs); //AH=0a
			virtual void nop(); //AH=0c
			virtual uint8 getPlayStatus(); //AH=0e
			virtual const uint8 *getPtrPlayStatus(); //AH=10
		private:
			Audio::Mixer *_mixer;
			uint8 _playState;
			Audio::SoundHandle _handleSfx;
			uint8 _sfxId;
			Audio::SoundHandle _handleSpeech;
			uint8 _speechId;

			Common::MemoryReadStream *_mstr;
			Audio::AudioStream *_astr;

			uint8 _lastLoopCount;
			char *_lastVocData;

		};
		bool _isAudioInited;
		AdlibMusicDriver *_adlib;
		SBDigiDriver *_sbdigi;


	};
}
#endif

