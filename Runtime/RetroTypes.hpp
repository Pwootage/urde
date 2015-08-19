#ifndef __RETRO_TYPES_HPP__
#define __RETRO_TYPES_HPP__

#include <utility>
#include "GCNTypes.hpp"

namespace Retro
{

/**
 * @brief singleton static-allocator
 */
template<class T>
class TOneStatic
{
    static u8 m_allocspace[sizeof(T)];
    static u32 m_refCount;
public:
    static T* GetAllocSpace() {return (T*)m_allocspace;}
    static u32& ReferenceCount() {return m_refCount;}
    T* operator->() const {return (T*)m_allocspace;}
    T& operator*() const {return *(T*)m_allocspace;}

    void* operator new(size_t) = delete;
    void operator delete(void*) = delete;

    template<typename U = T>
    TOneStatic(typename std::enable_if<!std::is_default_constructible<U>::value>::type* = 0)
    {++ReferenceCount();}
    template<typename U = T>
    TOneStatic(typename std::enable_if<std::is_default_constructible<U>::value>::type* = 0)
    {++ReferenceCount(); new (m_allocspace) T();}

    template<typename... Args> TOneStatic(Args&&... args)
    {++ReferenceCount(); new (m_allocspace) T(std::forward<Args>(args)...);}

    ~TOneStatic() {--ReferenceCount();}

    template<typename... Args> void reset(Args&&... args)
    {new (m_allocspace) T(std::forward<Args>(args)...);}
};
template<class T> u8 TOneStatic<T>::m_allocspace[sizeof(T)];
template<class T> u32 TOneStatic<T>::m_refCount;

using TUniqueId = s16;
using TEditorId = u32;
using TAreaId = u32;

}

#endif // __RETRO_TYPES_HPP__