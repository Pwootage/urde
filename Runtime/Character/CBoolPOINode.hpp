#ifndef __URDE_CBOOLPOINODE_HPP__
#define __URDE_CBOOLPOINODE_HPP__

#include "CPOINode.hpp"

namespace urde
{
class IAnimSourceInfo;

class CBoolPOINode : public CPOINode
{
    bool x38_val = false;
public:
    CBoolPOINode();
    CBoolPOINode(CInputStream& in);
    bool GetValue() const {return x38_val;}
    static CBoolPOINode CopyNodeMinusStartTime(const CBoolPOINode& node,
                                               const CCharAnimTime& startTime);
};

}

#endif // __URDE_CBOOLPOINODE_HPP__
