// Monocle Game Engine source files - Alexandre Baron

#include "SceneGraph.h"

#include "SceneNode.h"

namespace moe
{

	void SceneGraph::AddChild(SceneNodeHandle parentHandle, SceneNodeHandle childHandle)
	{
		// First detach the child from its potential current parent
		auto newChildNode = MutSceneNode(childHandle);

		if (newChildNode->HasParent())
		{
			auto oldParentNode = newChildNode->MutParent();
			oldParentNode->DetachChild(childHandle);
		}

		auto newParentNode = MutSceneNode(parentHandle);

		newParentNode->AttachChild(childHandle);

		newChildNode->SetParentHandle(parentHandle);

	}


	void SceneGraph::RemoveNode(SceneNodeHandle handleToRemove, bool removeChildren)
	{
		auto deletedNode = MutSceneNode(handleToRemove);

		if (deletedNode->HasParent())
		{
			auto parentNode = deletedNode->MutParent();
			parentNode->DetachChild(handleToRemove);
		}

		if (deletedNode->m_leftChildHandle.IsNotNull())
		{
			if (removeChildren)
			{
				// Delete al children, and their siblings, and children of siblings, etc.
				for (SceneNodeHandle deletedNodeChildHandle = deletedNode->m_leftChildHandle; deletedNodeChildHandle.IsNotNull();)
				{
					auto deletedNodeChild = MutSceneNode(deletedNodeChildHandle);
					const SceneNodeHandle siblingHandle = deletedNodeChild->m_rightSiblingHandle;
					RemoveNode(deletedNodeChildHandle, removeChildren);
					deletedNodeChildHandle = siblingHandle;
				}
			}
			else
			{
				// Just attach all the orphaned children to root.
				// We need to change the parent handle for all of them...
				for (SceneNodeHandle deletedNodeChildHandle = deletedNode->m_leftChildHandle; deletedNodeChildHandle.IsNotNull();)
				{
					auto deletedNodeChild = MutSceneNode(deletedNodeChildHandle);
					deletedNodeChild->SetParentHandle(GetRootHandle());
					deletedNodeChildHandle = deletedNodeChild->m_rightSiblingHandle;
				}

				// but we only have to update teh last root child's sibling handle.
				auto rootNode = MutRoot();

				if (rootNode->m_leftChildHandle.IsNotNull())
				{
					auto rootChild = MutSceneNode(rootNode->m_leftChildHandle);
					while (rootChild->m_rightSiblingHandle.IsNotNull())
					{
						rootChild = MutSceneNode(rootChild->m_rightSiblingHandle);
					}

					rootChild->m_rightSiblingHandle = deletedNode->m_leftChildHandle;
				}
				else
				{
					rootNode->m_leftChildHandle = deletedNode->m_leftChildHandle;
				}
			}
		}

		m_nodes.Free(handleToRemove.Get() - 1);
	}
}
