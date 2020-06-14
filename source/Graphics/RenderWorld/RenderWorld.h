// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Containers/Array/Array.h"

#include "Monocle_Graphics_Export.h"

#include "Graphics/Mesh/Mesh.h"
#include "Graphics/Mesh/InstancedMesh.h"
#include "Graphics/Mesh/MeshDataDescriptor.h"

#include "Graphics/VertexLayout/VertexLayoutHandle.h"

#include "Graphics/Camera/ViewportHandle.h"
#include "Graphics/Camera/ViewportDescriptor.h"
#include "Graphics/Camera/CameraManager.h"

#include "Core/Containers/FreeList/Freelist.h"

#include "Core/Containers/FreeList/PolymorphicFreelist.h"

#include "Graphics/Renderer/Renderer.h"


namespace moe
{
	class RenderWorld
	{
	public:
		RenderWorld(class IGraphicsRenderer& renderer) :
			m_renderer(renderer)
		{
			m_meshFreelist.Reserve(1024); // TODO: temporary solution to avoid invalidating pointers
		}

		~RenderWorld() = default;

		/**
		 * \brief "Low-level" function used to transfer already available vertex data to the GPU.
		 * The created mesh will be "Static" in memory, i.e. the data store contents will be modified once and used many times.
		 * \param buffer A contiguous buffer containing the vertex data.
		 * \param size The size in bytes of the full buffer
		 */
		Monocle_Graphics_API [[nodiscard]]	Mesh*	CreateStaticMeshFromBuffer(const MeshDataDescriptor& vertexData, const MeshDataDescriptor& indexData);

		Monocle_Graphics_API [[nodiscard]]	InstancedMesh*	CreateInstancedMeshFromBuffer(const MeshDataDescriptor& vertexData, const MeshDataDescriptor& indexData);

		/**
		 * \brief Will delete any graphics resources associated to this mesh handle (vertex buffer, index buffer...).
		 * \param handle The handle of mesh to destroy
		 */
		Monocle_Graphics_API void	DeleteStaticMesh(Mesh* mesh);


		/**
		 * \brief This is a helper function to make CreateStaticMeshFromBuffer easier to use.
		 * \tparam VertexType The user-provided vertex data type
		 * \tparam N The number of vertices
		 * \param data The array containing the Vertices
		 * \return A handle referencing the created mesh
		 */
		template <typename VertexType, size_t N, size_t IndN = 0>
		Mesh*	CreateStaticMesh(const Array<VertexType, N>& vertexData, const Array<uint32_t, IndN>& indexData = {});

		template<typename VertexType, size_t N>
		Mesh* CreateStaticMesh(VertexType(&vertexData)[N]);

		template <typename VertexType, size_t N, size_t IndN = 0>
		InstancedMesh*	CreateInstancedMesh(const Array<VertexType, N>& vertexData, const Array<uint32_t, IndN>& indexData = {});

		template<typename VertexType, size_t N>
		InstancedMesh* CreateInstancedMesh(VertexType(&vertexData)[N]);

		/**
		 * \brief Creates an orthographic camera and a new viewport associated to it.
		 * \param orthoDesc The description of the orthographic camera
		 * \param vpDesc The description of the viewport to create
		 * \return A handle to the created camera
		 */
		Monocle_Graphics_API [[nodiscard]]  Camera*	CreateCamera(const OrthographicCameraDesc& orthoDesc, const ViewportDescriptor& vpDesc);

		/**
		 * \brief Creates a perspective camera and a new viewport associated to it.
		 * \param perspDesc The description of the perspective camera
		 * \param vpDesc The description of the viewport to create
		 * \return A handle to the created camera
		 */
		Monocle_Graphics_API [[nodiscard]]  Camera*	CreateCamera(const PerspectiveCameraDesc& perspDesc, const ViewportDescriptor& vpDesc);


