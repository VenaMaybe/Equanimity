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

struct Buffer
{
	static const unsigned int bufferSize = 500;
	float buffer[bufferSize] = {0};

	/*			So baiscally we want a circular access array which when we get to the end
			whether we write to or read from it at any point it can loop back on itself?

				So first we just need to have an access point in the array which corrisponds 
			to it's offset possition. From that point we can both read and write.

				Then we can have a "shift" function which will shift the array which we
			will probably call ever sample?

				I just have to understand how to correctly impliment the wrapping, perhaps
			without the modulus



	*/




/*
	

	unsigned int location = 0; //kinda
	unsigned int currentLocation = 0;

		//I would be accessing these with negative values?
	float read(int where) {
		return buffer[location + where];
	}

	void write(int where) {
		buffer[location + where];
	}

	void clock() {
		location++;
		if(location > (bufferSize - 1)) {
			location = 0;
		}
	}
*/


/*
	float operator[](unsigned int inLoc) {
		return buffer[inLoc];


		//phase = 0 * (phase > (bufferSize - 1));
	}

	void clock() {
		if(location > bufferSize - 1)  {

		}
	}
*/
	



	//an access function!
	//give it an int and - that from the offset?
	//if it's below zero add the size of the array




	/*
	float data[50];
	unsigned int buffersize = 50;
	unsigned int offset = 0;

	float& operator[](unsigned int index)
	{
		return data[(offset+index)%buffersize];
	}
	void shift()
	{
	offset++;
	offset%=buffersize;
	}
	*/
};

struct SlewLimiter {
	struct SlopeSmoothDelay {
			//Function
		float slopeSmooth(float signalIn, const Module::ProcessArgs& args, float riseIn = 0.f, float fallIn = 0.f);
			//Data
		float out = 0.f;					//Output!
		float previousIn = 0.f;
		float previousOut = 0.f;

		static const int stateSize = 30;	//Slew Ammount, also delay!
		float rate = 0.01;					//Slice of input!
		float state[stateSize] = {0};
	};
	struct SlopeSmoothStack {
			//Function
		float slopeSmooth(float signalIn, const Module::ProcessArgs& args, float riseIn = 0.f, float fallIn = 0.f);
			//Data
		float out = 0.f;
		static const unsigned int bufferSize = 500;
		float bufferA[bufferSize] = {0};	float bufferASum = 0.f;
		float bufferC[bufferSize] = {0};	float bufferCSum = 0.f;
		float bufferB[bufferSize] = {0};	float bufferBSum = 0.f;
		float bufferD[bufferSize] = {0};	float bufferDSum = 0.f;
			//Data for second try
		Buffer bT; //Buffer Test
	};

	//slopeSmoothData sS[5];








	float outBuffer = 0.f;

	

	//old
	const int outBufferASize = 500;
	float outBufferA[500] = {};
	float outBufferAdded = 0.f;

	float outBufferB = 0.f;
	

	float outBufferL = 0.f;
	float velocity = 0.f;
	float acceleration = 0.f;

	int time = 0;




    //float riseCV = 0.f;
	//float fallCV = 0.f;

	float slewLimit(float signalIn, const Module::ProcessArgs& args, float riseIn = 0.f, float fallIn = 0.f);
	



		//Returns the sign of the delta of signalIn and the past sample!
	int slopeDetector(float signalIn);	
};