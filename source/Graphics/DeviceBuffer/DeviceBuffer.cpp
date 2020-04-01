#include "DeviceBuffer.h"


moe::DeviceBuffer::DeviceBuffer(BufferUsage usage) :
	m_usage(usage)
{
	VerifyUsage();
}


/* Will try to add the usage bits of usage parameter to our current usage */
void moe::DeviceBuffer::AddUsage(BufferUsage usage)
{
	m_usage |= usage;
	VerifyUsage();
}


/* Will replace the current usage with the usage parameter */
void moe::DeviceBuffer::SetUsage(BufferUsage usage)
{
	m_usage = usage;
	VerifyUsage();
}


/* Will deactivate the parameter usage bits in our current usage */
void moe::DeviceBuffer::RemoveUsage(BufferUsage usage)
{
	m_usage &= ~usage;
	VerifyUsage();
}

void moe::DeviceBuffer::UpdateData(const BufferStorageDescription& )
{
}

#ifdef MOE_DEBUG
/*
	This function checks the right usage of buffer bitmask. It's disabled in Release builds.
*/
void	moe::DeviceBuffer::VerifyUsage() const
{
	// StructuredBufferReadOnly can only be combined with Dynamic.
	if (m_usage & (BufferUsage)BufferUsage::StructuredBufferReadOnly)
		MOE_ASSERT(m_usage == ((BufferUsage)BufferUsage::Dynamic | (BufferUsage)BufferUsage::StructuredBufferReadOnly));
	// StructuredBufferReadWrite cannot be combined with any other flag.
	else if (m_usage & (BufferUsage)BufferUsage::StructuredBufferReadWrite)
		MOE_ASSERT((m_usage == (BufferUsage)BufferUsage::StructuredBufferReadWrite));
	// IndirectBuffer cannot be combined with Dynamic.
	else if (m_usage & (BufferUsage)BufferUsage::IndirectBuffer)
		MOE_ASSERT(!(m_usage & (BufferUsage)BufferUsage::Dynamic));
	// Staging cannot be combined with any other flag.
	else if (m_usage & (BufferUsage)BufferUsage::Staging)
		MOE_ASSERT((m_usage == (BufferUsage)BufferUsage::Staging));

	MOE_ASSERT(false);

}

#endif // MOE_DEBUG