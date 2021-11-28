#include "plugin.hpp"

/* Manual

	Lilies is a module that takese a clock input and outputs
	multiplications and devisions of that clock. Those multiplications
	and devisions have a ratio between them which can be controlled
	with external cv. Each blip a distantly related lily floating
	through the waters of math.

	The Clock input can be any wave form which ocilates between
	0.1 volts and 2 volts. From this it will exterpalate the difference
	between it's rising edges to use for multiplication and devision.

	The Reset input is used to reset the clock's inner alighment on the
	next detected rising edge! It is also automatically triggered when
	the incoming clock's bpm/frequency changes or when the ratio changes. 
	(This functionality can be toggled via a context menu.)



	Context Menues
		Reset
			BPM/Freq change reset
			Ratio reset when finished moving
			Pulsewidth take-in?

		Slider
			Some kinda liniar or exponential option
			Range options (+-0.5, +-2, +-10) for voltage remap

	Tips
		Remember if resetting too soon you do not allow for the divisions to occure! :)




*/


bool near(float lhs, float rhs, float allowedDifference) {
	float delta = lhs - rhs;
	return delta < allowedDifference && delta > -allowedDifference;
}

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
		out_test,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
//--- Variables ----//
	dsp::SchmittTrigger clockInput;
	//Measuring incoming clock length
	double phaseClock = 0.f;
	double clockCycle = 0.f; //Usecase for this should be measured by cnt of rising edges
	//Pulse end calculations
		//pulseE is the falling edge
	bool  pulseE;
	bool  fallE;
	bool  previousClockState;
	//Calculating the multiplications
	double levelMult[5];
		//phase time for mult, clock cycle / level mult;
	double phaseTimeFM[5];
	double phaseMult[5] =  {0.f, 0.f, 0.f, 0.f, 0.f};
	bool  triggerMult[5];
	dsp::PulseGenerator generatorMult[5];
	//Reset
	bool resetTrig = false;
	dsp::SchmittTrigger resetInput;
	dsp::BooleanTrigger resetTrigger;
	bool resetTFF = false;
	double clockCyclePast = 0.f;
		//ratio reset
	//std::array<double, 10> ratioParamBuffer;
	double ratioParamBuffer[10];
	bool ratioStill = false;


	//Ratio y = b^x
	double ratioIn;		// x
	double ratioOut;	// y
	double ratioBase;	// b
	
	//Context Menu
	bool freqReset = true;
	bool ratioReset = false;
	bool exponential = true;
	bool hardReset = false;
		//(+-0.5, +-2, +-10) (0, 1, 2)
	int  range = 1;
	int  lastRange = -1;
	float rangeImp = 2.f;
	ParamQuantity ratioRange;

	Lilies() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RATIO_PARAM, -2.f, 2.f, 0.f, "Exponential Ratio");
	}

	void process(const ProcessArgs& args) override {
		//TODO context menu stuff!
		ratioRange.paramId = RATIO_PARAM;
			if(range != lastRange) {
				switch(range) {
				case 0 :
					rangeImp = 0.5f;
					ratioRange.minValue = -0.5f;
					ratioRange.maxValue = 0.5f;
					DEBUG("case 1");
				break;
				case 1 :
					rangeImp = 2.f;
					ratioRange.minValue = -2.f;
					ratioRange.maxValue = 2.f;
					DEBUG("case 2");
				break;
				case 2 :
					rangeImp = 10.f;
					ratioRange.minValue = -10.f;
					ratioRange.maxValue = 10.f;
					DEBUG("case 3");
				break;
			}
		}
		lastRange = range;
		
		

		float ratioParam = params[RATIO_PARAM].getValue();
		//processing input with proper schmitt trigger
		clockInput.process(rescale(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f, 0.f, 1.f));
		resetInput.process(rescale(inputs[RESET_INPUT].getVoltage(), 0.1f, 2.f, 0.f, 1.f));

		//If the current clock is low and the previous state was set pulseE to true
		//If that is not true set pulseE to false
		pulseE = (!clockInput.isHigh() && previousClockState);
		fallE = (clockInput.isHigh() && !previousClockState);

		previousClockState = clockInput.isHigh();

		//Calculates the clock phase and cycle
		//		NOTE calculate the phase more efficiently to reduce phase shift

		
		resetTrig = resetTrigger.process(resetInput.isHigh());

		//
		//Trying to get rid of start messup ----- if(fallE && phaseClock > (2 * args.sampleTime)) {
		
		//Detecting ratio change
		ratioStill = true;
		for(int i = 2; i < 10 && ratioStill; i++) {
			if(ratioParamBuffer[i] != ratioParamBuffer[1] || ratioParamBuffer[i] == ratioParamBuffer[0]) {
				ratioStill = false;
			}
		}

		if(ratioReset && ratioStill) {
			resetTrig = true;
		}

		if(fallE) {
			clockCycle = phaseClock;
			phaseClock -= phaseClock;
			if(freqReset  &&  !near(clockCycle, clockCyclePast, 3 * args.sampleTime))
			{
				resetTrig = true;
			}
			clockCyclePast = clockCycle;
		}
		phaseClock += args.sampleTime;

		
		


//		DEBUG("-------------------------");
//		DEBUG("clockCycle: %f", clockCycle);

		//Ratio stuff
		
		

		for(int i = 0; i < 5; i++) {

		ratioBase = ratioParam;
	//	ratioOut = pow(ratioBase, ratioIn);
		ratioOut = (pow(pow(2, ratioBase), ratioIn));
		ratioIn--;


		//Set multiplicaiton level
		levelMult[i] = ratioOut;	
		//Set the goal for the rising phase
		phaseTimeFM[i] = clockCycle / levelMult[i];
		
		//Reset stuff buffer;
		if(resetTrig) {
			resetTFF = true;
		}
		
		if(fallE && resetTFF) {
			if(hardReset) {
				triggerMult[i] = true;
			}
			phaseMult[i] = 0.f;
			phaseClock = 0.f;
			if(i == 4) {
				resetTFF = false;
			}
		}

//		DEBUG("differenc: %f", clockCycle - (phaseTimeFM[i] * 4.f));
//		DEBUG("Incoming Wave: %f", inputs[CLOCK_INPUT].getVoltage());


/*		if(fallE && resetTFF)  {
			triggerMult[i] = true;


		}
*/







		//Calculates the multiplication phase level?
		phaseMult[i] += args.sampleTime;

//		DEBUG("phaseMult preIf: %f", phaseMult[i]);
//		DEBUG("clockCycle *2/lm : %f", (clockCycle * 2) / levelMult[i]);
		
		if (phaseMult[i] >= phaseTimeFM[i]) {
		//	phaseMult[i] = 0.f;
			phaseMult[i] -= phaseTimeFM[i];
			triggerMult[i] = true;
//		DEBUG("										if triggered");
		}


//		DEBUG("fallE        : %s", fallE ? "trueXXX" : "false");
//		DEBUG("triggerMult  : %s", triggerMult[i] ? "true" : "false");




		//Pulse generator section
		//TODO, take into account pulse width;

/*		if (triggerMult[i]) {
			generatorMult[i].reset();
			generatorMult[i].trigger((clockCycle / levelMult[i]) / 2);
		}
		generatorMult[i].process(args.sampleTime);
*/		
		//Outputs
		outputs[i].setVoltage(10.f * triggerMult[i]);
		outputs[5].setVoltage(10.f * resetTrig);

//		outputs[5].setVoltage(10.f * resetTrig);

		//Trigger Reset
		triggerMult[i] = false;
		}

		
		
		//10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

	
		//Shifting the buffer
		
		std::memmove(ratioParamBuffer + 1, ratioParamBuffer, 9 * sizeof(double));
		ratioParamBuffer[0] = ratioParam;

		//Causing it to be a momentary trigger
	//	if(ratioStill) {
	//		ratioStill = false;
	//	}


		ratioIn = 2;
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
//		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.62, 107.277)), module, Lilies::SECRET_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.86, 19.894)), module, Lilies::ONE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.86, 35.134)), module, Lilies::TWO_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.86, 53.386)), module, Lilies::THREE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.86, 78.208)), module, Lilies::FOUR_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.86, 107.277)), module, Lilies::FIVE_OUTPUT));


		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.62, 107.277)), module, Lilies::out_test));

		// mm2px(Vec(28.876, 7.342))
		addChild(createWidget<Widget>(mm2px(Vec(0.802, 0.827))));
	}

	void appendContextMenu(Menu* menu) override {
		Lilies* lilies = (Lilies*)(this->module);
		assert( lilies);


		menu->addChild(new MenuSeparator);
		//Remember it's like a train line of pointers ponting to pointers
		struct ResetMenuItem : MenuItem {
			Lilies* module;

			struct FreqResetItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->freqReset = !module->freqReset;
				}
			};

			struct RatioResetItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->ratioReset = !module->ratioReset;
				}
			};

			struct HardResetItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->hardReset = !module->hardReset;
				}
			};


			Menu* createChildMenu() override {
				Menu* menu = new Menu;
				menu->addChild(createMenuLabel("And to god he said:"));
					//My frequency reset item
				FreqResetItem* freqResetItem = createMenuItem<FreqResetItem>("Reset on frequency", CHECKMARK(module->freqReset));
				freqResetItem->module = module;
					//My ratio reset item
				RatioResetItem* ratioResetItem = createMenuItem<RatioResetItem>("Reset on ratio", CHECKMARK(module->ratioReset));
				ratioResetItem->module = module;
					//My hard reset item
				HardResetItem* hardResetItem = createMenuItem<HardResetItem>("Hard reset", CHECKMARK(module->hardReset));
				hardResetItem->module = module;

				menu->addChild(new MenuSeparator);
				menu->addChild(freqResetItem);
				menu->addChild(ratioResetItem);
				menu->addChild(hardResetItem);

				return menu;
			};
		};

