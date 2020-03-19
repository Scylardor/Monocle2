// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLM

#include "Core/Preprocessor/moeAssert.h"

#include "Math/Angles/Angles.h"

#include "Vector_glm.h"

#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> // identity, translate, rotate, scale...

namespace moe
{
	namespace GLM
	{

		/**
		 * \brief Monocle GLM Matrix wrapper.
		 * \tparam ColsT : the number of columns in that matrix
		 * \tparam RowsT : the number of rows in that matrix
		 * \tparam ValT : the actual type of values stored in that matrix
		 */
		template <unsigned ColsT, unsigned RowsT, typename ValT = float>
		class Matrix
		{
			static_assert(ColsT > 0 && RowsT > 0, "Unsupported column or row number for Matrix.");
			static_assert(std::is_arithmetic<ValT>::value, "Unsupported template type for Matrix.");

			using MatrixType = glm::mat<ColsT, RowsT, ValT, glm::defaultp>;

		public:

			// Constructors

			Matrix() = default;

			explicit Matrix(ValT diagonal) :
				m_mat(diagonal)
			{}

			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			explicit Matrix(ValT(&valArray)[16]) :
				m_mat(glm::make_mat4x4(valArray))
			{}


			/**
			 * \brief Builds a Monocle Matrix from a GLM one
			 * This is mostly internally convenient to allow to build matrices directly from glm calls.
			 * \param matrix The initializer matrix
			 */
			explicit Matrix(const glm::mat<ColsT, RowsT, ValT>& matrix) :
				m_mat(matrix)
			{}


			// Math Functions


			/**
			 * \brief Sets the matrix to a transposed version of itself.
			 * Transposing a matrix effectively means switching rows and columns.
			 * This function is NOT const : the matrix is affected.
			 * For const version, use Matrix::GetTransposed.
			 * \return The same matrix, transposed
			 */
			Matrix&	Transpose()
			{
				m_mat = glm::transpose(m_mat);
				return *this;
			}


			/**
			 * \brief Computes a transposed version of the matrix.
			 * Transposing a matrix effectively means switching rows and columns.
			 * This function is const : the matrix is unaffected.
			 * If you want to affect the matrix, use Matrix::Transpose.
			 * \return A transposed copy of this matrix.
			 */
			[[nodiscard]] Matrix	GetTransposed() const
			{
				return Matrix(glm::transpose(m_mat));
			}


			/**
			* \brief Computes an inverse version of the matrix.
			* The inverse of a matrix A is another matrix B such as A*B and B*A both equal to the identity matrix.
			* Only square matrices are invertible, but that's not the only condition.
			* This function is NOT const : the matrix is affected.
			* For const version, use Matrix::GetInverse.
			* \return The same matrix, inverted
			*/
			template<typename = std::enable_if_t<ColsT == RowsT>>
			Matrix&	Invert()
			{
				m_mat = glm::inverse(m_mat);
				return *this;
			}


			/**
			* \brief Computes an inverse copy of the matrix.
			* The inverse of a matrix A is another matrix B such as A*B and B*A both equal to the identity matrix.
			* Only square matrices are invertible, but that's not the only condition.
			 * This function is const : the matrix is unaffected.
			 * If you want to affect the matrix, use Matrix::Invert.
			* \return An inverted copy of the matrix
			*/
			template<typename = std::enable_if_t<ColsT == RowsT>>
			[[nodiscard]] Matrix	GetInverse() const
			{
				return Matrix(glm::inverse(m_mat));
			}


			/**
			 * \brief Resets this matrix to the identity matrix.
			 * \return This matrix, filled with zeros except ones in the main diagonal.
			 */
			Matrix&	SetIdentity()
			{
				m_mat = glm::identity<MatrixType>();
				return *this;
			}


			/**
			 * \brief Yields an identity matrix.
			 * \return A matrix filled with zeros except ones in the main diagonal.
			 */
			[[nodiscard]] static Matrix	Identity()
			{
				return Matrix(glm::identity<MatrixType>());
			}


