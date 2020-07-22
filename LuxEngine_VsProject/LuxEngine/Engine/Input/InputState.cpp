
#include "LuxEngine/Engine/Input/InputState.h"




namespace lux::input{
	//This function sorts the sequences. You don't have to call it, they'll be sorted when needed.
	//Additional calls to this function does not affect performances
	//TODO use specific string sorting function
	void __vectorcall InputState::sort( ) {
		if(!sorted) {															//If the sequence is not sorted
			sorted = true;															//Set it as sorted
			for(uint16 i = 0; i < sequences.size( ); ++i) {							//For every key sequence
				for(uint16 j = i; j < sequences.size( ); ++j) {							//Checking every other sequence that comes after it
					if(j == i) continue;													//Skip useless iterations
					for(uint16 k = 0; k < min(sequences[j].sequence.size( ), sequences[i].sequence.size( )); ++k) {//For every key of the second sequence
						if(sequences[j].sequence[k] == sequences[i].sequence[k]) continue;	//Search for the next index with different keys
						if(sequences[j].sequence[k] < sequences[i].sequence[k]) {			//If the first is greater than the second
							KeySequence b = sequences[i];									//Swap the bindings and the whole sequences
							sequences[i] = sequences[j];
							sequences[j] = b;
							break;																//Exit the loop of the second sequence keys
						}
					}
				}
			}
		}
	}
}