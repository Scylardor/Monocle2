#pragma once
#include "Graphics/Handle/ObjectHandle.h"

#include "Monocle_Graphics_Export.h"
#include "Core/Misc/Types.h"
#include "Core/Resource/FileResource.h"
#include "Core/Resource/ResourceRef.h"
#include "Core/Preprocessor/moeBuilder.h"
#include "Graphics/Texture/TextureFormat.h"
#include "Graphics/Texture/TextureUsage.h"

namespace moe
{

	/* A structure describing the wanted creation parameters for a 2D texture. */
	struct Texture2DDescription
	{
		MOE_BUILDER_PROPERTY(byte_t*, TextureData, nullptr)
			MOE_BUILDER_PROPERTY(uint32_t, Width, 1)
			MOE_BUILDER_PROPERTY(uint32_t, Height, 1)
			MOE_BUILDER_PROPERTY(TextureFormat, SourceFormat, TextureFormat::Any)
			MOE_BUILDER_PROPERTY(TextureFormat, TargetFormat, TextureFormat::Any)
			MOE_BUILDER_PROPERTY(TextureUsage, Usage, TextureUsage::Sampled)
			MOE_BUILDER_PROPERTY(uint32_t, MipMaps, 1)

			bool	HasImageData() const { return m_TextureData != nullptr; }
	};


	struct Texture2DFileDescription
	{
		MOE_BUILDER_PROPERTY(std::string, Filename, nullptr)
			MOE_BUILDER_PROPERTY(TextureFormat, TargetFormat, TextureFormat::Any)
			MOE_BUILDER_PROPERTY(TextureFormat, RequiredFormat, TextureFormat::Any)
			MOE_BUILDER_PROPERTY(TextureUsage, Usage, TextureUsage::Sampled)
			MOE_BUILDER_PROPERTY(uint32_t, MipMaps, 1)
	};


	struct Monocle_Graphics_API DeviceTextureHandle : RenderableObjectHandle<std::uint64_t>
	{
	private:
		static const std::uint32_t ms_INVALID_ID = (std::uint32_t) - 1;

	public:

		DeviceTextureHandle() = default;
		DeviceTextureHandle(std::uint64_t handleID) :
			RenderableObjectHandle(handleID)
		{}

		static DeviceTextureHandle	Null() { return DeviceTextureHandle(); }

	};


	/*
	 * An abstract class for textures (1D, 2D, 3D, cubemap ...) and render targets (sometimes AKA renderbuffers) management.
	 */
	class ITextureManager
	{
	public:

		virtual ~ITextureManager() = default;

		virtual DeviceTextureHandle	CreateTexture2DFromFile(Ref<FileResource> const& textureFile) = 0;

		virtual DeviceTextureHandle	CreateTexture2D() = 0;
	};


}
