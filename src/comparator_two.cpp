#include "plugin.hpp"

struct Comparator_two : Module {
	enum ParamIds {
		SWITCH_TOP_LATCH_PARAM,
		SWITCH_TOP_GATE_PARAM,
		SLIDER_TOP_LATCH_PARAM,
		SWITCH_BOTTOM_LATCH_PARAM,
		SWITCH_BOTTOM_GATE_PARAM,
		SLIDER_BOTTOM_LATCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_A_INPUT,
		INPUT_B_INPUT,
		INPUT_C_INPUT,
		INPUT_D_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_A_OUTPUT,
		OUTPUT_B_OUTPUT,
		OUTPUT_C_OUTPUT,
		OUTPUT_D_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(TOP_LIGHT, 2),
		ENUMS(BOTTOM_LIGHT, 2),
		NUM_LIGHTS
	};

	dsp::ClockDivider lightDivider;

	Comparator_two() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SWITCH_TOP_LATCH_PARAM, 0.f, 1.f, 0.f, "Latch");
		configParam(SWITCH_TOP_GATE_PARAM, 0.f, 1.f, 0.f, "Gate");
		configParam(SLIDER_TOP_LATCH_PARAM, 0.f, 10.f, 5.f, "Latch Sensitivity");
		configParam(SWITCH_BOTTOM_LATCH_PARAM, 0.f, 1.f, 0.f, "Latch");
		configParam(SWITCH_BOTTOM_GATE_PARAM, 0.f, 1.f, 0.f, "Gate");
		configParam(SLIDER_BOTTOM_LATCH_PARAM, 0.f, 10.f, 5.f, "Latch Sensitivity");
		lightDivider.setDivision(16);
	}

	

	void process(const ProcessArgs& args) override 
	{
		
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
		};
	}
};


struct Comparator_twoWidget : ModuleWidget {
	Comparator_twoWidget(Comparator_two* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/comparator_two.svg")));

	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<Orange_Switch>(mm2px(Vec(13.142, 8.278)), module, Comparator_two::SWITCH_TOP_LATCH_PARAM));
		addParam(createParam<Orange_Switch>(mm2px(Vec(13.142, 21.386)), module, Comparator_two::SWITCH_TOP_GATE_PARAM));
		addParam(createParam<Orange_Slider>(mm2px(Vec(13.142, 36.909).plus(Vec(0.225, 0.72/2))), module, Comparator_two::SLIDER_TOP_LATCH_PARAM));

		addParam(createParam<Orange_Switch>(mm2px(Vec(13.142, 70.06)), module, Comparator_two::SWITCH_BOTTOM_LATCH_PARAM));
		addParam(createParam<Orange_Switch>(mm2px(Vec(13.142, 83.168)), module, Comparator_two::SWITCH_BOTTOM_GATE_PARAM));
		addParam(createParam<Orange_Slider>(mm2px(Vec(13.142, 98.691).plus(Vec(0.225, 0.72/2))), module, Comparator_two::SLIDER_BOTTOM_LATCH_PARAM));

		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 10.633)), module, Comparator_two::INPUT_A_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 23.747)), module, Comparator_two::INPUT_B_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 72.415)), module, Comparator_two::INPUT_C_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 85.529)), module, Comparator_two::INPUT_D_INPUT));

		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 39.262)), module, Comparator_two::OUTPUT_A_OUTPUT));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 47.182)), module, Comparator_two::OUTPUT_B_OUTPUT));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 101.044)), module, Comparator_two::OUTPUT_C_OUTPUT));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 108.964)), module, Comparator_two::OUTPUT_D_OUTPUT));

		addChild(createLightCentered<TinyLight<GreenRedLight>>(mm2px(Vec(7.62, 32.586)), module, Comparator_two::TOP_LIGHT));
		addChild(createLightCentered<TinyLight<GreenRedLight>>(mm2px(Vec(7.62, 94.368)), module, Comparator_two::BOTTOM_LIGHT));
	}
};

//Light sliders don't work/are too hard to figure out, impliment later!
//addParam(createLightParam<LEDLightSliderFixed<GreenLight>>(mm2px(Vec(5.8993969, 44.33149).plus(Vec(-2, 0))), module, VCMixer::LVL_PARAMS + 0, VCMixer::LVL_LIGHTS + 0));


Model* modelComparator_two = createModel<Comparator_two, Comparator_twoWidget>("comparator_two");