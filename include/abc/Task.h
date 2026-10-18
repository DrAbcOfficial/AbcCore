#pragma once

// abc::Task - background thread tasks with main-thread continuations.
// Ported from ABCEnchance src/core/library/Task.h/.cpp.
// per-DLL instance (static lib): abc::GetTaskManager() returns one manager per
// plugin DLL. The host plugin must pump CheckAll() from a per-frame hook
// (HUD_Frame or equivalent) for continuations to run on the main thread.

#include <atomic>
#include <functional>
#include <list>
#include <thread>
#include <mutex>
#include <exception>
#include <utility>

namespace abc {

class ITaskItem {
public:
  virtual ~ITaskItem() {}
  virtual ITaskItem *Start() = 0;
  virtual bool IsReady() = 0;
  virtual std::function<void()> GetCaller() = 0;

protected:
  std::thread m_pThread;
  std::atomic_bool m_bReady = false;
};

template <typename Result> class CTaskItem : public ITaskItem {
public:
  CTaskItem(std::function<Result()> &futrue) {
    auto &refB = m_bReady;
    auto &refV = m_pValue;
    auto &error = m_pError;
    m_pThread =
        std::thread([&refB, &refV, &error, ft = std::move(futrue)]() mutable {
          try {
            refV = ft();
          } catch (...) {
            error = std::current_exception();
          }
          refB = true;
        });
  }
  virtual ~CTaskItem() {
    if (m_pThread.joinable())
      m_pThread.join();
  }
  // continue in main thread
  // if you wanna capture some ref with recursion, PLEASE NOT TRUST lambda
  // capture First of function arg must same as Result
  template <typename F, typename... Args>
  CTaskItem *ContinueWith(F &&func, Args &&...args) {
    m_pContinue = std::bind(std::forward<F>(func), std::placeholders::_1,
                            std::forward<Args>(args)...);
    return this;
  }
  // start task
  //  No-op: the thread already starts in the constructor. Kept so the
  //  fluent chain Add(...)->Start()->ContinueWith(...) still reads naturally.
  virtual CTaskItem *Start() override {
    return this;
  }
  // is ready
  virtual bool IsReady() override {
    return m_bReady;
  }
  // get caller
  virtual std::function<void()> GetCaller() override {
    if (m_pError) {
      auto error = m_pError;
      return [error]() { std::rethrow_exception(error); };
    }
    // A task started without ContinueWith (fire-and-forget) yields a
    // no-op, so CheckAll never std::invoke()s an empty std::function.
    if (!m_pContinue)
      return []() {};
    return std::bind(m_pContinue, m_pValue);
  }

private:
  Result m_pValue{};
  std::exception_ptr m_pError;
  std::function<void(Result &)> m_pContinue;
};

class CTaskManager {
public:
  // Main-thread only: Add/ContinueWith/Has/CheckAll/Shutdown. Complete the
  // fluent chain before pumping CheckAll. Worker threads use Post only.
  // Workers must finish cooperatively before DLL unload: Shutdown joins;
  // it cannot safely bound arbitrary user work or detach DLL code.
  ~CTaskManager() {
    Shutdown();
  }
  bool Post(std::function<void()> fn);
  std::exception_ptr TakeException();
  // add a new async task
  template <typename Result, typename F, typename... Args>
  CTaskItem<Result> *Add(F &&func, Args &&...args) {
    if (!m_bAccepting)
      return nullptr;
    std::function<Result()> a =
        std::bind(std::forward<F>(func), std::forward<Args>(args)...);
    CTaskItem<Result> *item = new CTaskItem<Result>(a);
    m_aryPending.push_back(item);
    return item;
  }
  // has this task? (checks pending and in-flight)
  bool Has(ITaskItem *check);
  // check per frame
  void CheckAll();
  void Shutdown();

private:
  std::mutex m_Mutex;
  std::list<std::function<void()>> m_Callbacks;
  std::atomic_bool m_bAccepting = true;
  std::exception_ptr m_LastException; // main-thread error channel
  void Invoke(std::function<void()> fn);
  std::list<std::unique_ptr<ITaskItem>> m_aryList;
  std::list<std::unique_ptr<ITaskItem>> m_aryPending;
};

// per-DLL instance (static lib)
CTaskManager *GetTaskManager();

inline CTaskManager s_TaskManager;

inline CTaskManager *GetTaskManager() {
  // per-DLL instance (static lib)
  return &s_TaskManager;
}
inline bool CTaskManager::Has(ITaskItem *check) {
  for (const auto &item : m_aryList) {
    if (item.get() == check)
      return true;
  }
  for (const auto &item : m_aryPending) {
    if (item.get() == check)
      return true;
  }
  return false;
}
inline bool CTaskManager::Post(std::function<void()> fn) {
  std::lock_guard lock(m_Mutex);
  if (!m_bAccepting || !fn)
    return false;
  m_Callbacks.push_back(std::move(fn));
  return true;
}
inline std::exception_ptr CTaskManager::TakeException() {
  return std::exchange(m_LastException, {});
}
inline void CTaskManager::Invoke(std::function<void()> fn) {
  try {
    if (fn)
      fn();
  } catch (...) {
    if (!m_LastException)
      m_LastException = std::current_exception();
  }
}
inline void CTaskManager::Shutdown() {
  // Close publication before joining workers. Do not hold the mutex during
  // joins or callable destruction (captured destructors can post again).
  std::list<std::function<void()>> discarded;
  {
    std::lock_guard lock(m_Mutex);
    m_bAccepting = false;
    discarded.swap(m_Callbacks);
  }
  // unique_ptr owns every item: clear() joins the worker in each destructor.
  m_aryList.clear();
  m_aryPending.clear();
}
inline void CTaskManager::CheckAll() {
  std::list<std::function<void()>> callbacks;
  {
    std::lock_guard lock(m_Mutex);
    callbacks.swap(m_Callbacks);
  }
  for (auto &fn : callbacks) {
    if (!m_bAccepting)
      return;
    Invoke(std::move(fn));
  }

  for (auto iter = m_aryList.begin(); iter != m_aryList.end();) {
    const auto &item = *iter;
    if (item->IsReady()) {
      // GetCaller() binds the result by value, so the returned callable
      // is self-contained; pull it out before deleting the item (the
      // destructor joins the worker thread) and invoke after, keeping
      // the continuation free of any state owned by the item.
      std::function<void()> caller;
      try {
        caller = item->GetCaller();
      } catch (...) {
        if (!m_LastException)
          m_LastException = std::current_exception();
      }
      iter = m_aryList.erase(iter);
      Invoke(std::move(caller));
      if (!m_bAccepting)
        return;
    } else
      iter++;
  }
  // add pending
  if (!m_aryPending.empty())
    m_aryList.splice(m_aryList.end(), m_aryPending);
}

} // namespace abc
