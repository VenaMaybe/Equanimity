#include "plugin.hpp"

struct Comparator_one : Module
{
	enum ParamIds {
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

	Comparator_one() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs& args) override
	{
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

	}
};


struct Comparator_oneWidget : ModuleWidget {
	Comparator_oneWidget(Comparator_one* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/comparator_one.svg")));

	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 10.633)), module, Comparator_one::INPUT_A_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 23.747)), module, Comparator_one::INPUT_B_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 72.415)), module, Comparator_one::INPUT_C_INPUT));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 85.529)), module, Comparator_one::INPUT_D_INPUT));

		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 39.262)), module, Comparator_one::OUTPUT_A_OUTPUT));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 47.182)), module, Comparator_one::OUTPUT_B_OUTPUT));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 101.044)), module, Comparator_one::OUTPUT_C_OUTPUT));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 108.964)), module, Comparator_one::OUTPUT_D_OUTPUT));

		addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(Vec(7.62, 32.586)), module, Comparator_one::TOP_LIGHT_LIGHT));
		addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(Vec(7.62, 94.368)), module, Comparator_one::BOTTOM_LIGHT_LIGHT));
	}
};


Model* modelComparator_one = createModel<Comparator_one, Comparator_oneWidget>("comparator_one");