// Monocle Game Engine source files - Alexandre Baron

#include "Material.h"

#include "Graphics/Renderer/Renderer.h"

#include "Graphics/RenderWorld/RenderWorld.h"


namespace moe
{

	void Material::UpdateObjectMatrices(AGraphicObject& object, DeviceBufferHandle ubHandle)
	{
		RenderWorld* objWorld = object.GetRenderWorld();
		if (objWorld == nullptr)
			return;

		const Camera* cam = objWorld->GetCurrentCamera();
		if (!MOE_ASSERT(cam != nullptr))
			return;

		const Mat4& model = object.GetTransform().Matrix();

		const Mat4& view = cam->GetViewMatrix();
		const Mat4& vp = cam->GetViewProjectionMatrix();

		ObjectMatrices matrices{ model, view * model, vp * model, Mat3(model).GetInverse().GetTransposed() };

		objWorld->MutRenderer().UpdateSubBufferRange(ubHandle, 0, &matrices, sizeof ObjectMatrices);
	}



}
