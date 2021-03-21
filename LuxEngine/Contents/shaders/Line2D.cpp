#include <Line2D.hpp>



namespace lux::shd::Line2D{
	bufferType colorOutput_::type = Storage;
	uint32     colorOutput_::bind = 0;

	bufferType windowSize_::type = Storage;
	uint32     windowSize_::bind = 1;

	bufferType zBuffer_::type = Storage;
	uint32     zBuffer_::bind = 2;

	bufferType lineData_::type = Uniform;
	uint32     lineData_::bind = 3;
}