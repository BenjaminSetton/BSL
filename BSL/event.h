#pragma once

#include <algorithm>
#include <any>
#include <functional>
#include <unordered_map>
#include <vector>

#include "crc32.h"
#include "integral_types.h"
#include "uuid.h"

namespace BSL
{
	class EventListener;
	class EventBroadcaster;

	// Main event dispatcher class. EventListener and EventBroadcast instances will receive and send
	// event data from/to an EventDispatcher instance, respectively.
	class EventDispatcher
	{
		friend class EventListener;
		friend class EventBroadcaster;

	public:

		EventDispatcher()
		{
			m_pendingEvents.reserve(DEFAULT_PENDING_EVENTS);
		}

		// Delivers all queued events to their listeners, then clears the queue.
		// Events broadcast from inside a listener callback are deferred to the
		// next Update
		void Update()
		{
			std::vector<Event> events;
			events.swap(m_pendingEvents);

			for (const Event& e : events)
			{
				const auto it = m_listeners.find(e.name);
				if (it == m_listeners.end())
				{
					continue;
				}

				// NOTE - unsubscribing from inside a callback while its event is
				// being dispatched is not supported
				for (const ListenerEntry& listener : it->second)
				{
					listener.callback(e.payload);
				}
			}
		}

		void Clear()
		{
			m_pendingEvents.clear();
			m_listeners.clear();
		}

	private:

		// Queues an event with an arbitrary payload. The payload is stored by
		// value, so the type must be copy-constructible
		template<typename T>
		void Broadcast(CRC32 eventName, T payload)
		{
			m_pendingEvents.push_back({ eventName, std::move(payload) });
		}

		// Registers a member-function listener of the form obj->OnEvent(const T&).
		// The object must outlive the subscription - the listener holds a raw
		// pointer to it
		template<typename T, typename Obj>
		UUID Subscribe(CRC32 eventName, Obj* pObj, void (Obj::*callback)(const T&))
		{
			return AddListenerEntry(eventName, [pObj, callback](const std::any& payload)
			{
				const T* pTypedPayload = std::any_cast<T>(&payload);
				if (pTypedPayload != nullptr)
				{
					(pObj->*callback)(*pTypedPayload);
				}
			});
		}

		// Const member-function overload
		template<typename T, typename Obj>
		UUID Subscribe(CRC32 eventName, const Obj* pObj, void (Obj::*callback)(const T&) const)
		{
			return AddListenerEntry(eventName, [pObj, callback](const std::any& payload)
			{
				const T* pTypedPayload = std::any_cast<T>(&payload);
				if (pTypedPayload != nullptr)
				{
					(pObj->*callback)(*pTypedPayload);
				}
			});
		}

		// Registers a callable listener (free function, lambda, std::function)
		template<typename T>
		UUID Subscribe(CRC32 eventName, std::function<void(const T&)> callback)
		{
			return AddListenerEntry(eventName, [callback = std::move(callback)](const std::any& payload)
			{
				const T* pTypedPayload = std::any_cast<T>(&payload);
				if (pTypedPayload != nullptr)
				{
					callback(*pTypedPayload);
				}
			});
		}

		void Unsubscribe(CRC32 eventName, UUID id)
		{
			const auto it = m_listeners.find(eventName);
			if (it == m_listeners.end())
			{
				return;
			}

			std::vector<ListenerEntry>& listeners = it->second;
			listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
				[id](const ListenerEntry& entry) { return entry.id == id; }), listeners.end());

			if (listeners.empty())
			{
				m_listeners.erase(it);
			}
		}

	private:

		struct Event
		{
			CRC32 name;
			std::any payload;
		};

		struct ListenerEntry
		{
			UUID id;
			std::function<void(const std::any&)> callback;
		};

		UUID AddListenerEntry(CRC32 eventName, std::function<void(const std::any&)> callback)
		{
			const UUID id = GenerateUUID();
			m_listeners[eventName].push_back({ id, std::move(callback) });
			return id;
		}

	private:

		static constexpr u32 DEFAULT_PENDING_EVENTS = 20;

		std::unordered_map<CRC32, std::vector<ListenerEntry>> m_listeners;
		std::vector<Event> m_pendingEvents;
	};

	// Interface for broadcasting events to an EventDispatcher instance
	class EventBroadcaster
	{
	public:

		EventBroadcaster(EventDispatcher& eventDispatcher) : m_pEventDispatcher(&eventDispatcher)
		{
		}

		template<typename T>
		void Broadcast(CRC32 eventName, T payload)
		{
			m_pEventDispatcher->Broadcast(eventName, std::move(payload));
		}

	private:

		EventDispatcher* m_pEventDispatcher;
	};

	// Interface for subscribing to events sent from an EventDispatcher instance
	class EventListener
	{
	public:

		EventListener(EventDispatcher& eventDispatcher) : m_pEventDispatcher(&eventDispatcher)
		{
			m_subscriptions.reserve(DEFAULT_SUBSCRIPTION_COUNT);
		}

		~EventListener()
		{
			UnsubscribeAll();
		}

		EventListener(const EventListener& other) = delete;
		EventListener& operator=(const EventListener& other) = delete;

		EventListener(EventListener&& other) noexcept : m_pEventDispatcher(other.m_pEventDispatcher),
			m_subscriptions(std::move(other.m_subscriptions))
		{
			other.m_pEventDispatcher = nullptr;
		}

		EventListener& operator=(EventListener&& other) noexcept
		{
			if (this == &other)
			{
				return *this;
			}

			UnsubscribeAll();
			m_pEventDispatcher = other.m_pEventDispatcher;
			m_subscriptions = std::move(other.m_subscriptions);
			other.m_pEventDispatcher = nullptr;
			return *this;
		}

		template<typename T, typename Obj>
		void Subscribe(CRC32 eventName, Obj* pObj, void (Obj::*callback)(const T&))
		{
			AddSubscription(eventName, m_pEventDispatcher->Subscribe(eventName, pObj, callback));
		}

		template<typename T, typename Obj>
		void Subscribe(CRC32 eventName, const Obj* pObj, void (Obj::*callback)(const T&) const)
		{
			AddSubscription(eventName, m_pEventDispatcher->Subscribe(eventName, pObj, callback));
		}

		template<typename T>
		void Subscribe(CRC32 eventName, std::function<void(const T&)> callback)
		{
			AddSubscription(eventName, m_pEventDispatcher->Subscribe(eventName, std::move(callback)));
		}

		// Removes every subscription this listener made under the given event name
		void Unsubscribe(CRC32 eventName)
		{
			for (auto it = m_subscriptions.begin(); it != m_subscriptions.end(); )
			{
				if (it->first == eventName)
				{
					m_pEventDispatcher->Unsubscribe(it->first, it->second);
					it = m_subscriptions.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		void UnsubscribeAll()
		{
			for (const auto& subscription : m_subscriptions)
			{
				m_pEventDispatcher->Unsubscribe(subscription.first, subscription.second);
			}
			m_subscriptions.clear();
		}

	private:

		void AddSubscription(CRC32 eventName, UUID id)
		{
			m_subscriptions.push_back({ eventName, id });
		}

	private:

		static constexpr u32 DEFAULT_SUBSCRIPTION_COUNT = 5;

		EventDispatcher* m_pEventDispatcher;
		std::vector<std::pair<CRC32, UUID>> m_subscriptions;
	};
}
