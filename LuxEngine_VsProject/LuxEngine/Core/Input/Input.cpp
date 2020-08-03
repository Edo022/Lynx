
#include "LuxEngine/Core/Input/Input.h"
#include "LuxEngine/Core/Compute/CShader.h"
#include "LuxEngine/Types/LuxObject/2D/2DLines.h"




lux::input::InputState* lux::input::inStates;




namespace lux::input{
	//inline void setInputState(InputState* inputState) {
	//	inStates = inputState; inStates->sort( );
	//}

	//GLFWmousebuttonfun


	void mouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 mods) {
		float64 x, y;
		glfwGetCursorPos(window, &x, &y);
		rcast<lux::obj::Line2D*>(lux::core::c::CRenderSpaces[0]->children[0])->fp = vec2i32{ (int32)x, (int32)y };
		rcast<lux::obj::Line2D*>(lux::core::c::CRenderSpaces[0]->children[0])->update( );
	}


	void mouseAxisCallback(GLFWwindow* window, float64 x, float64 y) {
		rcast<lux::obj::Line2D*>(lux::core::c::CRenderSpaces[0]->children[0])->fw -= (float32)y * 10;
		rcast<lux::obj::Line2D*>(lux::core::c::CRenderSpaces[0]->children[0])->sw -= (float32)y * 10;
		rcast<lux::obj::Line2D*>(lux::core::c::CRenderSpaces[0]->children[0])->update( );
	}


	void mouseCursorPosCallback(GLFWwindow* window, float64 x, float64 y) {
		//*(rcast<lux::obj::Line2D*>(engine.objs[0]))->sp = vec2i32{ (int32)x, (int32)y };
		__noop;
	}







	//TODO add key seqeuence tree


	//This function manages the input from the keyboard and calls the functions binded to the input state key bindings
	//TODO
	//BROKEN FUNCTION
	void keyCallback(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods) {
		//FULL SCREEN
		static int32 fsstate = 0;
		static int32 wmx, wmy, wmw, wmh;
		if(key == LUX_KEY_F11 && action == GLFW_PRESS) {
			int32 mx, my, mw, mh;
			glfwGetMonitorWorkarea(glfwGetPrimaryMonitor( ), &mx, &my, &mw, &mh);
			if(fsstate == 0) {
				glfwGetWindowSize(window, &wmw, &wmh);
				glfwGetWindowPos(window, &wmx, &wmy);
				glfwSetWindowMonitor(window, glfwGetPrimaryMonitor( ), mx, my, mw, mh, GLFW_DONT_CARE);
				fsstate = 1;
			}
			else if(fsstate == 1) {
				glfwSetWindowMonitor(window, NULL, wmx, wmy, wmw, wmh, GLFW_DONT_CARE);
				glfwRestoreWindow(window);
				fsstate = 0;
			}
		}


		//static int vsstate = 0;
		//if(key == LUX_KEY_F && action == GLFW_PRESS) {
		//	if(vsstate == 0) lux::engine().useVSync = false;
		//	else if(vsstate == 1) lux::engine().useVSync = true;

		//}


		static uint16 yMin = 0, yMax = lux::input::inStates->sequences.size( ) - 1, x = 0;

		if(action != GLFW_REPEAT) {												//If the action is not repeat
			uint16 keyCode = (key | __lp_to_lux_act(action));						//Calculate the key code
			int16 i = yMin;															//Set the loop index as the minimum y
			while(keyCode != lux::input::inStates->sequences[i].sequence[x]) {		//Find the new minimum y
				++i;																	//Increase the counter until the input key is equal to the key of the input state sequence
				if(i > yMax) {															//If there are no equal keys
					x = 0;																	//Reset the key counter
					return;																	//Exit the function
				}
			}
			yMin = i;																//If there is an equal key, set the minimum y

			while(true) {															//Now find the maximum y
				if(keyCode == lux::input::inStates->sequences[i].sequence[x]) {			//if the input key is NOT equal to the key of the input state sequence
					++i;																	//Increase the counter
					if(i >= lux::input::inStates->sequences.size( )) {						//If there are no more different keys
						yMax = lux::input::inStates->sequences.size( ) - 1;					//Set the maximum y as the maximum index of the sequences
						break;																	//Exit the loop
					}
				}
				else {																		//If there are different keys
					yMax = i - 1;																//Set the maximum y
					break;																		//Exit the loop
				}
			}
			// UwU
			if(yMin == yMax && x == lux::input::inStates->sequences[yMax].sequence.size( ) - 1) {							//If the maximum and minimum y are the same
				lux::input::inStates->sequences[yMax].bindedFunction(lux::input::inStates->sequences[yMax].sequence);			//Call the binded function
				yMin = 0;																										//Reset the minimum y
				yMax = lux::input::inStates->sequences.size( ) - 1;																//Reset the maximum y
				x = 0;																											//Reset the x
				return;																											//Exit the function
			}
			x++;																											//Update the input key index
		}
	}
}