			/**
			 * \brief Computes an orthographic projection matrix (to use with cameras for example)
			 * An orthographic projection matrix directly maps coordinates to the 2D plane that is your screen, but it produces unrealistic results.
			 * \param left Coordinate of the left side of the frustum.
			 * \param right Coordinate of the right side of the frustum.
			 * \param bottom Coordinate of the bottom side of the frustum.
			 * \param top Coordinate of the top side of the frustum.
			 * \param nearVal Coordinate of the near plane (relative to camera).
			 * \param farVal Coordinate of the far plane (relative to camera).
			 * \return
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			[[nodiscard]] static Matrix Orthographic(ValT left, ValT right, ValT bottom, ValT top, ValT nearVal, ValT farVal)
			{
				return Matrix(glm::ortho(left, right, bottom, top, nearVal, farVal));
			}


			/**
			 * \brief Computes a perspective projection matrix (to use with cameras for example)
			 * This matrix maps a given frustum range to clip space, but also manipulates the w value of each vertex.
			 * The further away a vertex is from the viewer, the smaller it will appear on screen.
			 * \param fovy The camera's vertical field of view. In radians. Use something like 45_degf for automatic radian conversion
			 * \param aspectRatio The AR is calculated by dividing a viewport's width by its height.
			 * \param nearVal The coordinate of the near plane (clips polygons too close to the camera)
			 * \param farVal The coordinate of the far plane (clips polygons too far from the camera)
			 * \return
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			[[nodiscard]] static Matrix Perspective(Rads<ValT> fovy, ValT aspectRatio, ValT nearVal, ValT farVal)
			{
				return Matrix(glm::perspective(ValT(fovy), aspectRatio, nearVal, farVal));
			}


			// Member Accessors


			[[nodiscard]] ValT*	Ptr()
			{
				return glm::value_ptr(m_mat);
			}

			[[nodiscard]] const ValT*	Ptr() const
			{
				return glm::value_ptr(m_mat);
			}


			/**
			 * \brief Returns a Vector reference to the column at index idx in the matrix
			 * This function is NOT const: the matrix can be modified through the returned reference.
			 * Using reinterpret_cast trickery because Monocle matrix is only a thin wrapper over GLM.
			 * It works exactly because of that (Monocle class are just wrapped GLM structs), but it's not very future-proof.
			 * \param idx The index of the column to return a reference to
			 * \return A vector reference to the specified matrix column
			 */
			[[nodiscard]] Vector<RowsT>&	operator[](int idx)
			{
				MOE_DEBUG_ASSERT(idx < ColsT);
				return reinterpret_cast<Vector<RowsT>&>(m_mat[idx]);
			}

			/**
			 * \brief Returns a Vector reference to the column at index idx in the matrix
			 * This function is NOT const: the matrix cannot be modified through the returned reference.
			 * Using reinterpret_cast trickery because Monocle matrix is only a thin wrapper over GLM.
			 * It works exactly because of that (Monocle class are just wrapped GLM structs), but it's not very future-proof.
			 * \param idx The index of the column to return a reference to
			 * \return A vector reference to the specified matrix column
			 */
			[[nodiscard]] const Vector<RowsT>&	operator[](int idx) const
			{
				MOE_DEBUG_ASSERT(idx < ColsT);
				return reinterpret_cast<const Vector<RowsT>&>(m_mat[idx]);
			}


			[[nodiscard]] bool	operator==(const Matrix& other) const
			{
				return m_mat == other.m_mat;
			}

			[[nodiscard]] bool	operator!=(const Matrix& other) const
			{
				return m_mat != other.m_mat;
			}


			[[nodiscard]] Matrix operator*(Matrix other) const
			{
				return Matrix(m_mat * other.m_mat);
			}

			Matrix& operator*=(const Matrix& other)
			{
				m_mat *= other.m_mat;
				return *this;
			}


			[[nodiscard]] Matrix operator+(Matrix other) const
			{
				return Matrix(m_mat + other.m_mat);
			}

			Matrix& operator+=(Matrix other)
			{
				m_mat += other.m_mat;
				return *this;
			}

			[[nodiscard]] Matrix operator-(Matrix other) const
			{
				return Matrix(m_mat - other.m_mat);
			}

			Matrix& operator-=(Matrix other)
			{
				m_mat -= other.m_mat;
				return *this;
			}

			[[nodiscard]] Matrix operator/(Matrix other) const
			{
				return Matrix(m_mat / other.m_mat);
			}

			Matrix& operator/=(Matrix other)
			{
				m_mat /= other.m_mat;
				return *this;
			}


			/**
			 * \brief Matrix-vector operation
			 * As a vector can be seen as a matrix with one row (or column in column-major), we must respect the matrix-matrix multiplication rules:
			 * For a matrix with ColsT*RowsT dimension, we can only multiply this matrix with a vector of RowsT components (number of rows must match number of columns).
			 * \param vec The vector to transform with this matrix
			 * \return The transformed vector
			 */
			Vector<RowsT, ValT>	operator*(const Vector<RowsT, ValT>& vec)
			{
				return Vector<RowsT, ValT>(m_mat * vec.glmVector());
			}


		private:

			MatrixType	m_mat;

		};

	}
}


#endif // MOE_GLM
