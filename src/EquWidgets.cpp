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



float SlewLimiter::slopeSmooth(float signalIn, const Module::ProcessArgs& args, float riseIn, float fallIn) {
   
    std::memmove(outBufferA + 1, outBufferA, outBufferASize * sizeof(double));
	outBufferA[0] = signalIn;

    outBufferAdded += signalIn;
    outBufferAdded -= outBufferA[outBufferASize - 1];

    float out = outBufferAdded / (outBufferASize / 10);

    //float delta = signalIn - outBufferL;

    return out;


}


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