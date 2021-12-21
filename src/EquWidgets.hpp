#pragma once

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <rack.hpp>
#include <cmath>
#include "Equanimity.hpp"

using namespace rack;

//VCV OVERRIDING:
		//TODO: Rename and make it easier to set switch cases
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

//DATA STRCTURES:
		//Ring buffer
	struct RingBuffer {
			//Data
		std::unique_ptr<float[]> buffer;
		const unsigned int bufferSize;
		unsigned int clockHand = 0;
			//Constructor
		RingBuffer(unsigned int dataSize) 
		:	buffer(std::unique_ptr<float[]>(new float[dataSize])),
			bufferSize(dataSize)
		{
			std::fill(buffer.get(), buffer.get() + bufferSize, 0.0f);
		}
			//Functions
		float& operator[](int loc) {
			loc = clockHand - loc;
			int outLoc;
			
			outLoc = loc;
			if(loc < 0) {
				outLoc = bufferSize + loc;
			};

			float& out = buffer[outLoc];
			return out;
		}
			//just make a funciton that returns the locaiton of a 
			//pointer to there in the data structure

		//Returns a pointer to location in "imaginary" array 
		float* getSafePtr(int loc) { //TODO YOU STILL HAVE TO MAKE IT SAFE!!
			loc = clockHand - loc;
			int outLoc;

			outLoc = loc;
			if(loc < 0) {
				outLoc = bufferSize + loc;
			};

			float* outPtr = buffer.get() + outLoc;
			return outPtr;
		}

		void setUnused(int length) {
			float fillWith = 0.f;
			//from true beginning to imaginary beginning
				std::fill(buffer.get(), getSafePtr(clockHand - 1), fillWith);
			//from imaginary ending to true ending
				std::fill(getSafePtr(clockHand + length), buffer.get() + bufferSize, fillWith);

			//Notes
				//Rememeber that the imaginary beginning or ending can 
				// be equal to it's true counterpart!!!

		}

		/*
			//Returns pointer to beginning of host array
		float* begin() {
			float* outLoc;

			outLoc = buffer.get();
			DEBUG("buffer.get() 			%p", buffer.get());
			DEBUG("outLoc 					%p", outLoc);
			DEBUG("bufferSize 				%i", bufferSize);
			DEBUG("sizeof(float) 			%i", sizeof(float));
			DEBUG("sizeof(bufferSize) 		%i", sizeof(bufferSize));
			DEBUG("sizeof(buffer) 			%i", sizeof(buffer));
			DEBUG("outLoc + bufferSize		%p", outLoc + bufferSize);
			DEBUG("outLoc + bufferSize		%p", outLoc + bufferSize);

			return outLoc - (sizeof(float) * 4 );
		}
			//Returns pointer to ending of host array
		float* end() {
		//	int outLoc;
		//	outLoc = bufferSize + clockHand;
		//	if(outLoc > bufferSize) {
		//		outLoc = outLoc - bufferSize;
		//	}
		//	outLoc -= 1;

			return buffer.get() + bufferSize;
		}
		*/

			//Rotates the "imaginary" overlayed array
		void rotate(int amt = 1) {
			clockHand += amt;
			if(clockHand > bufferSize - 1) {
				clockHand = clockHand - bufferSize;
			}
		}

			//Notes
		//Remember our array will start at 0 first, so wherever clockHand points
		//will be the start of the circular array!
		/*			So baiscally we want a circular access array which when we get to the end
		whether we write to or read from it at any point it can loop back on itself?

			So first we just need to have an access point in the array which corrisponds 
		to it's offset possition. From that point we can both read and write.

			Then we can have a "shift" function which will shift the array which we
		will probably call ever sample?

			I just have to understand how to correctly impliment the wrapping, perhaps
		without the modulus

		NEGATIVE ONE IF OVER ZERO OF THE DIFFERENCE
		*/
	};

//FILTERS:
		//A single pass moving average filter
	struct MovingAverage {
			//Data
		const unsigned int bufferSizeMax;
		unsigned int bufferSizeCurrent = bufferSizeMax;
		float bufferSum = 0.f;

		unsigned int i = 2;
			//Data buffers
		RingBuffer buffer{bufferSizeMax};
			//Constructor
		MovingAverage(unsigned int dataSize)
		:	bufferSizeMax(dataSize) {}
			//Function
		float filter(float signalIn);
		void setCurrentSize(unsigned int sizeIn);
	};
		//A four pass moving average filter
	struct MovingAverageFourPass {
			//Data
			//Data buffers
			//Constructor
			//Function
		float filter();
	};

		//A single pass moving average filter weighed via a sin()
	struct SmoothSin {
			//Data
			//Data buffers
			//Constructor
			//Function
		float filter();
	};

//TESTING - ACCESS:


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

/*
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
	//The constructor determins the max size of the potential Slew! 
	struct SlopeSmoothStack {
			//Data
		float out = 0.f;
		const unsigned int bufferSize;
		const unsigned int bufferRMSSize = bufferSize;
		float bufferSum = 0.f;
		float rMSSum = 0.f;
			//Data buffers
		RingBuffer bA{bufferSize}; //Buffer Test
		RingBuffer bRMS{bufferRMSSize}; //Root Mean Square Average
			//Constructor
		SlopeSmoothStack(unsigned int dataSize)
		:	bufferSize(dataSize) {}
			//Function
		float slopeSmooth(float& TESTOUT, float signalIn, const Module::ProcessArgs& args, float riseIn = 0.f, float fallIn = 0.f);
	};

	struct SlopeSmoothStack {

	};
};

*/