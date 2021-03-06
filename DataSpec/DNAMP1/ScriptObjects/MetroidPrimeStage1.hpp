#ifndef _DNAMP1_METROIDPRIMESTAGE1_HPP_
#define _DNAMP1_METROIDPRIMESTAGE1_HPP_

#include "../../DNACommon/DNACommon.hpp"
#include "IScriptObject.hpp"
#include "Parameters.hpp"

namespace DataSpec
{
namespace DNAMP1
{
struct MetroidPrimeStage1 : IScriptObject
{
    DECL_YAML
    Value<atUint32> unknown1;
    String<-1> name;
    Value<atVec3f> location;
    Value<atVec3f> orientation;
    Value<atVec3f> scale;
    Value<bool> unknown2;
    Value<float> unknown3;
    Value<float> unknown4;
    Value<float> unknown5;
    Value<atUint32> unknown6;
    Value<bool> unknown7;
    Value<atUint32> unknown8;
    HealthInfo healthInfo1;
    HealthInfo healthInfo2;
    Value<atUint32> unknown9;

    struct PrimeParameters1 : BigYAML
    {
        DECL_YAML
        Value<atUint32> propertyCount;
        Value<float> unknown1;
        Value<float> unknown2;
        Value<float> unknown3;
        Value<float> unknown4;
        Value<float> unknown5;
        Value<float> unknown6;
        Value<float> unknown7;
        Value<float> unknown8;
        Value<float> unknown9;
        Value<float> unknown10;
        Value<float> unknown11;
        Value<float> unknown12;
        Value<float> unknown13;
        Value<float> unknown14;
    } primeStruct1_1, primeStruct1_2, primeStruct1_3, primeStruct1_4;

    Value<atUint32> unknown10;
    Value<atUint32> unknown11;

    struct MassivePrimeStruct : BigYAML
    {
        DECL_YAML
        Value<atUint32> unknown1;
        PatternedInfo patternedInfo;
        ActorParameters actorParameters;
        Value<atUint32> unknown2;
        struct PrimeStruct2 : BigYAML
        {
            DECL_YAML
            Value<bool> unknown1;
            Value<float> unknown2;
            Value<float> unknown3;
            struct PrimeStruct3 : BigYAML
            {
                DECL_YAML
                Value<bool> unknown1;
                Value<float> unknown2;
                Value<float> unknown3;
                Value<float> unknown4;
                Value<float> unknown5;
                Value<float> unknown6;
                Value<float> unknown7;
                Value<float> unknown8;
                Value<float> unknown9;
            } primeStruct3_1, primeStruct3_2, primeStruct3_3;
        } primeStruct2_1, primeStruct2_2, primeStruct2_3;

        Value<atUint32> unknown4;
        UniqueID32 particle1;
        UniqueID32 particle2;
        UniqueID32 particle3;
        DamageInfo damageInfo1;
        Value<float> unknown5;
        Value<float> unknown6;
        UniqueID32 texture1;
        Value<atUint32> unknown7;
        Value<atUint32> unknown8;
        UniqueID32 particle4;

        struct PrimeStruct4 : BigYAML
        {
            DECL_YAML
            Value<atUint32> unknown1;
            Value<atUint32> unknown2;
            UniqueID32 particle1;
            UniqueID32 particle2;
            UniqueID32 texture1;
            UniqueID32 texture2;
            Value<float> unknown3;
            Value<float> unknown4;
            Value<float> unknown5;
            Value<float> unknown6;
            Value<float> unknown7;
            Value<float> unknown8;
            Value<float> unknown9;
            Value<float> unknown10;
            Value<float> unknown11;
            Value<atVec4f> unknown12; // CColor
            Value<atVec4f> unknown13; // CColor
            UniqueID32 wpsc;
            DamageInfo damageInfo1;
            struct PrimeStruct5 : BigYAML
            {
                DECL_YAML
                Value<atUint32> propertyCount;
                UniqueID32 unknown1;
                Value<atUint32> unknown2;
                UniqueID32 unknown3;
                UniqueID32 unknown4;
                Value<atUint32> unknown5;
                Value<atUint32> unknown6;
                Value<atUint32> unknown7;
                Value<atUint32> unknown8;

                void nameIDs(PAKRouter<PAKBridge>& pakRouter, const std::string& name) const
                {
                    if (unknown1)
                    {
                        PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(unknown1);
                        ent->name = name + "_unk1";
                    }
                    if (unknown3)
                    {
                        PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(unknown3);
                        ent->name = name + "_unk3";
                    }
                    if (unknown4)
                    {
                        PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(unknown4);
                        ent->name = name + "_unk4";
                    }
                }

                void depIDs(std::vector<hecl::ProjectPath>& pathsOut) const
                {
                    g_curSpec->flattenDependencies(unknown1, pathsOut);
                    g_curSpec->flattenDependencies(unknown3, pathsOut);
                    g_curSpec->flattenDependencies(unknown4, pathsOut);
                }
            } primeStruct5;
            Value<float> unknown14;
            DamageInfo damageInfo2;

            void nameIDs(PAKRouter<PAKBridge>& pakRouter, const std::string& name) const
            {
                if (particle1)
                {
                    PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle1);
                    ent->name = name + "_part1";
                }
                if (particle2)
                {
                    PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle2);
                    ent->name = name + "_part2";
                }
                if (texture1)
                {
                    PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(texture1);
                    ent->name = name + "_tex1";
                }
                if (texture2)
                {
                    PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(texture2);
                    ent->name = name + "_tex2";
                }
                if (wpsc)
                {
                    PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(wpsc);
                    ent->name = name + "_wpsc";
                }
                primeStruct5.nameIDs(pakRouter, name + "_prime5");
            }

