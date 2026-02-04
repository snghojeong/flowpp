#include <iostream>
#include <memory>
#include <flowpp/flowpp.hpp> // Assuming standard header naming

using namespace flowpp;

int main() {
    try {
        // 1. Initialize the processing graph
        auto pipeline = std::make_unique<graph>();

        // 2. Define components directly within the graph context
        // Using descriptive names for clarity
        auto source  = pipeline->get<yuv_source>("input.yuv", 640, 360);
        auto encoder = pipeline->get<mpeg4_encoder>(640, 360, 30);
        auto writer  = pipeline->get<file_writer>("output.mp4");

        // 3. Link components using the pipe operator
        source | encoder | writer;

        // 4. Execute the pipeline
        // Parameters: timeout (ms), frame count
        const int result = pipeline->run(5000, 30);
        
        std::cout << "Pipeline execution finished with status: " << result << std::endl;

    } catch (const std::exception& e) {
        // Note: Standard exceptions use .what(), not .to_str()
        std::cerr << "Runtime Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
