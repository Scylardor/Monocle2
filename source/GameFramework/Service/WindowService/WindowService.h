// Monocle Game Engine source files - Alexandre Baron

#pragma once
#include "Core/Containers/Vector/Vector.h"
#include "GameFramework/Service/Service.h"
#include "GameFramework/Service/WindowService/Window/Window.h"

namespace moe
{


	class WindowService : public Service
	{
	public:

		WindowService(Engine& ownerEngine) :
			Service(ownerEngine)
		{}

		virtual ~WindowService() = default;

		virtual IWindow* CreateWindow() = 0;

		void	RemoveAllWindows()
		{
			m_windows.Clear();
		}

	protected:

		template <typename TWin, typename... Args>
		TWin*	EmplaceWindow(Args&&... args);

		template <typename T, typename... Args>
		bool	RemoveWindow(IWindow& toRemove);

	private:

		Vector<std::unique_ptr<IWindow>>	m_windows;
	};



}

#include "WindowService.inl"