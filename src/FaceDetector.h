#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>



/// @brief detection class incorporting OpenCV's Haar Cascade Classifier
/**
* This class provides functionality to load a Haar Cascade XML file for face detection
* and to detect faces in a given grayscale image. The class uses OpenCV's CascadeClassifier for the detection process.
* If the cascade file is not provided at construction, it can be loaded later using the loadCascade method.
* @file FaceDetector.h
* @author Naween Sawari
*/
class FaceDetector {
public:
    /// @brief Constructor that optionally loads a cascade path
    /**
    * @param cascadePath Path to the Haar Cascade XML file. If empty, the cascade will be loaded later.
    *
    * The constructor initializes the FaceDetector object and optionally loads the cascade classifier if a
    * path is provided. If the path is empty, the object will be created but no cascade will be loaded.
    */
    explicit FaceDetector(const std::string& cascadePath = "");

    /// Method used by the constructor to Load or reload the cascade XML file at runtime
    /**
     * @param cascadePath Path to the Haar Cascade XML file.
     * @return true if the cascade was loaded successfully, false otherwise.
     *
     * This method is used by the constructor, and loads the cascade classifier from the speicified file path.
     * If the file is not found or an error occurs during loading, the method returns false.
     */
    bool loadCascade(const std::string& cascadePath);

    /// @brief Detect faces in a given grayscale image
    /**
     * @param grayFrame Grayscale image in which to detect faces.
     * @return A vector of rectangles representing the detected faces.
     *
     * This method takes a grayscale image as input and uses the loaded Haar Cascade classifier to detect faces.
     * It returns a vector of rectangles, each representing the bounding box of a detected face.
     */
    std::vector<cv::Rect> detectFaces(const cv::Mat& grayFrame);

private:
    cv::CascadeClassifier faceCascade;
};