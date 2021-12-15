#pragma once

#include <memory>


#include "Core/Resource/BaseResource.h"
#include "Core/Resource/ResourceRef.h"
#include "Core/Resource/TextureResource.h"
#include "BlendState/BlendStateDescriptor.h"
#include "DepthStencilState/DepthStencilStateDescriptor.h"
#include "Graphics/DeviceBuffer/DeviceBufferHandle.h"
#include "Graphics/RHI/TextureManager/TextureManager.h"
#include "Graphics/Sampler/SamplerDescriptor.h"
#include "Graphics/VertexLayout/VertexElementFormat.h"
#include "Graphics/VertexLayout/VertexLayoutDescriptor.h"
#include "RasterizerState/RasterizerStateDescriptor.h"
#include "Shader/Program/ShaderProgramDescription.h"
#include "Topology/PrimitiveTopology.h"


namespace moe
{
	class MaterialModulesResource;


	struct VertexBindingDescription
	{
		VertexBindingDescription(std::string sem, VertexBindingFormat fmt, uint32_t div = 0) :
			Semantic(std::move(sem)), Format(fmt), Divisor(div)
		{}

		bool	operator==(const VertexBindingDescription& rhs) const
		{
			if (&rhs != this)
			{
				return (Semantic == rhs.Semantic && Format == rhs.Format && Divisor == rhs.Divisor);
			}
			return true;
		}

		std::string			Semantic{ "" };
		VertexBindingFormat	Format;
		uint32_t			Divisor{ 0 };
	};

	struct VertexLayoutDescription
	{
		VertexLayoutDescription() = default;

		VertexLayoutDescription(Vector<VertexBindingDescription> bindings, LayoutType layout = LayoutType::Interleaved) :
			Bindings(std::move(bindings)), BindingsLayout(layout)
		{}


		bool	operator==(const VertexLayoutDescription& rhs) const
		{
			if (&rhs != this)
			{
				return (Bindings == rhs.Bindings && BindingsLayout == rhs.BindingsLayout);
			}
			return true;
		}


		Vector<VertexBindingDescription>	Bindings;
		LayoutType							BindingsLayout{ LayoutType::Interleaved };
	};


	/**
	* \brief This describes how a graphics pipeline should be built.
	* It is meant to describe a pipeline object in an agnostic way (regardless of OpenGL, D3D, Vulkan or Metal).
	*/
	struct PipelineDescription
	{
		BlendStateDescriptor		BlendStateDesc;
		DepthStencilStateDescriptor	DepthStencilStateDesc;
		RasterizerStateDescriptor	RasterizerStateDesc;
		PrimitiveTopology			Topology{ PrimitiveTopology::TriangleList };
		VertexLayoutDescription		VertexLayout;
	};


	struct ResourceBinding
	{
		ResourceBinding(int nbr, BindingType type, ShaderStage stage) :
			Number((uint16_t)nbr), Type(type), Stage(stage)
		{}

		uint16_t	Number = 0;
		BindingType	Type = BindingType::None;
		ShaderStage	Stage = ShaderStage::All;
	};
	using ResourceBindingList = Vector<ResourceBinding>;


	struct ResourceSetLayout
	{
		ResourceSetLayout(int nbr, ResourceBindingList bindings) :
			Number((uint16_t)nbr), Bindings(std::move(bindings))
		{}

		uint16_t			Number = 0;
		ResourceBindingList	Bindings;
	};

	struct ResourceSetLayoutsDescription
	{
		ResourceSetLayout& AddResourceLayout(uint16_t setNumber, Vector<ResourceBinding> bindings)
		{
			Layouts.EmplaceBack(setNumber, std::move(bindings));
			return Layouts.Back();
		}

		Vector<ResourceSetLayout>	Layouts{};
	};


	struct ResourceSetBinding
	{
		ResourceSetBinding(int binding, int set = 0) :
			BindingNumber((uint16_t)binding), SetNumber((uint16_t)set)
		{}

		bool operator==(const ResourceSetBinding& other) const
		{
			if (&other == this)
				return true;
			return (BindingNumber == other.BindingNumber && SetNumber == other.SetNumber);
		}

		uint16_t	BindingNumber{ 0 };
		uint16_t	SetNumber{ 0 };
	};

	struct BufferBinding : ResourceSetBinding
	{
		inline static const uint64_t WHOLE_RANGE = (uint64_t)-1;

		BufferBinding(DeviceBufferHandle buffer, int binding, int set, uint64_t off = 0, uint64_t range = WHOLE_RANGE) :
			ResourceSetBinding(binding, set),
			BufferHandle(buffer),
			Offset(off),
			Range(range)
		{}

