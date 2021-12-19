#include "EquWidgets.hpp"

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

float SlewLimiter::SlopeSmoothDelay::slopeSmooth(float signalIn, const Module::ProcessArgs& args, float riseIn, float fallIn) {
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

float SlewLimiter::SlopeSmoothStack::slopeSmooth(float signalIn, const Module::ProcessArgs& args, float riseIn, float fallIn) {
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






//exp();
//float passThroughB
//float passThroughA = (bufferASum / 500) - (signalIn);


bufferSum -= bA[bufferSize - 1];
bA[0] = signalIn;
bufferSum += bA[0];


float filter = 0.f; 
filter = bufferSum / (bufferSize);
DEBUG("filter: %f", filter);

float weight = 0.f; //use M_PI later
weight = sin(filter * (M_PI * 0.1) /*fallIn*/);

//~'~'~'~'~'~'~//
float outA = 0.f;
outA = weight * riseIn;

bA.rotate(1);



        //Keeping the moving Sum stable!
//    bufferASum -= bufferA[bufferSize - 1];
        //Shifting buffer back by one "float"
//    std::memmove(bufferA + 1, bufferA, (bufferSize - 1) * sizeof(float));
        //Insertion into first "waterfall"
//    bufferA[0] = signalIn;
        //Creating what the filter kernal will effect, the moving Sum
//    bufferASum += bufferA[0];
        //This is our convolution? Our filter kernal I believe!



/*
        //Keeping the moving Sum stable!
    bufferBSum -= bufferB[bufferSize - 1];
        //Shifting buffer back by one "float"
    std::memmove(bufferB + 1, bufferB, (bufferSize - 1) * sizeof(float));
        //Insertion into first "waterfall"
    bufferB[0] = outA;
        //Creating what the filter kernal will effect, the moving Sum
    bufferBSum += bufferB[0];
        //This is our convolution? Our filter kernal I believe!
float outB = bufferBSum / bufferSize;

    //Keeping the moving Sum stable!
    bufferCSum -= bufferC[bufferSize - 1];
        //Shifting buffer back by one "float"
    std::memmove(bufferC + 1, bufferC, (bufferSize - 1) * sizeof(float));
        //Insertion into first "waterfall"
    bufferC[0] = outB;
        //Creating what the filter kernal will effect, the moving Sum
    bufferCSum += bufferC[0];
        //This is our convolution? Our filter kernal I believe!
float outC = bufferCSum / bufferSize;

    //Keeping the moving Sum stable!
    bufferDSum -= bufferD[bufferSize - 1];
        //Shifting buffer back by one "float"
    std::memmove(bufferD + 1, bufferD, (bufferSize - 1) * sizeof(float));
        //Insertion into first "waterfall"
    bufferD[0] = outC;
        //Creating what the filter kernal will effect, the moving Sum
    bufferDSum += bufferD[0];
        //This is our convolution? Our filter kernal I believe!
float outD = bufferDSum / bufferSize;
*/  



    
    out = outA;
    return out;

}




/*
outBufferAdded -= outBufferA[outBufferASize - 1];

    std::memmove(outBufferA + 1, outBufferA, (outBufferASize - 1) * sizeof(float));


        //Input to fitler
	outBufferA[0] = (signalIn + outBufferB) / 2;

    outBufferAdded += outBufferA[0];
    
//    float outBufferAdded = 0.f;
//    for(int i = 0; i < outBufferASize; i++) {
//        outBufferAdded += outBufferA[i];
//    }

//    dsp::convolveNaive

        //outA is the output of the filter!!
    float outA = 0.f;
    outA = outBufferAdded / outBufferASize;
    




    outBufferB = outA;
*/
/*
     //AN ACTUAL FILTER???
    float target = signalIn;
    float delta = target - outBufferL; //The ideal slope;

    float flatDelta = (-sgn(delta))/(abs(delta) + 1);

    acceleration += (riseIn * flatDelta);
    acceleration -= (0.01 * acceleration);

    velocity +=     (acceleration);
    //                                      //Dampening ADSR?
    velocity -=     (0.01 * velocity);      //This is called dampening!!! Velocity slowly decays;

    outBufferL +=   (velocity * (fallIn * 0.0001));

    outBufferL = clamp(outBufferL, -10.f, 10.f);

    return outBufferL;
*/












int slopeDetector(float signalIn) {
    return sgn(signalIn);
};


//				This should return the slew limited version of inA?
//	outA = sL.slewLimiter(inA, rise, fall, args);