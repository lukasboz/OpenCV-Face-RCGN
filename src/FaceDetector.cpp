#include "FaceDetector.h"
#include <iostream>

/**
 * @brief detection class incorporting OpenCV's Haar Cascade Classifier
 * @file FaceDetector.cpp
 */

/// @brief Constructor that optionally loads a cascade path
/**
 * @param cascadePath Path to the Haar Cascade XML file. If empty, the cascade will be loaded later.
 *
 * The constructor initializes the FaceDetector object and optionally loads the cascade classifier if a
 * path is provided. If the path is empty, the object will be created but no cascade will be loaded.
 * @author Naween Sawari
 */
FaceDetector::FaceDetector(const std::string& cascadePath) {
    if (!cascadePath.empty()) {
        loadCascade(cascadePath);
    }
}
/// Method used by the constructor to Load or reload the cascade XML file at runtime
/**
 * @param cascadePath Path to the Haar Cascade XML file.
 * @return true if the cascade was loaded successfully, false otherwise.
 *
 * This method is used by the constructor, and loads the cascade classifier from the speicified file path.
 * If the file is not found or an error occurs during loading, the method returns false.
 */
bool FaceDetector::loadCascade(const std::string& cascadePath) {
    if (!faceCascade.load(cascadePath)) {
        std::cerr << "Error loading cascade: " << cascadePath << std::endl;
        return false;
    }
    return true;
}
/// @brief Detect faces in a given grayscale image
/**
 * @param grayFrame Grayscale image in which to detect faces.
 * @return A vector of rectangles representing the detected faces.
 *
 * This method takes a grayscale image as input and uses the loaded Haar Cascade classifier to detect faces.
 * It returns a vector of rectangles, each representing the bounding box of a detected face.
 */
std::vector<cv::Rect> FaceDetector::detectFaces(const cv::Mat& grayFrame) {
    std::vector<cv::Rect> faces;
    // Adjust these parameters (scaleFactor, minNeighbors) as needed
    faceCascade.detectMultiScale(grayFrame, faces, 1.3, 5, 0|cv::CASCADE_SCALE_IMAGE, cv::Size(60, 60), cv::Size(350, 350) );
    return faces;
}