// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "SceneNode.h"


#include "Monocle_Graphics_Export.h"
#include "Core/Containers/ObjectPool/ObjectPool.h"

namespace moe
{
	class SceneGraph
	{
	public:

		using Handle = PolymorphicObjectPool<ASceneNode>::PoolRef;

		Monocle_Graphics_API SceneGraph(uint32_t reservedSize = 1) :
			m_nodes(reservedSize)
		{
			// Create the root
			auto id = m_nodes.Emplace<ASceneNode>(*this);
			(void)id; //dont need it
		}

		~SceneGraph()
		{
			// TODO: delete all nodes
		}


		template <typename Derived, typename... Args>
		[[nodiscard]] SceneNodeHandle	CreateNode(SceneNodeHandle parentHandle, const Transform& localTransform, Args&&... args)
		{
			const FreelistID newNodeID = m_nodes.Emplace<Derived>(*this, std::forward<Args>(args)...);

			SceneNodeHandle newNodeHandle{ newNodeID.ToHandle<SceneNodeHandle>() };

			AddChild(parentHandle, newNodeHandle);

			MutSceneNode(newNodeHandle)->SetLocalTransform(localTransform);

			return newNodeHandle;
		}

		auto MutSceneNode(SceneNodeHandle nodeHandle)
		{
			return m_nodes.MutRef(nodeHandle.Get() - 1);
		}

		Monocle_Graphics_API void	AddChild(SceneNodeHandle parentHandle, SceneNodeHandle childHandle);


		Monocle_Graphics_API void	RemoveNode(SceneNodeHandle handleToRemove, bool removeChildren = true);



		auto MutRoot()
		{
			return m_nodes.MutRef(0);
		}

		static SceneNodeHandle	GetRootHandle()
		{
			return SceneNodeHandle{ 1 };
		}


	protected:
		void	CreateRoot();

	private:



		PolymorphicObjectPool<ASceneNode>	m_nodes;
	};

}
