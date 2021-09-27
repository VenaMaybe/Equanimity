#include "plugin.hpp"


struct Vena_drum : Module {
	enum ParamIds {
		ATTACK_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		V_PER_OCT_INPUT,
		TRIG_INPUT,
		DECAY_INPUT,
		DROP_INPUT,
		DRIVE_DISTORTION_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Vena_drum() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(ATTACK_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct Vena_drumWidget : ModuleWidget {
	Vena_drumWidget(Vena_drum* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/vena_drum.svg")));

	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.03, 39.955)), module, Vena_drum::ATTACK_PARAM));

		addInput(createInputCentered<Vena_in>(mm2px(Vec(14.03, 6.697)), module, Vena_drum::V_PER_OCT_INPUT));
		addInput(createInputCentered<Vena_in>(mm2px(Vec(6.287, 28.212)), module, Vena_drum::TRIG_INPUT));
		addInput(createInputCentered<Vena_in>(mm2px(Vec(6.287, 49.26)), module, Vena_drum::DECAY_INPUT));
		addInput(createInputCentered<Vena_in>(mm2px(Vec(14.03, 58.24)), module, Vena_drum::DROP_INPUT));
		addInput(createInputCentered<Vena_in>(mm2px(Vec(10.16, 81.741)), module, Vena_drum::DRIVE_DISTORTION_INPUT));

		addOutput(createOutputCentered<Vena_out>(mm2px(Vec(10.16, 109.221)), module, Vena_drum::OUTPUT_OUTPUT));
	}
};


Model* modelVena_drum = createModel<Vena_drum, Vena_drumWidget>("vena_drum");