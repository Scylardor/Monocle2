#pragma once


#include "Core/Resource/ResourceFactory.h"
#include "Graphics/Texture/TextureDescription.h"

#include "Graphics/Texture/TextureHandle.h"

#include "Monocle_GameFramework_Export.h"


namespace moe
{
	class ResourceManager;

	class Monocle_GameFramework_API ITextureFactory2  : public IResourceFactory
	{
	public:

		ITextureFactory2(ResourceManager& rscMgr) :
			m_ResourceManager(&rscMgr)
		{}

		virtual ~ITextureFactory2() = default;


		virtual TexHandle	CreateTexture2D(const Tex2DDescriptor& tex2dDesc) = 0;
		virtual TexHandle	CreateTexture2DFromFile(const Tex2DFileDescriptor& tex2dFileDesc) = 0;

		// Utilizes a Texture2D Descriptor to describe each face of the cube map.
		virtual TexHandle	CreateCubemapTexture(const CubemapTexDescriptor& cubeFaceTexDesc) = 0;

		// Describes properties of each face and a 6-size array of filenames to load cubemap's faces from.
		virtual TexHandle	CreateCubemapTextureFromFile(const CubemapTexFilesDescriptor& cubemapFilesDesc) = 0;


		virtual void			DeleteTexture(TexHandle deletedTexture) = 0;


	protected:
		ResourceManager* m_ResourceManager = nullptr;


	private:


	};

	//template <>
	//inline TextureResource* ResourceManager::CreateResource(const Tex2DDescriptor& tex2dDesc)
	//{
	//	auto* texFactory = EditFactory<ITextureFactory>(ResourceType::Texture);
	//	texFactory->CreateTexture2D(tex2dDesc);
	//	return nullptr;
	//}


}