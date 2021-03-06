#ifndef __URDE_CHALFTRANSITION_HPP__
#define __URDE_CHALFTRANSITION_HPP__

#include "IOStreams.hpp"
#include "IMetaTrans.hpp"

namespace urde
{

class CHalfTransition
{
    u32 x0_id;
    std::shared_ptr<IMetaTrans> x4_trans;
public:
    CHalfTransition(CInputStream& in);
    u32 GetId() const {return x0_id;}
    const std::shared_ptr<IMetaTrans>& GetMetaTrans() const {return x4_trans;}
};

}

#endif // __URDE_CHALFTRANSITION_HPP__
