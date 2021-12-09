/*#include "plugin.hpp"


struct Temp_mod : Module {
	enum ParamIds {
		SLEW_SLIDER_PARAM,
		LATCH_SLIDER_PARAM,
		LATCH_SWITCH_PARAM,
		SLEW_SWITCH_PARAM,
		GATE_SWITCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		A_INPUT,
		B_INPUT,
		SLEW_CV_INPUT,
		LATCH_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		GREATER_OUTPUT,
		LESSER_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Temp_mod() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SLEW_SLIDER_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LATCH_SLIDER_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LATCH_SWITCH_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SLEW_SWITCH_PARAM, 0.f, 1.f, 0.f, "");
		configParam(GATE_SWITCH_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct Temp_modWidget : ModuleWidget {
	Temp_modWidget(Temp_mod* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/temp_mod.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<RoundBlackKnob>(mm2px(Vec(10.752, 17.284)), module, Temp_mod::SLEW_SLIDER_PARAM));
		addParam(createParam<RoundBlackKnob>(mm2px(Vec(14.712, 17.284)), module, Temp_mod::LATCH_SLIDER_PARAM));
		addParam(createParam<RoundBlackKnob>(mm2px(Vec(3.372, 62.892)), module, Temp_mod::LATCH_SWITCH_PARAM));
		addParam(createParam<RoundBlackKnob>(mm2px(Vec(7.897, 62.892)), module, Temp_mod::SLEW_SWITCH_PARAM));
		addParam(createParam<RoundBlackKnob>(mm2px(Vec(12.449, 62.892)), module, Temp_mod::GATE_SWITCH_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.265, 18.835)), module, Temp_mod::A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.265, 52.478)), module, Temp_mod::B_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 78.208)), module, Temp_mod::SLEW_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 93.763)), module, Temp_mod::LATCH_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.265, 30.05)), module, Temp_mod::GREATER_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.265, 41.264)), module, Temp_mod::LESSER_OUTPUT));

		// mm2px(Vec(0.655, 36.46))
		addChild(createWidget<Widget>(mm2px(Vec(14.057, 17.284))));
		// mm2px(Vec(0.655, 36.46))
		addChild(createWidget<Widget>(mm2px(Vec(14.712, 17.284))));
	}
};


Model* modelTemp_mod = createModel<Temp_mod, Temp_modWidget>("temp_mod");*/