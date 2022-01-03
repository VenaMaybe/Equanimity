#include "Equanimity.hpp"

struct Reflections : Module {
	enum ParamIds {
		SLEW_SLIDER_PARAM,
		LATCH_SLIDER_PARAM,
		SLEW_SWITCH_PARAM,
		LATCH_SWITCH_PARAM,
		GATE_SWITCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		A_INPUT,
		B_INPUT,
		SLEW_CV_INPUT,
		LATCH_CV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		GREATER_OUTPUT,
		LESSER_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};
//--- Variables ----//
		//Inputs
	float inA 		= 0.f;
	float inB 		= 0.f;

	float* inAp = &inA;
	float* inBp = &inB;

		//Sliders
	float slewAmt	= 0.f;
	float latchAmt	= 0.f;
	float slewAmtIn	= 0.f;
	float latchAmtIn= 0.f;
		//Switches
	bool slewOn 	= 0;
	bool latchOn 	= 0;
	bool gateOn 	= 0;
		//Outputs
	float outA		= 0.f;
	float outB		= 0.f;
		//Comparison
	bool aGreater	= 0;
	bool bGreater	= 0;

		//Filter (slew limiter)
	MovingAverageFourPass inA_MAFP{4096};
	MovingAverageFourPass inB_MAFP{4096};
	unsigned int desiredBufferSizeCurrent = 0;
	


	// MovingAverageFourPass sS{4096}; //4096, 2048
	// MovingAverageFourPass sS2{4096};
	// MovingAverage test{4096};
	// MovingAverage test2{4096};
	// SmoothSin smoothSineTest{16392};

	// SlopeSmoothStack oldSinTest{16392};

	

	


	Reflections() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SLEW_SLIDER_PARAM, 0.f, 1.f, 0.f, "Slew Ammount");
		configParam(LATCH_SLIDER_PARAM, -10.f, 10.f, 0.f, "Latch Ammount");
		configParam(SLEW_SWITCH_PARAM, 0.f, 1.f, 0.f, "Slew");
		configParam(LATCH_SWITCH_PARAM, 0.f, 1.f, 0.f, "Latch");
		configParam(GATE_SWITCH_PARAM, 0.f, 1.f, 0.f, "Gate");
	}

	//TODO cv input and panel and switch to v2 for lights

	float temp = 0.f;

	void process(const ProcessArgs& args) override 
	{
		//Inputs
	inA 		= inputs[A_INPUT].getVoltage();
	inB 		= inputs[B_INPUT].getVoltage();
		//Sliders
	slewAmtIn	= params[SLEW_SLIDER_PARAM].getValue();
	latchAmtIn	= params[LATCH_SLIDER_PARAM].getValue();
		//Switches
	slewOn	= params[SLEW_SWITCH_PARAM].getValue();
	latchOn	= params[LATCH_SWITCH_PARAM].getValue();
	gateOn	= params[GATE_SWITCH_PARAM].getValue();
		//CV and Atenuverters
	slewAmt =  slewAmtIn;
	if(inputs[SLEW_CV_INPUT].isConnected()) {
		slewAmt = abs(inputs[SLEW_CV_INPUT].getVoltage());
		slewAmt *= slewAmtIn;
		slewAmt = rescale(slewAmt, 0, 10, 0, 1);
	}
	latchAmt = latchAmtIn;
	if(inputs[LATCH_CV_INPUT].isConnected()) {
		latchAmt = inputs[LATCH_CV_INPUT].getVoltage();
		latchAmt *= latchAmtIn / 10;
	}
	//Gate functionality!
	bool gate = true;

	//LATCH OFF
	if(inA < inB + latchAmt && !latchOn) {
		std::swap(inA, inB);
		gate = false;
	}

	//LATCH ON take abs of
	if(latchOn) {
		if(isNear(inA, inB, abs(latchAmt)) && latchOn) {
			std::swap(inA, inB);
		}
		gate = false;
		if(inA >= inB) {
			gate = true;
		}
	}

	//GATE ON
	if(gateOn) {
		//if(inA >= inB) {
		//gate = true;
		//}
		inA = gate * 10.f;
		inB = !gate * 10.f;
	}
	
	//SLEW ON
	if(slewOn) {
		desiredBufferSizeCurrent = slewAmt * 4096;
		inA = inA_MAFP.filter(inA, desiredBufferSizeCurrent);
		inB = inB_MAFP.filter(inB, desiredBufferSizeCurrent);
	}

	

	//OUTPUT
	outA = inA;
	outB = inB;
	outputs[GREATER_OUTPUT].setVoltage(outA);
	outputs[LESSER_OUTPUT].setVoltage(outB);

	

//	ifelse();
//	simd::sgn();
//	simd::crossfade();
//	args.sampleTime;


//	outputs[OUTPUT_A + (i * 2)].setVoltage(high * (high > low) + low * (high < low));
//	outputs[OUTPUT_B + (i * 2)].setVoltage(high * (low > high) + low * (low < high));


/*		
		//Lights Top
		lights[0].setSmoothBrightness(inputs[0].getVoltage() / 10, args.sampleTime * lightDivider.getDivision());
		lights[1].setSmoothBrightness(inputs[1].getVoltage() / 10, args.sampleTime * lightDivider.getDivision());
		//Lights Bottom
		lights[2].setSmoothBrightness(inputs[2].getVoltage() / 10, args.sampleTime * lightDivider.getDivision());
		lights[3].setSmoothBrightness(inputs[3].getVoltage() / 10, args.sampleTime * lightDivider.getDivision());
		//Goes through the top two then bottom two in and outs
		for (int i = 0, i_par = 0; i < 4; i+=2, i_par+=3)
		{

			if(inputs[i].isConnected() || inputs[i+1].isConnected())
			{
				bool boolLatch = params[i_par].getValue(); 
				bool boolGate = params[i_par+1].getValue();
				float latchSize = params[i_par+2].getValue();
				float atenuverterSize = latchSize;
				float outA = inputs[i].getVoltage();
				float outB = inputs[i+1].getVoltage();
				
				//If Gate is triggered without a B input you get the tri wave
				if (boolGate && !inputs[i+1].isConnected()) {
					if(outA > 5.f || outB > 5.f) {
					atenuverterSize += 5.f;
					if (latchSize < 5.f) {
							outB = clamp(outA * -1.f + (latchSize * 3.f), -10.f, 10.f);
						}
					}
				}
				
				
				//This is where the beginning of output direciton happens!

				if(boolGate && boolLatch)
				{
					float tempA = (outA > outB + latchSize or outA < outB - latchSize) ? outA : outB;
					float tempB = (outB > outA + latchSize or outB < outA - latchSize) ? outB : outA;
					
					bool gate = (tempA >= tempB);

					outputs[i].setVoltage(gate*10.f);
					outputs[i+1].setVoltage((not gate)*10.f);
				}
				//Remember Gate acts differently without B input
				else if(boolGate && inputs[i+1].isConnected())
				{
					bool gate = (outA >= outB + (atenuverterSize - 5.f));
					outputs[i].setVoltage(gate*10.f);
					outputs[i+1].setVoltage((not gate)*10.f);
				}
				else if(boolLatch)
				{
					outputs[i].setVoltage((outA > outB + latchSize or outA < outB - latchSize) ? outA : outB);
					outputs[i+1].setVoltage((outB > outA + latchSize or outB < outA - latchSize) ? outB : outA);
				}
				else 
				{
					float sizeShift = 2.f * (atenuverterSize - 5.f);
					
					if(sizeShift < 0.f) {
						outA += sizeShift;
					} else if (boolGate) {
						sizeShift = sizeShift * 0.50f;
					}
					

					if(outA > outB + sizeShift)
					{
					outputs[i].setVoltage(outA);
					outputs[i+1].setVoltage(outB);
					} 
					else 
					{
					outputs[i].setVoltage(outB);
					outputs[i+1].setVoltage(outA);
					}
				};

			}
		};*/
	}
};



