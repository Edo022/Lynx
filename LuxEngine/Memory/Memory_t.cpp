#include "LuxEngine/Memory/Memory_t.hpp"
#include "LuxEngine/Memory/Ram/Memory.hpp"




void lux::ram::Cell_t::free( ){
	buffer->cells.remove(cellIndex);
	//TODO destroy buffers from asyncrhonous garbage collector
	//TODO or destroy them only when there is not enough memory
	if(buffer->cells.usedSize( ) == 0) bufferType->buffers.remove(buffer->bufferIndex);
}
