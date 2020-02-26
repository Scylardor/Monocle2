#ifndef MOE_DLL_VISIBILITY_H_
#define MOE_DLL_VISIBILITY_H_

// DLL Symbols visibility defines

#ifdef MOE_USE_DLL
    #ifdef MOE_WINDOWS
		// The compiler is always worried that there is ABI incompatibility between DLLs but we compile them all using the same toolset
		// so disable the "needs to have dll-interface" warning.
		#ifdef _MSC_VER
			#pragma warning( disable: 4251 )
		#endif
        #ifdef MOE_DLL_EXPORT
            #define MOE_DLL_API __declspec( dllexport )
        #else
            #define MOE_DLL_API __declspec( dllimport )
        #endif
    #elif defined(MOE_LINUX)
        #ifdef MOE_DLL_EXPORT
            #define MOE_DLL_API __attribute__ ((visibility ("default")))
        #else
            #define MOE_DLL_API
        #endif
    #endif // MOE_WINDOWS / MOE_LINUX
#else
    #define MOE_DLL_API
#endif

#endif // MOE_DLL_VISIBILITY_H_
