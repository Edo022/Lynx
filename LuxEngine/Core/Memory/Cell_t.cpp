#include "LuxEngine/Core/Memory/Cell_t.hpp"
#include "LuxEngine/Core/Memory/Ram/Ram.hpp"




void lux::ram::Cell_t::free( ){
	buffer->cells.remove(cellIndex);
	//TODO destroy buffers from asyncrhonous garbage collector
	//TODO or destroy them only when there is not enough memory
	if(buffer->cells.usedSize( ) == 0) bufferType->buffers.remove(buffer->bufferIndex);
}
