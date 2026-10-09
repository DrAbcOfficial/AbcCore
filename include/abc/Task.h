#pragma once

// abc::Task - background thread tasks with main-thread continuations.
// Ported from ABCEnchance src/core/library/Task.h/.cpp.
// per-DLL instance (static lib): abc::GetTaskManager() returns one manager per
// plugin DLL. The host plugin must pump CheckAll() from a per-frame hook
// (HUD_Frame or equivalent) for continuations to run on the main thread.

#ifndef ABC_TASK_H
#define ABC_TASK_H

#include <atomic>
#include <functional>
#include <list>
#include <thread>

namespace abc {

class ITaskItem {
public:
	virtual ~ITaskItem() {
	}
	virtual ITaskItem* Start() = 0;
	virtual bool IsReady() = 0;
	virtual std::function<void()> GetCaller() = 0;
protected:
	std::thread m_pThread;
	std::atomic_bool m_bReady = false;
};

template <typename Result>
class CTaskItem : public ITaskItem {
public:
	CTaskItem(std::function<Result()>& futrue) {
		auto& refB = m_bReady;
		auto& refV = m_pValue;
		m_pThread = std::thread(
			[&refB, &refV, ft = std::move(futrue)]() mutable {
				refV = ft();
				refB = true;
			}
		);
	}
	virtual ~CTaskItem() {
		if (m_pThread.joinable())
			m_pThread.join();
	}
	//continue in main thread
	//if you wanna capture some ref with recursion, PLEASE NOT TRUST lambda capture
	//First of function arg must same as Result
	template <typename F, typename... Args>
	CTaskItem* ContinueWith(F&& func, Args&&... args) {
		m_pContinue = std::bind(std::forward<F>(func), std::placeholders::_1, std::forward<Args>(args)...);
		return this;
	}
	//start task
	virtual CTaskItem* Start() override {
		return this;
	}
	//is ready
	virtual bool IsReady() override {
		return m_bReady;
	}
	//get caller
	virtual std::function<void()> GetCaller() override {
		return std::bind(m_pContinue, m_pValue);
	}
private:
	Result m_pValue;
	std::function<void(Result&)> m_pContinue;
};

class CTaskManager {
public:
	//add a new async task
	template <typename Result, typename F, typename... Args>
	CTaskItem<Result>* Add(F&& func, Args&&... args) {
		std::function<Result()> a = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
		CTaskItem<Result>* item = new CTaskItem<Result>(a);
		m_aryPending.push_back(item);
		return item;
	}
	//has this task?
	bool Has(ITaskItem* check);
	//check per frame
	void CheckAll();
	void Shutdown();
private:
	std::list<ITaskItem*> m_aryList;
	std::list<ITaskItem*> m_aryPending;
};

// per-DLL instance (static lib)
CTaskManager* GetTaskManager();

}

#endif //ABC_TASK_H
