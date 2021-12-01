// Monocle Game Engine source files - Alexandre Baron

#pragma once


namespace moe
{

	/**
	 * \brief Type of resource we can use in a resource layout.
	 * A uniform buffer is as the name suggests a read-only buffer with data uniform across all shader invocations.
	 * Constant Buffer in D3D.
	 * TextureReadOnly equates to a texture that can be read inside shaders, but cannot be written.
	 * Sampler defines a sampler to use with the given Textures. Given there is no actual sampler in GLSL, in OpenGL
	 * samplers declared before textures will apply to all the following textures.
	 */
	enum class BindingType : std::uint8_t
	{
		None = 0,
		UniformBuffer,
		TextureReadOnly,
		Sampler
	};

}