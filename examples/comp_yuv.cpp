#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <functional>

using namespace flowpp;

// Type definitions for flowpp components
using GraphPtr = std::unique_ptr<graph>;
using YUVSourcePtr = std::unique_ptr<yuv_source>;
using EncoderPtr = std::unique_ptr<mpeg4_encoder>;
using DecoderPtr = std::unique_ptr<mpeg4_decoder>;
using FileWriterPtr = std::unique_ptr<file_writer>;
using DataPtr = std::unique_ptr<data<std::vector<uint8_t>>>;

int main() {
    try {
        // Initialize the graph for encoding YUV to MPEG-4
        auto encodeGraph = std::make_unique<graph>();

        // Components for the encoding pipeline
        auto yuvSource = encodeGraph->get<yuv_source>("input.yuv", 640, 480); // Source YUV file
        auto encoder = encodeGraph->get<mpeg4_encoder>(640, 480, 30);          // Encoder configuration
        auto fileWriter = encodeGraph->get<file_writer>("output.mp4");         // Output MPEG-4 file writer

        // Pipeline configuration
        yuvSource["YUV"] | encoder | fileWriter;

        // Run the encoding graph with a timeout of 5000 ms and 30 loops
        auto result = encodeGraph->run(5000 /* timeout */, 30 /* frames */);
        std::cout << "Encoding graph run result: " << result << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
