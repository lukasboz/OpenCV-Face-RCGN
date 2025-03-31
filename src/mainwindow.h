
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QProgressBar>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

// Include your face detection/recognition headers
#include "FaceDetector.h"
#include "FaceRecognizerWrapper.h"
#include "facemanager.h"


/**
 * @file mainwindow.h
 * @author Lukas Bozinov
 * @brief Header file for the mainwindow class, which is the window that identifies the user based
 * on the trained model from the training, using the user's face.
 *
 * This Header file defines the mainwindow class which creates the Mainwindow of the project. Within,
 * the face and employee database can be added to or deleted from, the user can access their profile
 * page after they are recognized by the model, the model can be retrained, and the user can access
 * the doors that they have permission to access.
 */

class MainWindow : public QMainWindow {
    Q_OBJECT
public:

    /**
     * @brief Construct a new Main Window object
     * 
     * @param parent Pointer to the parent widget
     */
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    private slots:
        /**
         * @brief Updates the frame from the camera and processes the face recognition
         * 
         */
        void updateFrame();

    /**
     * @brief Get the Permission Level For Name object
     * 
     * @param name The name to retrieve the permission level of
     * @return * QString The permission level for the inputted name
     */
    QString getPermissionLevelForName(const QString &name);

    /**
     * @brief Get the Door Number For Name object
     * 
     * @param name The name to retrieve the allowed door entry of
     * @return QString The door number that the inputted name is allowed to access
     */
    QString getDoorNumberForName(const QString &name);

public slots:
        /**
         * @brief Opens the pinwindow to verify the user before accessing their profile
         * 
         */
        static void openPinWindow();
        /**
         * @brief Retrains the model so that editting the databases can affect what the model detects
         * 
         */
        void openTrainProject();
        /**
         * @brief Enables the user to add a face or a profile
         * 
         */
        void addFace();
        /**
         * @brief Enables the user to delete a face or a profile
         * 
         */
        void deleteFace();


private:
    // UI Elements
    QLabel *videoLabel;    // To display the camera feed
    QLabel *nameLabel;     // Static text for name (updated during execution)
    QLabel *permLabel;     // Static text for permission level (updated during execution)
    QPushButton *adminButton;
    QPushButton *trainButton;
    QTimer *timer;
    QProgressBar *trainProgressBar;
    QPushButton *addFaceButton;
    QPushButton *deleteFaceButton;
    QLabel *doorLabel1;
    QLabel *doorLabel2;
    QLabel *doorLabel3;

    // OpenCV / face recognition members
    cv::VideoCapture cap;
    FaceDetector *detector;
    FaceRecognizerWrapper *faceRec;
    FaceManager *faceManager;
    std::vector<std::string> nameBuffer;

    /**
     * @brief Creates the layout that the UI will use
     * 
     */
    void setupUI();
};

#endif // MAINWINDOW_H