// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
#include "LuxObject.h"


uint64 lux::obj::Base::lastID = (uint64)-1;	//#LLID LOS000 initialize the last object ID at -1 so the first object will have ID 0
