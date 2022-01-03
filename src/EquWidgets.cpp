#include "EquWidgets.hpp"

/*
float SlewLimiter::slewLimit(float signalIn, const Module::ProcessArgs& args, float riseIn, float fallIn) {

    //TODO, I might try implimenting my own slew limiter from scratch.
    //I kinda get the idea of what I have to do, dialing the range though
    //is going to be the hardest part!! And implimenting smoothing, which I'm
    //not sure if can be done in real time? Maybe it can be with memory of past
    //samples!! As in the average difference bettween previous samples will cause
    //a decreese in the current sample based on the previous one?


    //DEBUG("FUNCTION ----------------");
    //DEBUG("sampleTime   %f", args.sampleTime);
    //DEBUG("signalIN     %f", signalIn);
    //DEBUG("outBuffer    %f", outBuffer);
    
    float rise = 0.f;
    float fall = 0.f;

    rise += riseIn * 10;

    //DEBUG("rise += %f", rise);

    float delta = signalIn - outBuffer;

    //DEBUG("delta    %f", delta);


    float rateCV;
    rateCV = delta > 0.f ? rise : 0.f;
    rateCV = delta < 0.f ? rise : rateCV; //fix this for testing
    rateCV *= 0.1;

    float slew = 10000.f * pow(0.1f / 10000.f, rateCV);

    outBuffer += slew * sgn(delta) * args.sampleTime;

    //DEBUG("outBuffer += %f", outBuffer);
    
    outBuffer = (delta > 0.f && outBuffer > signalIn) ? signalIn : outBuffer;

    //DEBUG("outBuffer 1 %f", outBuffer);

    outBuffer = (delta < 0.f && outBuffer < signalIn) ? signalIn : outBuffer;

    //DEBUG("outBuffer 2 %f", outBuffer);

    return outBuffer;

    //outBuffer = signalIn;
    

   // bool deltaGTZero = delta > 0.f;

    //SlewLimiter::outBuffer = simd::ifelse(outBuffer > signalIn, signalIn, outBuffer);
    
};
*/

/*
float SlopeSmoothDelay::slopeSmooth(float signalIn, const Module::ProcessArgs& args, float riseIn, float fallIn) {
    //can maybe use crossfade later?
        //new signal in doesn't change state as much, which the input has less effect
        //This means it'd take like 10 signal ins to equal the current value of signal in?
    state[0] = (rate * signalIn + (1 - rate) * state[0]);
    for(int i = 1; i < stateSize; i++) {
        state[i] = (rate * state[i - 1] + (1 - rate) * state[i]);
    }

    float newOut    = (signalIn + previousIn) / 2;
    float newOutTwo = (newOut + previousOut) / 2;

    previousIn = signalIn;
    previousOut = newOutTwo;

    out = newOutTwo;
    
    return state[stateSize - 1];
}
*/

//float SlopeSmoothStack::slopeSmooth(float signalIn, const Module::ProcessArgs& args, float riseIn, float fallIn) {
//pre shift it up and then shift it back down to get the pointy stuff
/*        //Keeping the moving Sum stable!
    bufferASum -= bufferA[bufferSize - 1];
        //Shifting buffer back by one "float"
    std::memmove(bufferA + 1, bufferA, (bufferSize - 1) * sizeof(float));
        //Insertion into first "waterfall"
    bufferA[0] = signalIn;
        //Creating what the filter kernal will effect, the moving Sum
    bufferASum += bufferA[0];
        //This is our convolution? Our filter kernal I believe!
float outA = bufferASum / bufferSize;*/
/*
std::memmove(bufferA + 1, bufferA, (bufferSize - 1) * sizeof(float));
bufferA[0] = signalIn;

float p = (bufferSize - 1)/2;
float q = p + 1;



float outA = bufferA[1] +  - ;
previousOutA = outA;


    bufferASum -= bT.data[49];
    bT.shift();
    bT.data[0] = signalIn;
    bufferASum += bT.data[0];
*/
/*
    float outA = 0.f;

    bufferASum -= bA[bufferSize-1];
    bA[0] = signalIn;
    bufferASum += bA[0];

    float filterFast = bufferASum / bufferSize;

    float weight = exp(-fallIn*pow(signalIn - outA, 2)) * exp(-fallIn*pow(outA - filterFast, 2));

    outA += riseIn * weight * (signalIn - outA) - 0.0001*outA;
    bA.rotate(1);
*/


