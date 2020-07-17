#pragma once


#pragma optimize( "g", off )	//Turn off global optimization. If the compiler optimizes the loop, it will be skipped and the program will probably freeze or crash
namespace lux{
	//This struct is used to synchronize operations from 2 different threads so that they can't overlap
	//startFirst() and endFisrt() functions mark the start and the end of the operation from the first thread
	//Same goes for startSecond() and endSecond()
	//When a thread have to start an operation, waits until the other thread has finished its job
	//In the very rare case where both threads start in the exact same moment, the second thread will wait for the first 
	//If you stop or suspend one of the threads, use quit() to prevent the other thread to wait forever
	//You can use those functions multiple times in the same thread. Be sure to use an end function for every start
	//    If you don't, you will probably cause a deadlock
	struct FenceDE{
		bool thr1{ false }, thr2{ true };
		inline void __vectorcall startFirst( ){ while(thr2); thr1 = true; }
		inline void __vectorcall endFirst( ){ thr1 = false; }

		inline void __vectorcall startSecond( ){ r: while(thr1); thr2 = true; if(thr1 && thr2) goto r; }
		inline void __vectorcall endSecond( ){ thr2 = false; }

		inline void __vectorcall quit( ){ thr2 = thr1 = false; }
	};
}
#pragma optimize( "g", on )