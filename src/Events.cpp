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
	bool s_Used[CChainedCommand::kMaxInstances] = {};
	xcommand_t s_Predecessors[CChainedCommand::kMaxInstances] = {};
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
		ConsoleWarn("CChainedCommand: slot table full (%zu), \"%s\" left unhooked\n",
			kMaxInstances, m_szName);
	}
}

CChainedCommand::~CChainedCommand() {
	if (m_iSlot < 0) return;
	// Engine-thread only. Restore only if still the top hook; never clobber
	// another plugin installed later. Retire the slot permanently because
	// another owner may retain our thunk as its predecessor.
	if (g_pMetaHookAPI && g_pMetaHookAPI->FindCmd) {
		auto* command = g_pMetaHookAPI->FindCmd(m_szName);
		if (command && command->function == kThunks[m_iSlot])
			g_pMetaHookAPI->HookCmd(m_szName, m_pfnOriginal);
	}
	s_Slots[m_iSlot] = nullptr;
}

void CChainedCommand::DispatchSlot(size_t slot) {
	if (slot < kMaxInstances && s_Slots[slot]) {
		s_Slots[slot]->RunChain();
		return;
	}
	if (slot < kMaxInstances && s_Predecessors[slot])
		s_Predecessors[slot]();
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
	GetTaskManager()->Post(std::move(fn));
}

}
