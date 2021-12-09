#pragma once

#include <rack.hpp>

using namespace rack;

struct SkinManager 
{

Svg getSkin() {

}

};

/*

struct LiliesWidget : ModuleWidget {
	MultiRangeParam* multiRangeParam;
	std::shared_ptr<Svg> dawn_svg;
	std::shared_ptr<Svg> sketch_svg;

	LiliesWidget(Lilies* module) {
		setModule(module);
			//make header file full of skins
		dawn_svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/dawn/lilies_dawn.svg"));
		sketch_svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/dawn/lilies_sketch.svg"));
//		int panelTheme = isDark(module ? (&(((Lilies*)module)->panelTheme)) : NULL) ? 1 : 0;// need this here since step() not called for module browser
		setPanel(dawn_svg);


        */
       