		/**
		 * \brief Creates an orthographic camera bound to the referenced viewport handle. Handy when you created the viewport separately from the camera.
		 * \param orthoDesc The description of the orthographic camera
		 * \param vpHandle The handle of the referenced viewport
		 * \return A handle to the created camera
		 */
		Monocle_Graphics_API [[nodiscard]]  Camera*	CreateCamera(const OrthographicCameraDesc& orthoDesc, ViewportHandle vpHandle);


		/**
		 * \brief Creates a perspective camera bound to the referenced viewport handle. Handy when you created the viewport separately from the camera.
		 * \param perspDesc The description of the perspective camera
		 * \param vpHandle The handle of the referenced viewport
		 * \return A handle to the created camera
		 */
		Monocle_Graphics_API [[nodiscard]]  Camera*	CreateCamera(const PerspectiveCameraDesc& perspDesc, ViewportHandle vpHandle);


		Monocle_Graphics_API void	UseCamera(Camera* cameraToUse);


		class IGraphicsRenderer&	MutRenderer() { return m_renderer; }

		[[nodiscard]] const CameraManager&	GetCameraManager() const { return m_cameraManager; }
		[[nodiscard]] CameraManager&		MutCameraManager() { return m_cameraManager; }

		const Camera*	GetCurrentCamera() const { return m_currentCamera; }

		Monocle_Graphics_API void	DrawMesh(Mesh* drawnMesh, VertexLayoutHandle layoutHandle, Material* material = nullptr);

		Monocle_Graphics_API void	DrawInstancedMesh(InstancedMesh* drawnInstancedMesh, VertexLayoutHandle layoutHandle, Material* material = nullptr);


		Monocle_Graphics_API void	BeginDraw();


		[[nodiscard]] GraphicObjectData	ReallocObjectUniformGraphicData(const GraphicObjectData& oldData, uint32_t newNeededSize);

	protected:

		IGraphicsRenderer&	m_renderer;

		CameraManager		m_cameraManager;

		Freelist<Mesh>		m_meshFreelist;

		Freelist<InstancedMesh>		m_instancedMeshFreelist; // TODO: rework that (merge the two mesh freelist together ?)

		PolymorphicFreelist<AGraphicObject*>	m_objects;

		Camera*				m_currentCamera = nullptr;

		Vector<char>		m_objectsDataBuffer;

		Vector<FreelistID>	m_activeObjects;

		Vector<CameraManager::CameraID>	m_activeCameras;

	};


	template <typename VertexType, size_t N, size_t IndN>
	Mesh* RenderWorld::CreateStaticMesh(const Array<VertexType, N>& vertexData, const Array<uint32_t, IndN>& indexData)
	{
		return CreateStaticMeshFromBuffer(
			MeshDataDescriptor{ vertexData.Data(), vertexData.Size() * sizeof(VertexType), N },
			MeshDataDescriptor{ indexData.Data(), indexData.Size() * sizeof(uint32_t), IndN }
		);
	}

	template <typename VertexType, size_t N>
	Mesh* RenderWorld::CreateStaticMesh(VertexType(& vertexData)[N])
	{
		return CreateStaticMeshFromBuffer(
			MeshDataDescriptor{ vertexData, sizeof(vertexData), N },
			MeshDataDescriptor {}
		);
	}

	template <typename VertexType, size_t N, size_t IndN>
	InstancedMesh* RenderWorld::CreateInstancedMesh(const Array<VertexType, N>& vertexData, const Array<uint32_t, IndN>& indexData)
	{
		return CreateInstancedMeshFromBuffer(
			MeshDataDescriptor{ vertexData.Data(), vertexData.Size() * sizeof(VertexType), N },
			MeshDataDescriptor{ indexData.Data(), indexData.Size() * sizeof(uint32_t), IndN }
		);
	}

	template <typename VertexType, size_t N>
	InstancedMesh* RenderWorld::CreateInstancedMesh(VertexType(& vertexData)[N])
	{
		return CreateInstancedMeshFromBuffer(
			MeshDataDescriptor{ vertexData, sizeof(vertexData), N },
			MeshDataDescriptor{}
		);
	}
}