// //exp();
// //float passThroughB
// //float passThroughA = (bufferASum / 500) - (signalIn);


// bufferSum -= bA[bufferSize - 1];
// bA[0] = signalIn;
// bufferSum += bA[0];
//                                                     //DEBUG("signalIn: %f", signalIn);

// float mAFilter = 0.f;


// //TESTING FOR NAN
// /*
// if(0.0001f > signalIn && signalIn > -0.0001f) {
//     mAFilter = 0.0001f;
//     rMS = 1.f;
// }*/
// //------

// mAFilter = bufferSum / bufferSize;
//                                                     //DEBUG("mAFilter: %f", mAFilter);
// float rMS = 0.f;

// rMSSum -= bRMS[bufferRMSSize - 1];
// bRMS[0] = signalIn * signalIn;
// rMSSum += bRMS[0];
//                                                     //DEBUG("rMSSum: %f", rMSSum);

// if(rMSSum > 0.0f) {
//                                                         //DEBUG("rMSSum / bufferRMSSize: %f", rMSSum / bufferRMSSize);
//     rMS = sqrt(rMSSum / bufferRMSSize);
//                                                         //DEBUG("rMS 0: %f", rMS);
// } else {
//     rMS = 0.0001f;
//                                                         //DEBUG("nanCatch1");
// }
//                                                     //DEBUG("rMS 1: %f", rMS);

// //rMS += 0.000001;

// float weight = 0.f; //create tracking statement that changes the denominator
// //                       So basically the 24 has to change to 2 * current max amplitude!
// //                       So I have to calculate (using the buffer for sure) the max
// //                       amplitude and then base the denominator off that!
// //                       I can't think of an easier way to do this sadly!!
// //                          https://www.desmos.com/calculator/7by26dxibp
// //
// //                       Pretty much the period of the sinewave has to be equal to 
// //                       the amplitude of the incoming signal * 2!!!
// weight = sin(mAFilter * M_PI * (1.f/(rMS * 2.f)) /*fallIn*/);
// //weight += 0.000001;
//                                                     //DEBUG("weight: %f", weight);
//                                                     //DEBUG("rMS 2: %f", rMS);

// //~'~'~'~'~'~'~//
// float outA = 0.f;
// outA = weight * rMS;



// //TESTOUT = rMS;

// bA.rotate(1);
// bRMS.rotate(1);




//         //Keeping the moving Sum stable!
// //    bufferASum -= bufferA[bufferSize - 1];
//         //Shifting buffer back by one "float"
// //    std::memmove(bufferA + 1, bufferA, (bufferSize - 1) * sizeof(float));
//         //Insertion into first "waterfall"
// //    bufferA[0] = signalIn;
//         //Creating what the filter kernal will effect, the moving Sum
// //    bufferASum += bufferA[0];
//         //This is our convolution? Our filter kernal I believe!

// /*
//         //Keeping the moving Sum stable!
//     bufferBSum -= bufferB[bufferSize - 1];
//         //Shifting buffer back by one "float"
//     std::memmove(bufferB + 1, bufferB, (bufferSize - 1) * sizeof(float));
//         //Insertion into first "waterfall"
//     bufferB[0] = outA;
//         //Creating what the filter kernal will effect, the moving Sum
//     bufferBSum += bufferB[0];
//         //This is our convolution? Our filter kernal I believe!
// float outB = bufferBSum / bufferSize;

//     //Keeping the moving Sum stable!
//     bufferCSum -= bufferC[bufferSize - 1];
//         //Shifting buffer back by one "float"
//     std::memmove(bufferC + 1, bufferC, (bufferSize - 1) * sizeof(float));
//         //Insertion into first "waterfall"
//     bufferC[0] = outB;
//         //Creating what the filter kernal will effect, the moving Sum
//     bufferCSum += bufferC[0];
//         //This is our convolution? Our filter kernal I believe!
// float outC = bufferCSum / bufferSize;

