// Ported from ABCEnchance src/core/library/Task.cpp (GBK comment translated).
#include "abc/Task.h"

namespace abc {

static CTaskManager s_TaskManager;

CTaskManager* GetTaskManager() {
	// per-DLL instance (static lib)
	return &s_TaskManager;
}
bool CTaskManager::Has(ITaskItem* check){
	for (auto iter = m_aryList.begin(); iter != m_aryList.end(); iter++) {
		if ((*iter) == check)
			return true;
	}
	for (auto iter = m_aryPending.begin(); iter != m_aryPending.end(); iter++) {
		if ((*iter) == check)
			return true;
	}
	return false;
}
bool CTaskManager::Post(std::function<void()> fn) {
	std::lock_guard lock(m_Mutex);
	if (!m_bAccepting || !fn) return false;
	m_Callbacks.push_back(std::move(fn));
	return true;
}
std::exception_ptr CTaskManager::TakeException() {
	return std::exchange(m_LastException, {});
}
void CTaskManager::Invoke(std::function<void()> fn) {
	try { if (fn) fn(); }
	catch (...) { if (!m_LastException) m_LastException = std::current_exception(); }
}
void CTaskManager::Shutdown() {
	// Close publication before joining workers. Do not hold the mutex during
	// joins or callable destruction (captured destructors can post again).
	std::list<std::function<void()>> discarded;
	{
		std::lock_guard lock(m_Mutex);
		m_bAccepting = false;
		discarded.swap(m_Callbacks);
	}
	for (auto iter = m_aryList.begin(); iter != m_aryList.end(); iter++) {
		auto item = *iter;
		delete item;
	}
	m_aryList.clear();

	for (auto iter = m_aryPending.begin(); iter != m_aryPending.end(); iter++) {
		auto item = *iter;
		delete item;
	}
	m_aryPending.clear();
}
void CTaskManager::CheckAll(){
	std::list<std::function<void()>> callbacks;
	{
		std::lock_guard lock(m_Mutex);
		callbacks.swap(m_Callbacks);
	}
	for (auto& fn : callbacks) {
		if (!m_bAccepting) return;
		Invoke(std::move(fn));
	}

	for (auto iter = m_aryList.begin(); iter != m_aryList.end();) {
		auto item = *iter;
		if (item->IsReady()) {
			// GetCaller() binds the result by value, so the returned callable
			// is self-contained; pull it out before deleting the item (the
			// destructor joins the worker thread) and invoke after, keeping
			// the continuation free of any state owned by the item.
			auto caller = std::move(item->GetCaller());
			delete item;
			iter = m_aryList.erase(iter);
			Invoke(std::move(caller));
			if (!m_bAccepting) return;
		}
		else
			iter++;
	}
	//add pending
	if (m_aryPending.size() > 0)
		m_aryList.splice(m_aryList.end(), m_aryPending);
}

}
