#ifndef MOE_INTRUSIVE_LIST_NODE_H_
#define MOE_INTRUSIVE_LIST_NODE_H_


// A circular doubly-linked intrusive list node inspired by the Doom 3 implementation
// cf. https://github.com/Edgarins29/Doom3/blob/master/neo/idlib/containers/LinkList.h
namespace moe
{
    template <typename T>
    class IntrusiveListNode
    {
    public:
        IntrusiveListNode(T* owner);
        ~IntrusiveListNode();

        void    LinkTo(IntrusiveListNode<T>* node);
        void    Unlink();
        bool    IsLinked() const;

        IntrusiveListNode<T>*   Next();
        IntrusiveListNode<T>*   Prev();
        T*  Owner();

    private:
        IntrusiveListNode<T>*   m_prev;
        IntrusiveListNode<T>*   m_next;
        T*  m_owner;
    };
}

#include "Core/Containers/Private/IntrusiveListNode.internal.hpp"

#endif // MOE_INTRUSIVE_LIST_NODE_H_
