// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <Core/Containers/FreeList/PolymorphicFreelist.h>

#include "SceneNode.h"


#include "Monocle_Graphics_Export.h"

namespace moe
{
	class SceneGraph
	{
	public:
		Monocle_Graphics_API SceneGraph();


		Monocle_Graphics_API SceneGraph(uint32_t reservedSize) :
			SceneGraph()
		{
			m_nodes.Reserve(reservedSize);
		}

		~SceneGraph()
		{
			// TODO: delete all nodes
		}


		template <typename Derived, typename... Args>
		[[nodiscard]] SceneNodeHandle	CreateNode(SceneNodeHandle parentHandle, const Transform& localTransform, Args&&... args)
		{
			const FreelistID newNodeID = m_nodes.Add<Derived>(*this, std::forward<Args>(args)...);

			SceneNodeHandle newNodeHandle{ newNodeID.ToHandle<SceneNodeHandle>() };

			AddChild(parentHandle, newNodeHandle);

			MutSceneNode(newNodeHandle)->SetLocalTransform(localTransform);

			return newNodeHandle;
		}

		ASceneNode*	MutSceneNode(SceneNodeHandle nodeHandle)
		{
			return &m_nodes.Lookup(nodeHandle.Get() - 1);
		}


		const ASceneNode*	GetSceneNode(SceneNodeHandle nodeHandle)
		{
			return &m_nodes.Lookup(nodeHandle.Get()-1);
		}


		Monocle_Graphics_API void	AddChild(SceneNodeHandle parentHandle, SceneNodeHandle childHandle);


		Monocle_Graphics_API void	RemoveNode(SceneNodeHandle handleToRemove, bool removeChildren = true);



		ASceneNode*	MutRoot()
		{
			return &m_nodes.Lookup(0);
		}


		const ASceneNode*	GetRoot()
		{
			return &m_nodes.Lookup(0);
		}


		static SceneNodeHandle	GetRootHandle()
		{
			return SceneNodeHandle{1 };
		}


	protected:
		void	CreateRoot();

	private:



		PolymorphicFreelist<ASceneNode>	m_nodes;
	};

}