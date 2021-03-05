// Monocle Game Engine source files

#pragma once

#ifdef MOE_STD_SUPPORT
#include <string>
#endif

#include "Core/Misc/Literals.h"

#include "Core/Misc/Types.h"

#include "Core/Preprocessor/moeBuilder.h"
#include "Core/Preprocessor/moeJoin.h"

#include "TextureFormat.h"
#include "TextureUsage.h"

namespace moe
{

	struct Texture2DDescriptor
	{
		char*			m_imageData{ nullptr };
		Width_t			m_width{1};
		Height_t		m_height{1};
		TextureFormat	m_targetFormat{ TextureFormat::Any };
		TextureUsage	m_texUsage{ TextureUsage::Sampled };
		uint32_t		m_wantedMipmapLevels{ 1 };
		TextureFormat	m_sourceFormat{ TextureFormat::RGBA8 }; // TODO: refactor the ordering of this structure.
	};

	// Renamed for backwards compatibility
	struct Tex2DDescriptor
	{
		MOE_BUILDER_PROPERTY(byte_t*, ImageData, nullptr)
		MOE_BUILDER_PROPERTY(uint32_t, Width, 1)
		MOE_BUILDER_PROPERTY(uint32_t, Height, 1)
		MOE_BUILDER_PROPERTY(TextureFormat, SourceFormat, TextureFormat::Any)
		MOE_BUILDER_PROPERTY(TextureFormat, TargetFormat, TextureFormat::Any)
		MOE_BUILDER_PROPERTY(TextureUsage, Usage, TextureUsage::Sampled)
		MOE_BUILDER_PROPERTY(uint32_t, MipMaps, 1)

		bool	HasImageData() const { return m_ImageData != nullptr;  }
	};

	// Renamed for backwards compatibility
	struct Tex2DFileDescriptor
	{
		MOE_BUILDER_PROPERTY(std::string, Filename, nullptr)
		MOE_BUILDER_PROPERTY(TextureFormat, TargetFormat, TextureFormat::Any)
		MOE_BUILDER_PROPERTY(TextureFormat, RequiredFormat, TextureFormat::Any)
		MOE_BUILDER_PROPERTY(TextureUsage, Usage, TextureUsage::Sampled)
		MOE_BUILDER_PROPERTY(uint32_t, MipMaps, 1)
	};

	// CubemapTexDescriptor works really the same as Tex2DDescriptor except the description is for each of the 6 faces.
	using CubemapTexDescriptor = Tex2DDescriptor;

	// Renamed for backwards compatibility
	struct CubemapTexFilesDescriptor
	{
		MOE_BUILDER_ARRAY_PROPERTY(std::string, 6, RightLeftTopBottomFrontBackTexFiles, )
		MOE_BUILDER_PROPERTY(TextureFormat, TargetFormat, TextureFormat::Any)
		MOE_BUILDER_PROPERTY(TextureFormat, RequiredFormat, TextureFormat::Any)
		MOE_BUILDER_PROPERTY(TextureUsage, Usage, TextureUsage::Sampled)
		MOE_BUILDER_PROPERTY(uint32_t, MipMaps, 1)
	};


	struct Texture2DFileDescriptor
	{
		std::string		m_filename;
		TextureFormat	m_targetFormat{ TextureFormat::Any };
		TextureUsage	m_texUsage{ TextureUsage::Sampled };
		uint32_t		m_wantedMipmapLevels{1};
		TextureFormat	m_requiredFormat{ TextureFormat::Any };
	};


	// TODO: refactor with Texture2DDescriptor, it's nearly the same !
	struct CubeMapTextureDescriptor
	{
		char*			m_imageData{ nullptr };
		Width_t			m_width{ 1 };
		Height_t		m_height{ 1 };
		TextureFormat	m_sourceFormat{ TextureFormat::Any };
		TextureFormat	m_targetFormat{ TextureFormat::Any };
		TextureUsage	m_texUsage{ TextureUsage::Sampled };
		uint32_t		m_wantedMipmapLevels{ 1 };
	};


	struct CubeMapTextureFilesDescriptor
	{
		std::string				m_rightLeftTopBottomFrontBackTexFiles[6]{};
		TextureFormat			m_targetFormat{ TextureFormat::Any };
		TextureUsage			m_texUsage{ TextureUsage::Sampled };
		uint32_t				m_wantedMipmapLevels{ 1 };
		TextureFormat			m_requiredFormat{ TextureFormat::Any };
	};
}