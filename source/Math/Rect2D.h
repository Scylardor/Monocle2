// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{
	template <typename T>
	struct Rect2D
	{
		Rect2D() = default;

		Rect2D(T px, T py, T pw, T ph) :
			x(px),
			y(py),
			Width(pw),
			Height(ph)
		{}


		T	x{ 0 };
		T	y{ 0 };
		T	Width{ 0 };
		T	Height{ 0 };

	};

	using Rect2Di = Rect2D<int>;
	using Rect2Du = Rect2D<unsigned>;
	using Rect2Df = Rect2D<float>;

}

