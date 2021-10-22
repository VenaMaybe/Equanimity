#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;
//		extern Model* modelVena_lfo;
extern Model* modelComparator_one;
extern Model* modelComparator_two;

//		extern Model* modelVena_comparator2;
//		extern Model* modelVena_drum;
// Declare each Model, defined in each module source file
// extern Model* modelMyModule;

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

// Slider stuff :>

//Orange Slider
struct Orange_Slider : app::SvgSlider {
	Orange_Slider() {
	maxHandlePos = mm2px(math::Vec(-0.225, -0.70));
	minHandlePos = mm2px(math::Vec(-0.225, 11.91 - 2.35 - 0.70 - 0.70));
	setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/grey_slider_background.svg")));
	setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/pink_swichOne_frameOn.svg")));
	}
};


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
