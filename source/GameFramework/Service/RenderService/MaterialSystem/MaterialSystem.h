// Monocle source files - Alexandre Baron

#pragma once

#include "Core/Containers/ObjectPool/ObjectPool.h"

namespace moe
{
	/*
	 * Interface of a material system to e able to store them in a container
	 */
	class IMaterialSystem
	{
	public:

		virtual ~IMaterialSystem() = default;


	};


	template <typename TMaterial>
	class TMaterialSystem : public IMaterialSystem
	{
	public:

		template <typename... Ts>
		TMaterial&	EmplaceMaterial(Ts&&... args)
		{
			static_assert(std::is_base_of_v<MaterialInstance, TMaterial>, "Only works with derived classes of MaterialInstance");

			auto id = m_materialModules.Emplace(std::forward<Ts>(args)...);
			TMaterial& material = m_materialModules.Mut(id);
			material.SetID(id);

			return material;
		}


		void	DeleteMaterial(ObjectPoolID id)
		{
			m_materialModules.Free(id);
		}


	private:
		DynamicObjectPool<TMaterial>	m_materialModules;
	};
}