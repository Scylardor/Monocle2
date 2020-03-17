// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLM

#include <glm/vec3.hpp>
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

			explicit Vector3(ValT x, ValT y, ValT z = ValT(0)) :
				m_vec(x, y, z)
			{}

			explicit Vector3(ValT(&valArray)[3]) :
				m_vec(glm::make_vec3(valArray))
			{}



			ValT*	Ptr()
			{
				return glm::value_ptr(m_vec);
			}

			const ValT*	Ptr() const
			{
				return glm::value_ptr(m_vec);
			}


			ValT&	x()
			{
				return m_vec.x;
			}

			ValT	x() const
			{
				return m_vec.x;
			}


			ValT&	y()
			{
				return m_vec.y;
			}

			ValT	y() const
			{
				return m_vec.y;
			}


			ValT&	z()
			{
				return m_vec.z;
			}

			ValT	z() const
			{
				return m_vec.z;
			}


			ValT&	operator[](int idx)
			{
				return m_vec[idx];
			}

			ValT	operator[](int idx) const
			{
				return m_vec[idx];
			}

			bool	operator==(const Vector3& other) const
			{
				return m_vec == other.m_vec;
			}

			bool	operator!=(const Vector3& other) const
			{
				return m_vec != other.m_vec;
			}

			bool	operator<(const Vector3& other) const
			{
				return m_vec < other.m_vec;
			}

			bool	operator<=(const Vector3& other) const
			{
				return m_vec <= other.m_vec;
			}

			bool	operator>(const Vector3& other) const
			{
				return m_vec > other.m_vec;
			}

			bool	operator>=(const Vector3& other) const
			{
				return m_vec >= other.m_vec;
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
