// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include <Core/Preprocessor/moeAssert.h>

// Use pragmas to disable warnings coming from GLM.
#pragma warning(push, 0)
#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <glm/gtx/norm.hpp> // length2, distance2
#pragma warning(pop)

namespace moe
{
	namespace GLM
	{
		/**
		 * \brief Monocle GLM Vector wrapper.
		 * It uses SFINAE to enable member functions depending on the number of wanted components.
		 * \tparam NumT : the number of components in that vector
		 * \tparam ValT : the actual type of value stored in that vector
		 */
		template <unsigned NumT, typename ValT = float>
		class Vector
		{
			static_assert(NumT >= 2 && NumT <= 4, "Unsupported number of components for Vector.");
			static_assert(std::is_arithmetic_v<ValT>, "Unsupported template type for Vector.");

			using VectorType = glm::vec<NumT, ValT, glm::defaultp>;

		public:
			// Constructors

			Vector() = default;

			explicit Vector(ValT xyzw) :
				m_vec(xyzw)
			{}

			template<typename = std::enable_if_t<NumT >= 2>>
			Vector(ValT x, ValT y) :
				m_vec(x, y)
			{}

			template<typename = std::enable_if_t<NumT >= 3>>
			Vector(ValT x, ValT y, ValT z) :
				m_vec(x, y, z)
			{}

			template<typename = std::enable_if_t<NumT == 4>>
			Vector(ValT x, ValT y, ValT z, ValT w) :
				m_vec(x, y, z, w)
			{}

			template<typename = std::enable_if_t<NumT == 1>>
			Vector(const ValT(&valArray)[1]) :
				m_vec(glm::make_vec1(valArray))
			{}

			template<typename = std::enable_if_t<NumT == 2>>
			Vector(const ValT(&valArray)[2]) :
				m_vec(glm::make_vec2(valArray))
			{}

			template<typename = std::enable_if_t<NumT == 3>>
			Vector(const ValT(&valArray)[3]) :
				m_vec(glm::make_vec3(valArray))
			{}

			template<typename = std::enable_if_t<NumT == 4>>
			Vector(const ValT(&valArray)[4]) :
				m_vec(glm::make_vec4(valArray))
			{}


			/**
			* \brief Constructor to initialize X and Y from a vec2. Takes an optional Z and W values.
			* Usable only by Vector3.
			* \param xy Vec2 that contains values to set for x and y.
			* \param z Optional value for z. Defaults to 0
			*/
			template<typename = std::enable_if_t<NumT == 3>>
			explicit Vector(const Vector<2, ValT>& xy, ValT z = ValT(0)) :
				m_vec(xy.x(), xy.y(), z)
			{}


			/**
			* \brief Builds a Monocle Vector from a GLM one
			* This is mostly internally convenient to allow to build vectors directly from glm calls.
			* \param vector The initializer vector
			*/
			explicit Vector(const glm::vec<NumT, ValT>& vector) :
				m_vec(vector)
			{}


			/**
			* \brief Constructor to initialize X and Y from a vec2. Takes an optional Z value.
			* Usable only by Vector4.
			* \param xyz Vec3 that contains values to set for x, y and z.
			* \param w Optional value for w. Defaults to 0
			*/
			template<typename = std::enable_if_t<NumT == 4>>
			explicit Vector(const Vector<3, ValT>& xyz, ValT w = ValT(0)) :
				m_vec(xyz.x(), xyz.y(), xyz.z(), w)
			{}

			// Math Functions


			/**
			 * \brief Normalizes this vector.
			 * This effectively divides the vector by its norm (or length, or magnitude).
			 * WARNING! Since it divides by vector length, it will perform a division by zero on zero vectors !!
			 * This function is NOT const : the vector is affected.
			 * For const version, use Vector::GetNormalized.
			 * \return this
			 */
			Vector&	Normalize()
			{
				MOE_DEBUG_ASSERT(*this != Vector::ZeroVector());
				m_vec = glm::normalize(m_vec);
				return *this;
			}


			/**
			 * \brief Computes a normalized copy of this vector.
			 * This effectively divides the vector by its norm (or length, or magnitude).
			 * WARNING! Since it divides by vector length, it will perform a division by zero on zero vectors !!
			 * This function is const : the vector is unaffected.
			 * If you want to affect the vector, use Vector::Normalize.
			 * \return Normalized copy of this vector.
			 */
			[[nodiscard]] Vector	GetNormalized() const
			{
				MOE_DEBUG_ASSERT(*this != Vector::ZeroVector());
				return Vector(glm::normalize(m_vec));
			}


