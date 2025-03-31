#include "FaceRecognizerWrapper.h"
#include <fstream>
#include <iostream>

/**
 * @brief Provides a wrapper for OpenCV's Face recognizer as well as managing label-to-name mapping.
 * @file FaceRecognizerWrapper.cpp
 */

/// @brief Constructor sets up OpenCV LBPH with your chosen parameters
/**
 * @param radius The radius of the circle used for the Local Binary Patterns
 * @param neighbors The number of neighbors to consider for the Local Binary Patterns
 * @param grid_x The number of cells in the x direction for the Local Binary Patterns
 * @param grid_y The number of cells in the y direction for the Local Binary Patterns
 * @param threshold The threshold for the confidence level
 *
 * The consutrctor initializes the LBHPFaceRecognizer with the specified parameters.
     * The default values are:
     * - radius: 2
     * - neighbors: 2
     * - grid_x: 7
     * - grid_y: 7
     * - threshold: 17.0
     *
     * @note The default values are the same as those used in the OpenCV LBPHFaceRecognizer.
     */
FaceRecognizerWrapper::FaceRecognizerWrapper(int radius, int neighbors,
    int grid_x, int grid_y, double threshold) {
    // Create LBPHFaceRecognizer with custom parameters
    recognizer = cv::face::LBPHFaceRecognizer::create(radius, neighbors, grid_x, grid_y, threshold);
}

/// @brief Load the trained model (embeddings.xml)
/**
 * Loads a pre-trained face recognition model from a file. If the model file is not found or cannot be read,
 * an error message is printed to the console.
 */
void FaceRecognizerWrapper::loadModel(const std::string& modelPath) {
    try {
        recognizer->read(modelPath);
    }
    catch (cv::Exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
    }
}

///@brief Load the label mapping from labels.txt
/**
 * Loads the label-to-name mapping from a text file. Each line in the file should contain an integer label
 * followed by a string name, separated by whitespace. If the file cannot be opened, an error message is printed.
 * Each mapping is stored in the labels map, where the key is the label and the value is the name.
 */
void FaceRecognizerWrapper::loadLabels(const std::string& labelsPath) {
    std::ifstream infile(labelsPath);
    if (!infile.is_open()) {
        std::cerr << "Error opening labels file: " << labelsPath << std::endl;
        return;
    }

    int id;
    std::string name;
    while (infile >> id >> name) {
        labels[id] = name;
    }
}

///@brief Predict the label for a cropped face ROI
/**
 * Predicts the label for a given face region of interest (ROI). The predicted label and confidence level
 * are returned by reference. If the prediction fails, -1 is returned.
 *
 * @param faceROI The cropped face image to be recognized
 * @param confidence Reference to a double where the confidence level will be stored
 * @return The predicted label for the face ROI
 */
int FaceRecognizerWrapper::predict(const cv::Mat& faceROI, double& confidence) const {
    int predictedLabel = -1;
    // This call sets both the predicted label and confidence
    recognizer->predict(faceROI, predictedLabel, confidence);
    return predictedLabel;
}

///@brief Get the name associated with a label
/**
 * Retrieves the name associated with a given label. If the label is not found in the mapping, "Unknown" is returned.
 *
 * @param label The label for which to retrieve the name
 * @return The name associated with the label, or "Unknown" if not found
 */
std::string FaceRecognizerWrapper::getLabelName(int label) const {
    auto it = labels.find(label);
    if (it != labels.end()) {
        return it->second;
    }
    return "Unknown";
}