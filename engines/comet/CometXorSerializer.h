#ifndef __COMETXORSERIALIZER__
#define __COMETXORSERIALIZER__
#include "common/scummsys.h"
#include "common/serializer.h"
#include "common/stream.h"
namespace Cometengine {

class CometXorSerializer : public ::Common::Serializer {
public:
	CometXorSerializer(::Common::SeekableReadStream *in, ::Common::WriteStream *out);
	void startXor(uint8 stVal, uint8 step);
	void stopXor();

private:
	bool _xorActive;
	uint8 _currXor;
	uint8 _xorStep;
	byte getXorByte();
	template<typename T>
	T getXorValue() {
		T tmp = 0;
		for (uint8 i = 0; i < sizeof(T); i++) {
			tmp |= (getXorByte() << (8 * i));
		}
		return tmp;
	}
	
	// Serializer's methods are not virtual, so I have to work with hiding them
#define COMETSYNC_AS(SUFFIX)                                                                 \
	template<typename T>                                                                     \
	void syncAs##SUFFIX(T &val, Version minVersion = 0, Version maxVersion = kLastVersion) { \
		T xVal = 0;                                                                          \
		if (_xorActive)                                                                      \
			xVal = getXorValue<T>();                                                         \
		if (_xorActive && isSaving())                                                        \
			val ^= xVal;                                                                     \
		Serializer::syncAs##SUFFIX(val, minVersion, maxVersion);                             \
		if (_xorActive)                                                                      \
			val ^= xVal;                                                                     \
	}

public:
	COMETSYNC_AS(Byte)
	COMETSYNC_AS(SByte)
	COMETSYNC_AS(Uint16LE)
	COMETSYNC_AS(Sint16LE)
	COMETSYNC_AS(Uint32LE)
	COMETSYNC_AS(Sint32LE)

}; // namespace Cometengine
} // namespace Cometengine
#endif
