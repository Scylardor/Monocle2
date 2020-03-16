// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Graphics/ResourceFactory/ResourceFactory.h"

namespace moe
{
	class OpenGLResourceFactory : public IGraphicsResourceFactory
	{
	public:
		virtual class VertexBuffer* CreateVertexBuffer() override;

		virtual class IndexBuffer* CreateIndexBuffer() override;

		virtual class VertexLayout* CreateVertexLayout() override;

		virtual class Texture* CreateTexture() override;

		virtual class TextureView* CreateTextureView() override

			;

	};

}