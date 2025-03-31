/**
 * @file training.cpp
 * @author Naween Sarwari
 * @brief This class implements the training class to train the face recognizer model.
 *
 * Creates and verifies paths for the dataset, cascade, model and labels. Afterwards it iterates through every
 * subdirectory in the dataset folder with each subdirectory representing a different employee. Within each subdirectory,
 * it filters out every non-image file, grayscales the image files and filters out the images without a face
 * or has an unreadable face. The remaining images are resized and added to the training dataset while getting a unique
 * labelID. Finally dots are added to the faces and the model is trained
 * using the training dataset and saved in the path defined at the beginning.
 */
#include <opencv2/face.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

// Include our FaceDetector class header
#include "FaceDetector.h"

namespace fs = std::filesystem;

/**
 * @brief Trains the face recognizer model using labeled images
 * 
 * This function begins by processing the dataset by validating the existence of the required paths (dataset, cascade, model, labels).
 * Afterwhich, it iterates throughout each subdirectory in the dataset directory with each subdirectory representing a unique employee.
 * Next, all images that are not suitable to be used are filtered out and the remaining images are resized, grayscaled and added to 
 * the training dataset with a unique ID label. Finally, the facerecognizer is 
 * trained using the training dataset and saved to the validated path for the model.
 * 
 * @return int 0 upon successfully training the model, -1 upon failure to train the model.
 */
int training() {
    std::cout << "Project root directory: " << PROJECT_ROOT_DIR << "\n";

    // Define paths for dataset, cascade, model, and labels
    std::string datasetPath = std::string(PROJECT_ROOT_DIR) + "/dataset";
    std::string cascadePath = std::string(PROJECT_ROOT_DIR) + "/cascades/haarcascade_frontalface_default.xml";
    std::string modelPath = std::string(PROJECT_ROOT_DIR) + "/recognizer/embeddings.xml";
    std::string labelsPath = std::string(PROJECT_ROOT_DIR) + "/recognizer/labels.txt";

    // Verify paths
    std::cout << "[INFO] Dataset path: " << datasetPath << "\n";
    std::cout << "[INFO] Cascade path: " << cascadePath << "\n";
    std::cout << "[INFO] Model path: " << modelPath << "\n";
    std::cout << "[INFO] Labels path: " << labelsPath << "\n";

    // Create a FaceDetector instance with the cascade file
    FaceDetector detector(cascadePath);

    // Containers to store training images and corresponding labels
    std::vector<cv::Mat> trainingImages;
    std::vector<int> trainingLabels;

    // Open the labels file for writing label mappings
    std::ofstream labelsFile(labelsPath);
    if (!labelsFile.is_open()) {
        std::cerr << "Error: Unable to open " << labelsPath << " for writing." << std::endl;
        return -1;
    }

    int labelID = 0;
    // Iterate over each subdirectory in the dataset folder
    try {
        for (const auto& personEntry : fs::directory_iterator(datasetPath)) {
            // Checking to see if the current file is a directory
            if (personEntry.is_directory()) {
                std::string personName = personEntry.path().filename().string();
                std::cout << "[INFO] Processing person: " << personName << std::endl;
                labelsFile << labelID << " " << personName << std::endl;
                
                // Iterate over each file within the subdirectory
                for (const auto& imgEntry : fs::directory_iterator(personEntry.path())) {
                    // Verifies if the current file is an image
                    if (imgEntry.is_regular_file()) {
                        std::string imagePath = imgEntry.path().string();
                        std::cout << "[INFO] Reading image: " << imagePath << std::endl;
                        
                        // Checking if the current image readable
                        cv::Mat image = cv::imread(imagePath);
                        if (image.empty()) {
                            std::cerr << "Warning: Could not read image " << imagePath << std::endl;
                            continue;
                        }

                        // Grayscales the image
                        cv::Mat gray;
                        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

                        // Checks to see if a face can be recognized in the grayscaled image
                        std::vector<cv::Rect> faces = detector.detectFaces(gray);
                        if (faces.empty()) {
                            std::cerr << "Warning: No face detected in " << imagePath << std::endl;
                            continue;
                        }

                        // Resizes the image 
                        cv::Mat faceROI = gray(faces[0]);
                        cv::Mat faceResized;
                        cv::resize(faceROI, faceResized, cv::Size(100, 100));

                        // Adds the edited image to the training dataset and assigns it a label
                        trainingImages.push_back(faceResized);
                        trainingLabels.push_back(labelID);
                    }
                }
                labelID++;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    labelsFile.close();

    // Ensuring that there is at least one image that is able to be trained on
    if (trainingImages.empty() || trainingLabels.empty()) {
        std::cerr << "Error: No training data found. Check your dataset folder structure." << std::endl;
        return -1;
    }

    // Applies the points to each face and trains the face recognizer model
    cv::Ptr<cv::face::LBPHFaceRecognizer> recognizer =
        cv::face::LBPHFaceRecognizer::create(1, 10, 8, 8, 100.0);

    std::cout << "[INFO] Training the recognizer with " << trainingImages.size() << " face(s)..." << std::endl;
    recognizer->train(trainingImages, trainingLabels);

    // Saves the model to the path defined at the beginning of the function
    recognizer->save(modelPath);
    std::cout << "[INFO] Training complete. Model saved at " << modelPath << std::endl;

    return 0;
}

/**
 * @brief Calls the training function to train the model
 * 
 * @return int 0 upon success, -1 upon failure
 */
int main() {
    training();
}