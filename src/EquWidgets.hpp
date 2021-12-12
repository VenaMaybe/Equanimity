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
	float outBuffer = 0.f;

	//slopeSmooth
	const int outBufferASize = 100;
	float outBufferA[100];
	float outBufferAdded = 0.f;

	float outBufferL = 0.f;
	float velocity = 0.f;
	float acceleration = 0.f;

	int time = 0;




    //float riseCV = 0.f;
	//float fallCV = 0.f;

	float slewLimit(float signalIn, const Module::ProcessArgs& args, float riseIn = 0.f, float fallIn = 0.f);
	float slopeSmooth(float signalIn, const Module::ProcessArgs& args, float riseIn = 0.f, float fallIn = 0.f);



		//Returns the sign of the delta of signalIn and the past sample!
	int slopeDetector(float signalIn);	
};