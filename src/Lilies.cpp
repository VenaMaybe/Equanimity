#include "Equanimity.hpp"

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

double divCurve(double ratioIn, double ratioOut, double ratioBase) {
	//Brancheless x+/1 and 1/x+ depending on input!
	ratioIn = (ratioBase < 0) * (-2 * ratioIn) + ratioIn;

	ratioOut = (ratioIn > 0) * (std::abs(ratioBase * ratioIn) + 1) +
		(ratioIn <= 0) * (1/(std::abs(ratioBase * ratioIn) + 1));

	return ratioOut;
};

//--------------------------------
//            MODULE
//--------------------------------
struct Lilies : Module {
	enum ParamIds {
		RATIO_PARAM,
		DEBUG_PARAM,
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
		DEBUG_OUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	//--------------------------------
	//            VARIABLES
	//--------------------------------
		//INPUTS
			double clockInput 	= 0;
			double resetInput 	= 0;
			double ratioParamIn = 0;

		//SCHMITT TRIGGERS
			dsp::SchmittTrigger clockSchmitt;
			dsp::SchmittTrigger resetSchmitt;

		//RISING AND FALLING EDGE
			bool fallingEdge	= false;				//A one sample trigger
			bool risingEdge		= false;				//A one sample trigger

		//RESET TRIGGERS
			dsp::BooleanTrigger resetBooleanTrigger;	//Detects change
			bool resetTrigger 	= false;				//A one sample trigger

			bool resetTFF 		= false;	

		//HISTORY BUFFERS (start all buffers with b_)
			bool b_clockSchmitt	= false;
			RingBuffer b_ratioParam{16};

		//CONTEXT MENU RESET OPTIONS (Water)
				//Reset on frequency change
			bool freqReset 		= true;
				//Reset on ratio change
			bool ratioReset 	= false;
			bool ratioStill 	= false;
				//Hard reset (all channels)
			bool hardReset 		= false;


			




		
		//Measuring incoming clock length
		double phaseClock = 0.0;
		double clockCycle = 0.0; //Usecase for this should be measured by cnt of rising edges

		//Calculating the multiplications
		double levelMult[5];
			//phase time for mult, clock cycle / level mult;
		double phaseTimeFM[5];
		double phaseMult[5] =  {0.0, 0.0, 0.0, 0.0, 0.0};
		bool  triggerMult[5];
		dsp::PulseGenerator generatorMult[5];
		//Reset
		
		
		
		double clockCyclePast = 0.0;
		
		

		//Ratio
		//Ratio y = b^x (for expo)
		int ratioIn = 2.0;	// x										CHANGE: FROM DOUBLE TO INT
		double ratioOut;	// y
		double ratioBase;	// b
		
		//CV Input
		
		double ratioParam;
		
		//Context Menu
		
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