			/**
			* \brief Computes the length of a vector, ie. the square root of the sum of the squares of all components.
			* \return Returns the geometric length of the vector.
			*/
			[[nodiscard]] ValT	Length() const
			{
				return glm::length(m_vec);
			}


			/**
			* \brief Same as Vector::Length. It's just for the convenience if you're used to another naming convention.
			* \return Same as Vector::Length.
			*/
			[[nodiscard]] ValT	Magnitude() const
			{
				return Length();
			}


			/**
			* \brief Computes the squared length of a vector, ie. the sum of the squares of all components.
			* The rationale is that computing length involves a costy square root operation, and that sometimes, we don't need the actual length value.
			* It is advised to use the squared length when possible to reduce computations to a minimum.
			* \return Returns the squared geometric length of the vector.
			*/
			[[nodiscard]] ValT	SquaredLength() const
			{
				return glm::length2(m_vec);
			}


			/**
			* \brief Same as Vector::SquaredLength. It's just for the convenience if you're used to another naming convention.
			* \return Same as Vector::SquaredLength.
			*/
			[[nodiscard]] ValT	SquaredMagnitude() const
			{
				return SquaredLength();
			}


			/**
			 * \brief Returns the distance between the position expressed by this vector and the parameter vector.
			 * The distance is computed as the square root of the sum of differences for all components (Pythagoras rule).
			 * \return the distance between the position expressed by this vector and the parameter vector.
			 */
			[[nodiscard]] ValT	Distance(const Vector& vec) const
			{
				return glm::distance(m_vec, vec.m_vec);
			}


			/**
			* \brief Returns the squared distance between the position expressed by this vector and the parameter vector.
			* The distance is computed as the square root of the sum of differences for all components (Pythagoras rule).
			* The rationale is that computing distance involves a costy square root operation, and that sometimes, we don't need the actual distance value.
			* It is advised to use the squared distance when possible to reduce computations to a minimum.
			* \return the distance between the position expressed by this vector and the parameter vector.
			*/
			ValT	SquaredDistance(const Vector& vec) const
			{
				return glm::distance2(m_vec, vec.m_vec);
			}


			/**
			 * \brief Computes whether this vector is unit (length = 1) or not.
			 * \return True if this is an unit vector
			 */
			[[nodiscard]] bool	IsUnit() const
			{
				return (Length() == ValT(1));
			}


			/**
			 * \brief Computes the dot product of this vector and the parameter vector.
			 * If both vectors are unit vectors, the dot product will return the cosine of the angle between them.
			 * It is commutative : a . b = b . a
			 * It is distributive over vector addition : a . (b + c) = a . b + a . c
			 * It is bilinear : a . (rb + c) = r*(a . b) + (a . c)
			 * It is not associative except for scalar multiplication : (c1*a) . (c2*b) = c1c2 (a . b).
			 * Two non-zero vectors a and b are orthogonal if and only if a . b = 0.
			 * This function is const : the vector is unaffected.
			 * \param vec The vector to do a dot product with
			 * \return The dot product of this vector and the parameter vector.
			 */
			[[nodiscard]] ValT	Dot(const Vector& vec) const
			{
				return Vector(glm::dot(m_vec, vec.m_vec));
			}


			/**
			 * \brief Computes the cross product of this vector and the parameter vector.
			 * The cross product of two vectors yields a third vector that is orthogonal with the first two.
			 * It is NOT associative, i.e. a x (b x c) =/= (a x b) x c.
			 * It is NOT commutative, i.e. (a x b) =/= (b x a).
			 * This function is const : the vector is unaffected.
			 * \param vec The vector to do a cross product with
			 * \return The cross product of vec3 and this
			 */
			template<typename = std::enable_if_t<NumT >= 3>>
			Vector	Cross(const Vector& vec) const
			{
				return Vector(glm::cross(m_vec, vec.m_vec));
			}


			/**
			* \brief Computes the perpendicular vector to this vector.
			* The perpendicular vector is simply this vector rotated 90° (or PI/2).
			* Usable only by 2D vectors.
			* \return A vector perpendicular to this
			*/
			template<typename = std::enable_if_t<NumT == 2>>
			[[nodiscard]] Vector	Perp() const
			{
				// TODO: this may have to change regarding the handedness we're currently using.
				return Vector(-m_vec.y, m_vec.x);
			}


			/**
			* \brief Computes the perpendicular dot product of this vector and the parameter vector.
			* The perp dot product is sometimes called "the 2D cross product".
			* Usable only by 2D vectors.
			* This function is const : the vector is unaffected.
			* \param vec2 The vector to make the dot product with
			* \return The dot product of vec2 and the perpendicular vector of this
			*/
			template<typename = std::enable_if_t<NumT == 2>>
			[[nodiscard]] Vector	PerpDot(const Vector& vec2) const
			{
				return Perp().Dot(vec2);
			}


