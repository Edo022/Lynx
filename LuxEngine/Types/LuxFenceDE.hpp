#pragma once
#include "LuxEngine/Threads/Thread.hpp"


//calls      |                    A----.                   |    void A(){                 |   void B(){
//           |      A  B---.  B  B  B  | B----.            |        fence.startFirst();   |       fence.startSecond();
//           |      |      |  |  |  |  |      |            |        ...                   |       ...
//unordered  |      AAAAAA-BB-BB-BB-BB-AAAAAA-BB           |        fence.endFirst();     |       fence.endSecond();
//ordered    |      AAAAAA-BB----------AAAAAA-BB           |    }                         |   }
//once		 |      AAAAAA-BB-------------------           |                              |
//           |                                             |                              |
//           -----------------------------------> t        |                              |




namespace lux{
	//This struct is used to synchronize operations from 2 different threads so that they can't overlap
	//TODO USE NORMAL MUTEX OR POLL FENCE
	struct FenceDE{
		bool thr1, thr2;
		FenceDE( ) : thr1{ false }, thr2{ true } { }


		inline void startFirst( ){ while(thr2) lux::thr::self::yield(); thr1 = true; }
		inline void endFirst( ){ thr1 = false; }

		inline void startSecond( ){ r: while(thr1) lux::thr::self::yield(); thr2 = true; if(thr1 && thr2) goto r; }
		inline void endSecond( ){ thr2 = false; }

		inline void quit( ){ thr2 = thr1 = false; }
	};







	//The checked value is always 1 to prevent race conditions
	//Even if the other thread didn't finish to write the variable, it can only skip one iteration or read it as false


	//Allows only one thread at a time to execute the code between the lock() and unlock() function calls
	//The mutex must alwas be unlocked to prevent a deadlock
	//This structure don't have a thread limit
	struct mutex{
		char k = 1;
		void lock(){ while(!k){ lux::thr::self::yield(); } k = 0; }
		void unlock(){ k = 1; }
	};


	//Forces any thread to stop in the wait() function call until the fence gets signaled
	//In the case it's already signaled, nothing will happen
	//This structure doesn't have a thread limit
	struct pollFence{
		char s = 1;
		void wait(){ while(!s){ lux::thr::self::yield(); } }
		void signal(){ s = 0; }
		void reset(){ s = 1; }
	};


	// struct eventFence{
	// 	char s = 1;
	// 	void signal(){ }
	// };

}
