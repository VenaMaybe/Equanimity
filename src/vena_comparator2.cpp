#include "plugin.hpp"


struct Vena_comparator2 : Module {
	enum ParamIds {
		SWITCH_TOP_LATCH_PARAM,
		SWITCH_TOP_GATE_PARAM,
		SWITCH_BOTTOM_LATCH_PARAM,
		SWITCH_BOTTOM_GATE_PARAM,
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

	Vena_comparator2() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SWITCH_TOP_LATCH_PARAM, 0.f, 1.f, 0.f, "Latch");
		configParam(SWITCH_TOP_GATE_PARAM, 0.f, 1.f, 0.f, "Gate");
		configParam(SWITCH_BOTTOM_LATCH_PARAM, 0.f, 1.f, 0.f, "Latch");
		configParam(SWITCH_BOTTOM_GATE_PARAM, 0.f, 1.f, 0.f, "Gate");
		lightDivider.setDivision(16);
	}

	void process(const ProcessArgs& args) override 
	{

		//TODO!!!
		//Add in a slider that changes Param's +/- 3 volt range, however
			//we wouldn't be able to fit in a cv for that so it'd just be
			//an up and down slider, so perhaps that would be a whole
			//other module by itself!!! Work on it tomorrow!!! <3
		


		//Lights Top
		lights[0].setSmoothBrightness(inputs[0].getVoltage() / 10, args.sampleTime * lightDivider.getDivision());
		lights[1].setSmoothBrightness(inputs[1].getVoltage() / 10, args.sampleTime * lightDivider.getDivision());
		//Lights Bottom
		lights[2].setSmoothBrightness(inputs[2].getVoltage() / 10, args.sampleTime * lightDivider.getDivision());
		lights[3].setSmoothBrightness(inputs[3].getVoltage() / 10, args.sampleTime * lightDivider.getDivision());
		//Goes through the top two then bottom two in and outs
		for (int i = 0; i < 4; i+=2)
		{
			if(inputs[i].isConnected() || inputs[i+1].isConnected())
			{
				bool boolParam = params[i].getValue(); 
				bool boolGate = params[i+1].getValue();
				float outA = inputs[i].getVoltage();
				float outB = inputs[i+1].getVoltage();

				if(boolGate && boolParam)
				{
					float tempA = (outA > outB + 3.f or outA < outB - 3.f) ? outA : outB;
					float tempB = (outB > outA + 3.f or outB < outA - 3.f) ? outB : outA;
					
					bool gate = (tempA >= tempB);

					outputs[i].setVoltage(gate*10.f);
					outputs[i+1].setVoltage((not gate)*10.f);
				}
				else if(boolGate)
				{
					bool gate = (outA >= outB);
					outputs[i].setVoltage(gate*10.f);
					outputs[i+1].setVoltage((not gate)*10.f);
				}
				else if(boolParam)
				{
					outputs[i].setVoltage((outA > outB + 3.f or outA < outB - 3.f) ? outA : outB);
					outputs[i+1].setVoltage((outB > outA + 3.f or outB < outA - 3.f) ? outB : outA);
				}
				else 
				{
					if(outA > outB)
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


struct Vena_comparator2Widget : ModuleWidget {
	Vena_comparator2Widget(Vena_comparator2* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/vena_comparator2.svg")));

	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<Orange_Switch>(mm2px(Vec(13.142, 8.278)), module, Vena_comparator2::SWITCH_TOP_LATCH_PARAM));
		addParam(createParam<Orange_Switch>(mm2px(Vec(13.142, 21.386)), module, Vena_comparator2::SWITCH_TOP_GATE_PARAM));
		addParam(createParam<Orange_Switch>(mm2px(Vec(13.142, 70.06)), module, Vena_comparator2::SWITCH_BOTTOM_LATCH_PARAM));
		addParam(createParam<Orange_Switch>(mm2px(Vec(13.142, 83.168)), module, Vena_comparator2::SWITCH_BOTTOM_GATE_PARAM));

		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 10.633)), module, Vena_comparator2::INPUT_A_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 23.747)), module, Vena_comparator2::INPUT_B_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 72.415)), module, Vena_comparator2::INPUT_C_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 85.529)), module, Vena_comparator2::INPUT_D_INPUT));

		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 39.262)), module, Vena_comparator2::OUTPUT_A_OUTPUT));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 47.182)), module, Vena_comparator2::OUTPUT_B_OUTPUT));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 101.044)), module, Vena_comparator2::OUTPUT_C_OUTPUT));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 108.964)), module, Vena_comparator2::OUTPUT_D_OUTPUT));

		addChild(createLightCentered<TinyLight<GreenRedLight>>(mm2px(Vec(7.62, 32.586)), module, Vena_comparator2::TOP_LIGHT));
		addChild(createLightCentered<TinyLight<GreenRedLight>>(mm2px(Vec(7.62, 94.368)), module, Vena_comparator2::BOTTOM_LIGHT));
	}
};


Model* modelVena_comparator2 = createModel<Vena_comparator2, Vena_comparator2Widget>("vena_comparator2");