// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <utility> // std::pair
#include <string_view>
#endif

#include "Core/Containers/Vector/Vector.h"
#include "Core/Containers/Array/Array.h"
#include "Graphics/Camera/CameraHandle.h"

#include "Monocle_Graphics_Export.h"

#include "Graphics/Shader/Handle/ShaderHandle.h"
#include "Graphics/Shader/Program/ShaderProgramDescriptor.h"

#include "Graphics/VertexLayout/VertexLayoutHandle.h"
#include "Graphics/VertexLayout/VertexLayoutDescriptor.h"

#include "Graphics/Mesh/MeshHandle.h"
#include "Graphics/Mesh/MeshDataDescriptor.h"

#include "Graphics/Camera/ViewportHandle.h"
#include "Graphics/Camera/ViewportDescriptor.h"
#include "Graphics/Camera/Camera.h"
#include "Graphics/Camera/CameraManager.h"

#include "Graphics/Color/Color.h"

#include "Graphics/DeviceBuffer/UniformBufferHandle.h"

#include "Graphics/Resources/ResourceSet/ResourceSetHandle.h"
#include "Graphics/Resources/ResourceSet/ResourceSetDescriptor.h"

#include "Graphics/Resources/ResourceLayout/ResourceLayoutHandle.h"
#include "Graphics/Resources/ResourceLayout/ResourceLayoutDescriptor.h"

#include "Graphics/Device/GraphicsDevice.h"

namespace moe
{
	struct ShaderProgramDescriptor;


	/**
	 * \brief The base class for all other renderer classes.
	 *	Contains the graphics device necessary to output commands to the GPU.
	 *	Acts like a Facade to the rest of the world, hiding the underlying HAL rendering system.
	 *	https://en.wikipedia.org/wiki/Facade_pattern
	 */
	class Monocle_Graphics_API IGraphicsRenderer
	{
	public:

		using GraphicsContextSetup = void* (*)(const char *name);
		using ShaderFileList = Vector<std::pair<ShaderStage, std::string_view>>;

		virtual ~IGraphicsRenderer() = default;

		virtual bool	Initialize(IGraphicsRenderer::GraphicsContextSetup setupFunction) = 0;

		virtual void	Shutdown() = 0;

		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromSource(const ShaderProgramDescriptor& shaProDesc) = 0;
		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromBinary(const ShaderProgramDescriptor& shaProDesc) = 0;

		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromSourceFiles(const ShaderFileList& fileList) = 0;
		[[nodiscard]] virtual ShaderProgramHandle	CreateShaderProgramFromBinaryFiles(const ShaderFileList& fileList) = 0;

		virtual bool								RemoveShaderProgram(ShaderProgramHandle programHandle) = 0;


		[[nodiscard]] virtual VertexLayoutHandle	CreateVertexLayout(const VertexLayoutDescriptor& vertexLayoutDesc) = 0;


		/**
		 * \brief "Low-level" function used to transfer already available vertex data to the GPU.
		 * The created mesh will be "Static" in memory, i.e. the data store contents will be modified once and used many times.
		 * \param buffer A contiguous buffer containing the vertex data.
		 * \param size The size in bytes of the full buffer
		 */
		[[nodiscard]] virtual MeshHandle	CreateStaticMeshFromBuffer(const MeshDataDescriptor& vertexData, const MeshDataDescriptor& indexData) = 0;


		/**
		 * \brief Will delete any graphics resources associated to this mesh handle (vertex buffer, index buffer...).
		 * \param handle The handle of mesh to destroy
		 */
		virtual void	DeleteStaticMesh(MeshHandle handle) = 0;


		/**
		 * \brief This is a helper function to make CreateStaticMeshFromBuffer easier to use.
		 * \tparam VertexType The user-provided vertex data type
		 * \tparam N The number of vertices
		 * \param data The array containing the Vertices
		 * \return A handle referencing the created mesh
		 */
		template <typename VertexType, size_t N, size_t IndN = 0>
		MeshHandle	CreateStaticMesh(const Array<VertexType, N>& vertexData, const Array<uint32_t, IndN>& indexData = {});


		/**
		 * \brief Creates an orthographic camera and a new viewport associated to it.
		 * \param orthoDesc The description of the orthographic camera
		 * \param vpDesc The description of the viewport to create
		 * \return A handle to the created camera
		 */
		[[nodiscard]] virtual CameraHandle	CreateCamera(const OrthographicCameraDesc& orthoDesc, const ViewportDescriptor& vpDesc) = 0;

