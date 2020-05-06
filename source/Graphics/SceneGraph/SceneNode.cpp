// Monocle Game Engine source files - Alexandre Baron

#include "SceneNode.h"
#include "SceneGraph.h"

namespace moe
{
	ASceneNode::ASceneNode(SceneGraph& graph, SceneNodeHandle parentHandle, const Transform& localTransf) :
		m_graph(graph), m_parentHandle(parentHandle), m_localTransform(localTransf)
	{
		// From local transform, recompute world transform.
		if (m_parentHandle.IsNull())
		{
			m_worldTransform = m_localTransform; // we are a "root" node : no local transform
		}
		else
		{
			// Our world transform is parent's world transformed combined to our local transform.
			const ASceneNode* parentNode = m_graph.GetSceneNode(m_parentHandle);
			if (MOE_ASSERT(parentNode != nullptr))
			{
				m_worldTransform = m_localTransform * parentNode->GetWorldTransform();
			}
		}
	}


	void ASceneNode::SetLocalTransform(const Transform& newLocalTransf)
	{
		SetLocalTransformNoChildrenUpdate(newLocalTransf);

		// World transform has been updated : we need to update children's transforms
		UpdateChildrenTransforms();
	}


	void ASceneNode::SetWorldTransform(const Transform& newWorldTransf)
	{
		SetWorldTransformNoChildrenUpdate(newWorldTransf);


		// World transform has been updated : we need to update children's transforms
		UpdateChildrenTransforms();
	}


	void ASceneNode::UpdateChildrenTransforms()
	{
		// Now we have to walk all our children and update their transforms.
		SceneNodeHandle childHandle = m_leftChildHandle;
		while (childHandle.IsNotNull())
		{
			ASceneNode* child = m_graph.MutSceneNode(childHandle);
			child->m_worldTransform = child->m_localTransform * m_worldTransform;

			SceneNodeHandle siblingHandle = child->m_rightSiblingHandle;
			while (siblingHandle.IsNotNull())
			{
				ASceneNode* sibling = m_graph.MutSceneNode(siblingHandle);
				sibling->m_worldTransform = child->m_localTransform * m_worldTransform;

				// Recurse through each sibling
				sibling->UpdateChildrenTransforms();

				siblingHandle = sibling->m_rightSiblingHandle;
			}

			childHandle = child->m_leftChildHandle;
		}
	}


	const ASceneNode* ASceneNode::GetParent() const
	{
		return m_graph.GetSceneNode(m_parentHandle);
	}


	ASceneNode* ASceneNode::MutParent()
	{
		return m_graph.MutSceneNode(m_parentHandle);
	}


	void ASceneNode::AttachChild(SceneNodeHandle childHandle)
	{
		ASceneNode* newChildNode = m_graph.MutSceneNode(childHandle);

		if (!MOE_ASSERT(newChildNode != nullptr))
		{
			return; // This child is either null or we are already its parent
		}

		if (newChildNode->m_parentHandle.IsNotNull() && newChildNode->m_parentHandle != SceneGraph::GetRootHandle())
		{
			ASceneNode* newChildCurrentParent = newChildNode->MutParent();

			if (newChildCurrentParent == this)
			{
				return; // This child is either null or we are already its parent
			}

			// First detach the child from its potential current parent
			newChildCurrentParent->DetachChild(childHandle);
		}

		if (m_leftChildHandle.IsNull()) // First child, just set leftmost child handle.
		{
			m_leftChildHandle = childHandle;
		}
		else // we have to walk all our children until we find one with null next sibling.
		{
			ASceneNode* child = m_graph.MutSceneNode(m_leftChildHandle);
			while (child->m_rightSiblingHandle.IsNotNull())
			{
				child = m_graph.MutSceneNode(child->m_rightSiblingHandle);
			}

			// Attach the new child as right sibling of the last one
			child->m_rightSiblingHandle = childHandle;
		}

		const Transform newChildLocalTransform = GetInverseWorldTransform() * newChildNode->GetWorldTransform();
		// Don't use SetLocalTransform - it would uselessly recompute world matrix and update children.
		// The actor didn't move : we just attached it to a new parent. So only its local matrix (and only this) changes.
		newChildNode->m_localTransform = newChildLocalTransform;
	}


	void ASceneNode::AttachChild(ASceneNode& /*childNode*/)
	{
		if (m_leftChildHandle.IsNull())
		{

		}
	}



	void ASceneNode::SetLocalTransformNoChildrenUpdate(const Transform& newLocalTransf)
	{
		m_localTransform = newLocalTransf;

		// When we update world transform, we need to keep local transform up-to-date.
		if (m_parentHandle.IsNotNull())
		{
			const ASceneNode* parent = m_graph.GetSceneNode(m_parentHandle);

			// Update world transform.
			m_worldTransform = m_localTransform * parent->GetWorldTransform();
		}
		else
		{
			// Root node : world transform = local transform.
			m_worldTransform = m_localTransform;
		}
	}


	void ASceneNode::SetWorldTransformNoChildrenUpdate(const Transform& newWorldTransf)
	{
		m_worldTransform = newWorldTransf;

		// When we update world transform, we need to keep local transform up-to-date.
		if (m_parentHandle.IsNotNull())
		{
			const ASceneNode* parent = m_graph.GetSceneNode(m_parentHandle);

			// Update local transform.
			m_localTransform = parent->GetInverseWorldTransform() * m_worldTransform.Matrix();
		}
		else
		{
			// Root node : world transform = local transform.
			m_localTransform = m_worldTransform;
		}
	}


	void ASceneNode::DetachChild(SceneNodeHandle childToBeDetachedHandle)
	{
		ASceneNode* detachedChild = m_graph.MutSceneNode(childToBeDetachedHandle);
		if (!MOE_ASSERT(detachedChild != nullptr) || !MOE_ASSERT(detachedChild->m_parentHandle.IsNotNull())) // we were passed an invalid handle, something is wrong
			return;

		if (m_leftChildHandle == childToBeDetachedHandle) // It's our first child : easy
		{
			m_leftChildHandle = detachedChild->m_rightSiblingHandle;
			detachedChild->m_rightSiblingHandle = SceneNodeHandle::Null();
		}
		else // We have to walk our children until we find this one
		{
			ASceneNode* child = m_graph.MutSceneNode(m_leftChildHandle);
			ASceneNode* previousChild = child;

			while (child->m_rightSiblingHandle != childToBeDetachedHandle)
			{
				previousChild = child;
				child = m_graph.MutSceneNode(child->m_rightSiblingHandle);
			}

			// We found in previousChild the child going just before the one we detach.
			previousChild->m_rightSiblingHandle = detachedChild->m_rightSiblingHandle;
		}

		detachedChild->m_parentHandle = m_graph.GetRootHandle();
		detachedChild->m_rightSiblingHandle = SceneNodeHandle::Null();
		detachedChild->m_localTransform = detachedChild->m_worldTransform;
	}
}
