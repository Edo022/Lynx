#include "Input.h"


Engine* __lp_input_engine_ptr;
LuxInputState* __lp_input_states;
LuxArray<uint16> __lp_input_key_queue(100);
uint32 __lp_input_key_queue_size = 0;