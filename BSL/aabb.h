#pragma once

#include "vec_types.h"

namespace BSL
{
	struct AABB
	{
		BSL::Vec3f center;
		BSL::Vec3f halfExtent;
	};
}