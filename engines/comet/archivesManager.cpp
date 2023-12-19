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

#include"comet/archivesManager.h"
#include "common/file.h"

#define COMET_COMPRESSION_7BITS_COMPRESSION_FLAG 0x02
#define OMET_COMPRESSION_HUFF_COMPRESSED_IMMEDIATES 0x04

#define COMET_COMPRESSION_FLAG_IS_LEFT_NODE 0x01
#define COMET_COMPRESSION_FLAG_IS_RIGHT_NODE 0x02

namespace Cometengine {
	void ArchivesManager::decompress(uint8 typeFlags, char *src, char *dest, uint32 decompressedSize) {
		char *currDecompressedPtr = nullptr;
		uint32 backOffset = 0;
		uint32 howMany = 0;

		currDecompressedPtr = dest;

		currCompressedPtr = src;
		curr_byte = 0;
		bits_left = 0;

		unsigned char bitsSize = 6;
		unsigned char bitsMask = 0x3F;
		unsigned char c0a = 2;

		if (typeFlags & COMET_COMPRESSION_7BITS_COMPRESSION_FLAG) {
			bitsSize = 7;
			bitsMask = 0x7F;
		}
		if (typeFlags & OMET_COMPRESSION_HUFF_COMPRESSED_IMMEDIATES) {
			c0a = 3;
			readHuffmanTree(tree3, 0x100);
		}
		readHuffmanTree(tree1, 0x40);
		readHuffmanTree(tree2, 0x40);

		uint32 decompressedBytes = 0;

		do {


			if (readBits(1) == 1) {
				if ((typeFlags & OMET_COMPRESSION_HUFF_COMPRESSED_IMMEDIATES)) {
					decompressedBytes++;
					*currDecompressedPtr = decodeHuffman(tree3);
					currDecompressedPtr++;

					//				printf("Grave errore col tree3 inatteso\n");
				}
				else {
					decompressedBytes++;
					*currDecompressedPtr = readBits(8);
					currDecompressedPtr++;

				}
			}
			else {
				uint32 app = readBits(bitsSize);
				backOffset = (decodeHuffman(tree2) << bitsSize) + app + 1;

				uint32 app2 = decodeHuffman(tree1) + c0a;
				if (app2 == 0x3fU + c0a) {
					app2 += readBits(8);
				}


				howMany = app2;
				if ((decompressedBytes >> 16) == 0) {
					//if decompressedBytes < 65535 there's a chance that backOffset would go over the beginning of uncomp. data?
					uint16 spostam = backOffset;
					if (decompressedBytes < spostam) {
						//if this happens, fill with 0 bytes until we have a good situation
						app2 = (decompressedSize & 0x0FFFF) - decompressedBytes;
						spostam = spostam - decompressedBytes;
						if (app2 <= spostam) {
							spostam = app2;
							howMany = app2;
						}
						howMany -= spostam;
						decompressedBytes += spostam;
						for (int j = 0; j < spostam; j++) {
							*currDecompressedPtr = 0;
							currDecompressedPtr++;
						}

					}
				}


				for (uint j2 = 0; j2 < howMany; j2++) {
					*currDecompressedPtr = *(currDecompressedPtr - backOffset);
					currDecompressedPtr++;
				}
				decompressedBytes += howMany;


			}
		} while (decompressedBytes < decompressedSize);

	}
	uint32 ArchivesManager::readBits(uint16 n_bits) {
		unsigned int result = 0;
		for (uint16 i = 0; i < n_bits; i++) {
			if (bits_left == 0) {
				curr_byte = *currCompressedPtr;
				currCompressedPtr++;
				bits_left = 8;
			}
			result = result >> 1;
			if ((curr_byte & 0x01) != 0) {
				result += 0x80000000;
			}
			curr_byte = curr_byte >> 1;
			bits_left--;
		}
		uint16 tmp = 32 - n_bits;
		return (result >> tmp);
	}
	uint16 ArchivesManager::reverseBitsInWord(uint16 val) {
		uint16 retval = 0;
		for (uint16 i = 0; i < 16; i++) {
			retval = retval << 1;
			if ((val & 0x0001) != 0) {
				retval = retval + 1;
			}
			val = val >> 1;
		}
		return retval;

	}
	void ArchivesManager::readHuffmanTree(HuffmanTreeNode * &tree, uint16 maxElements) {

		// first thing: the huffman tree is stored in canonical format.
		// granted that symbols are 0..maxElements-1, we only get a sequence of huffman code bits-lengths.
		// this sequence is sort of RLE encoded, let's decode it!
		unsigned char symbolsIdx = 0;
		unsigned char numRLEValues = *currCompressedPtr++;
		numRLEValues++;
		for (uint8 i = 0; i < numRLEValues; i++) {
			unsigned char b = *currCompressedPtr++;
			unsigned char bitsLen = (b & 0x0f) + 1;
			unsigned char num = ((b & 0xf0) >> 4) + 1;
			for (uint8 j = 0; j < num; j++) {
				symbols[symbolsIdx].huffCode = 0;
				symbols[symbolsIdx].symbol = symbolsIdx;
				symbols[symbolsIdx].codeLen = bitsLen;
				symbolsIdx++;
			}

		}

		// ok, now we should sort the symbols list based on bitsLen ascending, if same bitsLen sort by symbol ascending
		// the game original code uses a comb-sort algorithm with a shrink factor of 2,
		// let's mimic the game and use the same, for now
		uint16 gap = 0;
		unsigned char sorted = 0;

		//printf("Situazione iniziale: \n");
		//for (int _i_ = 0; _i_ < maxElements; _i_++) {
		//	printf("%02X CL:%02x SY:%02X\n", _i_, symbols[_i_].codeLen, symbols[_i_].symbol);
		//}
		//printf("\n\n");


		unsigned char maxIdx = maxElements - 1;
		gap = maxIdx / 2;
		do {
			sorted = 1;
			uint32 idx = 0;
			uint8 diff = maxIdx - gap;
			while (idx <= diff) {
				HuffmanSymbol *p1 = &symbols[idx];
				HuffmanSymbol *p2 = &symbols[gap + idx];
				if (p1->codeLen > p2->codeLen || (p1->codeLen == p2->codeLen && p1->symbol >= p2->symbol)) {
					HuffmanSymbol tmpSym = symbols[idx];
					symbols[idx] = symbols[idx + gap];
					symbols[idx + gap] = tmpSym;
					sorted ^= sorted;
				}
				idx++;
			}
			//printf("ciclo con gap %02X: %02X\n",gap,sorted);
			//for (int _i_ = 0; _i_ < maxElements; _i_++) {
			//	printf("%02X CL:%02x SY:%02X\n", _i_, symbols[_i_].codeLen, symbols[_i_].symbol);
			//}
			//printf("\n\n");

			if (sorted) {
				gap = gap / 2;
				if (gap == 0) {
					break;
				}
			}

		} while (true);




		// ok, now the symbols list is correctly ordered
		// let assign huffman codes to every symbol
		// Note: the game uses 'negated' bits. Don't know why but I'll stick to this weird convention
		// I won't exactly mimic the original code, tough

		uint16 currentCode = 0;
		uint8 lastBitLength = symbols[0].codeLen;
		/*
		if (lastBitLength > 1) {
		currentCode = 1 << (lastBitLength - 1);
		}
		*/
		for (uint16 i = 0; i < maxElements; i++) {
			uint16 len = symbols[i].codeLen;
			if (len != lastBitLength) {
				currentCode <<= len - lastBitLength;
				lastBitLength = len;
			}
			symbols[i].huffCode = currentCode;

			// we choose to play by the game's rules:
			// let's negate the meaningful bits of the huffman code and reverse their order, 
			// for an easier building of the tree later
			symbols[i].huffCode = (~symbols[i].huffCode) << (16 - symbols[i].codeLen);
			symbols[i].huffCode = reverseBitsInWord(symbols[i].huffCode);

			currentCode += 1;
		}

		//printf("lista ordinata\n");
		//for (int _i_ = 0; _i_ < maxElements; _i_++) {
		//	printf("%02X CL:%02x SY:%02X CO:%04X\n", _i_, symbols[_i_].codeLen, symbols[_i_].symbol, symbols[_i_].huffCode);
		//}
		//printf("\n\n");


		//printf("dopo creazione bitcodes:\n",gap,sorted);
		//for (int _i_ = 0; _i_ < maxElements; _i_++) {
		//	printf("%02X CL:%02x SY:%02X HC:%02X\n", _i_, symbols[_i_].codeLen, symbols[_i_].symbol,symbols[_i_].huffCode);
		//}
		//printf("\n\n");

		// building the huffman tree:
		// insted of using linked nodes, the game uses an array of structs.
		// the link between nodes is not a pointer, but a simple offset that says:
		// "the next node is N elements away from myself!"
		// 1 bit means "left" while 0 bit means "right"

		uint16 nextFreeNodeIdx = 0;
		uint16 currNodeInChainIdx = 0;
		uint16 prevNodeInChainIdx = 0;
		uint16 lastCarry = 0;
		HuffmanTreeNode *node = nullptr;
		tree[0].flags = 0;
		tree[0].leftVal = 0;
		tree[0].rightVal = 0;
		nextFreeNodeIdx = 1;

		for (uint16 i = 0; i < maxElements; i++) {
			unsigned char codeLen = symbols[i].codeLen;
			uint16 huffCode = symbols[i].huffCode;
			currNodeInChainIdx = 0;
			prevNodeInChainIdx = 0;
			do {  //per ognuno dei bit della bitstream
				node = &tree[currNodeInChainIdx];
				unsigned int carry = 0;
				carry = huffCode & 0x0001;
				lastCarry = carry;
				huffCode = huffCode >> 1;
				if (carry != 0) {//altrimenti 01e3
					if (node->leftVal == 0) {//altrimenti 01c7
						node->leftVal = nextFreeNodeIdx - currNodeInChainIdx;
						prevNodeInChainIdx = currNodeInChainIdx;
						currNodeInChainIdx = nextFreeNodeIdx;
						nextFreeNodeIdx++;
						node = &tree[currNodeInChainIdx];

						node->flags = 0;
						node->leftVal = 0;
						node->rightVal = 0;

					}
					else {
						prevNodeInChainIdx = currNodeInChainIdx;
						currNodeInChainIdx = currNodeInChainIdx + node->leftVal;
					}
				}
				else {
					if (node->rightVal == 0) {
						node->rightVal = nextFreeNodeIdx - currNodeInChainIdx;
						prevNodeInChainIdx = currNodeInChainIdx;
						currNodeInChainIdx = nextFreeNodeIdx;
						nextFreeNodeIdx++;
						node = &tree[currNodeInChainIdx];
						node->flags = 0;
						node->leftVal = 0;
						node->rightVal = 0;
					}
					else {
						prevNodeInChainIdx = currNodeInChainIdx;
						currNodeInChainIdx = currNodeInChainIdx + node->rightVal;

					}
				}
				codeLen--;
			} while (codeLen > 0);
			node = &tree[prevNodeInChainIdx];
			if ((lastCarry & 0x0001) != 0) {
				node->flags |= COMET_COMPRESSION_FLAG_IS_LEFT_NODE;
				node->leftVal = symbols[i].symbol;
			}
			else {
				node->flags |= COMET_COMPRESSION_FLAG_IS_RIGHT_NODE;
				node->rightVal = symbols[i].symbol;
			}
		}
		//printf("albero creato:\n");
		//for (int _i_ = 0; _i_ < nextFreeNodeIdx; _i_++) {
		//	printf("%02X L:%02x R:%02X FL:%02X\n", _i_, tree[_i_].leftVal,tree[_i_].rightVal,tree[_i_].flags);
		//}
		//printf("\n\n");
		//exit(-2);

	}
	uint8 ArchivesManager::decodeHuffman(HuffmanTreeNode * huffTree) {
		unsigned short int di = 0;
		int found = 0;
		unsigned char val = 0;
		do {
			if (readBits(1) != 0) {
				//qua siamo a 02c7
				val = huffTree[di].leftVal;
				if ((huffTree[di].flags & COMET_COMPRESSION_FLAG_IS_LEFT_NODE)) found = 1;
				else {
					di += val;
				}
			}
			else {
				//qua siamo a 02f2
				val = huffTree[di].rightVal;
				if ((huffTree[di].flags & COMET_COMPRESSION_FLAG_IS_RIGHT_NODE)) found = 1;
				else {
					di += val;
				}

			}
		} while (found == 0);
		return val;
	}
	ArchivesManager::ArchivesManager(CometEngine *vm) :_vm(vm),compressed(nullptr), uncompressed(nullptr),
		tree1(nullptr), tree2(nullptr), tree3(nullptr),
		bits_left(0), curr_byte(0), currCompressedPtr(nullptr) {
		tree1 = &trees[0];
		tree2 = &trees[3000];
		tree3 = &trees[6000];
	}
	ArchivesManager::~ArchivesManager() {
		tree1 = nullptr;
		tree2 = nullptr;
		tree3 = nullptr;
	}
	void ArchivesManager::getFile(const char *archiveName, uint16 fileNum, char *destBuffer, uint32 *decompSize) {
		char filename[32];
		Common::File f;
		uint32 fileOffset;
		uint32 relocations[100];
		uint32 relocationsSize = 0;
		uint32 compressedSize = 0;
		uint32 decompressedSize = 0;
		uint8 compressionType = 0;
		uint8 compressionFlags = 0;
		uint16 relativeOffsetToData = 0;
		char *tmpBuffer = nullptr;

		addExtensionToFile(filename, archiveName, ".PAK");
		if (!f.open(filename)) {
			// handle failure to find/open file
			return;
		}
		f.seek((fileNum+1) * 4, SEEK_SET);
		fileOffset = f.readUint32LE();
		f.seek(fileOffset, SEEK_SET);
		relocationsSize = f.readUint32LE();
		if (relocationsSize != 0)
		{
			uint32 relocationNum = (relocationsSize / 4) - 1;
			for (uint32 i = 0; i < relocationNum; i++) {
				relocations[i] = f.readUint32LE();
			}
		}
		compressedSize = f.readUint32LE();
		decompressedSize = f.readUint32LE();
		compressionType = f.readByte();
		compressionFlags = f.readByte();
		relativeOffsetToData = f.readUint16LE();

		f.seek(relativeOffsetToData, SEEK_CUR);
		if (compressionType == 0) {
			f.read(destBuffer, compressedSize);
		}
		else if (compressionType == 1) {
			/*
			tmpBuffer = new char[30000];
			if (tmpBuffer == nullptr) {
				f.close();
				return;
			}
			*/
			tmpBuffer = destBuffer + decompressedSize + 300 - compressedSize;
			f.read(tmpBuffer, compressedSize);
			decompress(compressionFlags, tmpBuffer, destBuffer, decompressedSize);
			//delete[] tmpBuffer;
		}
		f.close();
		if (decompSize != nullptr) {
			*decompSize = decompressedSize;
		}
		//here we should manage relocations but they won't work in architectures with pointers != 32bits
	}
	bool ArchivesManager::checkValidSubfile(const char *archiveName, uint16 fileNum) {
		char filename[32];
		Common::File f;
		uint32 numSub;
		addExtensionToFile(filename, archiveName, ".PAK");
		if (!f.open(filename)) {
			// handle failure to find/open file
			return false;
		}
		f.seek(4, SEEK_SET);
		numSub = (f.readUint32LE() / 4) - 1;
		f.close();
		if (fileNum >= numSub) {
			return false;
		}
		return true;

	}
	char * ArchivesManager::allocateAndGetFile(const char *archiveName, uint16 fileNum, uint32 *decompSize) {
		char filename[32];
		char *destBuffer;
		Common::File f;
		uint32 fileOffset;
		uint32 relocations[100];
		uint32 relocationsSize = 0;
		uint32 compressedSize = 0;
		uint32 decompressedSize = 0;
		uint8 compressionType = 0;
		uint8 compressionFlags = 0;
		uint16 relativeOffsetToData = 0;
		char *tmpBuffer = nullptr;

		addExtensionToFile(filename, archiveName, ".PAK");
		if (!f.open(filename)) {
			// handle failure to find/open file
			return nullptr;
		}
		f.seek((fileNum+1) * 4, SEEK_SET);
		fileOffset = f.readUint32LE();
		f.seek(fileOffset, SEEK_SET);
		relocationsSize = f.readUint32LE();
		if (relocationsSize != 0)
		{
			uint32 relocationNum = (relocationsSize / 4) - 1;
			for (uint32 i = 0; i < relocationNum; i++) {
				relocations[i] = f.readUint32LE();
			}
		}
		compressedSize = f.readUint32LE();
		decompressedSize = f.readUint32LE();
		compressionType = f.readByte();
		compressionFlags = f.readByte();
		relativeOffsetToData = f.readUint16LE();

		f.seek(relativeOffsetToData, SEEK_CUR);
		destBuffer = new char[decompressedSize + 0x10cc];
		if (destBuffer == nullptr) {
			f.close();
			return nullptr;
		}
		if (compressionType == 0) {
			f.read(destBuffer, compressedSize);
		}
		else if (compressionType == 1) {
			tmpBuffer = destBuffer + (decompressedSize + 300 - compressedSize);

			f.read(tmpBuffer, compressedSize);
			decompress(compressionFlags, tmpBuffer, destBuffer, decompressedSize);
//			delete[] tmpBuffer;
		}
		f.close();
		if (decompSize != nullptr) {
			*decompSize = decompressedSize;
		}
		return destBuffer;
		//here we should manage relocations but they won't work in architectures with pointers != 32bits	
	}
	void ArchivesManager::addExtensionToFile(char *dest, const char *filename, const char *extension) {
		char *d = dest;
		do {
			*d++ = *filename++;
		} while (*(d - 1) != 0);
		d = dest;
		while (*d != 0) {
			if (*d++ == '.') {
				return;
			}
		}
		do {
			*d++ = *extension++;
		} while (*(d - 1) != 0);
	}
	void ArchivesManager::readFileBlock(const char *filename, uint8 block, char *buffer,uint32 size) {
		Common::File f;
		uint32 fileOffset;
		if (!f.open(filename)) {
			// handle failure to find/open file
			return;
		}
		f.seek(block * 4, SEEK_SET);
		fileOffset = f.readUint32LE();
		f.seek(fileOffset, SEEK_SET);
		f.read(buffer, size);
		f.close();
	}



}
