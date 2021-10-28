#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	// p->addModel(modelMyModule);
//	p->addModel(modelVena_lfo);
	p->addModel(modelComparator_one);
	p->addModel(modelComparator_two);
	p->addModel(modelClock_divider_one);

//	p->addModel(modelVena_comparator2);
//	p->addModel(modelVena_drum);
	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
