#include "comet/CometXorSerializer.h"
namespace Cometengine {
CometXorSerializer::CometXorSerializer(::Common::SeekableReadStream *in, ::Common::WriteStream *out) : Serializer(in, out), _xorActive(false), _currXor(0), _xorStep(0) {
}
void CometXorSerializer::startXor(uint8 stVal, uint8 step) {
	_currXor = stVal;
	_xorStep = step;
	_xorActive = true;
}
void CometXorSerializer::stopXor() {
	_currXor = 0;
	_xorStep = 0;
	_xorActive = false;
}
byte CometXorSerializer::getXorByte() {
	byte x = _currXor;
	_currXor += _xorStep;
	return x;
}

} // namespace Cometengine
