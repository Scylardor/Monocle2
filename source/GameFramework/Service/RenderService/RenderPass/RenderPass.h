// Monocle source files - Alexandre Baron

#pragma once

namespace moe
{
	class RenderHardwareInterface;

	class IRenderPass
	{
	public:

		IRenderPass(RenderHardwareInterface& rhi);

		virtual ~IRenderPass() = default;

		virtual void	Update() = 0;

		void	Destroy(RenderHardwareInterface& rhi);
	};



}