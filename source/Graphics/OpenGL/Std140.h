// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Math/Matrix.h"


namespace moe
{

	template <size_t N, typename T = float, typename Enable = void>
	struct Std140Vec;

	template <size_t N, typename T>
	struct Std140Vec<N, T, typename std::enable_if_t<N == 1 || N == 2>>
	{
		template<typename = std::enable_if_t<N >= 1>>
		Std140Vec(T x) :
			m_vec(x)
		{}

		template<typename = std::enable_if_t<N == 2>>
		Std140Vec(T x, T y) :
			m_vec(x, y)
		{}

		alignas(8) glm::vec<N, T> m_vec;
	};


	template <size_t N, typename T>
	struct Std140Vec<N, T, typename std::enable_if_t<N == 3 || N == 4>>
	{
		Std140Vec(T x) :
			m_vec(x)
		{}

		Std140Vec(T x, T y) :
			m_vec(x, y)
		{}

		template<typename = std::enable_if_t<N >= 3>>
		Std140Vec(T x, T y, T z) :
			m_vec(x, y, z)
		{}

		template<typename = std::enable_if_t<N == 4>>
		Std140Vec(T x, T y, T z, T w) :
			m_vec(x, y, z, w)
		{}

		alignas(16) glm::vec<N, T> m_vec;
	};


	template <typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr>
	struct Std140Scalar
	{
		alignas(4)	T m_val;
	};


	template <typename T>
	struct Std140Vec4Align
	{
		alignas(16) T m_val;

		const T& Get() const	{ return m_val; }
		T& Get()				{ return m_val; }
	};

	template <typename T, size_t N>
	struct Std140Array
	{
		Std140Vec4Align<T> m_vals[N];

		const T&	operator[](size_t idx) const	{ return m_vals[idx].Get(); }
		T&			operator[](size_t idx)			{ return m_vals[idx].Get(); }

	};


	template <size_t N, typename T = float, typename Enable = void>
	struct Std140Mat;

	template <size_t N, typename T>
	struct Std140Mat<N, T, typename std::enable_if_t<N == 2>>
	{
		Std140Mat(const Matrix<N, N, T>& mat) :
			m_vec1(mat[0]), m_vec2(mat[1])
		{}

		alignas(16) Vec<N, T> m_vec1;
		alignas(16) Vec<N, T> m_vec2;
	};

	template <size_t N, typename T>
	struct Std140Mat<N, T, typename std::enable_if_t<N == 3>>
	{
		Std140Mat(const Matrix<N, N, T>& mat = Matrix<N, N, T>::Identity()) :
			m_vec1(mat[0]), m_vec2(mat[1]), m_vec3(mat[2])
		{}

		alignas(16) Vec<4, T> m_vec1;
		alignas(16) Vec<4, T> m_vec2;
		alignas(16) Vec<4, T> m_vec3;
	};

	template <size_t N, typename T>
	struct Std140Mat<N, T, typename std::enable_if_t<N == 4>>
	{
		Std140Mat(const Matrix<N, N, T>& mat) :
			m_vec1(mat[0]), m_vec2(mat[1]), m_vec3(mat[2]), m_vec4(mat[3])
		{}

		alignas(16) Vec<4, T> m_vec1;
		alignas(16) Vec<4, T> m_vec2;
		alignas(16) Vec<4, T> m_vec3;
		alignas(16) Vec<4, T> m_vec4;
	};


	template <typename T, typename = std::enable_if_t<std::is_class_v<T>>>
	struct Std140Struct
	{
		Std140Struct(T&& data) : m_struct(std::move(data)) {}

		Std140Struct& operator=(T&& data)
		{
			m_struct = std::move(data);
			return *this;
		}

		alignas(16) T	m_struct;
	};

}
