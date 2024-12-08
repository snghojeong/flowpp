#include <iostream>
#include <memory>

using namespace flowpp;

// Counter class increments by 1 on each poll
class Counter : public source {
    using DataPtr = std::unique_ptr<data<uint_t>>;

public:
    Counter() : _cnt(0) {}

    DataPtr poll(int64_t /* timeout */) override {
        return std::make_unique<data<uint_t>>(_cnt++);
    }

    uint_t get_count() const {
        return _cnt;
    }

private:
    uint_t _cnt;
};

// Counter2 class increments by 2 on each poll
class Counter2 : public source {
    using DataPtr = std::unique_ptr<data<int>>;

public:
    Counter2() : _cnt(0) {}

    DataPtr poll(int64_t /* timeout */) override {
        _cnt += 2;
        return std::make_unique<data<int>>(_cnt);
    }

    int get_count() const {
        return _cnt;
    }

private:
    int _cnt;
};

int main() {
    try {
        // Create unique_ptrs for two graphs
        auto counter1Graph = std::make_unique<graph>();
        auto counter2Graph = std::make_unique<graph>();

        // Get counter instances from graphs
        auto counter1 = counter1Graph->get<Counter>();
        auto counter2 = counter2Graph->get<Counter2>();

        // Run the graphs with respective loop counts
        constexpr int counter1Loops = 10;
        constexpr int counter2Loops = 20;

        counter1Graph->run(INFINITE /* timeout */, counter1Loops);
        counter2Graph->run(INFINITE /* timeout */, counter2Loops);

        // Print the final counts
        std::cout << "Counter1 final count: " << counter1->get_count() << std::endl; // Should print 10
        std::cout << "Counter2 final count: " << counter2->get_count() << std::endl; // Should print 40
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
