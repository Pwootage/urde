#ifndef _DNAMP1_RELAY_HPP_
#define _DNAMP1_RELAY_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec
{
namespace DNAMP1
{
struct Relay : IScriptObject
{
    DECL_YAML
    String<-1> name;
    Value<bool> active;
};
}
}

#endif
