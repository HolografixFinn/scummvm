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

#ifndef COMET_TEXTMANAGER_H
#define COMET_TEXTMANAGER_H
#include "common/scummsys.h"


namespace Cometengine {
class CometEngine;
	class TextManager {
	public:
		TextManager(CometEngine *vm); //, uint8 langId);
		~TextManager();
		void initResources();
		void loadSystemAndObjects();
		uint16 calcStringWidth(const char *string);
		void setFontDataAndColor(uint8 *data, uint8 textColor);
		void drawString(uint16 x, uint16 y, uint8 *videoBuffer, const char *string);
		void drawOutlinedString(uint16 x, uint16 y, const char *string,uint8 stringColor,uint8 borderColor);

		void actorSaySentence(uint8 actorIdx, uint16 sentenceIdx, uint8 color);
		uint8 getSentence(uint8 block, uint16 sentenceIdx, char *buffer);
		const char *getSentencePtr(uint16 idx, const char *stringsBlock);

		//for encapsulation
		const char *getObjNames();
//		const char *sysMessages();
		char *getMultiChoiceText();
		bool isTextDisplayed();
		void sayDescription(uint16 idx, uint16 duration, const char * stringsBlock);
		void hideText();
		void setTextDisplayed(bool val);
		void showText();
		//		void setBlock(uint8 block);
		//		uint8 getBlock();
		int16 getPrevAnimationOrActor();
		void setPrevAnimationOrActor(int16 v);
		int16 getPrevFrameToDraw();
		void  setPrevFrameToDraw(int16 v);
		int16 getPrevFixedFrame();
		void setPrevFixedFrame(int16 v);
		int8 getSpeakingActor();
		uint16 getChoice();
		void setChoice(uint16 choiceIdx);
		void handleOnScreenText();
		uint16 getTextRemainingFrame();
		uint16 getTextDuration();
		void setTextRemainingFrame(uint16 v);
		void setTextDuration(uint16 p);
		const char *printDecimalNumber(int16 num);
		const char *getSysMessages();
		struct Choice {
			uint16 sentenceIdx;
			const char *sentence;
			const char *scriptData;
		};
		void setMultiChoiceIntroSentence(uint16 sentenceIdx);
		void setMultiChoiceTextCoords(uint16 x, uint16 y);
		void setMultiChoiceNum(uint8 num);
		Choice *getChoice(uint8 idx);
		void setMultiChoiceIntroSentenceData(const char *data);
		void toggleEvenFrame();
		bool hasLanguageFile(uint8 langid);
			//		uint16 getCurrentLanguageId();
		void hideDialogue();
		void showDialogue();
		void handleMultiChoice();

	protected:
	private:
		uint16 _choice;
		int16 _prevAnimationOrActor;
		int16 _prevFrameToDraw;
		int16 _prevFixedFrame;
		int8 _speakingActor;
		CometEngine *_vm;
		char *_systemMessages;
		char *_objectsName;
		//uint8 _currentLangId;
		char _textFilename[11];
		char _langsFirstChars[5];
		void decodeFromFile(char *buffer, uint8 blockIdx, uint32 bufferSize);
		void decodeTextBuffer(char *data, uint32 size, uint32 offset);

		//font/strings data
		uint8 *_fontData;
		uint8 *_charsGraphics;
		uint8 *_charsData;
		uint8 _charsHeight;
		uint16 _fontSheetWidth;
		uint8 _spaceCharWidth;
		uint8 _interCharsWidth;
		uint8 _currStringColor;
		uint16 _stringXPos;
		char *_multiChoiceText;
		bool _isTextDisplayed;
		uint8 _sentenceColor;
		uint16 _currSentenceIdx;
		uint16 _textOnScreenRemainingFrames;
		uint16 _textOnScreenDuration;

		void prepareString(const char *str);
		const char *_currString;
		uint16 _maxStringHalfWidth;
		bool _isMoreThan3Lines;
		const char *getNextString(const char *str);
		uint16 _textLinesHeight;
		const char *_nextString;
//		uint8 _textDurationScale;
//		uint8 _textBlock;
		bool _skipSpeechBox;
		int8 _textPadding;
		int16 printStringLines(const char *string,int16 x,int16 y,uint8 color,bool isCentered);
		void drawColoredString(const char *string, int16 x, int16 y, uint8 color);
		uint8 _textSingleLineHeight;
		char _decimalConversionBuffer[32];
		bool printDigit(uint16 &val, uint16 division, char * &ptr, bool isPrintEmpty);
		void drawMultiChoice();
		uint16 _multiChoiceIntroSentence;
		const char *_multiChoiceIntroSentencePtr;
		uint16 _multiChoiceTextX;
		uint16 _multiChoiceTextY;
		uint16 _multiChoiceNumElems;
		uint8 _currentChoiceColor;
		int8 _currentChoiceColorStep;
		Choice _choices[6];
		bool _isEvenFrame;
	};
}
#endif

