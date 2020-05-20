// Monocle Game Engine source files - Alexandre Baron


#include "Mesh.h"

#include "Graphics/RenderWorld/RenderWorld.h"

#include "Graphics/Material/MaterialBindings.h"

namespace moe
{
	Mesh::Mesh(RenderWorld* world, const GraphicObjectData& data):
		AGraphicObject(world, data)
	{
		// TODO : clean this up, this is probably not a good place to do that.

		ResourceLayoutDescriptor cubeLayoutDesc{
			{"ObjectMatrices", MaterialBlockBinding::OBJECT_MATRICES, ResourceKind::UniformBuffer, ShaderStage::Vertex }
		};

		auto objLayout = world->MutRenderer().MutGraphicsDevice().CreateResourceLayout(cubeLayoutDesc);

		m_perObjectUniformBuffer = world->MutRenderer().CreateUniformBufferFrom(ObjectMatrices());

		ResourceSetDescriptor cubeSetDesc{
			objLayout, {m_perObjectUniformBuffer}
		};

		m_perObjectResourceSetHandle = world->MutRenderer().MutGraphicsDevice().CreateResourceSet(cubeSetDesc);
	}


	void Mesh::UpdateObjectMatrices(const Camera& currentCamera)
	{
		const Mat4& model = GetTransform().Matrix();
		const Mat4& view = currentCamera.GetViewMatrix();
		const Mat4& vp = currentCamera.GetViewProjectionMatrix();
		const Mat4 modelView = view * model;

		ObjectMatrices matrices{ model, modelView, vp * model, Mat3(modelView).GetInverseTransposed() };

		m_world->MutRenderer().MutGraphicsDevice().UpdateBuffer(m_perObjectUniformBuffer, &matrices, sizeof(ObjectMatrices));

		m_world->MutRenderer().UseResourceSet(m_perObjectResourceSetHandle);
	}
}
