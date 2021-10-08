#include <cmath>
#include <math.h>
#include <algorithm>
#include "plugin.hpp"

typedef float (*waveFunctions) (float phase);
float noise = 0.f;

float lerp(float a,float b,float t){
	return a + t * (b - a);
}

float noisewave(float phase){
	return noise/5;
}

float sinwave(float phase){
	return std::sin(2.f * M_PI * phase);
}

float trianglewave(float phase){
	phase+=.25;
	if (phase >= 0.5f)
			phase -= 1.f;
	return 5*abs(phase)-1.25;
}

float sawwave(float phase){
	return phase*2;
}

float squarewave(float phase){
	return (round(phase/2-.5)+.5)*2;
}

struct Vena_drum : Module {
	enum ParamIds {
		ATTACK_PARAM,
		DECAY_PARAM,
		DROP_PARAM,
		WAVE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		V_PER_OCT_INPUT,
		TRIG_INPUT, //DONE
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
		configParam(ATTACK_PARAM, 0.f, 1.f, 0.f, "Gain");
		configParam(DECAY_PARAM, 0.f, 1.f, 0.f, "Fall");
		configParam(DROP_PARAM, -1.f, 1.f, 0.f, "Drop");
		configParam(WAVE_PARAM, 0.f, 0.99f, 0.f, "Wave");
	}

	float phase = 0.f;
	float volume = 0.f;
	float drop = 0.f;
	bool rising = false;

	void process(const ProcessArgs& args) override {
		waveFunctions waves[] = {noisewave, sinwave, trianglewave, sawwave, squarewave};

		if (inputs[TRIG_INPUT].getVoltage()) {
			drop = 0.f;
			rising = true;
		}

		if (rising) {
			volume += pow(params[ATTACK_PARAM].getValue(),2) / 600;
			volume = clamp(volume, 0.f, 1.f);
			if (volume == 1) {
				rising = false;
			}
		}

		if (!rising) {
			volume -= pow(params[DECAY_PARAM].getValue(),2) / 600;
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

		float slide = params[WAVE_PARAM].getValue();
		int numWaves = ((sizeof(waves)/sizeof(waves[0]))-1);
		int iWave = floor(slide * numWaves);
		float lWave = 5*lerp(waves[iWave](phase),waves[iWave+1](phase),(slide - floor(slide*numWaves)/numWaves)*numWaves);

		if (volume > 0) {
			drop -= params[DROP_PARAM].getValue()/2000;
		}

		outputs[OUTPUT_OUTPUT].setVoltage(volume * lWave);
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
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.287, 49.26)), module, Vena_drum::DECAY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.03, 58.24)), module, Vena_drum::DROP_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 81.741)), module, Vena_drum::WAVE_PARAM));

		addInput(createInputCentered<Vena_in>(mm2px(Vec(14.03, 6.697)), module, Vena_drum::V_PER_OCT_INPUT));
		addInput(createInputCentered<Vena_in>(mm2px(Vec(6.287, 28.212)), module, Vena_drum::TRIG_INPUT));

		addOutput(createOutputCentered<Vena_out>(mm2px(Vec(10.16, 109.221)), module, Vena_drum::OUTPUT_OUTPUT));
	}
};


Model* modelVena_drum = createModel<Vena_drum, Vena_drumWidget>("vena_drum");