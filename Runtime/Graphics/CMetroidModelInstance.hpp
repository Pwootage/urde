#ifndef __URDE_CMETROIDMODELINSTANCE_HPP__
#define __URDE_CMETROIDMODELINSTANCE_HPP__

#include <vector>
#include "RetroTypes.hpp"
#include "zeus/CTransform.hpp"
#include "zeus/CAABox.hpp"

namespace urde
{
class CBooModel;

class CMetroidModelInstance
{
    friend class CBooRenderer;

    int x0_visorFlags;
    zeus::CTransform x4_xf;
    zeus::CAABox x34_aabb;
    CBooModel* m_instance;
public:
    CMetroidModelInstance(CBooModel* inst);
    CMetroidModelInstance(const void* modelHeader, CBooModel* inst);
    ~CMetroidModelInstance();
};

}

#endif // __URDE_CMETROIDMODELINSTANCE_HPP__
