#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_

#include <list>
#include <memory>
#include <algorithm>
#include <cstdint>
#include <chrono>

using namespace flowpp;

template <typename T>
class graph {
    using ObservablePtr = std::unique_ptr<observable<T>>;
    using DataPtr = std::unique_ptr<data<T>>;

public:
    // Run method with a timeout and loop count
    void run(uint64_t timeout, int loop) {
        const auto start_time = millis();
        uint64_t curr_time = 0;
        int cnt = 0;

        while ((loop <= 0) || (cnt++ < loop && start_time + timeout > curr_time)) {
            // Iterate over the list of observables and poll them
            std::for_each(_observableList.begin(), _observableList.end(), 
                [timeout](const ObservablePtr& observable) {
                    observable->poll(timeout);
                });
            curr_time = millis();
        }
    }

    // Run method with only a timeout
    void run(uint64_t timeout) {
        run(timeout, INFINITE);
    }

    // Default run method with infinite timeout and loop
    void run() {
        run(INFINITE, INFINITE);
    }

    // Method to add an observable to the list
    void add_observable(ObservablePtr obs) {
        _observableList.push_back(std::move(obs));
    }

protected:
    std::list<ObservablePtr> _observableList;

private:
    // Helper method to get current time in milliseconds
    uint64_t millis() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }
};

#endif // _GRAPH_HPP_
