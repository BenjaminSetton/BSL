#pragma once

#include "vec_types.h"

namespace BSL
{
	inline constexpr u32 FRUSTUM_FACE_COUNT = 6;
	struct Frustum
	{
		BSL::Vec4f planes[FRUSTUM_FACE_COUNT];
	};
}