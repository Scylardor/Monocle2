// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/Transform/Transform.h"

#include "SceneNodeHandle.h"

#include "Monocle_Graphics_Export.h"
#include "Core/Containers/ObjectPool/ObjectPool.h"

namespace moe
{
	class SceneGraph;
	/**
	 * \brief An implementation of an abstract scene node class.
	 * It uses a LCRS (Left Child - Right Sibling) system to try to optimize memory usage and access patterns.
	 * I believe it's more efficient than having a vector of children Node pointers per Scene node.
	 * The runtime inefficiency penalty of traversing sibling nodes is mitigated by the assumption the scene graph is using a contiguous data structure to store all nodes.
	 */
	class ASceneNode
	{
		friend SceneGraph;
		using Handle = PolymorphicObjectPool<ASceneNode>::PoolRef;


	public:
		Monocle_Graphics_API ASceneNode(class SceneGraph& graph, SceneNodeHandle parentHandle = SceneNodeHandle::Null(),
			const Transform& localTransf = Transform());

		virtual ~ASceneNode() {}

		template <typename T>
		ASceneNode* AddNode()
		{
			return nullptr;
		}

		[[nodiscard]] const Transform& GetWorldTransform() const { return m_worldTransform; }
		[[nodiscard]] const Transform& GetLocalTransform() const { return m_worldTransform; }

		[[nodiscard]] Transform	GetInverseWorldTransform() const { return Transform(m_worldTransform.Matrix().GetInverse()); }

		Monocle_Graphics_API void	SetLocalTransform(const Transform& newLocalTransf);
		Monocle_Graphics_API void	SetWorldTransform(const Transform& newWorldTransf);

		[[nodiscard]] bool					HasParent() const { return m_parentHandle.IsNotNull(); }
		[[nodiscard]] Handle MutParent();


		void	AttachChild(SceneNodeHandle childHandle);
		void	AttachChild(ASceneNode& childNode);

	protected:

		void	SetLocalTransformNoChildrenUpdate(const Transform& newLocalTransf);
		void	SetWorldTransformNoChildrenUpdate(const Transform& newWorldTransf);

		void	UpdateChildrenTransforms();

		void	SetParentHandle(SceneNodeHandle handle)
		{
			m_parentHandle = handle;
		}
		void	DetachChild(SceneNodeHandle childToBeDetachedHandle);


		SceneGraph& m_graph;

		SceneNodeHandle		m_parentHandle;
		SceneNodeHandle		m_leftChildHandle;
		SceneNodeHandle		m_rightSiblingHandle;

		Transform	m_localTransform;

		Transform	m_worldTransform;

	};


}
