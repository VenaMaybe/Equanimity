#pragma once
#include <rack.hpp>


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;
extern Model* modelVena_lfo;
extern Model* modelVena_comparator;
extern Model* modelVena_comparator2;
extern Model* modelVena_drum;
// Declare each Model, defined in each module source file
// extern Model* modelMyModule;

struct Vena_in : app::SvgPort
{
	Vena_in()
	{
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/vena_in.svg")));
	}
};

struct Vena_out : app::SvgPort
{
	Vena_out()
	{
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance,"res/vena_out.svg")));
	}
};

struct Vena_switch : app::SvgSwitch
{
	Vena_switch()
	{
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance,"res/vena_swichOne_frameOff.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance,"res/vena_swichOne_frameOn.svg")));	
	}
};