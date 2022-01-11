/*#include "plugin.hpp"


struct Temp : Module {
	enum ParamId {
		SLEW_SLIDER_PARAM,
		LATCH_SLIDER_PARAM,
		LATCH_SWITCH_PARAM,
		RECT12144_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		A_INPUT,
		B_INPUT,
		SLEW_CV_INPUT,
		LATCH_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		GREATER_OUTPUT,
		LESSER_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Temp() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SLEW_SLIDER_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LATCH_SLIDER_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LATCH_SWITCH_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RECT12144_PARAM, 0.f, 1.f, 0.f, "");
		configInput(A_INPUT, "");
		configInput(B_INPUT, "");
		configInput(SLEW_CV_INPUT, "");
		configInput(LATCH_CV_INPUT, "");
		configOutput(GREATER_OUTPUT, "");
		configOutput(LESSER_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct TempWidget : ModuleWidget {
	TempWidget(Temp* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/temp.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<RoundBlackKnob>(mm2px(Vec(10.752, 17.284)), module, Temp::SLEW_SLIDER_PARAM));
		addParam(createParam<RoundBlackKnob>(mm2px(Vec(14.712, 17.284)), module, Temp::LATCH_SLIDER_PARAM));
		addParam(createParam<RoundBlackKnob>(mm2px(Vec(3.372, 63.586)), module, Temp::LATCH_SWITCH_PARAM));
		addParam(createParam<RoundBlackKnob>(mm2px(Vec(10.16, 63.586)), module, Temp::GATE_SWITCH_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.265, 19.894)), module, Temp::A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.265, 53.386)), module, Temp::B_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 78.208)), module, Temp::SLEW_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.16, 93.763)), module, Temp::LATCH_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.265, 31.058)), module, Temp::GREATER_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.265, 42.222)), module, Temp::LESSER_OUTPUT));

		// mm2px(Vec(0.655, 36.46))
		addChild(createWidget<Widget>(mm2px(Vec(14.057, 17.284))));
		// mm2px(Vec(0.655, 36.46))
		addChild(createWidget<Widget>(mm2px(Vec(14.712, 17.284))));
	}
};


Model* modelTemp = createModel<Temp, TempWidget>("temp");*/