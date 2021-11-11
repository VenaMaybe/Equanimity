#include "plugin.hpp"


struct Lilies : Module {
	enum ParamIds {
		RATIO_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		RESET_INPUT,
		CLOCK_INPUT,
		RATIO_CV_INPUT,
		SECRET_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		ONE_OUTPUT,
		TWO_OUTPUT,
		THREE_OUTPUT,
		FOUR_OUTPUT,
		FIVE_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
//--- Variables ----//
	dsp::SchmittTrigger clockInput;
	//Measuring incoming clock length
	float phaseClock = 0.f;
	float clockCycle = 0.f; //Usecase for this should be measured by cnt of rising edges
	//Pulse end calculations
		//pulseE is the falling edge
	bool  pulseE;
	bool  fallE;
	bool  previousClockState;
	//Calculating the multiplications
	float levelMult[5];
	float phaseMult[5] =  {0.f,0.f,0.f,0.f,0.f};
	bool  triggerMult[5];
	dsp::PulseGenerator generatorMult[5];

	Lilies() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RATIO_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
//		float ratioParam = params[RATIO_PARAM].getValue();
		//processing input with proper schmitt trigger
		clockInput.process(rescale(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f, 0.f, 1.f));

		//If the current clock is low and the previous state was set pulseE to true
		//If that is not true set pulseE to false
		pulseE = (!clockInput.isHigh() && previousClockState);
		previousClockState = clockInput.isHigh();

		//Calculates the clock phase and cycle
		//		NOTE calculate the phase more efficiently to reduce phase shift

		
		phaseClock += args.sampleTime;

		if(pulseE) {
			clockCycle = phaseClock;
			phaseClock -= phaseClock;
		}

		
	
		

//		for(int i = 0; i < 1; i++) {

		int i = 0;

		//Set multiplicaiton level
		levelMult[i] = 1.f;
		
//		DEBUG("-------------------------");
//		DEBUG("Incoming Wave: %f", inputs[CLOCK_INPUT].getVoltage());


		//Calculates the multiplication phase level?
		phaseMult[i] += args.sampleTime;

//		DEBUG("phaseMult preIf: %f", phaseMult[i]);
//		DEBUG("clockCycle * 2 : %f", (clockCycle * 2) / levelMult[i]);
		
		if (phaseMult[i] >= clockCycle / levelMult[i]) {
			//phaseMult[i] = 0.f;
			phaseMult[i] -= clockCycle / levelMult[i];
			triggerMult[i] = true;
//		DEBUG("										if triggered");
		}


//		DEBUG("PulseE       : %s", pulseE ? "trueXXX" : "false");
//		DEBUG("triggerMult  : %s", triggerMult[i] ? "true" : "false");




		//Pulse generator section
		//TODO, take into account pulse width;
		if (triggerMult[i]) {
			generatorMult[i].reset();
			generatorMult[i].trigger((clockCycle / levelMult[i]) / 2);
		}
		generatorMult[i].process(args.sampleTime);
		
		//Outputs
		outputs[i].setVoltage(10.f * triggerMult[i]);

		//Trigger Reset
		triggerMult[i] = false;
//		}
	}
};


struct LiliesWidget : ModuleWidget {
	LiliesWidget(Lilies* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/lilies_default.svg")));

//		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
//		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
//		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
//		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<Orange_Slider>(mm2px(Vec(6.944, 47.731)), module, Lilies::RATIO_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 19.894)), module, Lilies::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 35.134)), module, Lilies::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 78.208)), module, Lilies::RATIO_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 107.277)), module, Lilies::SECRET_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.86, 19.894)), module, Lilies::ONE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.86, 35.134)), module, Lilies::TWO_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.86, 53.386)), module, Lilies::THREE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.86, 78.208)), module, Lilies::FOUR_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.86, 107.277)), module, Lilies::FIVE_OUTPUT));

		// mm2px(Vec(28.876, 7.342))
		addChild(createWidget<Widget>(mm2px(Vec(0.802, 0.827))));
	}
};


Model* modelLilies = createModel<Lilies, LiliesWidget>("Lilies");