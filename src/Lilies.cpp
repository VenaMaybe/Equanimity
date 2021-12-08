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
			Some kinda oneshot mode if it doesn't do too much cpu
			Range options (+-0.5, +-2, +-10) for voltage remap

	Tips
		Remember if resetting too soon you do not allow for the divisions to occure! :)




*/

	//DELETE LATER!!!
bool near(double lhs, double rhs, double allowedDifference) {
	double delta = lhs - rhs;
	return delta < allowedDifference && delta > -allowedDifference;
}

double divCurve(double ratioIn, double ratioOut, double ratioBase) {
	//Brancheless x+/1 and 1/x+ depending on input!
	ratioIn = (ratioBase < 0) * (-2 * ratioIn) + ratioIn;

	ratioOut = (ratioIn > 0) * (std::abs(ratioBase * ratioIn) + 1) +
		(ratioIn <= 0) * (1/(std::abs(ratioBase * ratioIn) + 1));

	return ratioOut;
};

inline double fastPow(double a, double b) {
  union {
    double d;
    int x[2];
  } u = { a };
  u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;
  return u.d;
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
	double phaseClock = 0.0;
	double clockCycle = 0.0; //Usecase for this should be measured by cnt of rising edges
	//Pulse end calculations
		//pulseE is the falling edge
	bool  pulseE;
	bool  fallE;
	bool  previousClockState;
	//Calculating the multiplications
	double levelMult[5];
		//phase time for mult, clock cycle / level mult;
	double phaseTimeFM[5];
	double phaseMult[5] =  {0.0, 0.0, 0.0, 0.0, 0.0};
	bool  triggerMult[5];
	dsp::PulseGenerator generatorMult[5];
	//Reset
	bool resetTrig = false;
	dsp::SchmittTrigger resetInput;
	dsp::BooleanTrigger resetTrigger;
	bool resetTFF = false;
	double clockCyclePast = 0.0;
		//ratio reset
	//std::array<double, 10> ratioParamBuffer;
	double ratioParamBuffer[10];
	bool ratioStill = false;

	//Ratio
	//Ratio y = b^x (for expo)
	double ratioIn = 2.0;		// x
	double ratioOut;	// y
	double ratioBase;	// b
	
	//CV Input
	double ratioParamIn;
	double ratioParam;
	
	//Context Menu
		//Water
	bool freqReset = true;		//implimented
	bool ratioReset = false;	//implimented
	bool hardReset = false;		//implimented
		//Waves
	bool exponential = true;	//implimented
	bool trigger = true;
	int  range = 1;				//implimented (+-0.5, +-2, +-10) (0, 1, 2)
	bool hasLoaded = false;
	bool expoTFF = false;	//exponential optimizing tff
	bool expoTFFforChange = false;
//	int  lastRange = -1;

	ParamQuantity ratioRange;
	MultiRangeParam* ratioParamPointer;

	Lilies() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<MultiRangeParam>(RATIO_PARAM, -10.0, 10.0, 1.0, "Exponential Ratio");
		ratioParamPointer = reinterpret_cast<MultiRangeParam*>(paramQuantities[RATIO_PARAM]);
		//DEBUG("module Constructed");
	}

	void process(const ProcessArgs& args) override {

		//TODO!!! FIX THE BUG AT REALLY SLOW INCOMING BPMS
		//FIX THE BUG AT HIGH EXPO VALUES, IS THERE A BIG WITH CONST OUTS?
		//fIX WEIRD GLITCH BUG IDK but I think it has to do with initalizing doubles!


		//It doesn't do fm
		//IT IS ALMOST LIKE GRANULAR
		//I MAKE MAKE A VERSION OF IT THAT IS GRANULAR!!!
		//Grains but each one playing a different sample
		//Maybe a reset button
		//raise and fall indipendent lowpass filter

		//Canyon's name for module
		//Gremlin module

		//Range stuff
		if(!hasLoaded) {
			ratioParamPointer->setRange(range, false);
			hasLoaded = true;
		}
		//Input
		ratioParamIn = params[RATIO_PARAM].getValue();
		if(inputs[RATIO_CV_INPUT].isConnected()) {
			ratioParam = inputs[RATIO_CV_INPUT].getVoltage();
			ratioParam *= ratioParamIn / 10.0;
			//The slider now acts also as an atenuverter!
		} else {
			ratioParam = ratioParamIn;
		}

		



		//processing input with proper schmitt trigger
		clockInput.process(rescale(inputs[CLOCK_INPUT].getVoltage(), 0.1, 2.0, 0.0, 1.0));
		resetInput.process(rescale(inputs[RESET_INPUT].getVoltage(), 0.1, 2.0, 0.0, 1.0));

		//If the current clock is low and the previous state was set pulseE to true
		//If that is not true set pulseE to false
		pulseE = (!clockInput.isHigh() && previousClockState);
		fallE = (clockInput.isHigh() && !previousClockState);

		previousClockState = clockInput.isHigh();

		//Calculates the clock phase and cycle
		//		NOTE calculate the phase more efficiently to reduce phase shift

		if(!resetTrig)
		resetTrig = resetTrigger.process(resetInput.isHigh());

		//
		//Trying to get rid of start messup ----- if(fallE && phaseClock > (2 * args.sampleTime)) {
		
		//Detecting ratio change SHOULD BE FIXED!!!

		ratioStill = true;
		for(int i = 0; i < 8 /*&& ratioStill*/; i++) {
			if(ratioParamBuffer[i] != ratioParamBuffer[8] || ratioParamBuffer[i] == ratioParamBuffer[9]) {
				ratioStill = false;
			}
		}

		if(ratioReset && ratioStill) {
			resetTrig = true;
		}

		if(fallE) {
			clockCycle = phaseClock;
			phaseClock -= clockCyclePast;

			if(freqReset  &&  !isNear(clockCycle, clockCyclePast, 3 * args.sampleTime))
			{
				resetTrig = true;
			}
			clockCyclePast = clockCycle;

		}
		phaseClock += args.sampleTime;

//		DEBUG("clockCycle: %f", clockCycle);

		//Ratio stuff

//		DEBUG("-------------BEGINNING-------------");
		ratioBase = ratioParam;
		//ratioBase = 1.0;
		int i = 0;
		for(i = 0; i < 5; i++) {
			//int i = 1;

		//Decides whether to use liniar or exponential graph!
		if(!exponential) {
				//Forcing expo switch I THINK, check later!
			expoTFFforChange = true;
			//ratioParamBuffer[1] =  ratioParam + 1.0;

			levelMult[i] = divCurve(ratioIn, ratioOut, ratioBase);

			
			//Set multiplicaiton level
//			levelMult[i] = ratioOut;
		} else {
			expoTFF = false;
			if(ratioParam == ratioParamBuffer[1] && !expoTFFforChange) {
				//ratioOut = pow(ratioBase, ratioIn);
				//ratioOut = (pow(pow(2, ratioBase), ratioIn));
					//Holds exponenent when not in use for optimization	
				if(expoTFF) {
					ratioOut = pow(2, (ratioBase * ratioIn));
					levelMult[0] = levelMult[1] = levelMult[2] = levelMult[3] = levelMult[4] = ratioOut;
					expoTFF = false;
				}
/*
				int i = 0;
				i++;
				expoTFF = (i > 32);
				i = i * !(i > 32);
*/
/*
				for(int i = 0; i < 5 && expoTFF; i++) {
					ratioOut = pow(2, (ratioBase * ratioIn));
					//Set multiplicaiton level
					levelMult[i] = ratioOut;
					if(i == 4) {
						expoTFF = false;
					}
				} */
			} else {
				ratioOut = std::pow(2, (ratioBase * ratioIn));
				//ratioOut = fastPow(2, (ratioBase * ratioIn));

				//Set multiplicaiton level
				levelMult[i] = ratioOut;
				expoTFF = true;
				expoTFFforChange = false;
			}



		}
		

//		DEBUG("------Loop-----");
//		ratioOut = std::pow(2.0, (ratioBase * ratioIn));
//		levelMult[i] = ratioOut;

//		DEBUG("phaseMult preIf: %f", levelMult[i]);

		ratioIn--;
		
		//TODO:: Later add in a low cpu useage mode
		//which updates it ever 32-124 samples or smt
	
	//	DEBUG("ratioOut %f", ratioOut);
		//Set the goal for the rising phase
		phaseTimeFM[i] = clockCycle / levelMult[i];
//		DEBUG("clockCycle %f", clockCycle);
//		DEBUG("levelMult  %f", levelMult[i]);
		

//		DEBUG("resetTFF     : %s", resetTFF ? "trueTFF" : "false");
		//Reset stuff buffer;
		if(resetTrig) {
			resetTFF = true;
		}
//		DEBUG("resetTFF2    : %s", resetTFF ? "trueTFF" : "false");
		if(fallE && resetTFF) {
			if(hardReset) {
				triggerMult[i] = true;
			}
			phaseMult[0] = phaseMult[1] = phaseMult[2] = phaseMult[3] = phaseMult[4] = 0.0;
			expoTFFforChange = true;
			phaseClock = 0.0;
			if(i == 4) {
				resetTFF = false;
			}
		}
//		DEBUG("resetTF3     : %s", resetTFF ? "trueTFF" : "false");
/*		if(fallE && resetTFF)  {
			triggerMult[i] = true;


		}
*/
		//Calculates the multiplication phase level?
		phaseMult[i] += args.sampleTime;
		
		if (phaseMult[i] >= phaseTimeFM[i]) {
		//	phaseMult[i] = 0.0;
			phaseMult[i] -= phaseTimeFM[i];
			triggerMult[i] = true;
		}

		//Pulse generator section
		//TODO, take into account pulse width;

		if (triggerMult[i]) {
			generatorMult[i].reset(); //Stinky branchless stuff to learn
			generatorMult[i].trigger( ((1e-3f) * trigger) + (((clockCycle / levelMult[i]) / 2) * !trigger) );
		}
		bool outHigh = generatorMult[i].process(args.sampleTime);
		
		//Outputs
//		outputs[i].setVoltage(10.0 * triggerMult[i]);
		outputs[i].setVoltage(10.0 * outHigh);
		outputs[5].setVoltage(ratioParam);

		//Trigger Reset
		triggerMult[i] = false;
		}

		//Shifting the buffer		
		std::memmove(ratioParamBuffer + 1, ratioParamBuffer, 9 * sizeof(double));
	    ratioParamBuffer[0] = ratioParam;

		//That stuff I forgot about
		ratioIn = 2.0;
		resetTrig = false;
	}

	void dataFromJson(json_t* data) override {
		json_t* jsonObjectRange = json_object_get(data, "RangeSave");
    	//if json file is corrupted
		if(jsonObjectRange != NULL) {
			int range = json_integer_value(jsonObjectRange);
			ratioParamPointer->setRange(range, false);
			hasLoaded = true;
		}
		

		json_t* jsonObjectFreqReset = json_object_get(data, "freqReset");
    	freqReset = json_integer_value(jsonObjectFreqReset);
		json_t* jsonObjectRatioReset = json_object_get(data, "ratioReset");
    	ratioReset = json_integer_value(jsonObjectRatioReset);
		json_t* jsonObjectHardReset = json_object_get(data, "hardReset");
    	hardReset = json_integer_value(jsonObjectHardReset);
		json_t* jsonObjectExponential = json_object_get(data, "exponential");
    	exponential = json_integer_value(jsonObjectExponential);
		json_t* jsonObjectTrigger = json_object_get(data, "trigger");
    	trigger = json_integer_value(jsonObjectTrigger);

		//DEBUG("dataFromJson");
	}

	json_t* dataToJson() override {
		json_t* data = json_object();
		MultiRangeParam* ratioParamSave = reinterpret_cast<MultiRangeParam*>(paramQuantities[RATIO_PARAM]);
		json_object_set_new(data, "RangeSave", json_integer(ratioParamSave->rangeSelection));

		json_object_set_new(data, "freqReset", json_integer(freqReset));
		json_object_set_new(data, "ratioReset", json_integer(ratioReset));
		json_object_set_new(data, "hardReset", json_integer(hardReset));
		json_object_set_new(data, "exponential", json_integer(exponential));
		json_object_set_new(data, "trigger", json_integer(trigger));


		return data;

		//DEBUG("dataToJson");
	}

};

