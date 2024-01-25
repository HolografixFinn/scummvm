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

#ifndef COMET_STRUCTS_H
#define COMET_STRUCTS_H

#include "common/scummsys.h"

namespace Cometengine {

#include "common/pack-start.h"
	struct Actor {
		uint16 pivotX;
		uint16 pivotY;
		uint16 offsetWalkAnimation;
		uint16 animationType;
		uint16 facingDirection;
		bool isKillable;
		int8 resourceIdx;
		uint16 animationIdx;
		//
		uint16 frameToDraw;
		uint16 currentAnimationFactor;
		uint16 numFrames;
		int8 fixedAnimationFrame;
		uint8 halfWidth;
		uint8 height;
		uint8 intersectionType;
		uint8 intersectionIdx;
		uint8 lastIntersectionType;
		uint16 life;
		uint16 speechColor;
		int16 textPivotX;
		int16 textPivotY;
		uint16 whereToWalk;
		uint16 destinationX;
		uint16 destinationY;
		uint16 prevDestinationX;
		uint16 prevDestinationY;
		uint16 drawAreaMinX;
		uint16 drawAreaMaxX;
		uint8 drawAreaMinY;
		uint8 drawAreaMaxY;
		uint8 isVisible;
		uint8 _dummy;
	} PACKED_STRUCT;
#include "common/pack-end.h"

#include "common/pack-start.h"
	struct RoomExit {
//		uint16 direction;
		uint8 direction;
		uint8 pak;
		uint8 room;
		uint8 unk;
		uint16 x1;
		uint16 x2;
	} PACKED_STRUCT;
#include "common/pack-end.h"

#include "common/pack-start.h"
	struct RoomScript {
		const char *start;
		const char *current;
		int8 actorIdx;
		uint8 flags;
		uint16 syncCounter;
		uint16 loopCounter;
		uint16 left;
		uint16 top;
		uint16 right;
		uint16 bottom;
	} PACKED_STRUCT;
#include "common/pack-end.h"

#include "common/pack-start.h"
	struct Resource {
		uint8 type;
		uint8 fileIdx;
		uint8 *data;
	}PACKED_STRUCT;
#include "common/pack-end.h"

#include "common/pack-start.h"
	struct Hotspot {
		uint16 objIdx;
		bool isActive;
		uint8 objType;
		uint16 x;
		uint16 y;
	}PACKED_STRUCT;
#include "common/pack-end.h"

#include "common/pack-start.h"
	struct DisplayListElement {
		uint8 yPos;
		uint8 idx;
	}PACKED_STRUCT;
#include "common/pack-end.h"

#include "common/pack-start.h"
	struct StageElement {
		uint8 halfLeft;
		uint8 top;
		uint8 halfRight;
		uint8 bottom;
	}PACKED_STRUCT;
#include "common/pack-end.h"

} // End of namespace Comet

#endif