		/**
		 * \brief Creates a perspective camera and a new viewport associated to it.
		 * \param perspDesc The description of the perspective camera
		 * \param vpDesc The description of the viewport to create
		 * \return A handle to the created camera
		 */
		[[nodiscard]] virtual CameraHandle	CreateCamera(const PerspectiveCameraDesc& perspDesc, const ViewportDescriptor& vpDesc) = 0;


		/**
		 * \brief Creates an orthographic camera bound to the referenced viewport handle. Handy when you created the viewport separately from the camera.
		 * \param orthoDesc The description of the orthographic camera
		 * \param vpHandle The handle of the referenced viewport
		 * \return A handle to the created camera
		 */
		[[nodiscard]] virtual CameraHandle	CreateCamera(const OrthographicCameraDesc& orthoDesc, ViewportHandle vpHandle) = 0;


		/**
		 * \brief Creates a perspective camera bound to the referenced viewport handle. Handy when you created the viewport separately from the camera.
		 * \param perspDesc The description of the perspective camera
		 * \param vpHandle The handle of the referenced viewport
		 * \return A handle to the created camera
		 */
		[[nodiscard]] virtual CameraHandle	CreateCamera(const PerspectiveCameraDesc& perspDesc, ViewportHandle vpHandle) = 0;

		[[nodiscard]] virtual const CameraManager&	GetCameraManager() const = 0;
		[[nodiscard]] virtual CameraManager&		MutCameraManager() = 0;


		/**
		 * \brief Creates an uniform (or constant) buffer using the provided data and data size.
		 * WARNING : using OpenGL, this data buffer should respect the rules of STD 140 GLSL layout, or it won't upload properly to the GPU
		 * (see https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout for example).
		 * \param data The data buffer uploaded to the GPU
		 * \param dataSizeBytes The size of the buffer
		 * \return A handle to the created uniform buffer
		 */
		[[nodiscard]] virtual UniformBufferHandle		CreateUniformBuffer(const void* data, uint32_t dataSizeBytes) = 0;


		/**
		 * \brief Convenience helper function that creates an uniform buffer from the data you give it
		 * WARNING : using OpenGL, this data should respect the rules of STD 140 GLSL layout, or it won't upload properly to the GPU
		 * (see https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout for example).
		 * \tparam T The user data type
		 * \param data The data to send to the GPU.
		 * \return
		 */
		template <typename T>
		[[nodiscard]] UniformBufferHandle	CreateUniformBufferFrom(const T& data);







		/**
		 * \brief Generates a new resource layout from the description.
		 * Note : in OpenGL, there's no such thing as a resource layout : they're just stored in the device for convenience.
		 * \param desc The description of the resource layout to create
		 * \return A handle to the new resource layout
		 */
		[[nodiscard]] virtual ResourceLayoutHandle	CreateResourceLayout(const ResourceLayoutDescriptor& desc) = 0;

		/**
		 * \brief Generates a new resource set from the description.
		 * Note: in OpenGL, there's no such thing as a resource set : they're just stored in the device for convenience.
		 * \param desc The description of the resource set to create
		 * \return A handle to the new resource set
		 */
		[[nodiscard]] virtual ResourceSetHandle		CreateResourceSet(const ResourceSetDescriptor& desc) = 0;


		[[nodiscard]] virtual const IGraphicsDevice&	GetGraphicsDevice() const = 0;


		[[nodiscard]] virtual IGraphicsDevice&	MutGraphicsDevice() = 0;




		/* TODO: Test mini-framework, should be temporary and removed later */
		virtual void	UseCamera(CameraHandle camHandle) = 0;

		virtual void	Clear(const ColorRGBAf& clearColor) = 0;

		virtual void	UseMaterial(ShaderProgramHandle progHandle, ResourceSetHandle rscSetHandle) = 0;

		virtual void	DrawMesh(MeshHandle meshHandle, VertexLayoutHandle layoutHandle) = 0;
	};



	template <typename VertexType, size_t N, size_t IndN>
	MeshHandle IGraphicsRenderer::CreateStaticMesh(const Array<VertexType, N>& vertexData, const Array<uint32_t, IndN>& indexData)
	{
		return CreateStaticMeshFromBuffer(
			MeshDataDescriptor{ vertexData.Data(), vertexData.Size() * sizeof(VertexType), N},
			MeshDataDescriptor{ indexData.Data(), indexData.Size() * sizeof(uint32_t), IndN }
		);
	}

	template <typename T>
	UniformBufferHandle IGraphicsRenderer::CreateUniformBufferFrom(const T& data)
	{
		return CreateUniformBuffer(&data, sizeof(T));
	}
}
