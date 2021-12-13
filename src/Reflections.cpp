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
		//Sliders
	float slewAmt	= 0.f;
	float latchAmt	= 0.f;
		//Switches
	bool slewOn 	= 0;
	bool latchOn 	= 0;
	bool gateOn 	= 0;
		//Outputs
	float outA		= 0.f;
	float outB		= 0.f;
		//Comparison
	bool aGreater	= 0;

		//Slew Limiter
	SlewLimiter::SlopeSmoothStack sS;


	Reflections() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SLEW_SLIDER_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LATCH_SLIDER_PARAM, -10.f, 10.f, 0.f, "");
		configParam(SLEW_SWITCH_PARAM, 0.f, 1.f, 0.f, "");
		configParam(LATCH_SWITCH_PARAM, 0.f, 1.f, 0.f, "");
		configParam(GATE_SWITCH_PARAM, 0.f, 1.f, 0.f, "");
	}

	

	void process(const ProcessArgs& args) override 
	{
		//Inputs
	inA 		= inputs[A_INPUT].getVoltage();
	inB 		= inputs[B_INPUT].getVoltage();
		//Sliders
	slewAmt		= params[SLEW_SLIDER_PARAM].getValue();
	latchAmt	= params[LATCH_SLIDER_PARAM].getValue();
		//Switches
	slewOn	= params[SLEW_SWITCH_PARAM].getValue();
	latchOn	= params[LATCH_SWITCH_PARAM].getValue();
	gateOn	= params[GATE_SWITCH_PARAM].getValue();
		//Is a greater then b
	//aGreater = inA > inB + latchAmt;


	//outA = inA * (inA > inB) + inB * (inB < inA); cool idea

//	ifelse();
//	simd::sgn();
//	simd::crossfade();
//	args.sampleTime;

//	outA = inA * (aGreater) + inB * (!aGreater);
//	outB = inA * (!aGreater) + inB * (aGreater);

	
	//ifelse();

	//DEBUG("Start  ===================");
	//DEBUG("inA		%f", inA);
	outA = sS.slopeSmooth(inA, args, slewAmt, latchAmt);



	//DEBUG("outA		%f", outA);
//	DEBUG("signalIn2    %f", outA);

//				This should return the slew limited version of inA?
//	outA = sL.slewLimiter(inA, riseCV, fallCV, args);



		//Outputs
	outputs[GREATER_OUTPUT].setVoltage(outA);
	outputs[LESSER_OUTPUT].setVoltage(outB);

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


struct ReflectionsWidget : ModuleWidget {
	ReflectionsWidget(Reflections* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/dawn/reflections_dawn.svg")));

	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<Dawn_Slider_One>(mm2px(Vec(10.752, 17.284)), module, Reflections::SLEW_SLIDER_PARAM));
		addParam(createParam<Dawn_Slider_One>(mm2px(Vec(14.712, 17.284)), module, Reflections::LATCH_SLIDER_PARAM));

		addParam(createParam<Orange_Switch>(mm2px(Vec(7.897, 62.892)), module, Reflections::SLEW_SWITCH_PARAM));
		addParam(createParam<Orange_Switch>(mm2px(Vec(3.372, 62.892)), module, Reflections::LATCH_SWITCH_PARAM));
		addParam(createParam<Orange_Switch>(mm2px(Vec(12.449, 62.892)), module, Reflections::GATE_SWITCH_PARAM));

		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.265, 18.835)), module, Reflections::A_INPUT));
		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(7.265, 52.478)), module, Reflections::B_INPUT));
		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(10.16, 78.208)), module, Reflections::SLEW_CV_INPUT));
		addInput(createInputCentered<Dawn_Port_One>(mm2px(Vec(10.16, 93.763)), module, Reflections::LATCH_CV_INPUT));

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