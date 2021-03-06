#ifndef __URDE_IOBJECTSTORE_HPP__
#define __URDE_IOBJECTSTORE_HPP__

namespace urde
{
struct SObjectTag;
class CVParamTransfer;
class IFactory;
class CToken;

class IObjectStore
{
public:
    virtual CToken GetObj(const SObjectTag&, const CVParamTransfer&)=0;
    virtual CToken GetObj(const SObjectTag&)=0;
    virtual CToken GetObj(const char*)=0;
    virtual CToken GetObj(const char*, const CVParamTransfer&)=0;
    virtual bool HasObject(const SObjectTag&) const=0;
    virtual bool ObjectIsLive(const SObjectTag&) const=0;
    virtual IFactory& GetFactory() const=0;
    virtual void Flush()=0;
    virtual void ObjectUnreferenced(const SObjectTag&)=0;
};

}

#endif // __URDE_IOBJECTSTORE_HPP__
