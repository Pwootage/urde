#ifndef _DNAMP1_EFFECT_HPP_
#define _DNAMP1_EFFECT_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec
{
namespace DNAMP1
{
struct Effect : IScriptObject
{
    DECL_YAML
    String<-1> name;
    Value<atVec3f> location;
    Value<atVec3f> orientation;
    Value<atVec3f> scale;
    UniqueID32 part;
    UniqueID32 elsc;
    Value<bool> unknown1;
    Value<bool> unknown2;
    Value<bool> unknown3;
    Value<bool> unknown4;
    Value<bool> unknown5;
    Value<float> unknown6;
    Value<float> unknown7;
    Value<float> unknown8;
    Value<float> unknown9;
    Value<bool> unknown10;
    Value<float> unknown11;
    Value<float> unknown12;
    Value<float> unknown13;
    Value<bool> unknown14;
    Value<bool> unknown15;
    Value<bool> unknown16;
    Value<bool> unknown17;
    LightParameters lightParameters;

    void nameIDs(PAKRouter<PAKBridge>& pakRouter) const
    {
        if (part)
        {
            PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(part);
            ent->name = name + "_part";
        }
        if (elsc)
        {
            PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(elsc);
            ent->name = name + "_elsc";
        }
    }

    void gatherDependencies(std::vector<hecl::ProjectPath>& pathsOut) const
    {
        g_curSpec->flattenDependencies(part, pathsOut);
        g_curSpec->flattenDependencies(elsc, pathsOut);
    }
};
}
}

#endif
