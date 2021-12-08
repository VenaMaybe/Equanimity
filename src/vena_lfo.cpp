#include <string>
#include <cmath>
#include "Equanimity.hpp"

struct Osc
{
	

	//Returns phase for v/oct in
	float phaseOsc(float voltin) 
	{
		float localPhase = 0.f;
		//Changes V/oct to Hz number https://pages.mtu.edu/~suits/notefreqs.html
		float freq = dsp::FREQ_C4 * std::pow(2.f, voltin);

		localPhase += freq * APP->engine->getSampleTime();
		//Will return the +/-1volt phase
		if (localPhase >= 0.5f) localPhase -= 1.f;

		return localPhase;

	}

	//float
	//uwu

};

struct Vena_lfo : Module {
	enum ParamIds {
		HUMANIZE_KNOB_PARAM,
		RATE_KNOB_PARAM,
		LENGTH_KNOB_PARAM,
		RANDOM_KNOB_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		LENGTH_IN_INPUT,
		RANDOM_IN_INPUT,
		HUMANIZE_IN_INPUT,
		RATE_IN_INPUT,
		SYNC_GATE_IN_INPUT,
		RNDM_GATE_IN_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		GLOBAL_OUT_OUTPUT,
		GLOBAL_OUT_OUTPUTI,
		GLOBAL_OUT_OUTPUTII,
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT_ONE_LIGHT,
		LIGHT_TWO_LIGHT,
		LIGHT_THREE_LIGHT,
		LIGHT_FOUR_LIGHT,
		LIGHT_FIVE_LIGHT,
		NUM_LIGHTS
	};

	Vena_lfo() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(HUMANIZE_KNOB_PARAM, 0.f, 10.f, 0.f, "Humanize");
		configParam(RATE_KNOB_PARAM, 0.f, 10.f, 0.f, "Rate");
		configParam(LENGTH_KNOB_PARAM, 0.f, 10.f, 0.f, "Length");
		configParam(RANDOM_KNOB_PARAM, 0.f, 10.f, 0.f, "Random");
	}
	
	float phase = 0.f;
	

	void process(const ProcessArgs& args) override {
		//Calculate the pitch to + to 
		//args.sampleTime is the length in seconds of a sample!!!

		//Vena u whore phase is the radius (not rlly however vvv
		//When at max hight, it x2*pi will give you the circumference, which
		//Is needed for the sinewave
		phase += params[LENGTH_KNOB_PARAM].getValue() * args.sampleTime;

		if(phase >= params[HUMANIZE_KNOB_PARAM].getValue()) phase -= 1.f;

		float sine = std::sin(2.f * params[RATE_KNOB_PARAM].getValue() * phase);

		// This takes the wave from a range of +/-1v to +/-5v
		// Bc we preform calculations on the wave in +/-1v
		outputs[GLOBAL_OUT_OUTPUT].setVoltage(5.f * sine);
		




	}
};
/*
struct Display : TransparentWidget {

	Vena_lfo* module;
	std::shared_ptr<Font> font;



	Display() {
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/sudo/Sudo.ttf"));
	}

	void draw(const DrawArgs& args) override {

		drawBackground(args);

		

	}


	void drawBackground(const DrawArgs& args) {
		nvgSave(args.vg);
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0, 0, 100, 100);
		nvgFillColor(args.vg, nvgRGBA(0x30, 0x54, 0x80, 0xff));
		nvgFill(args.vg);
		nvgStrokeColor(args.vg, nvgRGBA(0x50, 0x50, 0x50, 0xff));
		nvgStroke(args.vg);
		nvgRestore(args.vg);
	}

};
*/

struct Vena_lfoWidget : ModuleWidget {
	Vena_lfoWidget(Vena_lfo* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/vena_lfo.svg")));

//		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
//		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
//		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
//		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		/*{
			Display* display = new Display();
			display->module = module;
			display->box.pos = Vec(0, 44);
			display->box.size = Vec(box.size.x, 140);
			addChild(display);
		}*/ 

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.659, 74.51)), module, Vena_lfo::HUMANIZE_KNOB_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(49.381, 74.51)), module, Vena_lfo::RATE_KNOB_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.659, 94.711)), module, Vena_lfo::LENGTH_KNOB_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(49.381, 94.711)), module, Vena_lfo::RANDOM_KNOB_PARAM));

		addInput(createInputCentered<Orange_In>(mm2px(Vec(23.25, 103.322)), module, Vena_lfo::LENGTH_IN_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(43.98, 103.322)), module, Vena_lfo::RANDOM_IN_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(54.792, 103.322)), module, Vena_lfo::RNDM_GATE_IN_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(16.659, 107.796)), module, Vena_lfo::HUMANIZE_IN_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(37.388, 107.796)), module, Vena_lfo::RATE_IN_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(61.383, 107.796)), module, Vena_lfo::SYNC_GATE_IN_INPUT));

		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(58.034, 120.538)), module, Vena_lfo::GLOBAL_OUT_OUTPUT));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(58.034, 45.538)), module, Vena_lfo::GLOBAL_OUT_OUTPUTI));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(58.034, 30.538)), module, Vena_lfo::GLOBAL_OUT_OUTPUTII));

		addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(Vec(37.609, 120.538)), module, Vena_lfo::LIGHT_ONE_LIGHT));
		addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(Vec(40.784, 120.538)), module, Vena_lfo::LIGHT_TWO_LIGHT));
		addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(Vec(43.959, 120.538)), module, Vena_lfo::LIGHT_THREE_LIGHT));
		addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(Vec(47.134, 120.538)), module, Vena_lfo::LIGHT_FOUR_LIGHT));
		addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(Vec(50.309, 120.538)), module, Vena_lfo::LIGHT_FIVE_LIGHT));

		// mm2px(Vec(66.04, 37.743))
		addChild(createWidget<Widget>(mm2px(Vec(0.0, 15.863))));
		// mm2px(Vec(5.159, 13.56))
		addChild(createWidget<Widget>(mm2px(Vec(58.804, 67.73))));
		// mm2px(Vec(5.159, 13.56))
		addChild(createWidget<Widget>(mm2px(Vec(58.804, 87.931))));
	}
};


Model* modelVena_lfo = createModel<Vena_lfo, Vena_lfoWidget>("vena_lfo");