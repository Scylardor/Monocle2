// Monocle Game Engine source files - Alexandre Baron

#include "BasicQuad.h"


#include "Core/Containers/Array/Array.h"
#include "GameFramework/Engine/Engine.h"
#include "GameFramework/Service/RenderService/RenderService.h"
#include "GameFramework/Service/ResourceService/ResourceService.h"
#include "Graphics/Vertex/VertexFormats.h"


namespace moe
{

	void BasicQuad::Start()
	{
		OpenGLApp3D::Start();

		auto* rscSvc = EditEngine()->AddService<ResourceService>();
		//auto* render = EditEngine()->EditService<RenderService>();

		Array<Vertex_PosColorUV, 4> helloQuad{
		{{0.5f, 0.5f, 0.0f } , {1.0f, 0.0f, 0.0f }, {1.0f, 1.0f} },   // top right
		{{0.5f, -0.5f, 0.0f } , {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // bottom right
		{{-0.5f, -0.5f, 0.0f} , {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},   // bottom left
		{{-0.5f, 0.5f, 0.0f } , {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}    // top left
		};

		Array<uint32_t, 6> helloIndices = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};

		auto meshRsc = rscSvc->EmplaceResource<MeshResource>(
			HashString("QuadMesh"),
			helloQuad.Data(), sizeof(helloQuad[0]), helloQuad.Size(),
			helloIndices.Data(), sizeof(helloIndices[0]), helloIndices.Size());



		//render->
	}
}
