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
#include "common/events.h"

#ifndef COMET_MOUSEMANAGER_H
#define COMET_MOUSEMANAGER_H

namespace Cometengine {
class CometEngine;
class MouseManager {
public:
	MouseManager(CometEngine *vm);
	~MouseManager();
	void loadCursorsGraphics(uint8 *dataPtr, uint8 numCursors);
	bool isInitialized() {
		return _initialized;
	};

	const uint8 *setMouseCursor(int idx, const uint8* ptr = nullptr);
	uint8* loadedCursors[10];
	static const uint8* defaultCursors[];

	void setMouseVisibility(uint32 check);
	bool updateMouseStatus();

	struct mouseTarget {
		uint16 left;
		uint16 top;
		uint16 right;
		uint16 bottom;
		int16 ID;
	};
	enum class Targets : uint8 {
		MAINMENU = 0,
		DISKMENU,
		OBJSMENU,
		FILESMENU,
		DIARY,
		OPTIONSMENU
	};
	int16 getCurrentTarget(Targets tag, uint8 numElems, int16 notFound);
	mouseTarget getTarget(Targets tag, uint16 idx);
	int16 getCurrX();
	int16 getCurrY();
	bool getLeftBut();
	bool getRightBut();
	void warpMouse(int16 x, int16 y);
	void warpMouseOffset(int16 x, int16 y);
private:
	static const mouseTarget mainMenuMouseTarget[];
	static const mouseTarget diskMenuMouseTarget[];
	static const mouseTarget objsMenuMouseTarget[];
	static const mouseTarget filesMenuMouseTarget[];
	static const mouseTarget diaryMouseTarget[];
	static const mouseTarget optionsMenuMouseTarget[];

	static const mouseTarget *mouseTargets[];

	CometEngine *_vm;
	Common::EventManager* _evtMgr;
	bool _initialized;
	const uint8* _cursorData;
	bool _mouseEnabled;
	Common::Point _currPos;
	bool _leftButStatus;
	bool _rightButStatus;

	/*
	uint8 _cursorWidth;
	uint8 _cursorHeight;
	uint8* _mouseBackgroundBuffer;	//latest video buffer used to draw mouse cursor
	uint32 _cursorOffset;		//offset of cursor position into the video buffer
	uint32 _prevCursorOffset;	//previous offset of cursor position into the video buffer 
	int16 _prevMouseDrawX;
	int16 _prevMouseDrawY;
	int16 _mouseDrawX;
	int16 _mouseDrawY;
	int16 _mouseX;
	int16 _mouseY;
	*/
	uint8 _tmpCursorBuffer[256];			//temp buffer used to decompress the whole cursor image, when it has to be drawn partially on screen
	uint8 _customCursorData[256];
/*
	uint8 _cursorBackgroundData[256]; // the original portion of graphics under the mouse cursor
	bool _isFrameDrawing;			  // some sort of "sync" flag, to avoid drawing when not needed/allowed
*/
	uint8 *_cursorGraphics;
	/*
	uint8 *_cursor0;
	uint8 *_cursor1;
	uint8 *_cursor2;
	uint8 *_cursor3;
	uint8 *_cursor4;
	uint8 *_cursor5;
	uint8 *_cursor6;
	*/
	static const uint8 defaultCursor0[];
	static const uint8 defaultCursor1[];
	static const uint8 defaultCursor2[];
	static const uint8 defaultCursor3[];
	static const uint8 defaultCursor4[];
	static const uint8 defaultCursor5[];
	static const uint8 defaultCursor6[];
	static const uint8 defaultCursor7[];
	static const uint8 defaultCursor8[];

	void decompressMouse(const uint8* src);
	/*
	void deleteMouseCursor();
	void saveCursorBackground(uint8* videobuffer);
	void drawMouse();
	void mousePreDraw();
	void mousePostDraw();
	*/
};
inline int16 MouseManager::getCurrX() {
	return _currPos.x;
}
inline int16 MouseManager::getCurrY() {
	return _currPos.y;
}
inline bool MouseManager::getLeftBut() {
	return _leftButStatus;
}
inline bool MouseManager::getRightBut() {
	return _rightButStatus;
}
} // namespace Cometengine
#endif
