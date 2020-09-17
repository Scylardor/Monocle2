#pragma once


#include "AbstractResourceFactory.h"

#include "GameFramework/Resources/Resource/TextureResource.h"

#include "Graphics/Texture/TextureHandle.h"



namespace moe
{
	struct Tex2DDescriptor;
	struct Texture2DFileDescriptor;
	struct Texture2DDescriptor;

	class ITextureFactory
{
public:

	virtual ~ITextureFactory() = default;


	virtual TextureHandle	CreateTexture2D(const Tex2DDescriptor& tex2dDesc);
	virtual TextureHandle	CreateTexture2DFromFile(const Texture2DFileDescriptor& tex2dFileDesc);

	// Utilizes a Texture2D Descriptor to describe each face of the cube map.
	virtual TextureHandle	CreateCubemapTexture(const Tex2DDescriptor& );

	virtual TextureHandle	CreateCubemapTextureFromFile(const Texture2DFileDescriptor& tex2dFileDesc);


protected:


private:


};


}