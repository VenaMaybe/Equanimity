//=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=
//			    -',> MADE  BY  VENA! <,'-				-+=
//									 					-+=
//					-~-  Credits  -~-					-+=
//					Me, obviously! :3					-+=
//					In no parc order!					-+=
//					Omri -~=REVERB=~-					-+=
//					Dale  	   }Bugs{					-+=
//					David	   }Bugs{					-+=
//					Asedev	   }Bugs{					-+=
//					Wapor	%Contrib%					-+=
//					Allie   %Contrib%					-+=
//					Avum	)Testing(					-+=
//					Silver	)Testing(					-+=
//					Pierre  =Inspire=					-+=
//					Aria	=Inspire=					-+=
//					Ivy	    =Inspire=					-+=
//					Andrew ^Teaching^					-+=
//					Vega   ^Teaching^					-+=
//					Squinky^Teaching^					-+=
//					Paul   ^Teaching^					-+=
//					Cornus ^Teaching^					-+=
//									 					-+=
//			  >`~ { THE CHERNO SERVER } ~`<				-+=
//					Stonks   :Advice:					-+=
//					Spirit   :Advice:					-+=
//					Tdb      :Advice:					-+=
//					DarkBlue :Advice:					-+=
//					Lima     :Advice:					-+=
//					Bazooka  :Advice:					-+=
//					Homelike :Advice:					-+=
//									 					-+=
//				-',> MADE WITH LOVE! <,'-				-+=				
//									 					-+=
//									 					-+=
//		   Equanimity is a state of psychological 		-+=
//		stability and composure which is undisturbed	-+=
//		  by experience of or exposure to emotions,		-+=
//		   pain, or other phenomena that may cause		-+=
//		  others to lose the balance of their mind.		-+=
//									 					-+=
//				   FEMBOYS MY BELOVED!					-+=
//				   >DOWN WITH GENDER!<					-+=
//									 					-+=
//=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=+-+=

#pragma once

#include <rack.hpp>
#include "EquWidgets.hpp"


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

//Modules
//		extern Model* modelVena_lfo;
extern Model* modelReflect;
extern Model* modelReflections;
extern Model* modelClock_divider_one;
extern Model* modelLilies;

//		extern Model* modelVena_comparator2;
//		extern Model* modelVena_drum;
// Declare each Model, defined in each module source file
// extern Model* modelMyModule;



//change form orange to purple!

struct Orange_In : app::SvgPort
{
	Orange_In()
	{
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/pink_in.svg")));
	}
};
struct Orange_Out : app::SvgPort
{
	Orange_Out()
	{
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/pink_out.svg")));
	}
};
struct Orange_Switch : app::SvgSwitch
{
	Orange_Switch()
	{
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance,"res/pink_swichOne_frameOff.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance,"res/pink_swichOne_frameOn.svg")));	
	}
};
struct Orange_Slider : app::SvgSlider {
	Orange_Slider() {
	maxHandlePos = mm2px(math::Vec(-0.225, -0.70));
	minHandlePos = mm2px(math::Vec(-0.225, 11.91 - 2.35 - 0.70 - 0.70));
	setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/grey_slider_background.svg")));
	setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/pink_swichOne_frameOn.svg")));
	}
};

//DAWN

struct Dawn_Slider_One : app::SvgSlider 
{
	Dawn_Slider_One() {
	maxHandlePos = mm2px(math::Vec(-1.54 + 1.65, 0.6));
	minHandlePos = mm2px(math::Vec(-1.54 + 1.65, 15.06));
	setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/black_slider_bg_one.svg")));
	setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/black_slider_handle_one.svg")));
	}
};
struct Dawn_Slider_Right : app::SvgSlider 
{
	Dawn_Slider_Right() {
	maxHandlePos = mm2px(math::Vec(-1.54 + 1.65, -2.06));
	minHandlePos = mm2px(math::Vec(-1.54 + 1.65, 33.49 - 2.06));
	setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/black_slider_bg_invis.svg")));
	setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/black_slider_handle_right.svg")));
	}
};
struct Dawn_Slider_Left : app::SvgSlider 
{
	Dawn_Slider_Left() {
	maxHandlePos = mm2px(math::Vec(-1.54 + 1.65 + 3.96 - 2.45, -2.06));
	minHandlePos = mm2px(math::Vec(-1.54 + 1.65 + 3.96 - 2.45, 33.49 - 2.06));
	setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/black_slider_bg_invis.svg")));
	setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/black_slider_handle_left.svg")));
	}
};

//Slider lights

struct Dawn_Slider_Light_Right : SvgWidget {
	Dawn_Slider_Light_Right() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/black_slider_light_right.svg")));
	}
	void draw(const DrawArgs& args) override {
		Widget::draw(args);
	}
	void drawLayer(const DrawArgs& args, int layer) override {
		if(args.fb) {
			//If we're in the module browser!
			SvgWidget::draw(args);
		} else if(layer == 1) {
			//If we're on the light layer!
			SvgWidget::draw(args);
		}
	}
};

struct Dawn_Slider_Light_Left : SvgWidget {
	Dawn_Slider_Light_Left() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/black_slider_light_left.svg")));
	}
	void draw(const DrawArgs& args) override {
		Widget::draw(args);
	}
	void drawLayer(const DrawArgs& args, int layer) override {
		if(args.fb) {
			//If we're in the module browser!
			SvgWidget::draw(args);
		} else if(layer == 1) {
			//If we're on the light layer!
			SvgWidget::draw(args);
		}
	}
};





//Buttons
struct Dawn_Button_Latch : SvgSwitchNoShadow {
	Dawn_Button_Latch()
	{
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/dawn_button_bg_blank.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/dawn_button_bg_latch.svg")));
	}
};

struct Dawn_Port_One : app::SvgPort
{
	Dawn_Port_One()
	{
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/drawn_port_dawn_one.svg")));
	}
};

struct TriangleThingi : SvgWidget
{
	TriangleThingi()
	{
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/pink_in.svg")));
	}
};







//  Utility (not used)
	//Gate handler struct
struct gateHandler {
	//float to gate function, pass float return gate
	bool ftg(float in) {
		if(in > 1.f || in < -1.f) {
			return 1;
		} else { return 0;}
	}
	//gate to float function, pass gate return float
	float gtf(bool in) {
		if(in) {
			return 10.f;
		} else { return 0.f;}
	}
};