            void depIDs(std::vector<hecl::ProjectPath>& pathsOut) const
            {
                g_curSpec->flattenDependencies(particle1, pathsOut);
                g_curSpec->flattenDependencies(particle2, pathsOut);
                g_curSpec->flattenDependencies(texture1, pathsOut);
                g_curSpec->flattenDependencies(texture2, pathsOut);
                g_curSpec->flattenDependencies(wpsc, pathsOut);
                primeStruct5.depIDs(pathsOut);
            }
        } primeStruct4_1, primeStruct4_2, primeStruct4_3, primeStruct4_4;

        UniqueID32 wpsc1;
        DamageInfo damageInfo2;
        PrimeStruct2 primeStruct2_4;
        UniqueID32 wpsc2;
        DamageInfo damageInfo3;
        PrimeStruct2 primeStruct2_5;
        Value<atUint32> unknown3;
        UniqueID32 particle5;
        DamageInfo damageInfo4;
        Value<float> unknown9;
        Value<float> unknown10;
        Value<float> unknown11;
        UniqueID32 texture2;
        Value<bool> unknown12;
        Value<bool> unknown13;
        Value<bool> unknown14;
        Value<bool> unknown15;
        DamageInfo damageInfo5;
        PrimeStruct2 primeStruct2_6;
        UniqueID32 particle6;
        UniqueID32 swhc;
        UniqueID32 particle7;
        UniqueID32 particle8;

        struct PrimeStruct6 : BigYAML
        {
            DECL_YAML
            Value<atUint32> propertyCount;
            DamageVulnerability damageVulnerability;
            Value<atVec4f> unknown1;
            Value<atUint32> unknown2;
            Value<atUint32> unknown3;
        } primeStruct6_1, primeStruct6_2, primeStruct6_3, primeStruct6_4;

        void nameIDs(PAKRouter<PAKBridge>& pakRouter, const std::string& name) const
        {
            if (particle1)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle1);
                ent->name = name + "_part1";
            }
            if (particle2)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle2);
                ent->name = name + "_part2";
            }
            if (particle3)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle3);
                ent->name = name + "_part3";
            }
            if (particle4)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle4);
                ent->name = name + "_part4";
            }
            if (particle5)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle5);
                ent->name = name + "_part5";
            }
            if (particle6)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle6);
                ent->name = name + "_part6";
            }
            if (particle7)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle7);
                ent->name = name + "_part7";
            }
            if (particle8)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(particle8);
                ent->name = name + "_part8";
            }
            if (swhc)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(swhc);
                ent->name = name + "_swhc";
            }
            if (texture1)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(texture1);
                ent->name = name + "_tex1";
            }
            if (texture2)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(texture2);
                ent->name = name + "_tex2";
            }
            if (wpsc1)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(wpsc1);
                ent->name = name + "_wpsc1";
            }
            if (wpsc2)
            {
                PAK::Entry* ent = (PAK::Entry*)pakRouter.lookupEntry(wpsc2);
                ent->name = name + "_wpsc2";
            }
            patternedInfo.nameIDs(pakRouter, name + "_patterned");
            actorParameters.nameIDs(pakRouter, name + "_actp");
            primeStruct4_1.nameIDs(pakRouter, name + "_prime41");
            primeStruct4_2.nameIDs(pakRouter, name + "_prime42");
            primeStruct4_3.nameIDs(pakRouter, name + "_prime43");
            primeStruct4_4.nameIDs(pakRouter, name + "_prime44");
        }

        void depIDs(std::vector<hecl::ProjectPath>& pathsOut) const
        {
            g_curSpec->flattenDependencies(particle1, pathsOut);
            g_curSpec->flattenDependencies(particle2, pathsOut);
            g_curSpec->flattenDependencies(particle3, pathsOut);
            g_curSpec->flattenDependencies(particle4, pathsOut);
            g_curSpec->flattenDependencies(particle5, pathsOut);
            g_curSpec->flattenDependencies(particle6, pathsOut);
            g_curSpec->flattenDependencies(particle7, pathsOut);
            g_curSpec->flattenDependencies(particle8, pathsOut);
            g_curSpec->flattenDependencies(swhc, pathsOut);
            g_curSpec->flattenDependencies(texture1, pathsOut);
            g_curSpec->flattenDependencies(texture2, pathsOut);
            g_curSpec->flattenDependencies(wpsc1, pathsOut);
            g_curSpec->flattenDependencies(wpsc2, pathsOut);
            patternedInfo.depIDs(pathsOut);
            actorParameters.depIDs(pathsOut);
            primeStruct4_1.depIDs(pathsOut);
            primeStruct4_2.depIDs(pathsOut);
            primeStruct4_3.depIDs(pathsOut);
            primeStruct4_4.depIDs(pathsOut);
        }

        void scanIDs(std::vector<Scan>& scansOut) const
        {
            actorParameters.scanIDs(scansOut);
        }
    } massivePrimeStruct;

    void addCMDLRigPairs(PAKRouter<PAKBridge>& pakRouter,
            std::unordered_map<UniqueID32, std::pair<UniqueID32, UniqueID32>>& addTo) const
    {
        massivePrimeStruct.actorParameters.addCMDLRigPairs(addTo,
        massivePrimeStruct.patternedInfo.animationParameters.getCINF(pakRouter));
    }

    void nameIDs(PAKRouter<PAKBridge>& pakRouter) const
    {
        massivePrimeStruct.nameIDs(pakRouter, name + "_massiveStruct");
    }

    void gatherDependencies(std::vector<hecl::ProjectPath> &pathsOut) const
    {
        massivePrimeStruct.depIDs(pathsOut);
    }

    void gatherScans(std::vector<Scan>& scansOut) const
    {
        massivePrimeStruct.scanIDs(scansOut);
    }
};
}
}

#endif
