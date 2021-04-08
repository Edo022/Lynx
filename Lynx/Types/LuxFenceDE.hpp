#pragma once
#include "Lynx/Threads/Thread.hpp"








namespace lux{
	//This struct is used to synchronize operations from 2 different threads so that they can't overlap
	//TODO USE NORMAL MUTEX OR POLL FENCE
	struct FenceDE{
		bool thr1, thr2;
		FenceDE() : thr1{ false }, thr2{ true } {}


		inline void startFirst() { while(thr2) lux::thr::self::yield(); thr1 = true; }
		inline void endFirst() { thr1 = false; }

		inline void startSecond() { r: while(thr1) lux::thr::self::yield(); thr2 = true; if(thr1 && thr2) goto r; }
		inline void endSecond() { thr2 = false; }

		inline void quit() { thr2 = thr1 = false; }
	};








	//Allows only one thread at a time to execute the code between the lock() and unlock() function calls
	//The mutex must alwas be unlocked to prevent a deadlock
	//This structure don't have a thread limit
	struct mutex{
		char k = 1;
		void lock() { while(!k) { lux::thr::self::yield(); } k = 0; }
		void unlock() { k = 1; }
	};


	//Forces any thread to stop in the wait() function until the fence gets set
	//By default, the fence is unset
	struct pollFence{
		char s = 0;
		void wait() { while(!s) { lux::thr::self::yield(); } }
		//Sets the fence, allowing other threads to resume execution
		void set() { s = 1; }
		//Unsets the fence
		void unset() { s = 0; }
	};


	// struct eventFence{
	// 	char s = 1;
	// 	void signal() {}
	// };

}
