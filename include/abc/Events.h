#pragma once

// abc::Events - event infrastructure shared by both plugins.
// Core deliberately defines NO domain events: ABCEnchance's command/hudevents/
// networkmessage concrete signals belong to the feature plugins (AbcEssential
// autofunc signals, AbcUi panel events). What core provides is the eventpp
// vocabulary plus the chained-command pattern that ABCEnchance
// src/core/events/command.cpp repeated by hand for every command.

#include <eventpp/callbacklist.h>

#include <functional>

// Platform header (xcommand_t for CChainedCommand) - the header must be
// self-sufficient, consumers must not rely on including metahook.h first.
#include <metahook.h>

#include <abc/Console.h>
#include <abc/Task.h>

#include <cstdio>

namespace abc {

// Shared vocabulary so plugins don't include eventpp headers directly.
template <typename Sig> using CallbackList = eventpp::CallbackList<Sig>;

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

  explicit CChainedCommand(const char *cmdName);
  ~CChainedCommand();

  // Instances register their own address in the slot table at hook time;
  // copying/moving would leave that slot pointing at the wrong object.
  CChainedCommand(const CChainedCommand &) = delete;
  CChainedCommand &operator=(const CChainedCommand &) = delete;
  CChainedCommand(CChainedCommand &&) = delete;
  CChainedCommand &operator=(CChainedCommand &&) = delete;

  Callback &Event() {
    return m_event;
  }

  const char *Name() const {
    return m_szName;
  }

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

inline CChainedCommand *s_Slots[CChainedCommand::kMaxInstances] = {};
inline bool s_Used[CChainedCommand::kMaxInstances] = {};
inline xcommand_t s_Predecessors[CChainedCommand::kMaxInstances] = {};

// Captureless lambdas decay to xcommand_t; one distinct entry point per slot
// so HookCmd can tell the instances apart.
inline const xcommand_t kThunks[CChainedCommand::kMaxInstances] = {
    []() { CChainedCommand::DispatchSlot(0); },
    []() { CChainedCommand::DispatchSlot(1); },
    []() { CChainedCommand::DispatchSlot(2); },
    []() { CChainedCommand::DispatchSlot(3); },
    []() { CChainedCommand::DispatchSlot(4); },
    []() { CChainedCommand::DispatchSlot(5); },
    []() { CChainedCommand::DispatchSlot(6); },
    []() { CChainedCommand::DispatchSlot(7); },
    []() { CChainedCommand::DispatchSlot(8); },
    []() { CChainedCommand::DispatchSlot(9); },
    []() { CChainedCommand::DispatchSlot(10); },
    []() { CChainedCommand::DispatchSlot(11); },
    []() { CChainedCommand::DispatchSlot(12); },
    []() { CChainedCommand::DispatchSlot(13); },
    []() { CChainedCommand::DispatchSlot(14); },
    []() { CChainedCommand::DispatchSlot(15); },
    []() { CChainedCommand::DispatchSlot(16); },
    []() { CChainedCommand::DispatchSlot(17); },
    []() { CChainedCommand::DispatchSlot(18); },
    []() { CChainedCommand::DispatchSlot(19); },
    []() { CChainedCommand::DispatchSlot(20); },
    []() { CChainedCommand::DispatchSlot(21); },
    []() { CChainedCommand::DispatchSlot(22); },
    []() { CChainedCommand::DispatchSlot(23); },
    []() { CChainedCommand::DispatchSlot(24); },
    []() { CChainedCommand::DispatchSlot(25); },
    []() { CChainedCommand::DispatchSlot(26); },
    []() { CChainedCommand::DispatchSlot(27); },
    []() { CChainedCommand::DispatchSlot(28); },
    []() { CChainedCommand::DispatchSlot(29); },
    []() { CChainedCommand::DispatchSlot(30); },
    []() { CChainedCommand::DispatchSlot(31); },
};

inline CChainedCommand::CChainedCommand(const char *cmdName) {
  _snprintf_s(m_szName, sizeof(m_szName), _TRUNCATE, "%s", cmdName);
  for (size_t i = 0; i < kMaxInstances; i++) {
    if (!s_Used[i]) {
      s_Used[i] = true;
      m_iSlot = (int)i;
      s_Slots[i] = this;
      break;
    }
  }
  // HookCmd only re-wires commands that already exist (NULL otherwise);
  // a hooked-but-unknown command stays inert: subscribers run, nothing is
  // chained through.
  if (m_iSlot >= 0) {
    m_pfnOriginal = g_pMetaHookAPI->HookCmd(m_szName, kThunks[m_iSlot]);
    s_Predecessors[m_iSlot] = m_pfnOriginal;
  } else {
    ConsoleWarn(
        "CChainedCommand: slot table full (%zu), \"%s\" left unhooked\n",
        kMaxInstances, m_szName);
  }
}

inline CChainedCommand::~CChainedCommand() {
  if (m_iSlot < 0)
    return;
  // Engine-thread only. Restore only if still the top hook; never clobber
  // another plugin installed later. Retire the slot permanently because
  // another owner may retain our thunk as its predecessor.
  if (g_pMetaHookAPI && g_pMetaHookAPI->FindCmd) {
    auto *command = g_pMetaHookAPI->FindCmd(m_szName);
    if (command && command->function == kThunks[m_iSlot])
      g_pMetaHookAPI->HookCmd(m_szName, m_pfnOriginal);
  }
  s_Slots[m_iSlot] = nullptr;
}

inline void CChainedCommand::DispatchSlot(size_t slot) {
  if (slot < kMaxInstances && s_Slots[slot]) {
    s_Slots[slot]->RunChain();
    return;
  }
  if (slot < kMaxInstances && s_Predecessors[slot])
    s_Predecessors[slot]();
}

inline void CChainedCommand::RunChain() {
  bool ret = true;
  m_event.forEach(
      [&](const Callback::Handle &, const Callback::Callback &callback) {
        ret &= callback();
      });
  if (ret && m_pfnOriginal)
    m_pfnOriginal();
}

inline void DispatchToMainThread(std::function<void()> fn) {
  GetTaskManager()->Post(std::move(fn));
}

} // namespace abc
