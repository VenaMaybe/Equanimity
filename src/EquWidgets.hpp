#pragma once

#include <rack.hpp>
#include "Equanimity.hpp"

using namespace rack;


struct MultiRangeParam : ParamQuantity {
	//selector to make checkmarks :>
	int rangeSelection;

	void setRange(int rangeChoice, bool normalize = true) {
		rangeSelection = rangeChoice;
		float normalized;
		if(normalize) {
			normalized = rescale(getValue(), minValue, maxValue, 0, 1);
		}
		switch(rangeChoice) {
				case 0 :
					
					minValue = -0.5f;
					maxValue = 0.5f;
					
				break;
				case 1 :
					
					minValue = -2.f;
					maxValue = 2.f;
					
				break;
				case 2 :
					
					minValue = -10.f;
					maxValue = 10.f;
					
				break;
		}
		if(normalize) {
			setValue(rescale(normalized, 0, 1, minValue, maxValue));
		}
	}
};

struct SlewLimiter {
	float slewLimiter(float signalIn, float signalOut, const Module::ProcessArgs& args, float riseIn = 0.f, float fallIn = 0.f);


	void foo();

};