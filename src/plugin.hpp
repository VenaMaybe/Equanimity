#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;
//		extern Model* modelVena_lfo;
extern Model* modelReflect;
extern Model* modelReflections;
extern Model* modelClock_divider_one;
extern Model* modelLilies;

//		extern Model* modelVena_comparator2;
//		extern Model* modelVena_drum;
// Declare each Model, defined in each module source file
// extern Model* modelMyModule;



struct MultiRangeParam : ParamQuantity {
	//selector to make checkmarks :>
	int rangeSelection;
	

	void setRange(int rangeChoice, bool normalize = true) {
		rangeSelection = rangeChoice;
		float normalized;
		if(normalize) {
			normalized = rescale(getValue(), minValue, maxValue, 0, 1);
		}
		switch(rangeChoice) {
				case 0 :
					
					minValue = -0.5f;
					maxValue = 0.5f;
					
				break;
				case 1 :
					
					minValue = -2.f;
					maxValue = 2.f;
					
				break;
				case 2 :
					
					minValue = -10.f;
					maxValue = 10.f;
					
				break;
		}
		if(normalize) {
			setValue(rescale(normalized, 0, 1, minValue, maxValue));
		}
	}
};






//IHAVENOIDEA

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

//Orange Slider
struct Orange_Slider : app::SvgSlider {
	Orange_Slider() {
	maxHandlePos = mm2px(math::Vec(-0.225, -0.70));
	minHandlePos = mm2px(math::Vec(-0.225, 11.91 - 2.35 - 0.70 - 0.70));
	setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/grey_slider_background.svg")));
	setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/pink_swichOne_frameOn.svg")));
	}
};

//DAWN

struct Dawn_Slider_One : app::SvgSlider {
	Dawn_Slider_One() {
	maxHandlePos = mm2px(math::Vec(-1.54 + 1.65, 0.6));
	minHandlePos = mm2px(math::Vec(-1.54 + 1.65, 15.06));
	setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/black_slider_bg_one.svg")));
	setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/black_slider_handle_one.svg")));
	}
};

struct Dawn_Port_One : app::SvgPort
{
	Dawn_Port_One()
	{
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/dawn/drawn_port_dawn_one.svg")));
	}
};







//  Utility





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





//


//----BEWARE BEWARE (bad code graveyard ahead)----//
//
//

/*
struct E_SliderLight : ModuleLightWidget {
	void draw(NVGcontext *vg) override
	{
		//replaced circle by rect
		nvgRect(vg, 20, 20, 50, 50);
		nvgFillColor(vg, nvgRGBA(200,159,0,255));


	//	nvgRect(vg, 0, 0, box.size.x, box.size.y);

	}
//	void drawHalo(NVGcontext *vg) override
//	{
//		//made halo smaller
//	}
};
*/


/////


/*
struct E_LEDSlider : app::SvgSlider {
	E_LEDSlider() {
		maxHandlePos = app::mm2px(math::Vec(0.738, 0.738).plus(math::Vec(2, 0)));
		minHandlePos = app::mm2px(math::Vec(0.738, 22.078).plus(math::Vec(2, 0)));	
		setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/grey_slider_background.svg")));

	}
};

template <typename TBase, typename TLightBase = RedLight>
struct E_LightSlider : TBase {
	app::ModuleLightWidget* light;

	E_LightSlider() {
		light = new RectangleLight<TLightBase>;
		this->addChild(light);
	}

	void setFirstLightId(int firstLightId) {
		if (this->paramQuantity)
			light->module = this->paramQuantity->module;
		light->firstLightId = firstLightId;
	}

	void step() override {
		TBase::step();
		// Move center of light to center of handle
		light->box.pos = this->handle->box.pos
		                 .plus(this->handle->box.size.div(2))
		                 .minus(light->box.size.div(2));
	}
};

template <typename TLightBase = RedLight>
struct E_LEDLightSlider : E_LightSlider<E_LEDSlider, TLightBase> {
	LEDLightSlider() {
		setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/orange_swichOne_frameOn.svg")));
		this->light->box.size = app::mm2px(math::Vec(1.524, 3.276));
	}
};

template <typename TLightBase = RedLight>
struct E_GreenLightSlider : E_LEDLightSlider<TLightBase> {
	E_GreenLightSlider() {
		
	}
};
*/


		//setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/grey_slider_background.svg")));
