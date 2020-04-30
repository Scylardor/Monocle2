// Monocle Game Engine source files - Alexandre Baron


#ifdef MOE_OPENGL

#include "OpenGLGraphicsDevice.h"

#include "Graphics/VertexLayout/OpenGL/OpenGLVertexLayout.h"

#include "Graphics/VertexLayout/OpenGL/OpenGLVertexFormat.h"

namespace moe
{

	void OpenGLGraphicsDevice::Initialize()
	{
		m_vertexBufferPool.ReservePoolMemory(GL_DYNAMIC_STORAGE_BIT);
		m_indexBufferPool.ReservePoolMemory(GL_DYNAMIC_STORAGE_BIT);
	}


	void OpenGLGraphicsDevice::Destroy()
	{
		m_shaderManager.Clear();

		// Destroy all VAOs at the same time.
		Vector<GLuint> vaoIDs(m_layouts.Size());

		int iLayout = 0;
		for (const auto& layout : m_layouts)
		{
			vaoIDs[iLayout] = layout;
			iLayout++;
		}

		glDeleteVertexArrays((GLsizei)vaoIDs.Size(), vaoIDs.Data());

		m_layouts.Clear();
	}


	VertexLayoutHandle OpenGLGraphicsDevice::CreateVertexLayout(const VertexLayoutDescriptor& vertexLayoutDesc)
	{
		// First and foremost, check that we do not have an existing vertex layout that could fit this description...
		// It's ok to linear search we don't expect a lot of existing vertex layouts anyway
		auto vtxLayoutIt = std::find_if(m_layouts.Begin(), m_layouts.End(), [&vertexLayoutDesc](const OpenGLVertexLayout& oglVtxLayout)
		{
			return (oglVtxLayout.ReadDescriptor() == vertexLayoutDesc);
		});

		if (vtxLayoutIt != m_layouts.End())
		{
			// we found a matching one : don't bother recreating a new one and give out a handle
			return VertexLayoutHandle{*vtxLayoutIt};
		}

		GLuint vaoID;
		glCreateVertexArrays(1, &vaoID);

		// Yeah it's a bit weird,don't initialize the handle just yet ! Will help to know if our algorithm worked.
		VertexLayoutHandle handle;

		uint32_t totalStride{ 0 }; // only really useful for Interleaved mode

		switch (vertexLayoutDesc.Type())
		{
			case VertexLayoutDescriptor::Interleaved:
			{
				int iAttrib = 0;

				for (const VertexElementDescriptor& desc : vertexLayoutDesc)
				{
					auto OpenGLInfoOpt = TranslateOpenGLVertexElementFormat(desc.m_format);
					if (false == OpenGLInfoOpt.has_value())
					{
						MOE_ERROR(ChanGraphics, "Failed to translate following OpenGL format : %u", desc.m_format);
						break;
					}

					const auto& glVertexElemFmt = OpenGLInfoOpt.value();

					// TODO : switch to glVertexAttribPointer if retrocompat needed
					// Enable attrib index for given VAO
					glEnableVertexArrayAttrib(vaoID, iAttrib);

					// Specify format for this attrib : type; number of components, normalized. Use totalStride as relativeoffset because data is interleaved (XYZ XYZ...)
					glVertexArrayAttribFormat(vaoID, iAttrib, glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type, glVertexElemFmt.m_normalized, totalStride);

					// We can bind all attributes to same binding index because data is interleaved.
					glVertexArrayAttribBinding(vaoID, iAttrib, 0);

					auto sizeOpt = FindOpenGLTypeSize(glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
					if (false == sizeOpt.has_value())
					{
						MOE_ERROR(ChanGraphics, "Failed to find the OpenGL type size of following combination : %u %i", glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
						break; // invalid, could not find component size
					}

					totalStride += sizeOpt.value();

					iAttrib++;
				}

				handle = VertexLayoutHandle{vaoID}; // if we're here initialization was successful until the end: set the handle.
			}
			break;

			case VertexLayoutDescriptor::Packed:
			{
				int iAttrib = 0;

				for (const VertexElementDescriptor& desc : vertexLayoutDesc)
				{
					auto OpenGLInfoOpt = TranslateOpenGLVertexElementFormat(desc.m_format);
					if (false == OpenGLInfoOpt.has_value())
					{
						MOE_ERROR(ChanGraphics, "Failed to translate following OpenGL format : %u", desc.m_format);
						break;
					}

					const auto& glVertexElemFmt = OpenGLInfoOpt.value();

					// More of a safety measure because we actually don't need the total size in packed mode.
					auto sizeOpt = FindOpenGLTypeSize(glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
					if (false == sizeOpt.has_value())
					{
						MOE_ERROR(ChanGraphics, "Failed to find the OpenGL type size of following combination : %u %i", glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type);
						break;
					}

					// TODO : switch to glVertexAttribPointer if retrocompat needed
					// Enable attrib index for given VAO
					glEnableVertexArrayAttrib(vaoID, iAttrib);

					// Specify format for this attrib : type; number of components, normalized. Use 0 as relativeoffset because data will be tightly packed (XXX YYY...)
					glVertexArrayAttribFormat(vaoID, iAttrib, glVertexElemFmt.m_numCpnts, glVertexElemFmt.m_type, glVertexElemFmt.m_normalized, 0);

					// As data will be tightly packed, we need to bind each attribute to a different binding index.
					glVertexArrayAttribBinding(vaoID, iAttrib, iAttrib);

					iAttrib++;
				}

				handle = VertexLayoutHandle{ vaoID }; // if we're here initialization was successful until the end: set the handle.
			}
			break;

			default:
			MOE_ASSERT(false); // not supposed to happen
		}

		if (handle.IsNull())
		{
			// There was a problem somewhere : delete our created VAO
			glDeleteVertexArrays(1, &vaoID);
		}
		else
		{
			// The VAO was successfully initialized : we store our vertex layout
			m_layouts.EmplaceBack(vertexLayoutDesc, vaoID, totalStride);
		}

		return handle;
	}


	const VertexLayout* OpenGLGraphicsDevice::GetVertexLayout(VertexLayoutHandle handle) const
	{
		// Kind of a trick : we rely on the fact all valid VAO IDs will always start at 1 and increment by one.
		// So, a valid handle is simply at index (VAO id - 1) in the array.
		// NOTE: this assumption won't stand if we add the ability to remove vertex layouts. So far, a vertex layout gets created "forever" (you cannot delete them).

		if (MOE_ASSERT(handle.Get() <= m_layouts.Size()))
		{
			return &m_layouts[handle.Get() - 1];
		}

		return nullptr;
	}


	VertexBufferHandle OpenGLGraphicsDevice::CreateStaticVertexBuffer(const void* data, size_t byteSize)
	{
		const uint32_t meshOffset = m_vertexBufferPool.Allocate(data, (uint32_t)byteSize);
		if (meshOffset == OpenGLBuddyAllocator::ms_INVALID_OFFSET)
		{
			return VertexBufferHandle::Null();
		}
		else
		{
			// Encode the VBO ID and the offset in the handle.
			// The handle looks like : | VBO ID (32 bits) | offset in VBO (32 bits) |
			uint64_t handleValue = (uint64_t)m_vertexBufferPool.GetBufferHandle() << 32;
			handleValue |= meshOffset;
			return VertexBufferHandle{handleValue};
		}
	}


	void OpenGLGraphicsDevice::DeleteStaticVertexBuffer(VertexBufferHandle vtxHandle)
	{
		// Right now, the pool only needs the offset (we use a single VBO).

		// Controlled narrowing conversion to only keep the 32 least-significant bits (containing the offset value)
		uint32_t offset = (uint32_t)vtxHandle.Get();

		m_vertexBufferPool.Free(offset);
	}


	IndexBufferHandle OpenGLGraphicsDevice::CreateIndexBuffer(const void* indexData, size_t indexDataSizeBytes)
	{
		const uint32_t indexOffset = m_indexBufferPool.Allocate(indexData, (uint32_t)indexDataSizeBytes);
		if (indexOffset == OpenGLBuddyAllocator::ms_INVALID_OFFSET)
		{
			return IndexBufferHandle::Null();
		}
		else
		{
			// Encode the VBO ID and the offset in the handle.
			// The handle looks like : | VBO ID (32 bits) | offset in VBO (32 bits) |
			uint64_t handleValue = (uint64_t)m_indexBufferPool.GetBufferHandle() << 32;
			handleValue |= indexOffset;
			return IndexBufferHandle{ handleValue };
		}
	}
}

#endif