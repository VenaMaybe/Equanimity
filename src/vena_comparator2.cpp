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
		TOP_LIGHT_LIGHT,
		BOTTOM_LIGHT_LIGHT,
		NUM_LIGHTS
	};

	Vena_comparator2() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SWITCH_TOP_LATCH_PARAM, 0.f, 1.f, 0.f, "Latch");
		configParam(SWITCH_TOP_GATE_PARAM, 0.f, 1.f, 0.f, "Gate");
		configParam(SWITCH_BOTTOM_LATCH_PARAM, 0.f, 1.f, 0.f, "Latch");
		configParam(SWITCH_BOTTOM_GATE_PARAM, 0.f, 1.f, 0.f, "Gate");
	}

	void process(const ProcessArgs& args) override 
	{
		

		for (int i = 0; inputs[i].isConnected() && i < 4; i+=2)
		{
			bool boolParam = params[i].getValue(); 
			bool boolGate = params[i+1].getValue();
			float outA = inputs[i].getVoltage();
			float outB = inputs[i+1].getVoltage();
			
			//only on when gate on
			
			
			if(boolGate)
			{
				bool gate=(outA>outB);
				outputs[i].setVoltage(gate*10.f);
				outputs[i+1].setVoltage((not gate)*10.f);
			}
			else if(boolParam)
			{
				outputs[i].setVoltage((outA > outB + 3.f or outA < outB - 3.f) ? outA : outB);
			}
			else 
			{
				if(outA > outB)
				{
				outputs[i].setVoltage(outA);
				outputs[i+1].setVoltage(outB);
				lights[i/2+1].setBrightness(1.f);
				} 
				else 
				{
				outputs[i].setVoltage(outB);
				outputs[i+1].setVoltage(outA);
				lights[i/2+1].setBrightness(-1.f);
			}
			};
		};

/*
		if (inputs[INPUT_A_INPUT].isConnected() || inputs[INPUT_B_INPUT].isConnected())
		{



			float outA = inputs[INPUT_A_INPUT].getVoltage();
			float outB = inputs[INPUT_B_INPUT].getVoltage();
			if  (outA > outB)
			{
				outputs[OUTPUT_A_OUTPUT].setVoltage(outA);
				outputs[OUTPUT_B_OUTPUT].setVoltage(outB);
				lights[TOP_LIGHT_LIGHT].setBrightness(1.f);
			} else {
				outputs[OUTPUT_A_OUTPUT].setVoltage(outB);
				outputs[OUTPUT_B_OUTPUT].setVoltage(outA);
				lights[TOP_LIGHT_LIGHT].setBrightness(-1.f);
			}

		} else {
			lights[TOP_LIGHT_LIGHT].setBrightness(-1.f);
		}

		if (inputs[INPUT_C_INPUT].isConnected() || inputs[INPUT_D_INPUT].isConnected())
		{
			float outC = inputs[INPUT_C_INPUT].getVoltage();
			float outD = inputs[INPUT_D_INPUT].getVoltage();

			if  (outC > outD)
			{
				outputs[OUTPUT_C_OUTPUT].setVoltage(outC);
				outputs[OUTPUT_D_OUTPUT].setVoltage(outD);
				lights[BOTTOM_LIGHT_LIGHT].setBrightness(1.f);
			} else {
				outputs[OUTPUT_C_OUTPUT].setVoltage(outD);
				outputs[OUTPUT_D_OUTPUT].setVoltage(outC);
				lights[BOTTOM_LIGHT_LIGHT].setBrightness(-1.f);
			}

		} else {
			lights[BOTTOM_LIGHT_LIGHT].setBrightness(-1.f);
		}
		*/
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

		addParam(createParam<CKSS>(mm2px(Vec(13.142, 8.278)), module, Vena_comparator2::SWITCH_TOP_LATCH_PARAM));
		addParam(createParam<CKSS>(mm2px(Vec(13.142, 21.386)), module, Vena_comparator2::SWITCH_TOP_GATE_PARAM));
		addParam(createParam<CKSS>(mm2px(Vec(13.142, 70.06)), module, Vena_comparator2::SWITCH_BOTTOM_LATCH_PARAM));
		addParam(createParam<CKSS>(mm2px(Vec(13.142, 83.168)), module, Vena_comparator2::SWITCH_BOTTOM_GATE_PARAM));

		addInput(createInputCentered<Vena_in>(mm2px(Vec(7.62, 10.633)), module, Vena_comparator2::INPUT_A_INPUT));
		addInput(createInputCentered<Vena_in>(mm2px(Vec(7.62, 23.747)), module, Vena_comparator2::INPUT_B_INPUT));
		addInput(createInputCentered<Vena_in>(mm2px(Vec(7.62, 72.415)), module, Vena_comparator2::INPUT_C_INPUT));
		addInput(createInputCentered<Vena_in>(mm2px(Vec(7.62, 85.529)), module, Vena_comparator2::INPUT_D_INPUT));

		addOutput(createOutputCentered<Vena_out>(mm2px(Vec(7.62, 39.262)), module, Vena_comparator2::OUTPUT_A_OUTPUT));
		addOutput(createOutputCentered<Vena_out>(mm2px(Vec(7.62, 47.182)), module, Vena_comparator2::OUTPUT_B_OUTPUT));
		addOutput(createOutputCentered<Vena_out>(mm2px(Vec(7.62, 101.044)), module, Vena_comparator2::OUTPUT_C_OUTPUT));
		addOutput(createOutputCentered<Vena_out>(mm2px(Vec(7.62, 108.964)), module, Vena_comparator2::OUTPUT_D_OUTPUT));

		addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(Vec(7.62, 32.586)), module, Vena_comparator2::TOP_LIGHT_LIGHT));
		addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(Vec(7.62, 94.368)), module, Vena_comparator2::BOTTOM_LIGHT_LIGHT));
	}
};


Model* modelVena_comparator2 = createModel<Vena_comparator2, Vena_comparator2Widget>("vena_comparator2");