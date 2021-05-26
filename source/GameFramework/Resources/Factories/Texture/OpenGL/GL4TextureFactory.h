#pragma once

#include "GameFramework/Resources/Factories/Texture/TextureFactory.h"
#include "Core/Resource/ResourceManager.h"

#include "Core/Containers/SparseArray/SparseArray.h"

#include "Monocle_GameFramework_Export.h"

namespace moe
{
	class Monocle_GameFramework_API GL4TextureFactory : public ITextureFactory2
	{
	public:
		GL4TextureFactory(ResourceManager& rscMgr, bool verticalFlipLoadedTextures = true);

		~GL4TextureFactory() override;

		// Disallow copy of the factory
		GL4TextureFactory(const GL4TextureFactory&) = delete;
		GL4TextureFactory& operator=(const GL4TextureFactory& rhs) = delete;


		TexHandle	CreateTexture2D(const Tex2DDescriptor & tex2dDesc) override;

		TexHandle	CreateTexture2DFromFile(const Tex2DFileDescriptor & tex2dFileDesc) override;

		// Utilizes a Texture2D Descriptor to describe each face of the cube map.
		TexHandle	CreateCubemapTexture(const CubemapTexDescriptor & cubeFaceTexDesc) override;

		// Describes properties of each face and a 6-size array of filenames to load cubemap's faces from.
		TexHandle	CreateCubemapTextureFromFile(const CubemapTexFilesDescriptor & cubemapFilesDesc) override;

		void		DeleteTexture(TexHandle deletedTexture) override;

	private:

		void		AddTexture();
		TexHandle	AddRenderbuffer(GLuint GLtextureFormat, uint32_t width, uint32_t height);


		static GLuint	EncodeRenderbufferHandle(GLuint renderBufferID);
		static GLuint	DecodeRenderbufferHandle(GLuint renderbufferHandle);
		static bool		IsARenderBufferHandle(GLuint texHandle);

		bool	m_verticalFlipLoadedImages = true;

		SparseArray<GLuint>		m_TexHandles;

	};




}
