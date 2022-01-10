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
			Some kinda liniar or j_exponential option
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
			double clockInput 		= 0.0;
			double resetInput 		= 0.0;
			double ratioInput		= 0.0;
			double ratioParamIn 	= 0.0;

		//SCHMITT TRIGGERS
			dsp::SchmittTrigger clockSchmitt;
			dsp::SchmittTrigger resetSchmitt;

		//RISING AND FALLING EDGE
			bool fallingEdge		= false;				//A one sample trigger
			bool risingEdge			= false;				//A one sample trigger

		//RESET TRIGGERS
			dsp::BooleanTrigger 	resetBooleanTrigger;	//Detects change
			bool resetTrigger 		= false;				//A one sample trigger

			bool resetTFF 			= false;	

		//HISTORY BUFFERS (start all buffers with b_)
			bool b_clockSchmitt		= false;
			double b_clockLength	= 0.0;
			RingBuffer b_ratioInput{16};
			bool wasLinear			= false;

		//CONTEXT MENU RESET OPTIONS (Water)
			//(items stored in json start with j_)
				//Reset on frequency change
			bool j_freqReset 			= true;
				//Reset on ratio change
			bool j_ratioReset 		= false;
			bool ratioStill 		= false;
				//Hard reset (all channels)
			bool j_hardReset 			= false;

		//CONTEXT MENU OUTPUT OPTIONS
			bool j_exponential		= true;
			bool j_trigger 			= false;

		//PHASE ACCUMULATORS (start all phase accumulators with p_)
			double p_phaseClock 	= 1.0;
			
		//LENGTH VARIABLES
			double clockLength 		= 100.0;

		//EQUATION OUTPUT
			double ratioOut			= 0.0;

		//MULTIPLE RANGE PARAMITER (Slider)
			//ParamQuantity ratioRange;
			MultiRangeParam* ratioParamPointer;
			int ratioParamRange 	= 1;
			bool moduleHasLoaded 	= false;

		



		//Calculating the multiplications
		double levelMult[5];
			//phase time for mult, clock cycle / level mult;
		double phaseTimeFM[5];
		double phaseMult[5] =  {0.0, 0.0, 0.0, 0.0, 0.0};
		bool  triggerMult[5];
		dsp::PulseGenerator generatorMult[5];
		
		
		//Ratio
		//Ratio y = b^x (for expo)
		//int ratioIn = 2.0;// x										CHANGE: FROM DOUBLE TO INT
		//double ratioOut;	// y
		//double ratioBase;	// b
		
		
		//Context Menu
			//Waves
		
						//implimented (+-0.5, +-2, +-10) (0, 1, 2)
		



		//ParamQuantity ratioRange;

	//--------------------------------
	//       MODULE CONSTRUCTOR
	//--------------------------------
	Lilies() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<MultiRangeParam>(RATIO_PARAM, -10.0, 10.0, 1.0, "Exponential Ratio");
		ratioParamPointer = reinterpret_cast<MultiRangeParam*>(paramQuantities[RATIO_PARAM]);
		//ratioParamPointer->setValue(1.f);
	}

	bool zxc = true;

	//--------------------------------
	//        MODULE PROCESS
	//--------------------------------

	void onAdd(const AddEvent& e) override {
		if(!moduleHasLoaded) {
			ratioParamPointer->setRange(ratioParamRange, false);
		}
        
	}

	void process(const ProcessArgs& args) override {

		//#region [rgba(100,25,20,0.2)] WORKING Inputs and Atenuator (test atenuator)

			//Inputs
		ratioParamIn = params[RATIO_PARAM].getValue();
		clockInput = inputs[CLOCK_INPUT].getVoltage();
		resetInput = inputs[RESET_INPUT].getVoltage();
			//Temporary debug switch
		bool DEBUG = params[DEBUG_PARAM].getValue();

		if(inputs[RATIO_CV_INPUT].isConnected()) {
			ratioInput = inputs[RATIO_CV_INPUT].getVoltage();
			ratioInput *= ratioParamIn / 10.0;
			//The slider now acts also as an atenuverter!
		} else {
			ratioInput = ratioParamIn;
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
		if(j_ratioReset) {
			ratioStill = true;
			for(int i = 0; i < 14; i++) {
				if(b_ratioInput[i] != b_ratioInput[14] || b_ratioInput[i] == b_ratioInput[15]) {
					ratioStill = false;
				} else {
					resetTrigger = true;
				}
			}
			
			//resetTrigger = ratioStill;
		}
			//We put this outside the if for use in pow optimization
		b_ratioInput.rotate(1);
		b_ratioInput[0] = ratioInput;
		//#endregion



		//#region [rgba(250,50,5,0.1)]

			//Resets if frequency changes by 10 * sampleTime
		if(j_freqReset) {
			if(!isNear(clockLength, b_clockLength, 10 * args.sampleTime)) {
				resetTrigger = true;
			}
		}
		//#endregion

		if(DEBUG){
		//DEBUG("wasLinear     : %s", wasLinear ? "wasLinear TRUE" : "wasLinear FALSE");
		//DEBUG("ratioOut %f", ratioOut);
		DEBUG("ratioParamIn %f", ratioParamIn);
		}

		outputs[DEBUG_OUT].setChannels(1);

		outputs[DEBUG_OUT].setVoltage(resetTrigger, 0);
		//outputs[DEBUG_OUT].setVoltage(p_phaseClock, 1);

		//#region [rgba(20,255,56,0.06)]

			//Calculates
		if(risingEdge) {
			b_clockLength = clockLength;
			clockLength = p_phaseClock;
			p_phaseClock = 0;
		}
		p_phaseClock += args.sampleTime;
		//#endregion
		



		

		

		//--------------------------------
		// FOR - EACH OUTPUT (FIVE)
		//--------------------------------
		
		for(int i = 0, ratioIn = 2.0; i < 5; i++, ratioIn--)
		{
			//#region [rgba(0,255,200,0.05)]

			//--------------------------------
			// DECIDES - LINIAR V EXPOENENTIAL
			//--------------------------------
			if(!j_exponential)
			{
					//Liniar Graphed Curve
				levelMult[i] = divCurve(ratioIn, ratioOut, ratioInput);
					//For context menu when changing to expo
				wasLinear = true;
			}
			else
			{
				if(ratioInput != b_ratioInput[1] || wasLinear) {
					ratioOut = std::pow(2, (ratioInput * ratioIn));
					levelMult[i] = ratioOut;
						//when coming from liniar to j_exponential
					if(i == 4) {
						wasLinear = false;
					}
				}
			}
			//#endregion


			

			//Set the goal for the rising phase
			phaseTimeFM[i] = clockLength / levelMult[i];
			
			//Reset stuff buffer;
			if(resetTrigger) {
				resetTFF = true;
			}
			if(risingEdge && resetTFF) {
				if(j_hardReset) {
					triggerMult[i] = true;
				}
				phaseMult[0] = phaseMult[1] = phaseMult[2] = phaseMult[3] = phaseMult[4] = 0.0;
				//expoTFFforChange = true;
				p_phaseClock = 0.0;
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
				generatorMult[i].trigger( ((1e-3f) * j_trigger) + (((clockLength / levelMult[i]) / 2) * !j_trigger) );
			}
			bool outHigh = generatorMult[i].process(args.sampleTime);
			
			//Outputs
				//outputs[i].setVoltage(10.0 * triggerMult[i]);
			outputs[i].setVoltage(10.0 * outHigh);

			//Trigger Reset
			triggerMult[i] = false;
		}

		

		
			//Makes sure resetTrigger is only one sample long;
		resetTrigger = false;
	}
	
	//--------------------------------
	//           JSON DATA 
	//--------------------------------
	void dataFromJson(json_t* data) override {
		DEBUG("DEBUG: dataFromJson");
		json_t* jsonObjectRange = json_object_get(data, "RangeSave");
    	//if json file is corrupted
		if(jsonObjectRange != NULL) {
			DEBUG("DEBUG: dataFromJson: jsonObjectRange != Null");
			int range = json_integer_value(jsonObjectRange);
			ratioParamPointer->setRange(range, false);
			moduleHasLoaded = true;
		}	

		json_t* jsonObjectFreqReset = json_object_get(data, "j_freqReset");
    	j_freqReset = json_integer_value(jsonObjectFreqReset);
		json_t* jsonObjectRatioReset = json_object_get(data, "j_ratioReset");
    	j_ratioReset = json_integer_value(jsonObjectRatioReset);
		json_t* jsonObjectHardReset = json_object_get(data, "j_hardReset");
    	j_hardReset = json_integer_value(jsonObjectHardReset);
		json_t* jsonObjectExponential = json_object_get(data, "j_exponential");
    	j_exponential = json_integer_value(jsonObjectExponential);
		json_t* jsonObjectTrigger = json_object_get(data, "j_trigger");
    	j_trigger = json_integer_value(jsonObjectTrigger);
	}
	json_t* dataToJson() override {
		DEBUG("DEBUG: dataToJson");
		json_t* data = json_object();
		MultiRangeParam* ratioParamSave = reinterpret_cast<MultiRangeParam*>(paramQuantities[RATIO_PARAM]);
		json_object_set_new(data, "RangeSave", json_integer(ratioParamSave->rangeSelection));

		json_object_set_new(data, "j_freqReset", json_integer(j_freqReset));
		json_object_set_new(data, "j_ratioReset", json_integer(j_ratioReset));
		json_object_set_new(data, "j_hardReset", json_integer(j_hardReset));
		json_object_set_new(data, "j_exponential", json_integer(j_exponential));
		json_object_set_new(data, "j_trigger", json_integer(j_trigger));

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

		//
		//DEBUG("DEBUG: LiliesWidget Constuctor");

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
					module->j_freqReset = !module->j_freqReset;
				}
			};

			struct RatioResetItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->j_ratioReset = !module->j_ratioReset;
				}
			};

			struct HardResetItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->j_hardReset = !module->j_hardReset;
				}
			};


			Menu* createChildMenu() override {
				Menu* menu = new Menu;
				menu->addChild(createMenuLabel("And to god he said:"));
					//My frequency reset item
				FreqResetItem* freqResetItem = createMenuItem<FreqResetItem>("Reset on frequency", CHECKMARK(module->j_freqReset));
				freqResetItem->module = module;
					//My ratio reset item
				RatioResetItem* ratioResetItem = createMenuItem<RatioResetItem>("Reset on ratio", CHECKMARK(module->j_ratioReset));
				ratioResetItem->module = module;
					//My hard reset item
				HardResetItem* hardResetItem = createMenuItem<HardResetItem>("Hard reset", CHECKMARK(module->j_hardReset));
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
					module->j_exponential = !module->j_exponential;
					module->resetTrigger = true;
				}
			};
			struct TriggerSwitchItem : MenuItem {
				Lilies* module;
				void onAction(const event::Action &e) override {
					module->j_trigger = !module->j_trigger;
					//module->resetTrigger = true;
				}
			};
			
			struct RangeZeroItem : MenuItem {
				MultiRangeParam* multiRangeParam;
				void onAction(const event::Action &e) override {
					multiRangeParam->setRange(0, true);
				}
			};
			struct RangeOneItem : MenuItem {
				MultiRangeParam* multiRangeParam;
				void onAction(const event::Action &e) override {
					multiRangeParam->setRange(1, true);
				}
			};
			struct RangeTwoItem : MenuItem {
				MultiRangeParam* multiRangeParam;
				void onAction(const event::Action &e) override {
					multiRangeParam->setRange(2, true);
					
				}
			};

			Menu* createChildMenu() override {
				Menu* menu = new Menu;
				menu->addChild(createMenuLabel("Let the waves rise."));

					//My expo switch item
				ExpoSwitchItem* expoSwitchItem = createMenuItem<ExpoSwitchItem>("Exponential", CHECKMARK(module->j_exponential));
				expoSwitchItem->module = module;
				TriggerSwitchItem* triggerSwitchItem = createMenuItem<TriggerSwitchItem>("Trigger", CHECKMARK(module->j_trigger));
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