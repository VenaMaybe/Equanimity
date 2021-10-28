#include "plugin.hpp"


struct Clock_divider_one : Module {
	enum ParamIds {
		PROBABILITY_AMT_A_PARAM,
		PROBABILITY_AMT_B_PARAM,
		PROBABILITY_AMT_C_PARAM,
		PROBABILITY_AMT_D_PARAM,
		PROBABILITY_AMT_E_PARAM,
		RATIO_PARAM,
		DIV_LEVEL_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PROBABILITY_CV_A_INPUT,
		PROBABILITY_CV_B_INPUT,
		PROBABILITY_CV_C_INPUT,
		PROBABILITY_CV_D_INPUT,
		PROBABILITY_CV_E_INPUT,
		CLOCK_INPUT,
		RESET_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		DIVISION_A_OUTPUT,
		DIVISION_B_OUTPUT,
		DIVISION_C_OUTPUT,
		DIVISION_D_OUTPUT,
		DIVISION_E_OUTPUT,
		TOTAL_IN_VOLTS_OUTPUT,
		SUM_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	gateHandler gH;
	dsp::SchmittTrigger clock;
	dsp::PulseGenerator cPulse;
	float counter;

	float clockDuration;
	float clockRemaining;

	bool outcomes[5];

	Clock_divider_one() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RATIO_PARAM, 0.f, 1.f, 0.f, "");
		configParam(DIV_LEVEL_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PROBABILITY_AMT_A_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PROBABILITY_AMT_B_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PROBABILITY_AMT_C_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PROBABILITY_AMT_D_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PROBABILITY_AMT_E_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
		//I don't think I need this for, could use an array!
		bool clockHigh;

		bool clockIn = clock.process(inputs[CLOCK_INPUT].getVoltage());
		if(clock.state > 0.1f) {
			clockDuration += args.sampleTime;
		} else {
			clockDuration = 0.f;
		}

		if(clockDuration > 0.f) {
			clockHigh = 1;
		} else {
			clockHigh = 0;
		}
		

		//DEBUG("my clock length is: %f", clockLength);

		if(clockIn)
   			cPulse.trigger();
		//float x = clockIn ? 10.f : 0.f;

		//The pulse gen has to be used for divisions, not mani clock,
		//set length equal to while gate in is high (ext clock)


		//Returns 1 for remaining time
		bool high = cPulse.process(args.sampleTime);
		outputs[SUM_OUTPUT].setVoltage(10.f * clockHigh);
		
		
		
		for(int i = 0; i < 5; i++){


			if(inputs[CLOCK_INPUT].isConnected()){
				bool gateA = 1;
				outputs[DIVISION_A_OUTPUT + i].setVoltage(gateA ? 10.f : 0.f);
			}
			
			
			

		}


	}
};


struct Clock_divider_oneWidget : ModuleWidget {
	Clock_divider_oneWidget(Clock_divider_one* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/temp_clock_divider.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<Orange_Slider>(mm2px(Vec(17.702, 8.24)), module, Clock_divider_one::RATIO_PARAM));
		addParam(createParam<Orange_Slider>(mm2px(Vec(24.035, 8.24)), module, Clock_divider_one::DIV_LEVEL_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.821, 53.545)), module, Clock_divider_one::PROBABILITY_AMT_A_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.821, 69.004)), module, Clock_divider_one::PROBABILITY_AMT_B_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.821, 84.462)), module, Clock_divider_one::PROBABILITY_AMT_C_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.821, 99.921)), module, Clock_divider_one::PROBABILITY_AMT_D_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.821, 115.379)), module, Clock_divider_one::PROBABILITY_AMT_E_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.493, 12.145)), module, Clock_divider_one::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.493, 25.463)), module, Clock_divider_one::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.493, 53.545)), module, Clock_divider_one::PROBABILITY_CV_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.493, 69.004)), module, Clock_divider_one::PROBABILITY_CV_B_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.493, 84.462)), module, Clock_divider_one::PROBABILITY_CV_C_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.493, 99.921)), module, Clock_divider_one::PROBABILITY_CV_D_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.493, 115.379)), module, Clock_divider_one::PROBABILITY_CV_E_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.461, 12.145)), module, Clock_divider_one::TOTAL_IN_VOLTS_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.461, 25.463)), module, Clock_divider_one::SUM_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.461, 53.545)), module, Clock_divider_one::DIVISION_A_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.461, 69.004)), module, Clock_divider_one::DIVISION_B_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.461, 84.462)), module, Clock_divider_one::DIVISION_C_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.461, 99.921)), module, Clock_divider_one::DIVISION_D_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.461, 115.379)), module, Clock_divider_one::DIVISION_E_OUTPUT));
	}
};


Model* modelClock_divider_one = createModel<Clock_divider_one, Clock_divider_oneWidget>("clock_divider_one");