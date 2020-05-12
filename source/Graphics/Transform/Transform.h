// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Math/Matrix.h"
#include "Math/Vec3.h"

namespace moe
{

	/**
	 * \brief A wrapper over a Mat4 providing convenient functions to alter and read translate, rotation, scale.
	 */
	class Transform
	{
	public:
		Transform(Mat4 matrix = Mat4::Identity()) :
			m_matrix(matrix)
		{}


		[[nodiscard]] const Mat4&	Matrix() const { return m_matrix; }


		[[nodiscard]] Transform	operator*(const Transform& rhs) const
		{
			Transform newTransf(m_matrix * rhs.m_matrix);
			return newTransf;
		}


		Transform&	operator*=(const Transform& rhs)
		{
			m_matrix *= rhs.m_matrix;
			return *this;
		}


		[[nodiscard]] static Transform	Translate(const Vec3 & translate)
		{
			return Transform(Mat4::Translation(translate));
		}


		[[nodiscard]] static Transform	Identity()
		{
			return Transform(Mat4::Identity());
		}


	private:

		Mat4	m_matrix = Mat4::Identity();
	};

}