#include "EquWidgets.hpp"

float SlewLimiter::slewLimiter(float signalIn, float signalOut, const Module::ProcessArgs& args, float riseIn, float fallIn) {

    float delta = signalIn - signalOut;

    bool deltaGTZero = delta > 0.f;

    return delta;


    //return args.sampleTime;
};
