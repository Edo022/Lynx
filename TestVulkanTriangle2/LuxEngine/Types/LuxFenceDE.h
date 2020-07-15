#pragma once


namespace lux{
	//This struct is used to synchronize operations from 2 different threads
	//startFirst() marks the start of the operation from the first thread. endFisrt() marks the end of it
	//The same goes for startSecond() and endSecond()
	//The thread that executes the second operation will wait until the first has finished its job and vice versa
	//If you stop or suspend one of the threads, use quit() to prevent the other thread to wait forever
	//You can use those functions multiple times in the same thread. Be sure to use an end function for every start
	//    If you don't, you will probably cause a deadlock
	struct FenceDE{
		bool thr1{ false }, thr2{ true };
		inline void __vectorcall startFirst( ){ while(thr2); thr1 = true; }
		inline void __vectorcall endFirst( ){ thr1 = false; }

		inline void __vectorcall startSecond( ){ while(thr1); thr2 = true; }
		inline void __vectorcall endSecond( ){ thr2 = false; }

		inline void __vectorcall quit( ){ thr2 = thr1 = false; }
	};
}