struct LiliesWidget : ModuleWidget {
	MultiRangeParam* multiRangeParam;
	LiliesWidget(Lilies* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/dawn/lilies_dawn.svg")));

//		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
//		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
//		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
//		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		Dawn_Slider_One* dawn_slider_one = createParam<Dawn_Slider_One>(mm2px(Vec(6.944 - 1.65, 47.731)), module, Lilies::RATIO_PARAM);
		multiRangeParam = reinterpret_cast<MultiRangeParam*>(dawn_slider_one->paramQuantity);

		addParam(dawn_slider_one);

		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.62, 19.894)), module, Lilies::RESET_INPUT));
		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.62, 35.134)), module, Lilies::CLOCK_INPUT));
		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.62, 78.208)), module, Lilies::RATIO_CV_INPUT));
//		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.62, 107.277)), module, Lilies::SECRET_INPUT));

		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(22.86, 19.894)), module, Lilies::ONE_OUTPUT));
		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(22.86, 35.134)), module, Lilies::TWO_OUTPUT));
		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(22.86, 53.386)), module, Lilies::THREE_OUTPUT));
		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(22.86, 78.208)), module, Lilies::FOUR_OUTPUT));
		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(22.86, 107.277)), module, Lilies::FIVE_OUTPUT));


//		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(7.62, 107.277)), module, Lilies::out_test));

		// mm2px(Vec(28.876, 7.342))
		addChild(createWidget<Widget>(mm2px(Vec(0.802, 0.827))));
	}

	void appendContextMenu(Menu* menu) override {
		Lilies* lilies = (Lilies*)(this->module);
		assert(lilies);



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
			MultiRangeParam* multiRangeParam;

			struct ExpoSwitchItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->exponential = !module->exponential;
					module->resetTrig = true;
				}
			};
			struct TriggerSwitchItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->trigger = !module->trigger;
					//module->resetTrig = true;
				}
			};
			
			struct RangeZeroItem : MenuItem {
				MultiRangeParam* multiRangeParam;
				void onAction(const event::Action &e) override {
					multiRangeParam->setRange(0);
				}
			};
			struct RangeOneItem : MenuItem {
				MultiRangeParam* multiRangeParam;
				void onAction(const event::Action &e) override {
					multiRangeParam->setRange(1);
				}
			};
			struct RangeTwoItem : MenuItem {
				MultiRangeParam* multiRangeParam;
				void onAction(const event::Action &e) override {
					multiRangeParam->setRange(2);
					
				}
			};

			Menu* createChildMenu() override {
				Menu* menu = new Menu;
				menu->addChild(createMenuLabel("Let the waves rise."));

					//My expo switch item
				ExpoSwitchItem* expoSwitchItem = createMenuItem<ExpoSwitchItem>("Exponential", CHECKMARK(module->exponential));
				expoSwitchItem->module = module;
				TriggerSwitchItem* triggerSwitchItem = createMenuItem<TriggerSwitchItem>("Trigger", CHECKMARK(module->trigger));
				triggerSwitchItem->module = module;
					//My range switches
				RangeZeroItem* rangeZeroItem = createMenuItem<RangeZeroItem>("Range: ±0.5", CHECKMARK(multiRangeParam->rangeSelection == 0));
				rangeZeroItem->multiRangeParam = multiRangeParam;
				RangeOneItem* rangeOneItem = createMenuItem<RangeOneItem>("Range: ±2", CHECKMARK(multiRangeParam->rangeSelection == 1));
				rangeOneItem->multiRangeParam = multiRangeParam;
				RangeTwoItem* rangeTwoItem = createMenuItem<RangeTwoItem>("Range: ±10", CHECKMARK(multiRangeParam->rangeSelection == 2));
				rangeTwoItem->multiRangeParam = multiRangeParam;
				

				menu->addChild(new MenuSeparator);
				menu->addChild(expoSwitchItem);
				menu->addChild(triggerSwitchItem);
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
		rangeItem->multiRangeParam = this->multiRangeParam;
		menu->addChild(rangeItem);




		

		menu->addChild(new MenuSeparator);
	//	menu->addChild(createMenuLabel("Hight of the waves"));

	}
};

Model* modelLilies = createModel<Lilies, LiliesWidget>("Lilies");