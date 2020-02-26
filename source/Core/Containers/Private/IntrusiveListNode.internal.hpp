#include "Core/Preprocessor/moeAssert.h"

namespace moe
{
    template <typename T>
    IntrusiveListNode<T>::IntrusiveListNode(T* owner) :
        m_prev(this),
        m_next(this),
        m_owner(owner)
    {
    }

    template <typename T>
    IntrusiveListNode<T>::~IntrusiveListNode()
    {
        Unlink();
    }

    template <typename T>
    bool IntrusiveListNode<T>::IsLinked() const
    {
        return (m_next != this);
    }

    template <typename T>
    void    IntrusiveListNode<T>::LinkTo(IntrusiveListNode<T>* node)
    {
        // Unlink ourselves from a potential existing list
        Unlink();

        if (MOE_ASSERT(node != nullptr))
        {
            m_prev = node->m_prev;
            m_next = node;
        }

        if (MOE_ASSERT(node->m_prev != nullptr))
        {
            node->m_prev->m_next = this;
            node->m_prev = this;
        }
    }

	// Unlink has no effect if we aren't linked to some other node.
    template <typename T>
    void    IntrusiveListNode<T>::Unlink()
    {
        m_prev->m_next = m_next;
        m_next->m_prev = m_prev;

        m_prev = this;
        m_next = this;
    }

    template <typename T>
    IntrusiveListNode<T>* IntrusiveListNode<T>::Next()
    {
        return m_next;
    }

    template <typename T>
    IntrusiveListNode<T>* IntrusiveListNode<T>::Prev()
    {
        return m_prev;
    }

    template <typename T>
    T* IntrusiveListNode<T>::Owner()
    {
        return m_owner;
    }
}