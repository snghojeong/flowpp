int main() {
    // Set up image source and processor
    auto imageSource = std::make_unique<ImageSource>("image.jpg"); // Provide a JPEG image path
    auto imageProcessor = std::make_unique<ImageProcessor>();
    auto imageSink = std::make_unique<ImageSink>("image_processed.jpg");
    
    // Subscribe the processor to the source
    imageProcessor->subscribe(std::move(imageSource));

    // Define the callback to handle the image data
    imageProcessor->listen([](const std::unique_ptr<ImageData>& imgData) {
        cv::imshow("Image", imgData->get()); // Display the image
        cv::waitKey(0); // Wait for a key press
    });

    imageSink->subscribe(std::move(imageProcessor));
    imageSink->listen([](const std::unique_ptr<ImageData>& imgData) {
        fwrite(this->filename(), imgData->get());
        cv::waitKey(0); // Wait for a key press
    });

    return 0;
}
