// Monocle Game Engine source files

#pragma once

#include "Graphics/Sampler/SamplerDescriptor.h"

namespace moe
{
	class OpenGLSampler
	{
	public:
		OpenGLSampler(unsigned int id, const SamplerDescriptor& desc) :
			m_samplerID(id), m_desc(desc)
		{}

		OpenGLSampler(const OpenGLSampler&) = delete;
		OpenGLSampler& operator=(const OpenGLSampler&) = delete;

		OpenGLSampler(OpenGLSampler&& other) noexcept;
		OpenGLSampler& operator=(OpenGLSampler&& rhs) noexcept;


		[[nodiscard]] unsigned int	GetID() const { return m_samplerID; }

		static unsigned int	TranslateToOpenGLWrapMode(SamplerWrapping wrapMode);
		static unsigned int	TranslateToOpenGLFilterMode(SamplerFilter filterMode);

	private:
		unsigned int	m_samplerID{0};

		SamplerDescriptor	m_desc{};
	};


	inline OpenGLSampler::OpenGLSampler(OpenGLSampler&& other) noexcept
	{
		m_samplerID = other.m_samplerID;
		other.m_samplerID = 0;

		m_desc = std::move(other.m_desc);

	}

	inline OpenGLSampler& OpenGLSampler::operator=(OpenGLSampler&& rhs) noexcept
	{
		if (&rhs != this)
		{
			m_samplerID = rhs.m_samplerID;
			rhs.m_samplerID = 0;

			m_desc = std::move(rhs.m_desc);
		}

		return *this;
	}

}
