#ifndef _DNAMP1_TIMER_HPP_
#define _DNAMP1_TIMER_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec
{
namespace DNAMP1
{
struct Timer : IScriptObject
{
    DECL_YAML
    String<-1> name;
    Value<float> startTime;
    Value<float> maxRandomAddition;
    Value<bool> resetToZero;
    Value<bool> startImmediately;
    Value<bool> active;
};
}
}

#endif
