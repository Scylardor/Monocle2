// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_GLM

#include "Core/Preprocessor/moeAssert.h"

#include "Math/Angles/Angles.h"

#include "Vector_glm.h"

#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <glm/mat4x4.hpp> // glm::mat
#include <glm/gtx/transform.hpp> // identity, translate, rotate, scale...
#include <glm/gtc/matrix_inverse.hpp>

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


			/**
			 * \brief Build a Mat3 out from a Mat4. The Mat3 will contain the upper-left 3x3 section of the Mat4.
			 * \param mat4 The source 4x4 matrix
			 */
			template<typename = std::enable_if_t<ColsT == 3 && RowsT == 3>>
			explicit Matrix(const Matrix<4, 4, ValT> & mat4) :
				m_mat(glm::mat3(mat4.m_mat))
			{}


			/**
			 * \brief Build a Mat4 out from a Mat3. This effectively builds a Mat4 without translation part.
			 * \param mat3 The source 3x3 matrix
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			explicit Matrix(const Matrix<3, 3, ValT> & mat3) :
				m_mat(glm::mat4(mat3.m_mat))
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
				if (*this != Identity())
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
				if (*this == Identity())
					return Matrix(m_mat);

				return Matrix(glm::inverse(m_mat));
			}


			/**
			 * \brief Computes the inverse transpose of the matrix.
			 * \return The transpose of the inverse of that matrix
			 */
			template<typename = std::enable_if_t<ColsT == RowsT>>
			[[nodiscard]] Matrix	GetInverseTransposed() const
			{
				if (*this == Identity())
					return Matrix(m_mat);

				return Matrix(glm::inverseTranspose(m_mat));
			}


			/**
			 * \brief Computes the inverse transpose of the matrix.
			 * This function is NOT const : the matrix is affected.
			 * For const version, use GetInverseTransposed.
			 * \return The transpose of the inverse of that matrix
			 */
			template<typename = std::enable_if_t<ColsT == RowsT>>
			[[nodiscard]] Matrix	InvertTranspose() const
			{
				if (*this == Identity())
					return *this;

				m_mat = glm::inverseTranspose(m_mat);
				return *this;
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


			/**
			 * \brief Computes a perspective projection matrix (to use with cameras for example) compatible with Vulkan and DirectX (Depth range of 0 to 1)
			 * This matrix maps a given frustum range to clip space, but also manipulates the w value of each vertex.
			 * The further away a vertex is from the viewer, the smaller it will appear on screen.
			 * \param fovy The camera's vertical field of view. In radians. Use something like 45_degf for automatic radian conversion
			 * \param aspectRatio The AR is calculated by dividing a viewport's width by its height.
			 * \param nearVal The coordinate of the near plane (clips polygons too close to the camera)
			 * \param farVal The coordinate of the far plane (clips polygons too far from the camera)
			 * \return
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			[[nodiscard]] static Matrix PerspectiveZeroOne(Rads<ValT> fovy, ValT aspectRatio, ValT nearVal, ValT farVal)
			{
				return Matrix(glm::perspectiveZO(ValT(fovy), aspectRatio, nearVal, farVal));
			}


			/**
			 * \brief Adds a translation based on the provided vector to the current matrix transformation.
			 * This function is NOT const : the matrix is affected.
			 * Note: GLM is column major by default, which means the last transformation in code is actually the first applied !
			 * \param translation The translation vector to add
			 * \return The matrix augmented with this translation transformation
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			Matrix& Translate(const Vector<3, ValT>& translation)
			{
				m_mat = glm::translate(m_mat, translation.glmVector());
				return *this;
			}


			/**
			 * \brief Adds a translation based on the provided translation axis values to the current matrix transformation.
			 * This function is NOT const : the matrix is affected.
			 * Note: GLM is column major by default, which means the last transformation in code is actually the first applied !
			 * \param x Translation in X axis
			 * \param y Translation in Y axis
			 * \param z Translation in Z axis
			 * \return The matrix augmented with this translation transformation
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			Matrix& Translate(ValT x, ValT y, ValT z)
			{
				m_mat = glm::translate(m_mat, glm::vec<3, ValT>(x, y, z));
				return *this;
			}


			/**
			 * \brief Returns the translation part of the matrix. In GLM (column-major API), it's easy : it's the last column of the matrix.
			 * \return A Vec3 containing the (x,y,z) translation of the matrix.
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			[[nodiscard]] Vec<3, ValT>	GetTranslation() const
			{
				return Vec<3, ValT>(m_mat[3][0], m_mat[3][1], m_mat[3][2]);
			}


			/**
			 * \brief Sets the last column of the matrix to a new translation.
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			void	SetTranslation(const Vec<3, ValT>& newTranslation)
			{
				std::memcpy(&m_mat[3], &newTranslation, sizeof(Vec<3, ValT>));
			}



			/**
			 * \brief Adds a rotation based on the provided vector for axis and angle to the current matrix transformation.
			 * GLM builds right-handed rotations by default.
			 * This function is NOT const : the matrix is affected.
			 * Note: GLM is column major by default, which means the last transformation in code is actually the first applied !
			 * \param angle The angle of rotation expressed in degrees. You can use something like 45_degf
			 * \param axis The axis you want to rotate about
			 * \return The matrix augmented with this rotation transformation
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			Matrix& Rotate(Degs<ValT> angle, const Vector<3, ValT>& axis)
			{
				m_mat = glm::rotate(m_mat, (ValT)angle, axis.glmVector());
				return *this;
			}


			/**
			 * \brief  Adds a rotation based on the provided angle and three scalars used as an axis.
			 * GLM builds right-handed rotations by default.
			 * This function is NOT const : the matrix is affected.
			 * Note: GLM is column major by default, which means the last transformation in code is actually the first applied !
			 * \param angle The angle of rotation expressed in degrees. You can use something like 45_degf
			 * \param x The scalar for X axis
			 * \param y The scalar for Y axis
			 * \param z The scalar for Z axis
			 * \return The matrix augmented with this rotation transformation
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			Matrix& Rotate(Degs<ValT> angle, ValT x, ValT y, ValT z)
			{
				m_mat = glm::rotate(m_mat, (ValT)angle, glm::vec<3, ValT>(x, y, z));
				return *this;
			}


			/**
			 * \brief Adds a scale based on the axis vector passed as parameter.
			 * A scale matrix extends or reduce transformed objects with given coefficients for each axis.
			 * This function is NOT const : the matrix is affected.
			 * Note: GLM is column major by default, which means the last transformation in code is actually the first applied !
			 * \param axis The scale we want to apply to each axis (X,Y,Z)
			 * \return The matrix augmented with this scale transformation
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			Matrix& Scale(const Vector<3, ValT>& axis)
			{
				m_mat = glm::scale(m_mat, axis.glmVector());
				return *this;
			}


			/**
			 * \brief Adds a scale based on the three axis values passed as parameters.
			 * A scale matrix extends or reduce transformed objects with given coefficients for each axis.
			 * This function is NOT const : the matrix is affected.
			 * Note: GLM is column major by default, which means the last transformation in code is actually the first applied !
			 * \param x Scale to apply in X axis
			 * \param y Scale to apply in Y axis
			 * \param z Scale to apply in Z axis
			 * \return The matrix augmented with this scale transformation
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			Matrix& Scale(ValT x, ValT y, ValT z)
			{
				m_mat = glm::scale(m_mat, glm::vec<3, ValT>(x, y, z));
				return *this;
			}


			/**
			 * \brief Computes a lookat matrix transformation.
			 * This function is NOT const : the matrix is affected.
			 * \param cameraPosWorld A camera position in world space
			 * \param lookatPointWorld A point to look at in world space
			 * \param upWorld A reference up vector in world space
			 * \return The matrix containing this lookat transformation.
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			Matrix&	LookAt(const Vec<3, ValT>& cameraPosWorld, const Vec<3, ValT>& lookatPointWorld, const Vec<3, ValT>& upWorld)
			{
				m_mat = Matrix(glm::lookAt(cameraPosWorld, lookatPointWorld, upWorld));
				return *this;
			}


			/**
			 * \brief Computes a translation matrix based on the provided translation vector.
			 * A translation matrix will apply the translation to any Vec4 with a w != 0.
			 * \param translation The translation vector of this matrix
			 * \return A matrix containing this translation transformation
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			[[nodiscard]] static Matrix Translation(const Vector<3, ValT>& translation)
			{
				return Matrix(glm::translate(translation.glmVector()));
			}


			/**
			 * \brief Computes a translation matrix based on the provided translation axis values.
			 * \param x Translation in X axis
			 * \param y Translation in Y axis
			 * \param z Translation in Z axis
			 * \return A matrix containing a translation equal to given components in respective axis
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			[[nodiscard]] static Matrix Translation(ValT x, ValT y, ValT z)
			{
				return Matrix(glm::translate(glm::vec<3, ValT>(x, y, z)));
			}


			/**
			 * \brief Computes a rotation matrix based on the provided vector for axis, and an angle.
			 * GLM builds right-handed rotations by default.
			 * \param angle The angle of rotation expressed in degrees. You can use something like 45_degf
			 * \param axis The axis you want to rotate about
			 * \return A rotation matrix encoding the wanted rotation.
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			[[nodiscard]] static Matrix Rotation(Degs<ValT> angle, const Vector<3, ValT>& axis)
			{
				return Matrix(glm::rotate((ValT)Rads<ValT>(angle), axis.glmVector()));
			}


			/**
			 * \brief Computes a rotation matrix based on the provided angle and three scalars used as an axis.
			 * GLM builds right-handed rotations by default.
			 * \param angle The angle of rotation expressed in degrees. You can use something like 45_degf
			 * \param x The scalar for X axis
			 * \param y The scalar for Y axis
			 * \param z The scalar for Z axis
			 * \return A rotation matrix encoding the wanted rotation.
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			[[nodiscard]] static Matrix Rotation(Degs<ValT> angle, ValT x, ValT y, ValT z)
			{
				return Matrix(glm::rotate((ValT)angle, glm::vec<3, ValT>(x, y, z)));
			}


			/**
			 * \brief Compute a scale matrix from the axis vector passed as parameter.
			 * A scale matrix extends or reduce transformed objects with given coefficients for each axis.
			 * \param axis The scale we want to apply to each axis (X,Y,Z)
			 * \return A matrix encoding the scale transformation for these axis values
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			[[nodiscard]] static Matrix Scaling(const Vector<3, ValT>& axis)
			{
				return Matrix(glm::scale(axis.glmVector()));
			}


			/**
			 * \brief Compute a scale matrix from the three axis values passed as parameters.
			 * A scale matrix extends or reduce transformed objects with given coefficients for each axis.
			 * \param x Scale to apply in X axis
			 * \param y Scale to apply in Y axis
			 * \param z Scale to apply in Z axis
			 * \return A matrix encoding the scale transformation for these axis values
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			[[nodiscard]] static Matrix Scaling(ValT x, ValT y, ValT z)
			{
				return Matrix(glm::scale(glm::vec<3, ValT>(x, y, z)));
			}


			/**
			 * \brief Computes a lookat matrix transformation.
			 * \param cameraPosWorld A camera position in world space
			 * \param lookatPointWorld A point to look at in world space
			 * \param upWorld A reference up vector in world space
			 * \return A 4x4 matrix of this lookat transformation.
			 */
			template<typename = std::enable_if_t<ColsT == 4 && RowsT == 4>>
			[[nodiscard]] static Matrix	LookAtMatrix(const Vec<3, ValT>& cameraPosWorld, const Vec<3, ValT>& lookatPointWorld, const Vec<3, ValT>& upWorld)
			{
				return Matrix(glm::lookAt(cameraPosWorld.glmVector(), lookatPointWorld.glmVector(), upWorld.glmVector()));
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


			[[nodiscard]] Matrix operator*(const Matrix& other) const
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

			[[nodiscard]] Matrix operator-(const Matrix& other) const
			{
				return Matrix(m_mat - other.m_mat);
			}

			Matrix& operator-=(const Matrix& other)
			{
				m_mat -= other.m_mat;
				return *this;
			}

			[[nodiscard]] Matrix operator/(const Matrix& other) const
			{
				return Matrix(m_mat / other.m_mat);
			}

			Matrix& operator/=(const Matrix& other)
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

			friend Matrix<3, 3, ValT>; // Needed otherwise the Mat3 from Mat4 constructor doesn't work !
			friend Matrix<4, 4, ValT>; // Needed otherwise the Mat4 from Mat3 constructor doesn't work !

		};

	}

	template <unsigned ColsT, unsigned RowsT, typename ValT>
	using Matrix = GLM::Matrix<ColsT, RowsT, ValT>;
}


#endif // MOE_GLM
