#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <string>
#include <atomic>

using namespace flowpp;

/**
 * @brief Observable base with:
 *  - clean header hygiene (no global using, #pragma once)
 *  - safer pointer semantics (shared_ptr for fan-out)
 *  - thread-safe subscribe/notify
 *  - RAII-friendly subscribe()/unsubscribe() with token
 *  - observer attach/detach by key (non-owning API can be added if desired)
 *
 * Derivations implement poll(timeout) and can call notify_all() when new data arrives.
 */
template <class T>
class observable {
public:
    using Data      = flowpp::data<T>;
    using DataPtr   = std::unique_ptr<const Data>;
    using Callback  = std::function<void(DataPtr)>;
    using Observer  = flowpp::observer;
    using ObserverPtr = std::unique_ptr<Observer>;
    using SubscriptionId = std::uint64_t;

    observable() = default;
    virtual ~observable() = default;

    // Implement in derived classes. Return nullptr on timeout if desired.
    virtual DataPtr poll(std::uint64_t timeout_ms) = 0;

    // Functional subscription. Returns a token you can use to unsubscribe.
    SubscriptionId subscribe(Callback cb) {
        if (!cb) return 0;
        std::lock_guard<std::mutex> lock(m_);
        const auto id = next_id_++;
        callbacks_.emplace(id, std::move(cb));
        return id;
    }

    // Remove a functional subscriber by token.
    void unsubscribe(SubscriptionId id) {
        if (id == 0) return;
        std::lock_guard<std::mutex> lock(m_);
        callbacks_.erase(id);
    }

    // Observer object management (keyed).
    void attach(const std::string& key, ObserverPtr obs) {
        std::lock_guard<std::mutex> lock(m_);
        observers_[key] = std::move(obs);
    }

    void detach(const std::string& key) {
        std::lock_guard<std::mutex> lock(m_);
        observers_.erase(key);
    }

protected:
    // Call from derived classes when you have new data to publish.
    // Copies subscriber lists under lock, then notifies without holding the lock.
    void notify_all(DataPtr data) {
        if (!data) return;

        std::vector<Callback> callbacks_copy;
        std::vector<ObserverPtr> observers_copy;

        {
            std::lock_guard<std::mutex> lock(m_);
            callbacks_copy.reserve(callbacks_.size());
            for (auto& kv : callbacks_) callbacks_copy.push_back(kv.second);
            observers_copy.reserve(observers_.size());
            for (auto& kv : observers_) if (kv.second) observers_copy.push_back(kv.second);
        }

        // Notify function subscribers
        for (auto& cb : callbacks_copy) {
            // Protect against user callbacks throwing
            try { cb(data); } catch (...) { /* swallow or log */ }
        }
        
        for (auto& obs : observers_copy) {
            try {
                obs->notify(data);
            } catch (...) { std::cerr << "Exception" << std::endl; }
        }
    }

private:
    std::unordered_map<std::string, ObserverPtr> observers_;
    std::unordered_map<SubscriptionId, Callback> callbacks_;
    std::atomic<SubscriptionId> next_id_{1};
    std::mutex m_;
};
