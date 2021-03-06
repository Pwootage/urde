#ifndef __URDE_CSCRIPTGRAPPLEPOINT_HPP__
#define __URDE_CSCRIPTGRAPPLEPOINT_HPP__

#include "CActor.hpp"
#include "CGrappleParameters.hpp"

namespace urde
{
class CScriptGrapplePoint : public CActor
{
    zeus::CAABox xe8_;
    CGrappleParameters x100_parameters;
public:
    CScriptGrapplePoint(TUniqueId uid, const std::string& name, const CEntityInfo& info, const zeus::CTransform& transform,
                        bool active, const CGrappleParameters& params);

    void Accept(IVisitor& visitor);
};
}

#endif // __URDE_CSCRIPTGRAPPLEPOINT_HPP__
