#include "LuxEngine/Core/Core.hpp"
#include "LuxEngine/Core/ConsoleOutput.hpp"
#include <exception>




//TODO real time indicator in nodes that changed based on the execution time of the node (runtime, compile time, one time runtime...)

//TODO implement a real output console

namespace lux::out{
	//Prints an error in the console
	//*   pMessage    | the error message
	//*   vFatalError | if true, the engine will stop its execution
	//*   vErrorCode  | the code the process will exit with. It will also be displayed in the console
	void _printError(const std::string& pMessage, const bool vFatalError, const int32 vErrorCode){
		lux::core::running = false;
		Failure printf("%srror. Code: %d\n%s\n", vFatalError ? "Fatal e" : "E", vErrorCode, pMessage.c_str( )); NormalNoNl;
		/*if(vFatalError) */throw std::out_of_range("lux_test");
		//if(vFatalError) { system("pause"); exit(vErrorCode); }
	}



	void _printWarning(const std::string& pMessage){
		Warning printf("Warning: %s\n", pMessage.c_str( ));
	}


	void __stop__( ){
		// lux::core::running = false;

		//TODO fix
		#ifdef _WIN64
		throw std::exception("UwU");
		#elif defined __linux__
		// throw std::out_of_range("UwU");
		#endif
	}
}

//BUG graphic objects are bugged or not initialized and cause exceptions