	//--------------------------------
	//       MODULE CONSTRUCTOR
	//--------------------------------
	Lilies() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<MultiRangeParam>(RATIO_PARAM, -10.0, 10.0, 1.0, "Exponential Ratio");
		ratioParamPointer = reinterpret_cast<MultiRangeParam*>(paramQuantities[RATIO_PARAM]);
	}
	//--------------------------------
	//        MODULE PROCESS
	//--------------------------------
	void process(const ProcessArgs& args) override {

		//Checks if module has loaded to set the range
		if(!hasLoaded) {
			ratioParamPointer->setRange(range, false);
			hasLoaded = true;
		}

		//#region [rgba(100,25,20,0.2)] WORKING Inputs and Atenuator (test atenuator)

			//Inputs
		ratioParamIn = params[RATIO_PARAM].getValue();
		clockInput = inputs[CLOCK_INPUT].getVoltage();
		resetInput = inputs[RESET_INPUT].getVoltage();
			//Temporary debug switch
		bool DEBUG = params[DEBUG_PARAM].getValue();

		if(inputs[RATIO_CV_INPUT].isConnected()) {
			ratioParam = inputs[RATIO_CV_INPUT].getVoltage();
			ratioParam *= ratioParamIn / 10.0;
			//The slider now acts also as an atenuverter!
		} else {
			ratioParam = ratioParamIn;
		}
		//#endregion
		
		

		//#region [rgba(10,25,50,0.5)] WORKING (Processing Clock and Reset Input into Bools)

			//Schmitt Trigger for clock and reset (turns voltage into bool)
		clockSchmitt.process(rescale(clockInput, 0.1, 2.0, 0.0, 1.0));
		resetSchmitt.process(rescale(resetInput, 0.1, 2.0, 0.0, 1.0));
		//#endregion

		

		//#region [rgba(150,105,5,0.1)] WORKING (Processing Clock and Reset bools into one sample triggers)

			//One sample trigger for rising and falling edge
		fallingEdge = (!clockSchmitt.isHigh() && b_clockSchmitt);
		risingEdge = (clockSchmitt.isHigh() && !b_clockSchmitt);

			//Sets clockSchmitt Buffer
		b_clockSchmitt = clockSchmitt.isHigh();

			//One sample trigger for reset input
		resetTrigger = resetBooleanTrigger.process(resetSchmitt.isHigh());
		//#endregion

		//--------------------------------
		//  CONTEXT MENU RESET SETTINGS
		//--------------------------------

		//#region [rgba(150,05,5,0.1)]
		
			//Reset on ratio change (when it hasn't been moved for 16 samples)
		if(ratioReset) {
			ratioStill = true;
			for(int i = 0; i < 14; i++) {
				if(b_ratioParam[i] != b_ratioParam[14] || b_ratioParam[i] == b_ratioParam[15]) {
					ratioStill = false;
				}
			}
			
			resetTrigger = ratioStill;

			b_ratioParam.rotate(1);
			b_ratioParam[0] = ratioParam;
		}
		//#endregion

		if(DEBUG){
			DEBUG("ratioStill     : %s", ratioStill ? "ratioStill TRUE" : "ratioStill FALSE");
			//DEBUG("risingEdge     : %s", risingEdge ? "reseteTRUE" : "resetFALSE");
		}

		outputs[DEBUG_OUT].setChannels(2);

		outputs[DEBUG_OUT].setVoltage(ratioStill, 0);
		outputs[DEBUG_OUT].setVoltage(resetTrigger, 1);

	
		

		






		if(risingEdge) {
			clockCycle = phaseClock;
			phaseClock -= clockCyclePast;

			if(freqReset  &&  !isNear(clockCycle, clockCyclePast, 3 * args.sampleTime))
			{
				resetTrigger = true;
			}
			clockCyclePast = clockCycle;

		}
		phaseClock += args.sampleTime;

		//Ratio stuff

		ratioBase = ratioParam;
		//ratioBase = 1.0;










		//--------------------------------
		// FOR - EACH OUTPUT (FIVE)
		//--------------------------------
		for(int i = 0, ratioIn = 2.0; i < 5; i++, ratioIn--)
		{
			//#region [rgba(0,255,200,0.05)]
			//--------------------------------
			// DECIDES - LINIAR V EXPOENENTIAL
			//--------------------------------
			if(!exponential)
			{
					//Forcing expo switch I THINK, check later!
				expoTFFforChange = true;
				//ratioParamBuffer[1] =  ratioParam + 1.0;

				levelMult[i] = divCurve(ratioIn, ratioOut, ratioBase);
				//Set multiplicaiton level
				//levelMult[i] = ratioOut;
			}
			else
			{
				ratioOut = std::pow(2, (ratioBase * ratioIn));

				//Set multiplicaiton level
				levelMult[i] = ratioOut;
				expoTFF = true;
				expoTFFforChange = false;

				/*
				expoTFF = false;
				if(ratioParam == ratioParamBuffer[1] && !expoTFFforChange) {
						//Holds exponenent when not in use for optimization	
					if(expoTFF) {
						ratioOut = pow(2, (ratioBase * ratioIn));
						levelMult[0] = levelMult[1] = levelMult[2] = levelMult[3] = levelMult[4] = ratioOut;
						expoTFF = false;
					}
				} else {
					ratioOut = std::pow(2, (ratioBase * ratioIn));

					//Set multiplicaiton level
					levelMult[i] = ratioOut;
					expoTFF = true;
					expoTFFforChange = false;
				}
				*/
			}
			//#endregion


			

			//Set the goal for the rising phase
			phaseTimeFM[i] = clockCycle / levelMult[i];
			
			//Reset stuff buffer;
			if(resetTrigger) {
				resetTFF = true;
			}
			if(risingEdge && resetTFF) {
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

			//Calculates the multiplication phase level?
			phaseMult[i] += args.sampleTime;
			
			if (phaseMult[i] >= phaseTimeFM[i]) {
			//	phaseMult[i] = 0.0;
				phaseMult[i] -= phaseTimeFM[i];
				triggerMult[i] = true;
			}

			//Pulse generator section
			if (triggerMult[i]) {
				generatorMult[i].reset(); //Stinky branchless stuff to learn
				generatorMult[i].trigger( ((1e-3f) * trigger) + (((clockCycle / levelMult[i]) / 2) * !trigger) );
			}
			bool outHigh = generatorMult[i].process(args.sampleTime);
			
			//Outputs
				//outputs[i].setVoltage(10.0 * triggerMult[i]);
			outputs[i].setVoltage(10.0 * outHigh);

			//Trigger Reset
			triggerMult[i] = false;
		}

		

		//That stuff I forgot about
		
		resetTrigger = false;
	}

	//--------------------------------
	//           JSON DATA 
	//--------------------------------
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
	}

};



