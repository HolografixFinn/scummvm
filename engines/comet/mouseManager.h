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
#ifndef COMET_MOUSEMANAGER_H
#define COMET_MOUSEMANAGER_H

namespace Cometengine {
class CometEngine;
class MouseManager {
public:
	MouseManager(CometEngine *vm);
	~MouseManager();
	void setCursorsGraphics(uint8 *dataPtr);
	bool isInitialized() {
		return _initialized;
	};

private:
	CometEngine *_vm;
	bool _initialized;
	uint8 *_cursorGraphics;
	uint8 *_cursor0;
	uint8 *_cursor1;
	uint8 *_cursor2;
	uint8 *_cursor3;
	uint8 *_cursor4;
	uint8 *_cursor5;
	uint8 *_cursor6;
	static const uint8 defaultCursor0[];
	static const uint8 defaultCursor1[];
	static const uint8 defaultCursor2[];
	static const uint8 defaultCursor3[];
	static const uint8 defaultCursor4[];
	static const uint8 defaultCursor5[];
	static const uint8 defaultCursor6[];
	static const uint8 defaultCursor7[];
	static const uint8 defaultCursor8[];
	static const uint8* defaultCursors[];
};
} // namespace Cometengine
#endif