		bool operator==(const BufferBinding& other) const
		{
			if (&other == this)
				return true;
			return ResourceSetBinding::operator==(other) &&
				(BufferHandle == other.BufferHandle && Offset == other.Offset && Range == other.Range);
		}

		DeviceBufferHandle	BufferHandle{};
		uint64_t			Offset{ 0 }; // TODO : this information is also in the buffer's MeshData
		uint64_t			Range{ WHOLE_RANGE };
	};


	struct TextureBinding : ResourceSetBinding
	{
		// TODO: needs to change (organize by set or swap set and binding as its confusing right now)
		TextureBinding(int set, int binding, DeviceTextureHandle texture) :
			ResourceSetBinding(binding, set),
			TextureHandle(texture)
		{}

		bool operator==(const TextureBinding& other) const
		{
			if (&other == this)
				return true;
			return ResourceSetBinding::operator==(other) &&
				(TextureHandle == other.TextureHandle);
		}

		DeviceTextureHandle	TextureHandle{};
	};

	struct SamplerBinding : ResourceSetBinding
	{
		SamplerBinding(int set, int binding, DeviceSamplerHandle sampler) :
			ResourceSetBinding(binding, set),
			SamplerHandle(sampler)
		{}

		bool operator==(const SamplerBinding& other) const
		{
			if (&other == this)
				return true;
			return ResourceSetBinding::operator==(other) &&
				(SamplerHandle == other.SamplerHandle);
		}

		DeviceSamplerHandle	SamplerHandle{};
	};


	struct ResourceSetsDescription
	{
		using BindingVariant = std::variant< BufferBinding, TextureBinding, SamplerBinding >;

		Vector<BindingVariant>	Bindings;

		template <typename T, typename... Args>
		T& EmplaceBinding(Args&&... args)
		{
			Bindings.EmplaceBack(std::in_place_type<T>, std::forward<Args>(args)...);
			return std::get<T>(Bindings.Back());
		}

	};


	struct MaterialPassDescription
	{
		MaterialPassDescription& AssignShaderProgramDescription(ShaderProgramDescription programDesc)
		{
			ShaderProgram = std::move(programDesc);
			return *this;
		}

		MaterialPassDescription& AssignPipelineVertexLayout(VertexLayoutDescription layoutDesc)
		{
			Pipeline.VertexLayout = std::move(layoutDesc);
			return *this;
		}


		PipelineDescription				Pipeline;
		ShaderProgramDescription		ShaderProgram;
		ResourceSetLayoutsDescription	ResourceSetLayouts;
		ResourceSetsDescription			ResourceBindings;
	};

	struct MaterialDescription
	{
		MaterialPassDescription& NewPassDescription()
		{
			PassDescriptors.EmplaceBack();
			return PassDescriptors.Back();
		}


		Vector<MaterialPassDescription>	PassDescriptors;
	};



	class MaterialResource : public IBaseResource
	{
	public:

		MaterialResource(MaterialDescription desc) :
			Description(std::move(desc))
		{}

		MaterialDescription const& GetDescription() const
		{
			return Description;
		}

	private:

		MaterialDescription	Description;

	};

	class AMaterialModule
	{
	public:

		AMaterialModule(uint8_t setNumber) :
			m_setNumber(setNumber)
		{}


		virtual ~AMaterialModule() = default;


		virtual void	UpdateResources(MaterialModulesResource& updatedMaterial) = 0;


	protected:

		uint8_t	m_setNumber{ 0xFF };

	};

	class MaterialModulesResource : public IBaseResource
	{
	public:

		MaterialModulesResource() = default;

		~MaterialModulesResource() override = default;

		virtual std::unique_ptr<MaterialModulesResource> Clone()
		{
			return nullptr;
		}

		template <typename TModule, typename... Args>
		TModule& EmplaceModule(uint8_t setNumber, Args&&... args)
		{
			static_assert(std::is_base_of_v<AMaterialModule, TModule>);
			auto modulePtr = std::make_unique<TModule>(setNumber, std::forward<Args>(args)...);
			// get the ptr before it gets moved
			TModule* ptr = modulePtr.get();

			AddNewModule(std::move(modulePtr));

			return *ptr;
		}


		virtual void				UpdateResources(uint8_t /*resourceSet*/)
		{

		}

		virtual MaterialModulesResource& BindTextureResource(uint32_t /*set*/, uint32_t /*binding*/, const Ref<TextureResource>& /*tex*/)
		{
			return *this;
		}

	protected:

		virtual void	AddNewModule(std::unique_ptr<AMaterialModule> newModule)
		{}

	};



}
