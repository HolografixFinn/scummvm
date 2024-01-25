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

#include "comet/textManager.h"
#include "comet/comet.h"

namespace Cometengine {
TextManager::TextManager(CometEngine *vm) :                                                                                                                                //, uint8 langId) :
											_vm(vm), _systemMessages(nullptr), _objectsName(nullptr), _langsFirstChars(), _textFilename("T.CC4"), _isTextDisplayed(false), //_currentLangId(langId),
											_fontData(nullptr), _charsGraphics(nullptr), _charsData(nullptr), _spaceCharWidth(2), _interCharsWidth(1), _currStringColor(0),
											_charsHeight(0), _fontSheetWidth(0), _stringXPos(0), _choice(0), _prevAnimationOrActor(-1), _prevFixedFrame(0), _prevFrameToDraw(0), _speakingActor(0),
											_multiChoiceText(nullptr),
											_sentenceColor(0), _currSentenceIdx(0), _textOnScreenRemainingFrames(0), _textOnScreenDuration(0), _currString(nullptr), _maxStringHalfWidth(0), _isMoreThan3Lines(false), _textLinesHeight(0),
											_nextString(nullptr),
											// _textDurationScale(0), //_textBlock(0),
											_skipSpeechBox(false), _textPadding(4), _textSingleLineHeight(8), _decimalConversionBuffer(),
											_multiChoiceIntroSentence(0), _multiChoiceTextX(0), _multiChoiceTextY(0), _multiChoiceNumElems(0), _choices(), _multiChoiceIntroSentencePtr(nullptr),
											_currentChoiceColor(0x4f), _currentChoiceColorStep(-1), _isEvenFrame(true) {
	memset(_langsFirstChars, 0, 5);
//	bool isCd=(desc->flags&&
	if (_vm->isCD() || _vm->_gameState.selectedLanguageID ==0) {
		_langsFirstChars[0] = 'T';
	}
	if (_vm->isCD() || _vm->_gameState.selectedLanguageID == 1) {
		_langsFirstChars[1] = 'E';
	}
	if (_vm->isCD() || _vm->_gameState.selectedLanguageID == 2) {
		_langsFirstChars[2] = 'S';
	}
	if (_vm->isCD() || _vm->_gameState.selectedLanguageID ==3) {
		_langsFirstChars[3] = 'D';
	}
	if (_vm->isCD() || _vm->_gameState.selectedLanguageID == 4) {
		_langsFirstChars[4] = 'I';
	}

	// { 'E',0,0,0,'I',0,0 };
}
TextManager::~TextManager() {
	if (_systemMessages != nullptr) {
		delete[] _systemMessages;
		_systemMessages = nullptr;
	}
	if (_objectsName != nullptr) {
		delete[] _objectsName;
		_objectsName = nullptr;
	}
	if (_multiChoiceText != nullptr) {
		delete[] _multiChoiceText;
		_multiChoiceText = nullptr;
	}
}
void TextManager::initResources() {
	//ok
	_systemMessages = new char[1000];
	_objectsName = new char[3000];
	_multiChoiceText = new char[1000];
}

void TextManager::loadSystemAndObjects() {
	//ok
	if (_langsFirstChars[_vm->_gameState.selectedLanguageID] == 0) {
		_vm->_gameState.selectedLanguageID = _vm->_gmMgr->defaultLanguage;
		return;
	}
	_isTextDisplayed = false;
	_textFilename[0] = _langsFirstChars[_vm->_gameState.selectedLanguageID];
	decodeFromFile(_systemMessages, 0, 1000);
	decodeFromFile(_objectsName, 1, 3000);
}
void TextManager::decodeFromFile(char *buffer, uint8 blockIdx, uint32 bufferSize) {
	this->_vm->_archMgr->readFileBlock(_textFilename, blockIdx, buffer, bufferSize);
	uint32 offset = READ_LE_UINT32(buffer);
	decodeTextBuffer(buffer + offset, bufferSize - offset, 0);
}
void TextManager::decodeTextBuffer(char *data, uint32 size, uint32 offset) {
	char *ptr = data;
	uint8 decodeValue = 0x54;
	for (uint32 i = 0; i < size; i++) {

		*ptr -= (decodeValue * (i + offset + 1)) & 0x00ff;
		ptr++;
	}
}
uint16 TextManager::calcStringWidth(const char *string) {
	uint16 width = 0;
	while (*string != 0) {
		char c = *string++;
		uint16 charWidth = READ_BE_UINT16(this->_charsData + (c * 2)); //dunno why this is big endian
		charWidth = (charWidth >> 12) & 0x0f;
		if (charWidth == 0) {
			charWidth += _spaceCharWidth;
		} else {
			charWidth += _interCharsWidth;
		}
		width += charWidth;
	}
	return width;
}
void TextManager::setFontDataAndColor(uint8 *data, uint8 textColor) {
	//ok
	_fontData = data;
	uint16 numChars = READ_LE_UINT16(data);
	data += 2;
	_charsHeight = (uint8)(*data++);
	_fontSheetWidth = (uint8)(*data++);
	if (_fontSheetWidth == 0) {
		_fontSheetWidth = READ_LE_UINT16(data);
	}
	data += 2;
	uint16 offset = READ_BE_UINT16(data); //dunno why this value is BigEndian
	data += 2;
	_charsGraphics = data;
	_charsData = _fontData + offset - (numChars * 2);
	_currStringColor = textColor;
}
void TextManager::drawString(uint16 x, uint16 y, uint8 *videoBuffer, const char *string) {
	_stringXPos = x;
	uint8 *dest = videoBuffer;
	while (*string != 0) {
		byte c = *string++;
		uint16 charDescriptor = READ_BE_UINT16(_charsData + (c * 2)); //dunno why this is big endian
		uint16 tmp = 0;
		uint8 charWidth = (charDescriptor >> 12) & 0x0f;
		if (charWidth != 0) {
			charDescriptor = charDescriptor & 0x0fff;
			uint8 *data = this->_charsGraphics + (charDescriptor / 8);
			char charBitmask = 0x80 >> (charDescriptor & 0x07);
			uint16 currXPos = _stringXPos;
			uint16 currYPos = y;
			for (uint8 i = 0; i < _charsHeight; i++) {
				uint32 destOffs = _vm->_gMgr->_rowsOffsets[currYPos++] + currXPos;
				uint8 currBitmask = charBitmask;
				uint8 currData = *data;
				uint8 dataOffs = 0;
				for (uint8 j = 0; j < charWidth; j++) {
					if (currData & currBitmask) {
						dest[destOffs] = _currStringColor;
					}
					destOffs++;
					if (currBitmask == 1) {
						currBitmask = 0x80;
						dataOffs++;
						currData = *(data + dataOffs);
					} else {
						currBitmask = currBitmask >> 1;
					}
				}
				data += _fontSheetWidth;
			}
			tmp = _stringXPos;
			tmp += charWidth;
		} else {
			tmp = _spaceCharWidth + _stringXPos;
		}
		_stringXPos = tmp + 1;
	}
}
void TextManager::sayDescription(uint16 idx, uint16 duration, const char *stringsBlock) {
	_speakingActor = 0;
	_sentenceColor = 0x15;
	_currSentenceIdx = idx;
	prepareString(getSentencePtr(idx, stringsBlock));
	_textOnScreenDuration = _textOnScreenRemainingFrames = duration;
	_isTextDisplayed = true;
	_vm->_spMgr->setDescription(true);
}
const char *TextManager::getSentencePtr(uint16 idx, const char *stringsBlock) {
	uint32 offset = READ_LE_UINT32(stringsBlock + (idx * 4));
	return stringsBlock + offset + 1;
}
void TextManager::prepareString(const char *str) {
	uint16 currStringWidth = 0;
	uint16 currLinesNum = 0;
	_currString = str;
	_textLinesHeight = 0;
	_maxStringHalfWidth = 0;
	_isMoreThan3Lines = false;
	uint8 minDuration = _vm->isCD() ? 100 : 50;
	_textOnScreenRemainingFrames = 0;
	while (*str != '*') {
		currStringWidth = calcStringWidth(str);
		_textOnScreenRemainingFrames += currStringWidth / 4;
		if (_textOnScreenRemainingFrames < minDuration) {
			_textOnScreenRemainingFrames = minDuration;
		}
		if (currStringWidth > _maxStringHalfWidth) {
			_maxStringHalfWidth = currStringWidth;
		}
		str = getNextString(str);
		if (currStringWidth != 0) {
			_textLinesHeight++;
		}
		currLinesNum++;
		if (currLinesNum == 3 && *str != '*') {
			_isMoreThan3Lines = true;
			break;
		}
	}
	_nextString = str;
	_maxStringHalfWidth = _maxStringHalfWidth / 2;
	_textLinesHeight = _textLinesHeight * _textSingleLineHeight;
	if (_vm->_gameState.textDurationScale == 0) {
		_textOnScreenRemainingFrames = _textOnScreenRemainingFrames / 2;
	}
	if (_vm->_gameState.textDurationScale == 2) {
		_textOnScreenRemainingFrames += _textOnScreenRemainingFrames / 2;
	}
	_textOnScreenDuration = _textOnScreenRemainingFrames;
}
const char *TextManager::getNextString(const char *str) {
	while (*str++ != 0) {
	}
	return str;
}
uint8 TextManager::getSentence(uint8 block, uint16 sentenceIdx, char *buffer) {
	Common::File f;
	if (!f.open(_textFilename)) {
		// handle failure to find/open file
		return 0;
	}
	f.seek((block)*4, SEEK_SET);
	uint32 blockOffset = f.readUint32LE();
	f.seek(blockOffset, SEEK_SET);
	uint32 firstOfBlockOffs = f.readUint32LE();
	f.seek(blockOffset + (sentenceIdx * 4), SEEK_SET);
	uint32 sentenceOffset = f.readUint32LE();
	uint32 nextSentenceOffset = f.readUint32LE();
	f.seek(blockOffset + sentenceOffset, SEEK_SET);
	uint32 len = nextSentenceOffset - sentenceOffset + 1;
	f.read(buffer, len);
	f.close();
	decodeTextBuffer(buffer, len, sentenceOffset - firstOfBlockOffs);
	return len;
}

void TextManager::actorSaySentence(uint8 actorIdx, uint16 sentenceIdx, uint8 color) {
	_speakingActor = actorIdx;
	_currSentenceIdx = sentenceIdx;
	WRITE_LE_UINT32(_multiChoiceText, 4);
	getSentence(_vm->_gameState.textChapterID + 3, _currSentenceIdx, _multiChoiceText + 4);
	if (!_vm->isCD() || _vm->_gameState.speechOptions == 0) {
		prepareString(getSentencePtr(0, _multiChoiceText));
	} else if (_vm->_gameState.speechOptions == 2) {
		_vm->_spMgr->startSpeech(_currSentenceIdx);
	} else if (_vm->_gameState.speechOptions == 1) {
		prepareString(getSentencePtr(0, _multiChoiceText));
		_vm->_spMgr->startSpeech(_currSentenceIdx);
	}
	_isTextDisplayed = true;
	_sentenceColor = color;
}
void TextManager::handleOnScreenText() {
	if (_isTextDisplayed) {
		showDialogue();
	}
	if (_vm->_gmMgr->waitingForEnter()) {
		handleMultiChoice();
	}
}
void TextManager::handleMultiChoice() {
	uint8 currChoice = _choice;
	uint8 currMovFlags = _vm->_gmMgr->getCurrentMovementFlags();
	if (currMovFlags & GameManager::kUpFlag) {
		if (currChoice > 0) {
			_choice -= 1;
			_vm->_gmMgr->waitForNoInput();
		}
	}
	if (currMovFlags & GameManager::kDownFlag) {
		if (currChoice < _multiChoiceNumElems - 1) {
			_choice += 1;
			_vm->_gmMgr->waitForNoInput();
		}
	}
	if (currChoice == _vm->_txtMgr->getChoice()) {
		_currentChoiceColor = 0x4f;
	}
	drawMultiChoice();
}
void TextManager::drawMultiChoice() {
	uint16 x = _multiChoiceTextX;
	uint16 y = _multiChoiceTextY;
	uint8 color3 = 0;
	uint8 color = _vm->_gmMgr->getActor(0)->speechColor;
	if (color == 0x19) {
		color3 = 0x16;
	} else {
		color3 = color;
	}
	if (_multiChoiceIntroSentence != 0xffff) {
		prepareString(_multiChoiceIntroSentencePtr);
		_vm->_gMgr->drawSpeechBox(x - _textPadding, y - _textPadding, x + _textPadding + (_maxStringHalfWidth * 2), y + _textLinesHeight);
		y = printStringLines(_multiChoiceIntroSentencePtr, x, y, color, true); //ultimo parametro era -1 invec di true
		x += 16;
		y += _textSingleLineHeight;
	}
	for (uint8 i = 0; i < _multiChoiceNumElems; i++) {
		Choice *choice = &_choices[i];
		uint8 color2 = color3;
		if (i == _choice) {
			if (color == 0x19) {
				color2 = _currentChoiceColor;
				_currentChoiceColor += _currentChoiceColorStep;
				if (_currentChoiceColor > 0x19) {
					_currentChoiceColor = 0x19;
					_currentChoiceColorStep = -1;
				}
				if (_currentChoiceColor < 0x16) {
					_currentChoiceColor = 0x16;
					_currentChoiceColorStep = 1;
				}

			} else {
				if (_isEvenFrame) {
					color2 = color;
				} else {
					color2 = 0x9f;
				}
			}
		}
		prepareString(choice->sentence);
		_vm->_gMgr->drawSpeechBox(x - _textPadding, y - _textPadding, x + _textPadding + (_maxStringHalfWidth * 2), y + _textLinesHeight);
		y = printStringLines(choice->sentence, x, y, color2, 1);
		y += _textSingleLineHeight;
	}
}

void TextManager::showDialogue() {
	Actor *actor = _vm->_gmMgr->getActor(_speakingActor);
	int16 textCenterX, textY;
	if (actor->textPivotX != -1) {
		textCenterX = actor->textPivotX;
		textY = actor->textPivotY;
	} else {
		textCenterX = actor->pivotX;
		textY = actor->pivotY - (_textLinesHeight + 50);
	}
	if (!_skipSpeechBox) {
		_vm->_gMgr->drawSpeechBox(textCenterX - (_maxStringHalfWidth + _textPadding), textY - _textPadding, textCenterX + (_maxStringHalfWidth + _textPadding), textY + _textLinesHeight);
	}
	printStringLines(_currString, textCenterX + 1, textY, _sentenceColor, false);
	_textOnScreenRemainingFrames--;
	if (_textOnScreenRemainingFrames <= 0) {
		_isTextDisplayed = _isMoreThan3Lines;
		if (!_isTextDisplayed) {
			hideDialogue();
		} else {
			prepareString(_nextString);
		}
	}
}
void TextManager::setTextDisplayed(bool val) {
	_isTextDisplayed = val;
}
void TextManager::hideDialogue() {
	_isTextDisplayed = false;
	_vm->_spMgr->setDescription(false);
	_vm->_gmMgr->unsetWaitForEnter();
}
void TextManager::drawColoredString(const char *string, int16 x, int16 y, uint8 color) {
	_vm->_gmMgr->updateFontDataAndColor(color);
	drawString(x, y, _vm->_gMgr->getBackbuffer(), string);
}

int16 TextManager::printStringLines(const char *string, int16 px, int16 py, uint8 color, bool isCentered) {
	uint16 halfWidth = 1; //TODO is it right to init this to 1???
	int8 printedLines = 0;
	int16 left = px;
	int16 right;
	int16 y = py;
	if (y < 3) {
		y = 3;
	}
	while (*string != '*') {
		if (!isCentered) {
			uint16 stringWidth = calcStringWidth(string);
			halfWidth = stringWidth / 2;
			left = px - halfWidth;
			right = px + halfWidth;
			if (left < 3) {
				left = 3;
			}
			if (right > 316) {
				left -= right - 316;
			}
		}
		if (halfWidth != 0) {
			drawColoredString(string, left, y, color);
			y += _textSingleLineHeight;
		}
		string = getNextString(string);
		printedLines++;
		if (printedLines == 3) {
			break;
		}
	}
	return y;
}

void TextManager::drawOutlinedString(uint16 x, uint16 y, const char *string, uint8 stringColor, uint8 borderColor) {
	uint8 *videBuffer = _vm->_gMgr->getBackbuffer();
	_vm->_gmMgr->updateFontDataAndColor(borderColor);
	drawString(x + 1, y + 1, videBuffer, string);
	drawString(x + 1, y - 1, videBuffer, string);
	drawString(x + 1, y, videBuffer, string);
	drawString(x - 1, y, videBuffer, string);
	drawString(x, y + 1, videBuffer, string);
	drawString(x, y - 1, videBuffer, string);
	drawString(x - 1, y + 1, videBuffer, string);
	drawString(x - 1, y - 1, videBuffer, string);
	_vm->_gmMgr->updateFontDataAndColor(stringColor);
	drawString(x, y, videBuffer, string);
}
const char *TextManager::printDecimalNumber(int16 num) {
	char *ptr = _decimalConversionBuffer;
	if (num < 0) {
		*ptr = '-';
		ptr++;
		num = -num;
	}
	bool b = false;
	uint16 unum = (uint16)num;
	b = printDigit(unum, 10000, ptr, b);
	b = printDigit(unum, 1000, ptr, b);
	b = printDigit(unum, 100, ptr, b);
	b = printDigit(unum, 10, ptr, b);
	*ptr++ = 0x30 + unum;
	*ptr = 0;
	return _decimalConversionBuffer;
}
bool TextManager::printDigit(uint16 &val, uint16 division, char *&ptr, bool isPrintEmpty) {
	uint8 v = val / division;
	val = val % division;
	if (v != 0 || isPrintEmpty) {
		isPrintEmpty = true;
		*ptr = 0x30 + v;
		ptr++;
	}
	return isPrintEmpty;
}

//for encapsulation
const char *TextManager::getObjNames() {
	return _objectsName;
}
/*
	const char *TextManager::sysMessages() {
		return _systemMessages;
	}
	*/
char *TextManager::getMultiChoiceText() {
	return _multiChoiceText;
}
bool TextManager::isTextDisplayed() {
	return _isTextDisplayed;
}
void TextManager::hideText() {
	_isTextDisplayed = false;
}
void TextManager::showText() {
	_isTextDisplayed = true;
}
/*
	void TextManager::setBlock(uint8 block) {
		_textBlock = block;
	}
	uint8 TextManager::getBlock() {
		return _textBlock;
	}
	*/
int16 TextManager::getPrevAnimationOrActor() {
	return _prevAnimationOrActor;
}
void TextManager::setPrevAnimationOrActor(int16 v) {
	_prevAnimationOrActor = v;
}

int16 TextManager::getPrevFrameToDraw() {
	return _prevFrameToDraw;
}
void TextManager::setPrevFrameToDraw(int16 v) {
	_prevFrameToDraw = v;
}
int16 TextManager::getPrevFixedFrame() {
	return _prevFixedFrame;
}
void TextManager::setPrevFixedFrame(int16 v) {
	_prevFixedFrame = v;
}

int8 TextManager::getSpeakingActor() {
	return _speakingActor;
}
uint16 TextManager::getChoice() {
	return _choice;
}
uint16 TextManager::getTextRemainingFrame() {
	return _textOnScreenRemainingFrames;
}
uint16 TextManager::getTextDuration() {
	return _textOnScreenDuration;
}
void TextManager::setTextDuration(uint16 p) {
	_textOnScreenDuration = p;
}
void TextManager::setTextRemainingFrame(uint16 v) {
	_textOnScreenRemainingFrames = v;
}
const char *TextManager::getSysMessages() {
	return _systemMessages;
}
void TextManager::setChoice(uint16 choiceIdx) {
	_choice = choiceIdx;
}
void TextManager::setMultiChoiceIntroSentence(uint16 sentenceIdx) {
	_multiChoiceIntroSentence = sentenceIdx;
}
void TextManager::setMultiChoiceTextCoords(uint16 x, uint16 y) {
	_multiChoiceTextX = x;
	_multiChoiceTextY = y;
}
void TextManager::setMultiChoiceNum(uint8 num) {
	_multiChoiceNumElems = num;
}
TextManager::Choice *TextManager::getChoice(uint8 idx) {
	return &_choices[idx];
}
void TextManager::setMultiChoiceIntroSentenceData(const char *data) {
	_multiChoiceIntroSentencePtr = data;
}
void TextManager::toggleEvenFrame() {
	_isEvenFrame = !_isEvenFrame;
}
bool TextManager::hasLanguageFile(uint8 langid) {
	return (_langsFirstChars[langid] != 0);
}

/*
	uint16 TextManager::getCurrentLanguageId() {
		return _currentLangId;
	}
	*/

} // namespace Cometengine
