#include <iostream>
#include <memory>
#include <flowpp/flowpp.hpp>

using namespace flowpp;

int main() {
    // Define constants to ensure consistency across components
    const int width = 640;
    const int height = 360;
    const int fps = 30;

    try {
        auto pipeline = std::make_unique<graph>();

        // Now components are guaranteed to share the same dimensions
        auto source  = pipeline->get<yuv_source>("input.yuv", width, height);
        auto encoder = pipeline->get<mpeg4_encoder>(width, height, fps);
        auto writer  = pipeline->get<file_writer>("output.mp4");

        source | encoder | writer;

        const int result = pipeline->run(5000, fps);
        
        std::cout << "Pipeline execution finished with status: " << result << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
