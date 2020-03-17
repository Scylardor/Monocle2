// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLM

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp> // value_ptr

namespace moe
{
	namespace GLM
	{

		/**
		 * \brief Monocle GLM Mat4 wrapper.
		 * \tparam ValT : the actual type of value stored in that vector
		 */
		template <typename ValT = float>
		class Mat4
		{
			static_assert(std::is_arithmetic<ValT>::value, "Unsupported template type for Mat4.");

		public:
			Mat4() = default;

			explicit Mat4(ValT diagonal) :
				m_mat(diagonal)
			{}

			explicit Mat4(ValT(&valArray)[16]) :
				m_mat(glm::make_mat4x4(valArray))
			{}


			ValT*	Ptr()
			{

				return glm::value_ptr(m_mat);
			}

			const ValT*	Ptr() const
			{
				return glm::value_ptr(m_mat);
			}


			static Mat4	Identity()
			{
				return Mat4(ValT(1));
			}



			ValT&	operator[](int idx)
			{
				return m_mat[idx];
			}

			ValT	operator[](int idx) const
			{
				return m_mat[idx];
			}

			bool	operator==(const Mat4& other) const
			{
				return m_mat == other.m_mat;
			}

			bool	operator!=(const Mat4& other) const
			{
				return m_mat != other.m_mat;
			}

			bool	operator<(const Mat4& other) const
			{
				return m_mat < other.m_mat;
			}

			bool	operator<=(const Mat4& other) const
			{
				return m_mat <= other.m_mat;
			}

			bool	operator>(const Mat4& other) const
			{
				return m_mat > other.m_mat;
			}

			bool	operator>=(const Mat4& other) const
			{
				return m_mat >= other.m_mat;
			}


		private:

			glm::mat<4, 4, ValT, glm::defaultp> m_mat;
		};

	}

	using Mat4 =  GLM::Mat4<float>;
	using dMat4 = GLM::Mat4<double>;
	using iMat4 = GLM::Mat4<int>;
	using uMat4 = GLM::Mat4<uint32_t>;
}

#endif // MOE_GLM
