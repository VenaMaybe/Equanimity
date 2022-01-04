#include "Equanimity.hpp"

struct Reflect : Module
{
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_A,
		INPUT_B,
		INPUT_C,
		INPUT_D,
		NUM_INPUTS
	};
	enum OutputIds {
		OUTPUT_A,
		OUTPUT_B,
		OUTPUT_C,
		OUTPUT_D,
		NUM_OUTPUTS
	};
	enum LightIds {
		TOP_LIGHT_LIGHT,
		BOTTOM_LIGHT_LIGHT,
		NUM_LIGHTS
	};

	//--- Variables ----//
	float high = 0.f; //A and C
	float low = 0.f;  //B and D

	Reflect() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs& args) override
	{
		//TODO Impliment light bars in v2!
		for(int i = 0; i < 2; i ++) {
			high = inputs[INPUT_A + (i * 2)].getVoltage();
			low = inputs[INPUT_B + (i * 2)].getVoltage();

			outputs[OUTPUT_A + (i * 2)].setVoltage(high * (high > low) + low * (high < low));
			outputs[OUTPUT_B + (i * 2)].setVoltage(high * (low > high) + low * (low < high));
		}
	}
};

struct ModDisplay : TransparentWidget {
	Reflect* module;

	TriangleThingi* svgTriangle;

	ModDisplay() {
		svgTriangle = new TriangleThingi();
//		svgTriangle->box.pos = mm2px(Vec(0, 0));
//		addChild(svgTriangle);
	}

	~ModDisplay() {
			//WILL HAVE TO CHANGE IN V2!!!
		event::Remove eRemove;
		svgTriangle->onRemove(eRemove);
		delete svgTriangle;
	}

	

	void drawBox(const DrawArgs& args) {

	//	Vec box = Vec(box.size.x, b.pos.y + b.size.y);

		nvgBeginPath(args.vg);
		nvgStrokeColor(args.vg, nvgRGB(255,100,0));
		nvgRect(args.vg, rescale(module->inputs[module->INPUT_A].getVoltage(), -10, 10, 40, 0), 0, 5, 10);
		nvgStroke(args.vg);
		

		

		
		
		
		//loadSvg(mm2px(Vec(0, 55.06)));

		//SvgWidget

	}
	int i = 0;
	void draw(const DrawArgs& args) override {
		if(!module)
			return;

		drawBox(args);

		svgTriangle->box.pos = mm2px(Vec(rescale(module->inputs[module->INPUT_B].getVoltage(), -10, 10, 40, 0), 30));

		DrawArgs childCtx = args;
	//	DrawArgs thingi = args;
		// Intersect child clip box with self
//		childCtx.clipBox = childCtx.clipBox.intersect(svgTriangle->box);
//		childCtx.clipBox.pos = childCtx.clipBox.pos.minus(svgTriangle->box.pos);

		nvgTranslate(childCtx.vg, svgTriangle->box.pos.x, svgTriangle->box.pos.y);

		i++;
		if(i > 50) i = 0;

		
		svgTriangle->draw(childCtx);
		


	}
	/*
	void addMD(ModuleWidget* modWidget) {
		modWidget->addChild(svgTriangle);
	};*/
};

//ResetMenuItem *resetItem = createMenuItem<ResetMenuItem>("Water", RIGHT_ARROW);
//resetItem->module = reinterpret_cast<Lilies*>(this->module);
//menu->addChild(resetItem);

//Dawn_Slider_One* dawn_slider_one = createParam<Dawn_Slider_One>(mm2px(Vec(6.944 - 1.65, 47.731)), module, Lilies::RATIO_PARAM);
//multiRangeParam = reinterpret_cast<MultiRangeParam*>(dawn_slider_one->paramQuantity);

struct ReflectWidget : ModuleWidget {
	
		//assert(reflectWidget);
	//ReflectWidget* reflectWidget = reinterprete_cast<ReflectWidget*>(this->);

	ReflectWidget(Reflect* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/reflect_default.svg")));

	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	//	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	//	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		{
			//A pointer to the constructor of the display
			ModDisplay* display = new ModDisplay();
			//A pointer to the pointer to our module in ModDisplay struct
			display->module = module;
			display->box.pos = mm2px(Vec(0, 55.06));
			display->box.size = Vec(box.size.x, 140);
			//if(module) {
			//	display->addMD(this);
				
			//}
			addChild(display);
			
			
			
			//addChild(display->svgTriangle);

	//		TriangleThingi* svgTriangleT = new TriangleThingi();
	//		addChild(svgTriangleT);
	//		svgTriangleT->box.pos = mm2px(Vec(0, 55.06));
		}

		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 10.633)), module, Reflect::INPUT_A));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 23.747)), module, Reflect::INPUT_B));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 72.415)), module, Reflect::INPUT_C));
		addInput(createInputCentered<Orange_In>(mm2px(Vec(7.62, 85.529)), module, Reflect::INPUT_D));

		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 39.262)), module, Reflect::OUTPUT_A));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 47.182)), module, Reflect::OUTPUT_B));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 101.044)), module, Reflect::OUTPUT_C));
		addOutput(createOutputCentered<Orange_Out>(mm2px(Vec(7.62, 108.964)), module, Reflect::OUTPUT_D));

	//	addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(Vec(7.62, 32.586)), module, Reflect::TOP_LIGHT_LIGHT));
	//	addChild(createLightCentered<TinyLight<GreenLight>>(mm2px(Vec(7.62, 94.368)), module, Reflect::BOTTOM_LIGHT_LIGHT));
	}
};


Model* modelReflect = createModel<Reflect, ReflectWidget>("Reflect");