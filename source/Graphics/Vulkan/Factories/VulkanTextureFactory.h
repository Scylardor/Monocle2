#pragma once


namespace moe
{
	class MyVkDevice;

	class ITextureFactory
	{


	};



	class VulkanTextureFactory : public ITextureFactory
	{

	public:

		VulkanTextureFactory(MyVkDevice& device);

	private:

		MyVkDevice& m_device;
	};

}