//		menu->addChild(new MenuSeparator);
		//Remember it's like a train line of pointers ponting to pointers
		struct RangeMenuItem : MenuItem {
			Lilies* module;

			struct ExpoSwitchItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->exponential = !module->exponential;
				}
			};
//
			struct RangeZeroItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->range = 0;
				}
			};
			struct RangeOneItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->range = 1;
				}
			};
			struct RangeTwoItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->range = 2;
				}
			};

			Menu* createChildMenu() override {
				Menu* menu = new Menu;
				menu->addChild(createMenuLabel("Let the waves rise."));

					//My expo switch item
				ExpoSwitchItem* expoSwitchItem = createMenuItem<ExpoSwitchItem>("Exponential", CHECKMARK(module->exponential));
				expoSwitchItem->module = module;
					//My range switches
				RangeZeroItem* rangeZeroItem = createMenuItem<RangeZeroItem>("Range: ±0.5", CHECKMARK(module->range == 0));
				rangeZeroItem->module = module;
				RangeOneItem* rangeOneItem = createMenuItem<RangeOneItem>("Range: ±2", CHECKMARK(module->range == 1));
				rangeOneItem->module = module;
				RangeTwoItem* rangeTwoItem = createMenuItem<RangeTwoItem>("Range: ±10", CHECKMARK(module->range == 2));
				rangeTwoItem->module = module;

				menu->addChild(new MenuSeparator);
				menu->addChild(expoSwitchItem);
				menu->addChild(new MenuSeparator);
				menu->addChild(rangeZeroItem);
				menu->addChild(rangeOneItem);
				menu->addChild(rangeTwoItem);


			return menu;
			};
		};


		ResetMenuItem *resetItem = createMenuItem<ResetMenuItem>("Water", RIGHT_ARROW);
		resetItem->module = reinterpret_cast<Lilies*>(this->module);
		menu->addChild(resetItem);
		
		RangeMenuItem *rangeItem = createMenuItem<RangeMenuItem>("Waves", RIGHT_ARROW);
		rangeItem->module = reinterpret_cast<Lilies*>(this->module);
		menu->addChild(rangeItem);
		

		menu->addChild(new MenuSeparator);
	//	menu->addChild(createMenuLabel("Hight of the waves"));

	}
};


Model* modelLilies = createModel<Lilies, LiliesWidget>("Lilies");