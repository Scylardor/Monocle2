// Monocle Game Engine source files - Alexandre Baron

#pragma once

#ifdef MOE_STD_SUPPORT
#include <utility> // std::pair
#include <string_view>
#endif

#include "Core/Containers/Vector/Vector.h"
#include "Graphics/Camera/CameraHandle.h"

#include "Monocle_Graphics_Export.h"

#include "Graphics/Shader/Handle/ShaderHandle.h"
#include "Graphics/Shader/Program/ShaderProgramDescriptor.h"

#include "Graphics/VertexLayout/VertexLayoutHandle.h"
#include "Graphics/VertexLayout/VertexLayoutDescriptor.h"

#include "Graphics/Mesh/MeshHandle.h"

#include "Graphics/Camera/Camera.h"

#include "Graphics/Color/Color.h"

#include "Graphics/DeviceBuffer/UniformBufferHandle.h"

#include "Graphics/Resources/ResourceSet/ResourceSetHandle.h"
#include "Graphics/Resources/ResourceSet/ResourceSetDescriptor.h"
#include "Graphics/Resources/ResourceLayout/ResourceLayoutHandle.h"
#include "Graphics/Resources/ResourceLayout/ResourceLayoutDescriptor.h"

#include "Graphics/Device/GraphicsDevice.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"

#include "Graphics/RenderWorld/RenderWorldHandle.h"

#include "Graphics/RenderWorld/GraphicsObjectData.h"


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
		 * \brief Creates an uniform (or constant) buffer using the provided data and data size.
		 * WARNING : using OpenGL, this data buffer should respect the rules of STD 140 GLSL layout, or it won't upload properly to the GPU
		 * (see https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout for example).
		 * \param data The data buffer uploaded to the GPU
		 * \param dataSizeBytes The size of the buffer
		 * \return A handle to the created uniform buffer
		 */
		[[nodiscard]] virtual DeviceBufferHandle		CreateUniformBuffer(const void* data, uint32_t dataSizeBytes) = 0;


		/**
		 * \brief Convenience helper function that creates an uniform buffer from the data you give it
		 * WARNING : using OpenGL, this data should respect the rules of STD 140 GLSL layout, or it won't upload properly to the GPU
		 * (see https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout for example).
		 * \tparam T The user data type
		 * \param data The data to send to the GPU.
		 * \return
		 */
		template <typename T>
		[[nodiscard]] DeviceBufferHandle	CreateUniformBufferFrom(const T& data);



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



		/**
		 * \brief Will bind all resources (uniform buffers, texture units and samplers) contained in the referenced resource set.
		 * \param rscSetHandle
		 */
		virtual void		UseResourceSet(const ResourceSetHandle rscSetHandle) = 0;



		[[nodiscard]] virtual const IGraphicsDevice&	GetGraphicsDevice() const = 0;


		[[nodiscard]] virtual IGraphicsDevice&	MutGraphicsDevice() = 0;


		[[nodiscard]] virtual class RenderWorld&	CreateRenderWorld() = 0;

		[[nodiscard]] virtual DeviceBufferHandle	AllocateObjectMemory(const uint32_t size) = 0;
		virtual void								CopyObjectMemory(DeviceBufferHandle from, uint32_t fromSizeBytes, DeviceBufferHandle to) = 0;
		virtual void								ReleaseObjectMemory(DeviceBufferHandle freedHandle) = 0;

		virtual void	UpdateSubBufferRange(DeviceBufferHandle handle, uint32_t offset, void* data, uint32_t dataSize) = 0;

		[[nodiscard]] virtual GraphicObjectData		ReallocObjectUniformGraphicData(const GraphicObjectData& oldData, uint32_t newNeededSize) = 0;

		/* TODO: Test mini-framework, should be temporary and removed later */
		virtual void	Clear(const ColorRGBAf& clearColor) = 0;

		virtual void	UseMaterial(ShaderProgramHandle progHandle, ResourceSetHandle rscSetHandle) = 0;

		virtual void	UseMaterial(Material* material) = 0;

		virtual void	UseMaterialInstance(class MaterialInstance* material) = 0;


		virtual void	UseMaterialPerObject(Material* material, AGraphicObject& object) = 0;

		virtual void	BindFramebuffer(FramebufferHandle fbHandle) = 0;
		virtual void	UnbindFramebuffer(FramebufferHandle fbHandle) = 0;

	};

	template <typename T>
	DeviceBufferHandle IGraphicsRenderer::CreateUniformBufferFrom(const T& data)
	{
		return CreateUniformBuffer(&data, sizeof(T));
	}
}
