#include "Lynx/Core/Input/Input.hpp"
#include "Lynx/Core/Core.hpp"
#include "Lynx/Core/Render/Shaders/Shader.hpp"
#include "Lynx/Core/Render/Window/Window.hpp"
#include "Lynx/Types/Object/2D/RenderSpace2.hpp"
#include "Lynx/Types/Object/2D/Line2.hpp"








namespace lnx::input{
	InputState* inStates;




	#define wnd_ ((Window*)glfwGetWindowUserPointer(window))
	#define gtollx(n) ((n) / wnd_->swp.createInfo.imageExtent.width) //FIXME DONT DEPEND ON A WINDOW
	#define gtolly(n) ((n) / wnd_->swp.createInfo.imageExtent.height)
	void mouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 mods) {
		float64 x, y; glfwGetCursorPos(window, &x, &y);
		// rcast<lnx::obj::Line2*>(wnd_->CRenderSpaces[0]->children[0])->setFp(f32v2{ gtollx(x), gtolly(y) });
		// rcast<lnx::obj::Line2*>(wnd_->CRenderSpaces[0]->children[0])->update();
	}


	void mouseAxisCallback(GLFWwindow* window, float64 x, float64 y) {
		// rcast<lnx::obj::Line2*>(wnd_->CRenderSpaces[0]->children[0])->data.lineData_.wd0() -= ((float32)y * 10);
		// rcast<lnx::obj::Line2*>(wnd_->CRenderSpaces[0]->children[0])->data.lineData_.wd1() -= ((float32)y * 10);
		// rcast<lnx::obj::Line2*>(wnd_->CRenderSpaces[0]->children[0])->update();
	}


	void mouseCursorPosCallback(GLFWwindow* window, float64 x, float64 y) {
		if(wnd_->CRenderSpaces.count() > 0) {
			rcast<lnx::obj::RenderSpace2*>(wnd_->CRenderSpaces[0])->setMaxLim(f32v2{ ((float32)gtollx(x)), ((float32)gtolly(y)) });
			rcast<lnx::obj::RenderSpace2*>(wnd_->CRenderSpaces[0])->qHierarchy();
		}
	}






	//FIXME swrite a working input function
	//TODO add key seqeuence tree


	//This function manages the input from the keyboard and calls the functions binded to the input state key bindings
	//TODO
	//FIXME BROKEN FUNCTION
	void keyCallback(GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods) {
		//FULL SCREEN
		int32 fsstate = 0;
		int32 wmx, wmy, wmw, wmh;
		if(key == LNX_KEY_F11 && action == GLFW_PRESS) {
			int32 mx, my, mw, mh;
			glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &mx, &my, &mw, &mh);
			if(fsstate == 0) {
				glfwGetWindowSize(window, &wmw, &wmh);
				glfwGetWindowPos(window, &wmx, &wmy);
				glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), mx, my, mw, mh, GLFW_DONT_CARE);
				fsstate = 1;
			}
			else if(fsstate == 1) {
				glfwSetWindowMonitor(window, NULL, wmx, wmy, wmw, wmh, GLFW_DONT_CARE);
				glfwRestoreWindow(window);
				fsstate = 0;
			}
		}


		//static int vsstate = 0;
		//if(key == LNX_KEY_F && action == GLFW_PRESS) {
		//	if(vsstate == 0) lnx::engine().useVSync = false;
		//	else if(vsstate == 1) lnx::engine().useVSync = true;

		//}


		uint16 yMin = 0, yMax = lnx::input::inStates->sequences.count() - 1, x = 0;

		if(action != GLFW_REPEAT) {												//If the action is not repeat
			uint16 keyCode = (key | __lp_to_lnx_act(action));						//Calculate the key code
			int16 i = yMin;															//Set the loop index as the minimum y
			while(keyCode != lnx::input::inStates->sequences[i].sequence[x]) {		//Find the new minimum y
				++i;																	//Increase the counter until the input key is equal to the key of the input state sequence
				if(i > yMax) {															//If there are no equal keys
					x = 0;																	//Reset the key counter
					return;																	//Exit the function
				}
			}
			yMin = i;																//If there is an equal key, set the minimum y

			while(true) {															//Now find the maximum y
				if(keyCode == lnx::input::inStates->sequences[i].sequence[x]) {			//if the input key is NOT equal to the key of the input state sequence
					++i;																	//Increase the counter
					if(i >= lnx::input::inStates->sequences.count()) {						//If there are no more different keys
						yMax = lnx::input::inStates->sequences.count() - 1;					//Set the maximum y as the maximum index of the sequences
						break;																	//Exit the loop
					}
				}
				else {																		//If there are different keys
					yMax = i - 1;																//Set the maximum y
					break;																		//Exit the loop
				}
			}
			// UwU
			if(yMin == yMax && x == lnx::input::inStates->sequences[yMax].sequence.count() - 1) {							//If the maximum and minimum y are the same
				lnx::input::inStates->sequences[yMax].bindedFunction(lnx::input::inStates->sequences[yMax].sequence);			//Call the binded function
				yMin = 0;																										//Reset the minimum y
				yMax = lnx::input::inStates->sequences.count() - 1;																//Reset the maximum y
				x = 0;																											//Reset the x
				return;																											//Exit the function
			}
			x++;																											//Update the input key index
		}
	}
}