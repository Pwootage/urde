#ifndef _DNAMP1_ROOMACOUSTICS_HPP_
#define _DNAMP1_ROOMACOUSTICS_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec
{
namespace DNAMP1
{
struct RoomAcoustics : IScriptObject
{
    DECL_YAML
    String<-1> name;
    Value<bool> active;
    Value<atUint32> volumeScale;
    Value<bool> reverbHiEnable;
    Value<bool> reverbHiTmpDisable;
    Value<float> reverbHiColoration;
    Value<float> reverbHiMix;
    Value<float> reverbHiTime;
    Value<float> reverbHiDamping;
    Value<float> reverbHiPreDelay;
    Value<float> reverbHiCrosstalk;
    Value<bool> chorusEnable;
    Value<float> baseDelay;
    Value<float> variation;
    Value<float> period;
    Value<bool> reverbStdEnable;
    Value<bool> reverbStdTmpDisable;
    Value<float> reverbStdColoration;
    Value<float> reverbStdMix;
    Value<float> reverbStdTime;
    Value<float> reverbStdDamping;
    Value<float> reverbStdPreDelay;
    Value<bool> delayEnable;
    Value<atUint32> delay[3];
    Value<atUint32> feedback[3];
    Value<atUint32> output[3];
};
}
}

#endif
