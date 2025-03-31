#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <string>
#include <map>


/// @brief Provides a wrapper for OpenCV's Face recognizer as well as managing label-to-name mapping.
/**
 * This class is designed to initialize the OpenCV library's LBPHFaceRecognizer and load the pre-trained model on
 * face data and its corresponding face mapping. When a face is detected, it is passed to the predict method, which
 * returns the predicted label's name and its confidence level.
 *
 * @file FaceRecognizerWrapper.h
 * @author Naween Sawari
 */
class FaceRecognizerWrapper {
public:
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
    FaceRecognizerWrapper(int radius = 2, int neighbors = 2,
        int grid_x = 7, int grid_y = 7, double threshold = 17.0);

    /// @brief Load the trained model (embeddings.xml)
    /**
     * Loads a pre-trained face recognition model from a file. If the model file is not found or cannot be read,
     * an error message is printed to the console.
     */
    void loadModel(const std::string& modelPath);

    ///@brief Load the label mapping from labels.txt
    /**
     * Loads the label-to-name mapping from a text file. Each line in the file should contain an integer label
     * followed by a string name, separated by whitespace. If the file cannot be opened, an error message is printed.
     * Each mapping is stored in the labels map, where the key is the label and the value is the name.
     */
    void loadLabels(const std::string& labelsPath);

    ///@brief Predict the label for a cropped face ROI
    /**
     * Predicts the label for a given face region of interest (ROI). The predicted label and confidence level
     * are returned by reference. If the prediction fails, -1 is returned.
     *
     * @param faceROI The cropped face image to be recognized
     * @param confidence Reference to a double where the confidence level will be stored
     * @return The predicted label for the face ROI
     */
    int predict(const cv::Mat& faceROI, double& confidence) const;

    ///@brief Get the name associated with a label
    /**
     * Retrieves the name associated with a given label. If the label is not found in the mapping, "Unknown" is returned.
     *
     * @param label The label for which to retrieve the name
     * @return The name associated with the label, or "Unknown" if not found
     */
    std::string getLabelName(int label) const;

private:
    cv::Ptr<cv::face::LBPHFaceRecognizer> recognizer;
    std::map<int, std::string> labels;
};