#pragma once

#include <glad/glad.h>

#include "Graphics/RHI/TextureManager/TextureManager.h"
#include "Graphics/Sampler/SamplerDescriptor.h"


#include "Core/Resource/TextureResource.h"

#include "Core/Containers/Vector/Vector.h"

#include "Monocle_Graphics_Export.h"

namespace moe
{
	struct SamplerCacheTraits
	{
		static void	Create(GLsizei n, GLuint* samplers)
		{
			glCreateSamplers(n, samplers);
		}

		static void	Delete(GLsizei n, GLuint* samplers)
		{
			glDeleteSamplers(n, samplers);
		}
	};

	struct TextureCacheTraits
	{
		void	Create(GLsizei n, GLuint* textures)
		{
			glGenTextures(n, textures);
		}

		void	Delete(GLsizei n, GLuint* textures)
		{
			glDeleteTextures(n, textures);
		}
	};


	template <typename DataT, typename HandleT, typename Traits>
	struct OpenGLDataCache
	{
		OpenGLDataCache(size_t reserved = 0) :
			HandleDatas(reserved)
		{
			if (reserved != 0)
			{
				Handles.Resize(reserved);
				Traits::Create((GLsizei) Handles.Size(), Handles.Data());
			}
		}

		~OpenGLDataCache()
		{
			Traits::Delete((GLsizei)Handles.Size(), Handles.Data());
		}


		static HandleT	Encode(uint32_t index, uint32_t GLhandle)
		{
			typename HandleT::Underlying handle = (uint64_t)index << 32;
			handle |= GLhandle;
			return HandleT(handle);
		}

		static uint32_t	DecodeIndex(HandleT handle)
		{
			return (handle.Get() >> 32);
		}

		static uint32_t	DecodeGLHandle(HandleT handle)
		{
			return (uint32_t)handle.Get();
		}


		HandleT	Find(DataT const& data)
		{
			auto existingIt = std::find_if(HandleDatas.begin(), HandleDatas.end(),
				[&data](auto const& handleData)
				{
					return (handleData == data);
				});

			if (existingIt != HandleDatas.end())
			{
				uint32_t index = (uint32_t)(existingIt - HandleDatas.Begin());
				uint32_t glHandle = Handles[index];
				return Encode(index, glHandle);
			}

			return HandleT::Null();
		}


		HandleT	Emplace(DataT const& data)
		{
			uint32_t newID = (uint32_t)HandleDatas.Size();
			HandleDatas.EmplaceBack(data);

			if (Handles.Size() <= newID)
				Handles.Resize(newID + 1);

			if (Handles[newID] == 0)
				Traits::Create(1, &Handles[newID]);

			return Encode(newID, Handles[newID]);
		}


		HandleT	FindOrEmplace(DataT const& data)
		{
			auto existingIt = std::find_if(HandleDatas.begin(), HandleDatas.end(),
				[&data](auto const& handleData)
				{
					return (handleData == data);
				});

			if (existingIt != HandleDatas.end())
			{
				static_assert(sizeof(typename HandleT::Underlying) >= sizeof(uint64_t));

				uint32_t index = (uint32_t)(existingIt - HandleDatas.Begin());
				uint32_t glHandle = Handles[index];
				typename HandleT::Underlying handle = (uint64_t)index << 32;
				handle |= glHandle;
				return handle;
			}

			uint32_t newID = (uint32_t) HandleDatas.Size();
			HandleDatas.EmplaceBack(data);

			if (Handles.Size() <= newID)
				Handles.Resize(newID + 1);

			if (Handles[newID] == 0)
				Traits::Create(1, &Handles[newID]);

			return newID;
		}

		GLuint	GetGLHandle(uint32_t handleIdx)
		{
			MOE_DEBUG_ASSERT(handleIdx < Handles.Size());
			return Handles[handleIdx];
		}


		void	Delete(uint32_t id)
		{
			HandleDatas.Free(id);
			Traits::Delete(1, &Handles[id]);
			Handles[id] = 0;
		}


		Vector<DataT>	HandleDatas;
		Vector<GLuint>	Handles;
	};


	struct OpenGL4TextureData : TextureData
	{
		OpenGL4TextureData(TextureData const& data, GLuint texID) :
			TextureData(data),
			TextureID(texID)
		{}

		GLuint	TextureID{0};
	};

	using OpenGL4SamplerData = SamplerDescription;

	using GLTextureCache = OpenGLDataCache<OpenGL4TextureData, DeviceTextureHandle, TextureCacheTraits>;
	using GLSamplerCache = OpenGLDataCache<OpenGL4SamplerData, DeviceSamplerHandle, SamplerCacheTraits>;


	/*
	 * An abstract class for textures (1D, 2D, 3D, cubemap ...) and render targets (sometimes AKA renderbuffers) management.
	 */
	class OpenGL4TextureManager : public ITextureManager
	{
	public:

		~OpenGL4TextureManager() override = default;

		[[nodiscard]] DeviceTextureHandle Monocle_Graphics_API	CreateTexture2DFromFile(Ref<TextureResource> const& textureFile) override;

		[[nodiscard]] DeviceTextureHandle Monocle_Graphics_API	CreateTexture2D(TextureData const& data) override;

		[[nodiscard]] DeviceSamplerHandle Monocle_Graphics_API	CreateSampler(SamplerDescription const& samplerDesc) override;

		void	FreeUnderlyingTexture2D(DeviceTextureHandle tex);
		void	ResizeTexture2D(DeviceTextureHandle tex, std::pair<int, int> const& newDimensions);
		void	DestroyTexture2D(DeviceTextureHandle tex);

		[[nodiscard]] OpenGL4TextureData const&	GetTextureData(DeviceTextureHandle handle) const
		{
			return m_textures.Get((ObjectPoolID) handle.Get());
		}

		GLSamplerCache&	EditSamplerCache()
		{
			return m_samplers;
		}

	private:
		static float	GetMaximumAnisotropy();

		void	FreeUnderlyingTexture2D(OpenGL4TextureData& texData);

		DynamicObjectPool<OpenGL4TextureData>	m_textures;

		GLSamplerCache	m_samplers;
	};



}