//     //Keeping the moving Sum stable!
//     bufferDSum -= bufferD[bufferSize - 1];
//         //Shifting buffer back by one "float"
//     std::memmove(bufferD + 1, bufferD, (bufferSize - 1) * sizeof(float));
//         //Insertion into first "waterfall"
//     bufferD[0] = outC;
//         //Creating what the filter kernal will effect, the moving Sum
//     bufferDSum += bufferD[0];
//         //This is our convolution? Our filter kernal I believe!
// float outD = bufferDSum / bufferSize;
// */  

    
//     out = outA;
//     return out;

// }



/*

//ModuleWidgetEqu overriding rack ModuleWidget

	void ModuleWidgetEqu::setBackground(std::shared_ptr<Svg> svg) {
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
	void ModuleWidgetEqu::setPanel(widget::Widget* panel) {
	// Remove existing panel
	if (internal->panel) {
		removeChild(internal->panel);
		delete internal->panel;
		internal->panel = NULL;
	}

	if (panel) {
		addChildBottom(panel);
		internal->panel = panel;
		box.size.x = std::round(panel->box.size.x / RACK_GRID_WIDTH) * RACK_GRID_WIDTH;
		// If width is zero, set it to 12HP for sanity
		if (box.size.x == 0.0)
			box.size.x = 12 * RACK_GRID_WIDTH;
	}
}


*/















//RingBuffer

//MovingAverage
double MovingAverage::filter(double signalIn, unsigned int desiredBufferSizeCurrentIn) {
    //DEBUG("--=-=-=-=-=-=-=-=-=-=--");

    if(desiredBufferSizeCurrentIn == 0) {
        desiredBufferSizeCurrentIn = 1;
    }

        //if it needs to grow we make it grow, if it doesn't we clean the sum
    if(desiredBufferSizeCurrentIn > bufferSizeCurrent) {
        //bufferSizeCurrent = desiredBufferSizeCurrentIn;
        bufferSizeCurrent++;
    } else {
        bufferSum -= buffer[bufferSizeCurrent];
    }
            //DEBUG("bufferSum if1            ==  %f", bufferSum );
    if(desiredBufferSizeCurrentIn < bufferSizeCurrent) {
        bufferSizeCurrent--;
        bufferSum -= buffer[bufferSizeCurrent];
        double difference = (signalIn - buffer[bufferSizeCurrent]);
    }
    
    //bufferSum -= buffer[desiredBufferSizeCurrentIn];
    
    buffer[0] = signalIn;
    
    bufferSum += buffer[0];

    double out = bufferSum / (bufferSizeCurrent);
    
    buffer.rotate(1);
    
    return out;
}
//MovingAverageFourPass
double MovingAverageFourPass::filter(double signalIn, unsigned int desiredBufferSizeCurrentIn) {//, unsigned int passCount, unsigned int desiredBufferSizeCurrentIn) {
    double out = 0.f;
    
    /*
    MovingAverage filter[4] = {4096, 4096, 4096, 4096};
    
    

    filterOut[0] = filter[0].filter(signalIn);
*/

    filterOut[0] = filterA[0].filter(signalIn, desiredBufferSizeCurrentIn);

    for(
        int i = 1; i < 4; i++) {
        filterOut[i] = filterA[i].filter(filterOut[i - 1], desiredBufferSizeCurrentIn);
    }

    out = filterOut[3];

    return out;
}
//SmoothSin
double SmoothSin::filter(double signalIn, unsigned int desiredBufferSizeCurrentIn) {
    double out = 0.f;

    out = MAfilter.filter(signalIn, desiredBufferSizeCurrentIn);

    //Sin weight implimentation!

    double rMS = 0.f;
    rMSSum -= bufferRMS[bufferSizeMax - 1];
    bufferRMS[0] = signalIn * signalIn;
    rMSSum += bufferRMS[0];

    if(rMSSum > 0.0f) {
    rMS = sqrt(rMSSum / bufferSizeMax);
    } else {
    rMS = 0.0001f;
    }

    double weight = 0.f;
    weight = sin(out * M_PI * (1.f/(rMS * 2.f)));

    out = weight * rMS;
    bufferRMS.rotate(1);

    return out;
}