//--------------------------------
//        MODULE WIDGET 
//--------------------------------

struct LiliesWidget : ModuleWidgetEqu {
	MultiRangeParam* multiRangeParam;
	std::shared_ptr<Svg> dawn_svg;
	std::shared_ptr<Svg> sketch_svg;

	// PANEL WIDGETS
	LiliesWidget(Lilies* module) {
		setModule(module);
			//make header file full of skins ~-=WORK IN PROGRESS=-~
		dawn_svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/dawn/lilies_dawn.svg"));
		sketch_svg = APP->window->loadSvg(asset::plugin(pluginInstance, "res/dawn/lilies_sketch.svg"));
	//	int panelTheme = isDark(module ? (&(((Lilies*)module)->panelTheme)) : NULL) ? 1 : 0;// need this here since step() not called for module browser
		setPanelNoBg(dawn_svg);


	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		Dawn_Slider_One* dawn_slider_one = createParam<Dawn_Slider_One>(mm2px(Vec(6.944 - 1.65, 47.731)), module, Lilies::RATIO_PARAM);
		multiRangeParam = reinterpret_cast<MultiRangeParam*>(dawn_slider_one->getParamQuantity());

		addParam(dawn_slider_one);

		addParam(createParam<Orange_Switch>(mm2px(Vec(13.897, 62.892)), module, Lilies::DEBUG_PARAM));

		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.62, 19.894)), module, Lilies::RESET_INPUT));
		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.62, 35.134)), module, Lilies::CLOCK_INPUT));
		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.62, 78.208)), module, Lilies::RATIO_CV_INPUT));
	//	addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.62, 107.277)), module, Lilies::SECRET_INPUT));

		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(22.86, 19.894)), module, Lilies::ONE_OUTPUT));
		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(22.86, 35.134)), module, Lilies::TWO_OUTPUT));
		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(22.86, 53.386)), module, Lilies::THREE_OUTPUT));
		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(22.86, 78.208)), module, Lilies::FOUR_OUTPUT));
		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(22.86, 107.277)), module, Lilies::FIVE_OUTPUT));


		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(7.62, 107.277)), module, Lilies::DEBUG_OUT));

		// mm2px(Vec(28.876, 7.342))
		addChild(createWidget<Widget>(mm2px(Vec(0.802, 0.827))));
	}

	// CONTEXT MENU:
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
					module->resetTrigger = true;
				}
			};
			struct TriggerSwitchItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->trigger = !module->trigger;
					//module->resetTrigger = true;
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