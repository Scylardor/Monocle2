// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLM

#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp> // value_ptr

namespace moe
{
	namespace GLM
	{

		/**
		 * \brief Monocle GLM Vector3 wrapper.
		 * \tparam ValT : the actual type of value stored in that vector
		 */
		template <typename ValT = float>
		class Vector3
		{
			static_assert(std::is_arithmetic<ValT>::value, "Unsupported template type for Vector3.");

		public:
			Vector3() = default;

			explicit Vector3(ValT xyz) :
				m_vec(xyz)
			{}

			explicit Vector3(ValT x, ValT y) :
				m_vec(x, y, ValT(0))
			{}


			ValT*	Ptr()
			{
				return glm::value_ptr(m_vec);
			}

			const ValT*	Ptr() const
			{
				return glm::value_ptr(m_vec);
			}


		private:

			glm::vec<3, ValT, glm::defaultp>	m_vec;
		};

	}

	using Vec3 = GLM::Vector3<float>;
	using dVec3 = GLM::Vector3<double>;
	using iVec3 = GLM::Vector3<int>;
	using uVec3 = GLM::Vector3<uint32_t>;
}

#endif // MOE_GLM
