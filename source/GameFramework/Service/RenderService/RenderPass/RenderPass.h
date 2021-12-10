// Monocle source files - Alexandre Baron

#pragma once

namespace moe
{
	class RenderQueue;
	class RenderHardwareInterface;

	class IRenderPass
	{
	public:

		IRenderPass();

		virtual ~IRenderPass() = default;

		virtual void	Update(RenderQueue & drawQueue, uint8_t passIndex) = 0;

		void	Destroy(RenderHardwareInterface& rhi);
	};



}