#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <thread>
#include <vector>

// No "using namespace" in headers. Forward-declare only what we need.
namespace flowpp {
template <class T> class data;
template <class T> class observable;
}

/**
 * @brief A simple polling graph that runs a set of observables.
 *
 * Key improvements:
 *  - Clean header hygiene and forward declarations
 *  - Clear separation of *total run duration* vs. *per-observable poll timeout*
 *  - No magic INFINITE; explicit "unbounded" overloads provided
 *  - Safer container (vector), null checks, and exception isolation per poll
 *  - Non-copyable, movable semantics to avoid accidental copies
 */
template <typename T>
class graph {
public:
    using Observable      = flowpp::observable<T>;
    using ObservablePtr   = std::unique_ptr<Observable>;
    using clock           = std::chrono::steady_clock;
    using millis          = std::chrono::milliseconds;

    graph() = default;
    graph(const graph&) = delete;
    graph& operator=(const graph&) = delete;
    graph(graph&&) noexcept = default;
    graph& operator=(graph&&) noexcept = default;
    ~graph() = default;

    // Add an observable (ignored if null).
    void add_observable(ObservablePtr obs) {
        if (obs) observables_.push_back(std::move(obs));
    }

    // Poll all observables once with a per-poll timeout.
    // Returns the number of successfully polled observables.
    std::size_t run_once(ms poll_timeout) {
        std::size_t ok = 0;
        for (const auto& obs : observables_) {
            if (!obs) continue;
            try {
                // poll() may block up to poll_timeout; return value is ignored here.
                (void)obs->poll(static_cast<std::uint64_t>(poll_timeout.count()));
                ++ok;
            } catch (...) {
                // Swallow or hook up to your logging as needed.
            }
        }
        return ok;
    }

    // Run for a total wall-clock duration, polling each observable with poll_timeout each loop.
    // Stops when total_duration elapses.
    void run_for(ms total_duration, ms poll_timeout) {
        const auto end = clock::now() + total_duration;
        while (clock::now() < end) {
            run_once(poll_timeout);
            // If poll_timeout is zero, avoid hot-spinning:
            if (poll_timeout.count() == 0) std::this_thread::yield();
        }
    }

    // Run a fixed number of iterations (loops), each loop polling all observables.
    void run_iterations(std::size_t loops, ms poll_timeout) {
        for (std::size_t i = 0; i < loops; ++i) {
            run_once(poll_timeout);
            if (poll_timeout.count() == 0) std::this_thread::yield();
        }
    }

    // Unbounded run: keep polling forever (until the process is stopped),
    // with a per-poll timeout.
    void run_unbounded(millis poll_timeout) {
        for (;;) {
            run_once(poll_timeout);
            if (poll_timeout.count() == 0) std::this_thread::yield();
        }
    }

    // Convenience overloads to mirror your original API intent:

    // Run with a total wall-clock timeout (previously: overall "timeout").
    // Each observable gets the same per-poll timeout (defaults to 10ms).
    void run(std::uint64_t total_timeout_ms, std::int64_t loop_count) {
        // If loop_count <= 0, we ignore iterations and use only total duration.
        const auto total = millis(total_timeout_ms);
        const auto per_poll = millis(10); // sensible default; adjust as needed
        if (loop_count <= 0) {
            run_for(total, per_poll);
        } else {
            // Stop at whichever comes first: loop_count or total_timeout.
            const auto end = clock::now() + total;
            for (std::int64_t i = 0; i < loop_count && clock::now() < end; ++i) {
                run_once(per_poll);
                if (per_poll.count() == 0) std::this_thread::yield();
            }
        }
    }

    // Run with only a total timeout (kept for compatibility).
    void run(std::uint64_t total_timeout_ms) {
        run_for(millis(total_timeout_ms), millis(10));
    }

    // Default run: unbounded with a small per-poll timeout to prevent hot spin.
    void run() {
        run_unbounded(millis(10));
    }

    std::size_t size() const noexcept { return observables_.size(); }

private:
    std::vector<ObservablePtr> observables_;
};
