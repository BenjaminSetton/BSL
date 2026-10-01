#pragma once

#include <chrono>

namespace BSL
{
	// RAII timer that measures time spent in scope. Returns the timed 
	// result through the outSeconds constructor parameter
	class ScopedTimer
	{
	public:

		explicit ScopedTimer(float& outSeconds) : m_outSeconds(outSeconds), m_start(std::chrono::steady_clock::now())
		{
		}

		~ScopedTimer()
		{
			m_outSeconds = std::chrono::duration<float>(std::chrono::steady_clock::now() - m_start).count();
		}

		ScopedTimer(const ScopedTimer& other) = delete;
		ScopedTimer& operator=(const ScopedTimer& other) = delete;

	private:

		float& m_outSeconds;
		std::chrono::steady_clock::time_point m_start;
	};
}
