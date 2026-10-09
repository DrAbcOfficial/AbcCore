// Generic infrastructure; per-command wrappers from ABCEnchance
// src/core/events/command.cpp are NOT ported (domain events stay with the
// feature plugins).
#include <metahook.h>

#include <abc/Events.h>
#include <abc/Console.h>
#include <abc/Task.h>

#include <cstring>

namespace abc {

namespace {
	CChainedCommand* s_Slots[CChainedCommand::kMaxInstances] = {};
}

// Captureless lambdas decay to xcommand_t; one distinct entry point per slot
// so HookCmd can tell the instances apart.
static const xcommand_t kThunks[CChainedCommand::kMaxInstances] = {
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

CChainedCommand::CChainedCommand(const char* cmdName) {
	_snprintf_s(m_szName, sizeof(m_szName), _TRUNCATE, "%s", cmdName);
	for (size_t i = 0; i < kMaxInstances; i++) {
		if (!s_Slots[i]) {
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
	} else {
		ConsoleWarn("CChainedCommand: slot table full (%zu), \"%s\" left unhooked\n",
			kMaxInstances, m_szName);
	}
}

CChainedCommand::~CChainedCommand() {
	if (m_iSlot >= 0)
		s_Slots[m_iSlot] = nullptr;
	// No HookCmd uninstall exists in the MetaHook API; a destroyed command's
	// thunk finds an empty slot and returns without chaining.
}

void CChainedCommand::DispatchSlot(size_t slot) {
	if (slot < kMaxInstances && s_Slots[slot])
		s_Slots[slot]->RunChain();
}

void CChainedCommand::RunChain() {
	bool ret = true;
	m_event.forEach([&](const Callback::Handle& handle, const Callback::Callback& callback) {
		ret &= callback();
	});
	if (ret && m_pfnOriginal)
		m_pfnOriginal();
}

void DispatchToMainThread(std::function<void()> fn) {
	// The task body itself is a no-op on the worker thread; the payload rides
	// the continuation, which the host plugin runs on the main thread when it
	// pumps GetTaskManager()->CheckAll() (per frame).
	GetTaskManager()->Add<int>([]() -> int { return 0; })
		->ContinueWith([fn](int&) {
			if (fn)
				fn();
		});
}

}