//Dawn_Slider_One* dawn_slider_one = createParam<Dawn_Slider_One>(mm2px(Vec(6.944 - 1.65, 47.731)), module, Lilies::RATIO_PARAM);
//		multiRangeParam = reinterpret_cast<MultiRangeParam*>(dawn_slider_one->getParamQuantity());

struct ReflectionsWidget : ModuleWidgetEqu {
	//PanelBorderNoOutline* noBorder;

	ReflectionsWidget(Reflections* module) {
		//module = reinterpret_cast<PanelBorderNoOutline*>(module->set());

		ModuleWidgetEqu::setModule(module);
		ModuleWidgetEqu::setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/dawn/reflections_dawn.svg")));
		
		//PanelBorderNoOutline* pb = new PanelBorderNoOutline;
		//pb->box.size = mm2px(Vec(0, 55.06));
		//addChild(pb);

	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<Dawn_Slider_One>(mm2px(Vec(10.752, 17.284)), module, Reflections::SLEW_SLIDER_PARAM));
		addParam(createParam<Dawn_Slider_One>(mm2px(Vec(14.712, 17.284)), module, Reflections::LATCH_SLIDER_PARAM));

		addParam(createParam<Orange_Switch>(mm2px(Vec(7.897, 62.892)), module, Reflections::GATE_SWITCH_PARAM));
		addParam(createParam<Orange_Switch>(mm2px(Vec(3.372, 62.892)), module, Reflections::LATCH_SWITCH_PARAM));
		addParam(createParam<Orange_Switch>(mm2px(Vec(12.449, 62.892)), module, Reflections::SLEW_SWITCH_PARAM));

		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.265, 18.835)), module, Reflections::A_INPUT));
		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.265, 52.478)), module, Reflections::B_INPUT));
		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(10.16, 78.208)), module, Reflections::LATCH_CV_INPUT));
		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(10.16, 93.763)), module, Reflections::SLEW_CV_INPUT));

		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(7.265, 30.05)), module, Reflections::GREATER_OUTPUT));
		addOutput(createOutputCentered<Dawn_Port_One>(mm2px(Vec(7.265, 41.264)), module, Reflections::LESSER_OUTPUT));

		// mm2px(Vec(0.655, 36.46))
		addChild(createWidget<Widget>(mm2px(Vec(14.057, 17.284))));
		// mm2px(Vec(0.655, 36.46))
		addChild(createWidget<Widget>(mm2px(Vec(14.712, 17.284))));
	}
};

//Light sliders don't work/are too hard to figure out, impliment later!
//addParam(createLightParam<LEDLightSliderFixed<GreenLight>>(mm2px(Vec(5.8993969, 44.33149).plus(Vec(-2, 0))), module, VCMixer::LVL_PARAMS + 0, VCMixer::LVL_LIGHTS + 0));


Model* modelReflections = createModel<Reflections, ReflectionsWidget>("Reflections");