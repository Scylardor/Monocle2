// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{

	/**
	 * \brief Determines what type of triangle face will be culled: none, front faces or back faces.
	 */
	enum class CullFace : char
	{
		None = 0,
		Front,
		Back
	};


	/**
	 * \brief Describes what face of a triangle is considered the front face : the one with clockwise vertices or counterclockwise (from a camera point of view).
	 */
	enum class FrontFace : char
	{
		Clockwise = 0,
		CounterClockwise
	};



	/**
	 * \brief Controls how rendered polygons will look : only points at vertices, wireframe (lines), or fully colored (fill).
	 */
	enum class PolygonMode : char
	{
		Points = 0, // TODO : we may have to drop that as D3D11 fill mode does not support it.
		Wireframe,
		Fill
	};



	/**
	 * \brief Describes the state of a rasterizer in a graphics API-agnostic way.
	 */
	struct RasterizerStateDescriptor
	{
		/**
		 * \brief Controls whether features are enabled or not (I find it more expressive than a bool)
		 */
		enum State : char
		{
			Disabled = 0,
			Enabled
		};


		CullFace	m_cullMode{ CullFace::Back };
		FrontFace	m_frontFace{ FrontFace::CounterClockwise };
		PolygonMode	m_polyMode{ PolygonMode::Fill };
		State		m_depthClip{ Enabled };
		State		m_scissorTest{ Disabled };
	};

}