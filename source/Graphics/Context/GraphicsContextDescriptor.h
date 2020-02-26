// Monocle Game Engine source files - Alexandre Baron

#pragma once

#include "Core/Preprocessor/moeDLLVisibility.h"
#include "Core/Misc/Types.h"
#include <limits>


// A data holder that describes all we need to know to allocate
// a graphic context:
// - the bits per color / per pixel we want to allocate for pixel data in our window
// - depth and stencil buffer bpp
// - number of samples to use for MSAA
// - double-buffering enabled or not.
// It will then be used and converted by the platform-specific
// layer to express our choice with the appropriate API.
namespace moe
{
    struct MOE_DLL_API GraphicsContextDescriptor
    {
        // Since OpenGL has an extension to enable adaptive V-Sync (which offers better performance in case of FPS drops),
        // a boolean flag is not enough.
        // The only sensible use to the "v-sync enabled but not adaptive" would be for profiling reasons, though.
        // There's no such configuration on D3D contexts. They shall fallback on "regular" v-sync if they see this flag set.
        enum class VSyncFlag : std::uint8_t
        {
            VSF_Disabled =              1 << 0,
            VSF_Enabled =               1 << 1,
            VSF_Enabled_NotAdaptive =  (1 << 2) | VSF_Enabled
        };


        // The context stores a format for the back buffer that needs to be compatible with both D3D and OpenGL.
        // DXGI_FORMAT could fit on a byte, but OpenGL GLenums usually have 16-bits values.
        typedef std::uint16_t GenericFormat;

        // Version taking all the default parameters.
        GraphicsContextDescriptor(
            GenericFormat colorFormat       = DEFAULT_GENERIC_FORMAT,
            GenericFormat depthStencilFormat= DEFAULT_GENERIC_FORMAT,
            std::uint8_t numSamples         = DEFAULT_SAMPLES_COUNT,
            std::int8_t apiMajorVersion     = DEFAULT_API_MAJOR_VERSION,    std::int8_t apiMinorVersion = DEFAULT_API_MINOR_VERSION,
            std::uint8_t numBuffers         = DEFAULT_BUFFERS_COUNT,        VSyncFlag useVSync          = DEFAULT_USE_VSYNC,
            std::uint32_t width             = DEFAULT_WIDTH,                std::uint32_t height        = DEFAULT_HEIGHT) :
            ViewportWidth(width),
            ViewportHeight(height),
            ColorFormat(colorFormat),
            DepthStencilFormat(depthStencilFormat),
            SamplesCount(numSamples),
            BuffersCount(numBuffers),
            API_MajorVersion(apiMajorVersion),
            API_MinorVersion(apiMinorVersion),
            VSyncState(useVSync)
        {
        }

        bool    HasDefaultApiVersion() const { return API_MajorVersion == DEFAULT_API_MAJOR_VERSION && API_MinorVersion == DEFAULT_API_MINOR_VERSION; }


        // Viewport
        std::uint32_t   ViewportWidth = DEFAULT_WIDTH;
        std::uint32_t   ViewportHeight = DEFAULT_HEIGHT;

        // Color Format
        GenericFormat   ColorFormat;

        // Depth-stencil format
        GenericFormat   DepthStencilFormat;

        // MSAA / Buffering
        std::uint8_t    SamplesCount;
        std::uint8_t    BuffersCount;   // Rationale is that it's the count of *back* buffers, NOT total buffers. Hence, 1 = double buffering, 2 = triple buffering...

        // API versioning
        std::int8_t     API_MajorVersion;
        std::int8_t     API_MinorVersion;

        // VSync
        VSyncFlag       VSyncState;


        // Default are quite arbitrary, based on the most commonly used values for these usages...
        static const std::uint8_t   DEFAULT_BITS_PER_COLOR      = 8u;
        static const std::uint8_t   DEFAULT_DEPTH_BITS          = 24u;
        static const std::uint8_t   DEFAULT_STENCIL_BITS        = 8u;
        static const std::uint8_t   DEFAULT_SAMPLES_COUNT       = 2u;   // 2x MSAA by default. (It seems Windows 7 WGL doesn't like no MSAA, for example).
        static const std::uint8_t   DEFAULT_BUFFERS_COUNT       = 1u;   // Use double buffering by default.
        static const std::int8_t    DEFAULT_API_MAJOR_VERSION   = -1;
        static const std::int8_t    DEFAULT_API_MINOR_VERSION   = -1;
        static const VSyncFlag      DEFAULT_USE_VSYNC           = VSyncFlag::VSF_Disabled;
        static const GenericFormat  DEFAULT_GENERIC_FORMAT      = (std::numeric_limits<std::uint16_t>::max)();
        // DEFAULT_WIDTH * DEFAULT_HEIGHT = a 16:9 ratio that probably won't take full screen
        // TODO: maybe ask the system the ideal default size instead ?
        static const std::uint32_t  DEFAULT_WIDTH = 1280;
        static const std::uint32_t  DEFAULT_HEIGHT = 720;
    };
}