			/**
			 * \brief Returns a reference to the underlying GLM vector
			 *	It's a kind of code smell but AFAICT it's the easiest way to achieve matrix*vector operations without introducing some kind of odd coupling.
			 * \return the underlying GLM vector (const)
			 */
			const VectorType&	glmVector() const
			{
				return m_vec;
			}


			/**
			 * \brief Yields a zero vector
			 * \return A vector filled with zeros
			 */
			[[nodiscard]] static Vector	ZeroVector()
			{
				return Vector(ValT(0));
			}


			// Members Accessors


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


			template<typename = std::enable_if_t<NumT >= 2>>
			ValT&	y()
			{
				return m_vec.y;
			}

			template<typename = std::enable_if_t<NumT >= 2>>
			ValT	y() const
			{
				return m_vec.y;
			}


			template<typename = std::enable_if_t<NumT >= 3>>
			ValT&	z()
			{
				return m_vec.z;
			}

			template<typename = std::enable_if_t<NumT >= 3>>
			ValT	z() const
			{
				return m_vec.z;
			}


			template<typename = std::enable_if_t<NumT >= 4>>
			ValT&	w()
			{
				return m_vec.w;
			}

			template<typename = std::enable_if_t<NumT >= 4>>
			ValT	w() const
			{
				return m_vec.w;
			}


			template<typename = std::enable_if_t<NumT >= 4>>
			Vector<3, ValT>	xyz() const
			{
				return Vector<3, ValT>(m_vec.x, m_vec.y, m_vec.z);
			}



			// Relational operators


			ValT&	operator[](int idx)
			{
				MOE_DEBUG_ASSERT(idx < NumT);
				return m_vec[idx];
			}

			ValT	operator[](int idx) const
			{
				MOE_DEBUG_ASSERT(idx < NumT);
				return m_vec[idx];
			}

			bool	operator==(const Vector& other) const
			{
				return m_vec == other.m_vec;
			}

			bool	operator!=(const Vector& other) const
			{
				return m_vec != other.m_vec;
			}

			bool	operator<(const Vector& other) const
			{
				return m_vec < other.m_vec;
			}

			bool	operator<=(const Vector& other) const
			{
				return m_vec <= other.m_vec;
			}

			bool	operator>(const Vector& other) const
			{
				return m_vec > other.m_vec;
			}

			bool	operator>=(const Vector& other) const
			{
				return m_vec >= other.m_vec;
			}

			Vector operator*(ValT val) const
			{
				return Vector(m_vec * val);
			}

			Vector operator*(const Vector<NumT, ValT>& rhs) const
			{
				return Vector(m_vec * rhs.m_vec);
			}

			Vector& operator*=(ValT val)
			{
				m_vec *= val;
				return *this;
			}

			Vector operator+(ValT val) const
			{
				return Vector(m_vec + val);
			}

			Vector operator+(const Vector& rhs) const
			{
				return Vector(m_vec + rhs.m_vec);
			}

			Vector& operator+=(ValT val)
			{
				m_vec += val;
				return *this;
			}

			Vector& operator+=(const Vector& rhs)
			{
				m_vec += rhs.m_vec;
				return *this;
			}

			Vector operator-(ValT val) const
			{
				return Vector(m_vec - val);
			}

			Vector operator-(const Vector& rhs) const
			{
				return Vector(m_vec - rhs.m_vec);
			}

			Vector& operator-=(ValT val)
			{
				m_vec -= val;
				return *this;
			}

			Vector& operator-=(const Vector& rhs)
			{
				m_vec -= rhs.m_vec;
				return *this;
			}

			Vector operator/(ValT val) const
			{
				return Vector(m_vec / val);
			}

			Vector& operator/=(ValT val)
			{
				m_vec /= val;
				return *this;
			}


		private:
			VectorType	m_vec;
		};

	}

	template <size_t N, typename T>
	using Vec = GLM::Vector<N, T>;
}


/**
 * \brief Operator * to multiply a value by a vector (needed if the value comes first).
 * \tparam NumT number of vector components
 * \tparam ValT type of vector value
 * \param val the value
 * \param vec the vector
 * \return the vector multiplied by the value
 */
template <unsigned NumT, typename ValT>
moe::GLM::Vector<NumT, ValT> operator*(ValT val, const moe::GLM::Vector<NumT, ValT>& vec)
{
	return vec * val;
}