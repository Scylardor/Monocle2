#include "ResourceService.h"

#include "Core/Resource/ImageLoader.h"

namespace moe
{
	ResourceService::ResourceService(Engine& ownerEngine) :
		Service(ownerEngine)
	{
		// A lot of rendering APIs expects the 0.0 coordinate on the y-axis to be on the bottom side of the image,
		// but images usually have 0.0 at the top of the y-axis. We can fix this by asking the loader to load images flipped :
		ImageLoader::VerticallyFlipTexturesOnLoad(true);
	}
}
