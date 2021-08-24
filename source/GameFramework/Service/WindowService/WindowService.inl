#pragma once

namespace moe
{
	template <typename TWin, typename ... Args>
	TWin* WindowService::EmplaceWindow(Args&&... args)
	{
		static_assert(std::is_base_of_v<IWindow, TWin>, "TWin is not a IWindow derived class");

		m_windows.EmplaceBack(std::make_unique<TWin>(std::forward<Args>(args)...));
		return static_cast<TWin*>(m_windows.Back().get());
	}

	template <typename T, typename ... Args>
	bool WindowService::RemoveWindow(IWindow& toRemove)
	{
		auto windowIt = std::find_if(m_windows.begin(), m_windows.end(), [&toRemove](const auto& winPtr)
			{
				return winPtr.get() == toRemove;
			});

		bool found = windowIt != m_windows.end();

		if (found)
			m_windows.EraseBySwap(windowIt);

		return found;
	}

}