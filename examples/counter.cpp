#include <iostream>
#include <memory>

using namespace flowpp;

// Counter class increments by 1 on each poll
class Counter : public source {
    using DataPtr = std::unique_ptr<data<uint_t>>;

public:
    explicit Counter() : _cnt(0) {}

    DataPtr poll(int64_t timeout) override {
        return std::make_unique<data<uint_t>>(_cnt++);
    }

    uint_t get_cnt() const {
        return _cnt;
    }

private:
    uint_t _cnt;
};

// Counter2 class increments by 2 on each poll
class Counter2 : public source {
    using DataPtr = std::unique_ptr<data<int>>;

public:
    explicit Counter2() : _cnt(0) {}

    DataPtr poll(int64_t timeout) override {
        _cnt += 2;
        return std::make_unique<data<int>>(_cnt);
    }

    int get_cnt() const {
        return _cnt;
    }

private:
    int _cnt;
};

int main() {
    // Create unique_ptrs for two graphs
    auto counter1Graph = std::make_unique<graph>();
    auto counter2Graph = std::make_unique<graph>();

    // Get counter instances from graphs
    auto counter1 = counter1Graph->get<Counter>();
    auto counter2 = counter2Graph->get<Counter2>();

    // Run the graphs with respective loop counts
    counter1Graph->run(INFINITE /* timeout */, 10 /* number of loops */);
    counter2Graph->run(INFINITE /* timeout */, 20 /* number of loops */);

    // Print the final counts
    std::cout << "Counter1 final count: " << counter1->get_cnt() << std::endl; // Should print 10
    std::cout << "Counter2 final count: " << counter2->get_cnt() << std::endl; // Should print 40

    return 0;
}
