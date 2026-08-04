#pragma once

#include <functional>
#include <vector>

#include "integral_types.h"

namespace BSL
{
	// Generic deferred notification system. Allows any system to schedule a
	// callback (lambda) to be invoked after a specified number of frames have
	// elapsed
	class DeferredCaller
	{
	public:

		DeferredCaller()
		{
		}

		~DeferredCaller()
		{
		}

		DeferredCaller(const DeferredCaller&) = delete;
		DeferredCaller& operator=(const DeferredCaller&) = delete;

		DeferredCaller(DeferredCaller&&) = delete;
		DeferredCaller& operator=(DeferredCaller&&) = delete;

		// Registers a callback to be invoked after frameDelay frames have passed.
		// A frameDelay of 0 means the callback fires on the next Update() call
		void Register(u32 frameDelay, std::function<void()> callback)
		{
			m_pending.push_back({ std::move(callback), m_currentFrame + frameDelay });
		}

		// Advances the frame counter and fires any callbacks whose delay has elapsed
		void Update()
		{
			m_currentFrame++;

			for (auto it = m_pending.begin(); it != m_pending.end(); )
			{
				if (m_currentFrame >= it->targetFrame)
				{
					it->callback();
					it = m_pending.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		// Fires all remaining callbacks immediately and clears the queue
		void Flush()
		{
			for (auto& entry : m_pending)
			{
				entry.callback();
			}
			m_pending.clear();
		}

	private:

		struct DeferredEntry
		{
			std::function<void()> callback;
			u64 targetFrame = 0;
		};

		std::vector<DeferredEntry> m_pending;
		u64 m_currentFrame = 0;
	};
}
