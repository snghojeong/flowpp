#include <opencv2/opencv.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <utility>

class ImageSource {
public:
    using Callback = std::function<void(const cv::Mat&)>;

    explicit ImageSource(std::string path) : path_(std::move(path)) {}

    void setCallback(Callback callback) {
        callback_ = std::move(callback);
    }

    bool run() const {
        cv::Mat image = cv::imread(path_, cv::IMREAD_COLOR);
        if (image.empty()) {
            std::cerr << "Failed to load image: " << path_ << '\n';
            return false;
        }

        if (callback_) {
            callback_(image);
        }
        return true;
    }

private:
    std::string path_;
    Callback callback_;
};

class ImageProcessor {
public:
    using Callback = std::function<void(const cv::Mat&)>;

    void setCallback(Callback callback) {
        callback_ = std::move(callback);
    }

    void process(const cv::Mat& input) const {
        cv::Mat gray, blurred;

        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 0.8);

        cv::imshow("Processing Step", blurred);
        cv::waitKey(1);

        if (callback_) {
            callback_(blurred);
        }
    }

private:
    Callback callback_;
};

class ImageSink {
public:
    explicit ImageSink(std::string outputPath) : outputPath_(std::move(outputPath)) {}

    void save(const cv::Mat& image) const {
        cv::imshow("Final Output", image);
        cv::waitKey(0);

        if (!cv::imwrite(outputPath_, image)) {
            std::cerr << "Failed to write image: " << outputPath_ << '\n';
            return;
        }

        std::cout << "Successfully saved: " << outputPath_ << '\n';
    }

private:
    std::string outputPath_;
};

int main() {
    // 1. Initialize components
    ImageSource source{"image_src.jpg"};
    ImageProcessor processor;
    ImageSink sink{"image_processed.jpg"};

    // 2. Wire the pipeline (Linear Decoupling)
    // We define the destination first, then work backwards to the source.
    
    processor.setCallback([&](const cv::Mat& img) {
        sink.save(img);
    });

    source.setCallback([&](const cv::Mat& img) {
        processor.process(img);
    });

    // 3. Start the flow
    return source.run() ? 0 : 1;
}
