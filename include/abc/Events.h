#pragma once

// abc::Events - event infrastructure shared by both plugins.
// Core deliberately defines NO domain events: ABCEnchance's command/hudevents/
// networkmessage concrete signals belong to the feature plugins (AbcEssential
// autofunc signals, AbcUi panel events). What core provides is the eventpp
// vocabulary plus the chained-command pattern that ABCEnchance
// src/core/events/command.cpp repeated by hand for every command.

#ifndef ABC_EVENTS_H
#define ABC_EVENTS_H

#include <eventpp/callbacklist.h>

#include <functional>

// Platform header (xcommand_t for CChainedCommand) - the header must be
// self-sufficient, consumers must not rely on including metahook.h first.
#include <metahook.h>

namespace abc {

// Shared vocabulary so plugins don't include eventpp headers directly.
template <typename Sig>
using CallbackList = eventpp::CallbackList<Sig>;

// Hook an existing console command and let subscribers vote on it:
// - subscribers run in registration order on the engine command thread;
// - if every subscriber returns true the original command entry point is
//   invoked (chained through), otherwise the command is swallowed.
// This is the generic form of the Slot1..Attack1 wrappers in the old
// command.cpp. MetaHook's HookCmd hands back no context pointer, so instances
// live in a fixed slot table (kMaxInstances) - instantiate at plugin init
// time, never dynamically.
class CChainedCommand {
public:
	using Callback = CallbackList<bool()>;

	static constexpr size_t kMaxInstances = 32;

	explicit CChainedCommand(const char* cmdName);
	~CChainedCommand();

	// Instances register their own address in the slot table at hook time;
	// copying/moving would leave that slot pointing at the wrong object.
	CChainedCommand(const CChainedCommand&) = delete;
	CChainedCommand& operator=(const CChainedCommand&) = delete;
	CChainedCommand(CChainedCommand&&) = delete;
	CChainedCommand& operator=(CChainedCommand&&) = delete;

	Callback& Event() { return m_event; }

	const char* Name() const { return m_szName; }

	void RunChain();

	// Internal: entry point used by the thunk table in Events.cpp.
	static void DispatchSlot(size_t slot);

private:
	char m_szName[64];
	int m_iSlot = -1;
	xcommand_t m_pfnOriginal = nullptr;
	Callback m_event;
};

// Run fn on the main thread shortly after this call: it is queued through the
// task manager, and the host plugin must pump abc::GetTaskManager()->CheckAll()
// per frame (HUD_Frame) for it to run. Thread-safe, fire-and-forget.
// Shutdown rejects new posts and discards pending callbacks; use manager Post
// when the caller needs an acceptance result.
void DispatchToMainThread(std::function<void()> fn);

}

#endif //ABC_EVENTS_H
