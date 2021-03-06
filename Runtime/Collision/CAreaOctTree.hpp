#ifndef __URDE_CAREAOCTTREE_HPP__
#define __URDE_CAREAOCTTREE_HPP__

#include "RetroTypes.hpp"
#include "zeus/CAABox.hpp"
#include "Collision/CCollisionEdge.hpp"

namespace urde
{
class CCollisionEdge;
class CMaterialFilter;
class CAreaOctTree
{
    friend class CBooRenderer;
public:
    struct SRayResult
    {
    };

    class TriListReference
    {
        u16 m_count;
        std::unique_ptr<u16[]> m_refs;
    public:
        TriListReference(const u16* ptr)
        : m_count(ptr[0])
        {
            m_refs.reset(new u16[m_count]);
            for (u16 i=0 ; i<m_count ; ++i)
                m_refs[i] = ptr[i+1];
        }
        u16 GetAt(int idx) const { return m_refs[idx]; }
        u16 GetSize() const { return m_count; }
    };

    class Node
    {
    public:
        enum class ETreeType
        {
            Invalid,
            Branch,
            Leaf
        };

    private:
        const u8* m_ptr;
        zeus::CAABox m_aabb;
        const CAreaOctTree& m_owner;
        ETreeType m_nodeType;

    public:
        Node(const void* ptr, const zeus::CAABox& aabb,
             const CAreaOctTree& owner, ETreeType type)
        : m_ptr(reinterpret_cast<const u8*>(ptr)), m_aabb(aabb), m_owner(owner), m_nodeType(type)
        {
        }
#if 0
        void LineTestEx(const zeus::CLine&, const CMaterialFilter&, SRayResult&, float) const;
        void LineTestExInternal(const zeus::CLine&, const CMaterialFilter&, SRayResult&, float, float, float,
                                const zeus::CVector3f&) const;
#endif

        const CAreaOctTree& GetOwner() const
        {
            return m_owner;
        }

        const zeus::CAABox& GetBoundingBox() const
        {
            return m_aabb;
        }

        u16 GetChildFlags() const
        {
            return *reinterpret_cast<const u16*>(m_ptr);
        }

        Node GetChild(int idx) const;

        TriListReference GetTriangleArray() const
        {
            return TriListReference(reinterpret_cast<const u16*>(m_ptr + 24));
        }

        ETreeType GetChildType(int idx) const
        {
            u16 flags = *reinterpret_cast<const u16*>(m_ptr);
            return ETreeType((flags << (2 * idx)) & 0x3);
        }

        ETreeType GetTreeType() const { return m_nodeType; }
    };

    zeus::CAABox x0_aabb;
    Node::ETreeType x18_treeType;
    const u8* x1c_buf;
    const u8* x20_treeBuf;
    u32 x24_matCount;
    const u32* x28_materials;
    const u8* x2c_vertMats;
    const u8* x30_edgeMats;
    const u8* x34_polyMats;
    u32 x38_edgeCount;
    const CCollisionEdge* x3c_edges;
    u32 x40_polyCount;
    const u16* x44_polyEdges;
    u32 x48_vertCount;
    const float* x4c_verts;

    void SwapTreeNode(u8* ptr, Node::ETreeType type);

public:
    CAreaOctTree(const zeus::CAABox& aabb, Node::ETreeType treeType, const u8* buf, const u8* treeBuf,
                 u32 matCount, const u32* materials, const u8* vertMats, const u8* edgeMats, const u8* polyMats,
                 u32 edgeCount, const CCollisionEdge* edges, u32 polyCount, const u16* polyEdges,
                 u32 vertCount, const float* verts);

    Node GetRootNode() const { return Node(x20_treeBuf, x0_aabb, *this, x18_treeType); }
    const u8* GetTreeMemory() const { return x20_treeBuf; }
    zeus::CVector3f GetVert(int idx) const
    {
        const float* vert = &x4c_verts[idx * 3];
        return zeus::CVector3f(vert[0], vert[1], vert[2]);
    }
    const CCollisionEdge& GetEdge(int idx) const { return x3c_edges[idx]; }
    u32 GetEdgeMaterial(int idx) const { return x28_materials[x30_edgeMats[idx]]; }
    u32 GetTriangleMaterial(int idx) const { return x28_materials[x34_polyMats[idx]]; }
    u32 GetNumEdges() const { return x38_edgeCount; }
    u32 GetNumVerts() const { return x48_vertCount; }
    u32 GetNumTriangles() const { return x40_polyCount; }
    const u16* GetMasterListTriangle(u16 idx) const;
    const u16* GetTriangleVertexIndices(u16 idx) const;
    const u16* GetTriangleEdgeIndices(u16 idx) const;

    static std::unique_ptr<CAreaOctTree> MakeFromMemory(const u8* buf, unsigned int size);
};

}

#endif // __URDE_CAREAOCTTREE_HPP__
