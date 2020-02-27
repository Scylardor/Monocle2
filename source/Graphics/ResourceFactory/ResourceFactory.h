// Monocle Game Engine source files - Alexandre Baron

#pragma once

namespace moe
{
	/* An interface to generate API-specific resources. */
	class IGraphicsResourceFactory
	{
	public:
		virtual class VertexBuffer*	CreateVertexBuffer() = 0;

		virtual class IndexBuffer*	CreateIndexBuffer() = 0;

		virtual class VertexLayout* CreateVertexLayout() = 0;

		virtual class Texture*		CreateTexture() = 0;

		virtual class TextureView*	CreateTextureView() = 0;
	};

}