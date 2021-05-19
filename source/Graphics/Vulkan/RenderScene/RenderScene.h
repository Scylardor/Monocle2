#pragma once
#ifdef MOE_VULKAN

#include "Graphics/Vulkan/VulkanMacros.h"

#include "Graphics/Drawable/Drawable.h"

#include "Core/Containers/SparseArray/SparseArray.h"

namespace moe
{
	class RenderScene
	{

	public:
		using DrawableID = uint32_t;
		typedef SparseArray<Drawable> DrawableArray;


		static const uint32_t DEFAULT_ACTORS = 1024;

		void	Initialize(uint32_t nbrActors = DEFAULT_ACTORS);

		const Drawable&	GetObject(DrawableID id) const
		{
			return m_objects.Get(id);
		}

		Drawable& MutObject(DrawableID id)
		{
			return m_objects.Mut(id);
		}


		DrawableID	Emplace(Drawable::MeshID meshID, Drawable::MaterialID materialID, Mat4 transform = Mat4::Identity())
		{
			return m_objects.Emplace(meshID, materialID, transform);
		}


		void	Remove(DrawableID id)
		{
			m_objects.Remove(id);
		}

		// For C++11 range for syntax
		auto	begin()
		{
			return m_objects.begin();
		}

		auto begin() const
		{
			return m_objects.begin();
		}

		auto end()
		{
			return m_objects.end();
		}

		auto end() const
		{
			return m_objects.end();
		}


	protected:

	private:

		SparseArray<Drawable>	m_objects;

	};
}


#endif // MOE_VULKAN