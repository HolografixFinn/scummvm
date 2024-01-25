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

#ifndef COMET_ARCHIVESMANAGER_H
#define COMET_ARCHIVESMANAGER_H
#include "common/scummsys.h"


namespace Cometengine {
	class CometEngine;

	class ArchivesManager {
	public:
		ArchivesManager(CometEngine *vm);
		~ArchivesManager();
		void getFile(const char *archiveName, uint16 fileNum, uint8 *destBuffer, uint32 *decomSize=nullptr);
		uint8 * allocateAndGetFile(const char *archiveName, uint16 fileNum, uint32 *decomSize = nullptr);
		void readFileBlock(const char *filename, uint8 block, char *buffer, uint32 size);
		bool checkValidSubfile(const char *archiveName, uint16 fileNum);

	private:
		CometEngine *_vm;
		void decompress(uint8 type, uint8 *src, uint8 *dest, uint32 decompressedSize);
		unsigned char *compressed;
		unsigned char *uncompressed;

		struct HuffmanSymbol {
			uint16 huffCode;
			uint8 codeLen;
			uint8 symbol;
			HuffmanSymbol() :huffCode(0), codeLen(0), symbol(0) {}
		};
		struct HuffmanTreeNode {
			uint8 leftVal;
			uint8 rightVal;
			uint8 flags;
		};
		struct ArchivedFile {
			uint16 index;
			uint32 offset;
			uint32 relocationTablesSize;
			uint32 compressedSize;
			uint32 decompressedSize;
			uint16 compressionType;
			uint16 relativeOffsetToCompressedData;
			uint32 offsetToCompressedData;

			ArchivedFile() :index(0), offset(0), relocationTablesSize(0), compressedSize(0), decompressedSize(0), compressionType(0), relativeOffsetToCompressedData(0), offsetToCompressedData(0) {}


		};
		ArchivedFile subfiles[200];
		uint16 numSubfiles;

		HuffmanSymbol symbols[300];
		HuffmanTreeNode trees[10000];
		HuffmanTreeNode *tree1;
		HuffmanTreeNode *tree2;
		HuffmanTreeNode *tree3;

		void readHuffmanTree(HuffmanTreeNode * &tree, uint16 maxElements);
		uint32 readBits(uint16 n_bits);
		uint8 bits_left;
		unsigned char curr_byte;
		uint8 *currCompressedPtr;

		uint16 reverseBitsInWord(uint16 val);
		uint8 decodeHuffman(HuffmanTreeNode * huffTree);

		void addExtensionToFile(char *dest, const char *filename, const char *extension);
	};
}
#endif

