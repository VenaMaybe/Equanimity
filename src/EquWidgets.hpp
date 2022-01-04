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

	//=-=-=-=-=-=-=-=-=-=-=-=-----

	/*struct ModuleWidgetEqu : ModuleWidget {
	void setBackground(std::shared_ptr<Svg> svg);
	void setPanel(widget::Widget* panel);
	void setPanel(std::shared_ptr<Svg> svg);
	//widget::Widget* panel = NULL;
};*/

struct SvgPanelCustomBg : widget::Widget {
	widget::FramebufferWidget* fb;
	widget::SvgWidget* sw;
	//PanelBorder* panelBorder;
	std::shared_ptr<window::Svg> svg;

	SvgPanelCustomBg();
	void step() override;
	void setBackground(std::shared_ptr<window::Svg> svg);
};

inline SvgPanelCustomBg::SvgPanelCustomBg() {
	fb = new widget::FramebufferWidget;
	addChild(fb);

	sw = new widget::SvgWidget;
	fb->addChild(sw);

	//panelBorder = new PanelBorder;
	//fb->addChild(panelBorder);
}


inline void SvgPanelCustomBg::step() {
	if (APP->window->pixelRatio < 2.0) {
		// Small details draw poorly at low DPI, so oversample when drawing to the framebuffer
		fb->oversample = 2.0;
	}

	Widget::step();
}

inline void SvgPanelCustomBg::setBackground(std::shared_ptr<window::Svg> svg) {
	this->svg = svg;

	sw->setSvg(svg);
	fb->box.size = sw->box.size.div(RACK_GRID_SIZE).round().mult(RACK_GRID_SIZE);
	//panelBorder->box.size = fb->box.size;
	box.size = fb->box.size;
}

//beep boop

struct ModuleWidgetEqu : ModuleWidget {

	
	//void setPanel(widget::Widget* panel);


	//using ModuleWidget::setPanel;
	void setPanelNoBg(std::shared_ptr<window::Svg> svg) {
		// Create SvgPanel
		SvgPanelCustomBg* panel = new SvgPanelCustomBg;
		panel->SvgPanelCustomBg::setBackground(svg);
		ModuleWidget::setPanel(panel);
		DEBUG("ModuleWidget used");
	}
};













struct ModuleWidgetEqu2 : ModuleWidget {
	void setBackground(std::shared_ptr<Svg> svg) {
	widget::SvgWidget* sw = new widget::SvgWidget;
	sw->setSvg(svg);
	addChild(sw);

	// Set size
	box.size = sw->box.size.div(RACK_GRID_SIZE).round().mult(RACK_GRID_SIZE);

	DEBUG("mine was used");

	//PanelBorder* pb = new PanelBorder;
	//pb->box.size = box.size;
	//addChild(pb);
	}

	//void setPanel(widget::Widget* panel);
	widget::Widget* panel = NULL;
	void setPanel(std::shared_ptr<Svg> svg) {
		DEBUG("mine was used");
	// Remove existing panel
	if (panel) {
		removeChild(panel);
		delete panel;
		panel = NULL;
	}

	// Create SvgPanel
	ModuleWidgetEqu2* svgPanel = new ModuleWidgetEqu2;
	svgPanel->setBackground(svg);
	panel = svgPanel;
	addChildBottom(panel);

	// Set ModuleWidget size based on panel
	box.size.x = std::round(panel->box.size.x / RACK_GRID_WIDTH) * RACK_GRID_WIDTH;
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

		//Sets the from left over buffer to zero (Check why -1)
		void setUnused(int bufferSizeCurrent) {
			float fillWith = 0.f;
			//from true beginning to imaginary beginning
				std::fill(buffer.get(), getSafePtr(clockHand - 1), fillWith);
			//from imaginary ending to true ending
				std::fill(getSafePtr(clockHand + bufferSizeCurrent), buffer.get() + bufferSize, fillWith);

			//Notes
				//Rememeber that the imaginary beginning or ending can 
				// be equal to it's true counterpart!!!

		}

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
		double bufferSum = 0.f;

		unsigned int i = 2;
			//Data buffers
		RingBuffer buffer{bufferSizeMax};
			//Constructor
		MovingAverage(unsigned int dataSize)
		:	bufferSizeMax(dataSize) {}
			//Function
		double filter(double signalIn, unsigned int desiredBufferSizeCurrentIn);
		double filter(double signalIn) {
			return filter(signalIn, bufferSizeMax);
		};
			//maybe make unsigned later
		//void setCurrentSize(int desiredBufferSizeCurrentIn);
	};	
		//A four pass moving average filter
	struct MovingAverageFourPass {
			//Data
		const unsigned int bufferSizeMax;
			//Data buffers
		MovingAverage filterA[4]{bufferSizeMax,bufferSizeMax,bufferSizeMax,bufferSizeMax};
		double filterOut[4];
			//Constructor
		MovingAverageFourPass(unsigned int dataSize)
		:	bufferSizeMax(dataSize) {}
			//Function
		double filter(double signalIn, unsigned int desiredBufferSizeCurrentIn);// add passCount
	};

		//A single pass moving average filter weighed via a sin()
	struct SmoothSin {
			//Data
		const unsigned int bufferSizeMax;
		double rMSSum = 0.f;
			//Data buffers
		MovingAverage MAfilter{bufferSizeMax};
		RingBuffer bufferRMS{bufferSizeMax};
			//Constructor
		SmoothSin(unsigned int dataSize)
		:	bufferSizeMax(dataSize) {}
			//Function
		double filter(double signalIn, unsigned int desiredBufferSizeCurrentIn);
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
	struct SlopeSmoothDelay {
			//Function
		//float slopeSmooth(float signalIn, const Module::ProcessArgs& args, float riseIn = 0.f, float fallIn = 0.f);
			//Data
		float out = 0.f;					//Output!
		float previousIn = 0.f;
		float previousOut = 0.f;

		static const int stateSize = 30;	//Slew Ammount, also delay!
		float rate = 0.01;					//Slice of input!
		float state[stateSize] = {0};
	};
	*/
	//The constructor determins the max size of the potential Slew! 
	/*
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
		float slopeSmooth(float signalIn, const Module::ProcessArgs& args, float riseIn = 0.f, float fallIn = 0.f);
	};
	*/