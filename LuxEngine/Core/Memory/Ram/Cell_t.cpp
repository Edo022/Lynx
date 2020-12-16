#include "LuxEngine/Core/Memory/Ram/Cell_t.hpp"
#include "LuxEngine/Core/Memory/Ram/Ram.hpp"




void lux::ram::Cell_t::free( ){
	buffer->cells.remove(cellIndex);
	//TODO destroy buffers from asyncrhonous garbage collector
	//TODO or destroy them only when there is not enough memory

	//TODO this does not free the cell. it just makes the element invalid
	//TODO check if the cell is accidentally being re-allocated or not
	if(buffer->cells.usedCount() == 0) bufferType->buffers.remove(buffer->bufferIndex);
}
