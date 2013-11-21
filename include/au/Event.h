#pragma once

#include <vector>
#include <cassert>

#include "VarArgs.h"
#include "IdPool.h"

#ifndef AULIB_EXCEPTION_HANDLER
#define AULIB_EXCEPTION_HANDLER(msg, excp)
#endif

namespace au
{
	enum class EventCallbackResult
	{
		Ok,             // Calls the remaining listeners with no changes
		Cancel,         // Stops the event from calling the remaining listeners
		Unregister,     // Same as Ok but also unregisters the listener that returned this value afterwards
	};

	namespace EventPriority
	{
		enum EventPriority
		{
			Low = 0,
			Normal = 5,
			High = 10,
		};
	}

	class DeferredEventContext;

	class IDeferrableEvent
	{
	public:
		virtual ~IDeferrableEvent() {}

		virtual void TriggerDeferred() = 0;
		virtual void RegisterContext(DeferredEventContext*) = 0;
		virtual void UnregisterContext(const bool) = 0;
	};
	
	// Simplifies triggering a set of events at a certain point
	class DeferredEventContext
	{
	public:
		void Register(IDeferrableEvent* evt)
		{
			auto it = std::find(_Events.begin(), _Events.end(), evt);

			if(it == _Events.end())
				_Events.push_back(evt);
		}

		void Unregister(IDeferrableEvent* evt)
		{
			auto it = std::find(_Events.begin(), _Events.end(), evt);

			if(it != _Events.end())
				_Events.erase(it);
		}

		void Trigger()
		{
			for(auto &evt : _Events)
				evt->TriggerDeferred();
		}
	private:
		std::vector<IDeferrableEvent*> _Events;
	};

	// Essentially acts as an event source, allowing the user to register several listeners (composed of a callback
	// and a priority value) and later call them with a specific set of values.
	template<typename... Args>
	class Event : public IDeferrableEvent
	{
	private:
		struct Listener
		{
			unsigned int id;
			std::function<EventCallbackResult(Args...)> callback;
			int priority;
		};
	public:
		Event() : m_DeferredContext(nullptr), m_ListenerIdPool(0) {}

		unsigned int AddListener(const int priority, const std::function<EventCallbackResult(Args...)>& callback)
		{
			Listener nlistener = { m_ListenerIdPool.Get(), callback, priority };

			auto cListener = m_Listeners.begin();
			for(; cListener != m_Listeners.end(); cListener++)
			{
				if(cListener->priority < nlistener.priority)
				{
					break;
				}
			}
			m_Listeners.insert(cListener, nlistener);

			return nlistener.id;
		}

		bool RemoveListener(const unsigned int id)
		{
			for(auto it = m_Listeners.begin(); it != m_Listeners.end(); it++)
			{
				if(it->id == id)
				{
					it = m_Listeners.erase(it);
					m_ListenerIdPool.Free(id);

					return true;
				}
			}

			return false;
		}

		void RegisterContext(DeferredEventContext* context)
		{
			if(m_DeferredContext != nullptr)
				UnregisterContext(true);

			m_DeferredContext = context;
			m_DeferredContext->Register(this);
		}

		void UnregisterContext(const bool triggerRemaining)
		{
			if(m_DeferredContext != nullptr)
			{
				if (!m_DeferredTriggers.empty() && triggerRemaining)
					TriggerDeferred();

				m_DeferredContext->Unregister(this);
				m_DeferredContext = nullptr;
			}
		}

		void Trigger(const Args... args)
		{
			try
			{
				std::vector<unsigned int> removeIds;

				for(Listener &l : m_Listeners)
				{
					EventCallbackResult res = UnpackCall(l.callback, args...);

					if(res == EventCallbackResult::Cancel)
						break;
					else if(res == EventCallbackResult::Unregister)
						removeIds.push_back(l.id);
				}

				for(unsigned int id : removeIds)
					RemoveListener(id);
			}
			catch(const std::exception& e)
			{
				AULIB_EXCEPTION_HANDLER("An error occurred while processing an event", e.what());
				e; // fixes warning C4101 (unreferenced local variable), since AULIB_EXCEPTION_HANDLER might be defined as empty
#ifdef AULIB_EXCEPTIONS
				throw;
#endif
			}
		}

		void TriggerDeferred()
		{
			assert(m_DeferredContext != nullptr);

			try
			{
				std::vector<unsigned int> removeIds;

				for(auto &evt : m_DeferredTriggers)
				{
					removeIds.clear();

					for(Listener &l : m_Listeners)
					{
						EventCallbackResult res = UnpackCallWithTuple(l.callback, evt);

						if(res == EventCallbackResult::Cancel)
							break;
						else if(res == EventCallbackResult::Unregister)
							removeIds.push_back(l.id);
					}

					for(unsigned int id : removeIds)
						RemoveListener(id);
				}

				m_DeferredTriggers.clear();
			}
			catch(const std::exception& e)
			{
				AULIB_EXCEPTION_HANDLER("An error occurred while processing an event", e.what());
				e; // fixes warning C4101 (unreferenced local variable), since AULIB_EXCEPTION_HANDLER might be defined as empty
#ifdef AULIB_EXCEPTIONS
				throw;
#endif
			}
		}

		void Defer(const Args... args)
		{
			if(m_DeferredContext == nullptr)
			{
				Trigger(args...);
			}
			else
			{
				m_DeferredTriggers.push_back(std::make_tuple(args...));
			}
		}
	private:
		DeferredEventContext* m_DeferredContext;
		IdPool m_ListenerIdPool;
		std::vector<Listener> m_Listeners;
		std::vector<std::tuple<Args...>> m_DeferredTriggers;
	};
}