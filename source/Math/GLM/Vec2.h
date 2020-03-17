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
		 * \brief Monocle GLM Vector2 wrapper.
		 * \tparam ValT : the actual type of value stored in that vector
		 */
		template <typename ValT = float>
		class Vector2
		{
			static_assert(std::is_arithmetic<ValT>::value, "Unsupported template type for Vector2.");

		public:
			Vector2() = default;

			explicit Vector2(ValT xy) :
				m_vec(xy)
			{}

			explicit Vector2(ValT x, ValT y) :
				m_vec(x, y)
			{}

			explicit Vector2(ValT(&valArray)[2]) :
				m_mat(glm::make_vec2(valArray))
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


			ValT&	operator[](int idx)
			{
				return m_vec[idx];
			}

			ValT	operator[](int idx) const
			{
				return m_vec[idx];
			}

			bool	operator==(const Vector2& other) const
			{
				return m_vec == other.m_vec;
			}

			bool	operator!=(const Vector2& other) const
			{
				return m_vec != other.m_vec;
			}

			bool	operator<(const Vector2& other) const
			{
				return m_vec < other.m_vec;
			}

			bool	operator<=(const Vector2& other) const
			{
				return m_vec <= other.m_vec;
			}

			bool	operator>(const Vector2& other) const
			{
				return m_vec > other.m_vec;
			}

			bool	operator>=(const Vector2& other) const
			{
				return m_vec >= other.m_vec;
			}


		private:

			glm::vec<2, ValT, glm::defaultp>	m_vec;
		};

	}

	using Vec2 =  GLM::Vector2<float>;
	using dVec2 = GLM::Vector2<double>;
	using iVec2 = GLM::Vector2<int>;
	using uVec2 = GLM::Vector2<uint32_t>;
}

#endif // MOE_GLM
