// g++ -std=c++17 main.cpp `pkg-config --cflags --libs opencv4`
#include <opencv2/opencv.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <utility>

class ImageSource {
public:
    explicit ImageSource(std::string path) : _path(std::move(path)) {}
    void set_callback(std::function<void(cv::Mat&&)> cb) { on_image_ = std::move(cb); }

    bool run_once() {
        cv::Mat img = cv::imread(_path, cv::IMREAD_COLOR);
        if (img.empty()) {
            std::cerr << "[ImageSource] Failed to load: " << path_ << "\n";
            return false;
        }
        if (on_image_) on_image_(std::move(img));
        return true;
    }

private:
    std::string _path;
    std::function<void(cv::Mat&&)> on_image_;
};

class ImageProcessor {
public:
    // Downstream callback
    void set_callback(std::function<void(cv::Mat&&)> cb) { downstream_ = std::move(cb); }

    // Upstream entry point
    void on_image(cv::Mat&& img) {
        // Example processing: BGR -> Gray -> slight blur
        cv::Mat gray, blurred;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 0.8);

        // Optional: visualize intermediate
        cv::imshow("Processed (Gray+Blur)", blurred);
        cv::waitKey(1);

        if (downstream_) downstream_(std::move(blurred));
    }

private:
    std::function<void(cv::Mat&&)> downstream_;
};

class ImageSink {
public:
    explicit ImageSink(std::string out_path) : out_path_(std::move(out_path)) {}

    void on_image(cv::Mat&& img) {
        // Display & save
        cv::imshow("Final", img);
        cv::waitKey(0); // Wait for key before saving; adjust to taste.

        if (!cv::imwrite(out_path_, img)) {
            std::cerr << "[ImageSink] Failed to write: " << out_path_ << "\n";
        } else {
            std::cout << "[ImageSink] Saved: " << out_path_ << "\n";
        }
    }

private:
    std::string out_path_;
};

int main() {
    try {
        ImageSource source{"image_src.jpg"};
        ImageProcessor processor;
        ImageSink sink{"image_processed.jpg"};

        // Wire: source → processor → sink
        processor.set_callback([&sink](cv::Mat&& m) { sink.on_image(std::move(m)); });
        source.set_callback([&processor](cv::Mat&& m) { processor.on_image(std::move(m)); });

        if (!source.run_once()) return 1;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << "\n";
        return 2;
    }
}
