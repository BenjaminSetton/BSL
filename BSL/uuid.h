#pragma once

#include <chrono>
#include <functional>
#include <random>
#include <thread>

#include "integral_types.h"

namespace BSL
{
	typedef u64 UUID;
	inline constexpr UUID INVALID_UUID = 0;

	// Generates a random 64-bit UUID for runtime handles. Returns
	// thread-unique UUIDs
	inline UUID GenerateUUID()
	{
		thread_local std::mt19937_64 rngUUID([]
		{
			std::random_device rd;
			const u64 timeSeed   = static_cast<u64>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
			const u64 threadSeed = static_cast<u64>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
			return (static_cast<u64>(rd()) << 32) ^ static_cast<u64>(rd()) ^ timeSeed ^ threadSeed;
		}());

		UUID id = INVALID_UUID;
		do
		{
			id = rngUUID();
		} while (id == INVALID_UUID);

		return id;
	}
}
