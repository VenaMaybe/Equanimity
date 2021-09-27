#include "plugin.hpp"


struct Vena_drum : Module {
	enum ParamIds {
		ATTACK_PARAM, //DONE
		NUM_PARAMS
	};
	enum InputIds {
		V_PER_OCT_INPUT, //DONE
		TRIG_INPUT, //DONE
		DECAY_INPUT, //DONE
		DROP_INPUT, //DONE
		DRIVE_DISTORTION_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_OUTPUT, //DONE
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Vena_drum() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(ATTACK_PARAM, 0.f, 1.f, 0.f, "");
	}

	float phase = 0.f;
	float noise = 0.f;
	float volume = 0.f;
	float drop = 0.f;
	bool rising = false;

	void process(const ProcessArgs& args) override {
		if (inputs[TRIG_INPUT].getVoltage()) {
			drop = 0.f;
			rising = true;
		}

		if (rising) {
			volume += params[ATTACK_PARAM].getValue() / 500;
			volume = clamp(volume, 0.f, 1.f);
			if (volume == 1) {
				rising = false;
			}
		}

		if (!rising) {
			if (inputs[DECAY_INPUT].isConnected()){
				volume -= inputs[DECAY_INPUT].getVoltage() / 500;
			}else{
				volume -= 0.01f;
			}
			volume = clamp(volume, 0.f, 1.f);
		}

		float pitch = drop;
		pitch += inputs[V_PER_OCT_INPUT].getVoltage();
		pitch = clamp(pitch, -4.f, 8.f);

		float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);

		phase += freq * args.sampleTime;
		if (phase >= 0.5f){
			phase -= floor(phase+.5);
			noise = rand()/(RAND_MAX/10);
			noise = noise - 5;
		}

		if (volume > 0) {
			drop -= inputs[DROP_INPUT].getVoltage()/1000;
		}

		outputs[OUTPUT_OUTPUT].setVoltage(volume * noise);
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