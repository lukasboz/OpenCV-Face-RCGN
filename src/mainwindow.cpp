/**
 * @file mainwindow.cpp
 * @author Lukas Bozinov
 * @brief This class implements the mainwindow class to create the GUI.
 *
 * This Source file contains the implementation of the mainwindow class. It creates the mainwindow and provides the user
 * with various functions. These functions include detecting the employee based on the user's face, displaying the name of the
 * detected employee and the door that they're allowed to access, editing the profile and faces database, retraining the model
 * and accessing the admin panel,.
 */

#include "mainwindow.h"
#include "pin.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QProcess>
#include <fstream>
#include <filesystem>
#include <QCoreApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

/**
 * @brief Construct a new Main Window:: Main Window object and initializes the UI and face recognition components.
 * 
 * Constructs the MainWindow by initializing all labels, buttons, and progress bar. Setting the font-size in the style sheets.
 * Sets a window with a fixed size and title. Builds the layout by calling setupUI() method. Prepares the dataset directory and
 * clears the framedata CSV file. Opens the default camera, displaying an error message upon failure. Creates an instance of
 * both FaceDetector & FaceRecognizerWrapper. Connects to a Qtimer to refresh the video feed every 30 miliseconds by calling
 * the updateFrame() method.
 * 
 * @param parent The parent widget.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      videoLabel(new QLabel(this)),
      nameLabel(new QLabel("Name: ", this)),
      permLabel(new QLabel("Permission Level: ", this)),
      adminButton(new QPushButton("Admin Panel", this)),
      trainButton(new QPushButton("Train Model", this)),
      addFaceButton(new QPushButton("Add Face", this)),
      deleteFaceButton(new QPushButton("Delete Face", this)),
      timer(new QTimer(this)),
      trainProgressBar(new QProgressBar(this)),
      detector(nullptr),
      faceRec(nullptr)
{
    // Increase font sizes via style sheets
    nameLabel->setStyleSheet("font-size: 24pt;");
    permLabel->setStyleSheet("font-size: 24pt;");
    adminButton->setStyleSheet("font-size: 16pt; padding: 10px;");
    trainButton->setStyleSheet("font-size: 16pt; padding: 10px;");
    addFaceButton->setStyleSheet("font-size: 16pt; padding: 10px;");
    deleteFaceButton->setStyleSheet("font-size: 16pt; padding: 10px;");

    QString datasetPath = QString(PROJECT_ROOT_DIR) + "/dataset";
    faceManager = new FaceManager(this, datasetPath);

    // Basic window setup
    setWindowTitle("Face Recognition - Lukas, Naween, Kevin, Matthew, Woorim");
    setFixedSize(1400, 800);

    // Build the UI layout
    setupUI();

    // Prepare dataset directory and clear CSV file
    if (!std::filesystem::exists("../textfiles"))
        std::filesystem::create_directory("../textfiles");
    std::ofstream csvClear("../textfiles/framedata.csv", std::ofstream::out | std::ofstream::trunc);
    csvClear.close();

    // Open the default camera
    cap.open(0);
    if (!cap.isOpened()) {
        videoLabel->setText("Error: Could not open camera.");
        return;
    }

    // Initialize FaceDetector & FaceRecognizerWrapper
    detector = new FaceDetector("../cascades/haarcascade_frontalface_default.xml");
    faceRec = new FaceRecognizerWrapper(1, 10, 8, 8, 100.0);
    faceRec->loadModel("../recognizer/embeddings.xml");
    faceRec->loadLabels("../recognizer/labels.txt");

    // Update frames via QTimer
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(30);
}

/**
 * @brief Destroy the Main Window:: Main Window object
 * 
 * Destroys the MainWindow by deleting detector, faceRec, faceManager, and release cap if it is opened.
 */
MainWindow::~MainWindow() {
    if (cap.isOpened())
        cap.release();
    delete detector;
    delete faceRec;
    delete faceManager;
}

/**
 * @brief Setups the UI
 * 
 * Sections the GUI into 3 main sections, (sidebar, center, right). The sidebar section displays a title label
 * for the doors with a label for each door. Each door label has a white border, centered within the section and 
 * is distributed equally along the vertical axis. The center section displays the live video feed in the videolabel.
 * Two labels are displayed to show the name and permission level of the detected face. The right section contains various
 * buttons for administrative actions. The buttons enable the user to, access the admin panel, add face and/or employee,
 * delete face and/or employee, and retrain the face recognizer model. There also exists a progress bar within this section
 * which indicates the current training progress. Finally, the function also connects the buttons to their respective events.
 * 
 */
void MainWindow::setupUI() {
    // Central widget for the QMainWindow
    auto *central = new QWidget(this);
    setCentralWidget(central);

    // Main horizontal layout: [Sidebar] [Center] [Right]
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // 1. Sidebar (left)
    auto *sidebarWidget = new QWidget(this);
    sidebarWidget->setFixedWidth(250);
    sidebarWidget->setStyleSheet("background-color: #3e3e42;");
    auto *sidebarLayout = new QVBoxLayout(sidebarWidget);

    // Sidebar title label
    auto *sidebarLabel = new QLabel("Doors", sidebarWidget);
    sidebarLabel->setStyleSheet("font-size: 24pt; color: white;");
    sidebarLabel->setAlignment(Qt::AlignCenter);
    sidebarLayout->addWidget(sidebarLabel);

    // Door labels style (default white outline)
    QString doorStyle = "font-size: 18pt; color: white; border: 2px solid white; padding: 10px;";

    // Create door labels as member variables (assumed declared in MainWindow.h)
    doorLabel1 = new QLabel("Door 1", sidebarWidget);
    doorLabel2 = new QLabel("Door 2", sidebarWidget);
    doorLabel3 = new QLabel("Door 3", sidebarWidget);
    doorLabel1->setStyleSheet(doorStyle);
    doorLabel2->setStyleSheet(doorStyle);
    doorLabel3->setStyleSheet(doorStyle);
    doorLabel1->setAlignment(Qt::AlignCenter);
    doorLabel2->setAlignment(Qt::AlignCenter);
    doorLabel3->setAlignment(Qt::AlignCenter);

    // Evenly space out the door labels using stretches
    sidebarLayout->addStretch();
    sidebarLayout->addWidget(doorLabel1);
    sidebarLayout->addStretch();
    sidebarLayout->addWidget(doorLabel2);
    sidebarLayout->addStretch();
    sidebarLayout->addWidget(doorLabel3);
    sidebarLayout->addStretch();

    mainLayout->addWidget(sidebarWidget);

    // 2. Center (camera feed + text labels)
    auto *centerWidget = new QWidget(this);
    auto *centerLayout = new QVBoxLayout(centerWidget);

    videoLabel->setAlignment(Qt::AlignCenter);
    centerLayout->addWidget(videoLabel);

    auto *labelsLayout = new QHBoxLayout();
    nameLabel->setAlignment(Qt::AlignCenter);
    permLabel->setAlignment(Qt::AlignCenter);
    labelsLayout->addWidget(nameLabel);
    labelsLayout->addWidget(permLabel);
    centerLayout->addLayout(labelsLayout);

    mainLayout->addWidget(centerWidget, 1);

    // 3. Right side (buttons and progress bar)
    auto *rightWidget = new QWidget(this);
    auto *rightLayout = new QVBoxLayout(rightWidget);

    rightLayout->addWidget(adminButton);
    rightLayout->addWidget(addFaceButton);
    rightLayout->addWidget(deleteFaceButton);
    rightLayout->addWidget(trainButton);

    trainProgressBar->setMinimum(0);
    trainProgressBar->setMaximum(100);
    trainProgressBar->setValue(0);
    trainProgressBar->setTextVisible(true);
    trainProgressBar->setStyleSheet(
        "QProgressBar {"
        "   border: 2px solid grey;"
        "   border-radius: 5px;"
        "   text-align: center;"
        "   font: bold 14px;"
        "   background: #000;"
        "}"
    );
    rightLayout->addWidget(trainProgressBar);

    rightLayout->addStretch();
    mainLayout->addWidget(rightWidget);

    // Connect buttons to their slots
    connect(adminButton, &QPushButton::clicked, this, &MainWindow::openPinWindow);
    connect(addFaceButton, &QPushButton::clicked, this, &MainWindow::addFace);
    connect(deleteFaceButton, &QPushButton::clicked, this, &MainWindow::deleteFace);
    connect(trainButton, &QPushButton::clicked, this, &MainWindow::openTrainProject);
}

/**
 * @brief Update frame from camera and process face recognition
 * 
 * This function captures a frame from the camera, converts it to grayscale, and detects faces 
 * using the `detector` object. Every detected face gets surrounded by a bounding box. If the 
 * detected face has a confidence level that is greater than 7 it is stored within a buffer.
 * Every 60 frames, the most frequently identified name is determined and gets logged into 
 * framedata.csv alongside the name's permission level. After the ui updates the door labels 
 * based on the which door the user is allowed to access. Finally the name is overlaid onto the frame
 * which has a white border added to it and the videolabel is updated to display the processed frame.
 * 
 */
void MainWindow::updateFrame() {
    cv::Mat frame, gray;
    cap >> frame;
    if (frame.empty()) {
        qDebug() << "Error: blank frame grabbed.";
        return;
    }

    // Convert to grayscale for detection/recognition
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // Detect faces
    auto faces = detector->detectFaces(gray);
    for (const auto &rect : faces) {
        cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
        cv::Mat faceROI = gray(rect);
        double confidence = 0.0;
        int predictedLabel = faceRec->predict(faceROI, confidence);

        std::string labelName = "Unknown";
        if (confidence > 7) {
            labelName = faceRec->getLabelName(predictedLabel);
            nameBuffer.push_back(labelName);

            // Every 60 frames, compute the mode and update labels and door outlines
            if (nameBuffer.size() >= 60) {
                std::map<std::string, int> frequency;
                for (const auto &name : nameBuffer) {
                    frequency[name]++;
                }
                std::string mostFrequentName = "Unknown";
                int maxCount = 0;
                for (const auto &pair : frequency) {
                    if (pair.second > maxCount) {
                        mostFrequentName = pair.first;
                        maxCount = pair.second;
                    }
                }

                std::ofstream csvFile("../textfiles/framedata.csv", std::ios::app);
                if (csvFile.is_open() && mostFrequentName != "Unknown") {
                    csvFile << mostFrequentName << "," << maxCount << "\n";
                    csvFile.close();
                }
                nameBuffer.clear();

                // Update the name label
                QString mostFreqName = QString::fromStdString(mostFrequentName);
                nameLabel->setText(QString("Name: %1").arg(mostFreqName));

                // Update the permission level label (3rd field)
                QString permissionLevel = getPermissionLevelForName(mostFreqName);
                permLabel->setText(QString("Permission Level: %1").arg(permissionLevel));

                // Get the door number (4th field) and update door label outlines accordingly
                QString doorNum = getDoorNumberForName(mostFreqName);
                // For each door label, set border green if its number matches doorNum; else white
                QString defaultStyle = "font-size: 18pt; color: white; border: 2px solid white; padding: 10px;";
                QString activeStyle = "font-size: 18pt; color: white; border: 2px solid lime; padding: 10px;";
                if (doorLabel1) {
                    doorLabel1->setStyleSheet((doorNum == "1") ? activeStyle : defaultStyle);
                }
                if (doorLabel2) {
                    doorLabel2->setStyleSheet((doorNum == "2") ? activeStyle : defaultStyle);
                }
                if (doorLabel3) {
                    doorLabel3->setStyleSheet((doorNum == "3") ? activeStyle : defaultStyle);
                }
            }
        }
        cv::putText(frame, labelName, cv::Point(rect.x, rect.y - 5),
                    cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(0, 255, 0), 1);
    }

    int thickness = 4;
    cv::rectangle(frame,
                  cv::Point(0, 0),
                  cv::Point(frame.cols - 1, frame.rows - 1),
                  cv::Scalar(255, 255, 255),
                  thickness);

    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    videoLabel->setPixmap(QPixmap::fromImage(qimg));
}

/**
 * @brief Helper function to get permission level (3rd field) from names.csv for a given name
 * 
 * Opens the file path to the names.csv text file, where the employee data is stored. 
 * From there it checks to see if the file is able to be accessed, outputting an error message on failure.
 * Upon success, iterates through each line of the file, splicing at every ','. When the line is not empty
 * and the name in the line (index 0) and the inputted name matches, the file is closed and the permission level
 * (index 2) is returned.
 * 
 * @param name QString representing the name to get the permission level for
 * @return QString The permission level that the given name has, empty upon failure to locate the employee in the database
 */
QString MainWindow::getPermissionLevelForName(const QString &name) {
    QString textfilePath = QString(PROJECT_ROOT_DIR) + "/textfiles";
    QFile file(textfilePath + "/names.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file in getPermissionLevelForName:" << file.errorString();
        return "Unknown";
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        if (!parts.isEmpty() && parts[0] == name) {
            file.close();
            return (parts.size() >= 3) ? parts[2] : "Unknown";
        }
    }
    file.close();
    return "Unknown";
}

/**
 * @brief Helper function to get door number (4th field) from names.csv for a given name
 * 
 * Opens the file path to the names.csv text file, where the employee data is stored. 
 * From there it checks to see if the file is able to be accessed, outputting an error message on failure.
 * Upon success, iterates through each line of the file, splicing at every ','. When the line is not empty
 * and the name in the line (index 0) and the inputted name matches, the file is closed and the door number
 * (index 3) is returned.
 * 
 * @param name QString representing the name to get the allowed door for
 * @return QString The door number that the given name is allowed to access, empty upon failure to locate the employee in the database
 */
QString MainWindow::getDoorNumberForName(const QString &name) {
    QString textfilePath = QString(PROJECT_ROOT_DIR) + "/textfiles";
    QFile file(textfilePath + "/names.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file in getDoorNumberForName:" << file.errorString();
        return "";
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        if (!parts.isEmpty() && parts[0] == name) {
            file.close();
            // Return the 4th field (index 3) if available
            return (parts.size() >= 4) ? parts[3].trimmed() : "";
        }
    }
    file.close();
    return "";
}

/**
 * @brief Opens the pinwindow
 * 
 * Calls the pin class to validate the user via a password before enabeling them to access the admin page. Edits the
 * pinwindow to delete when the widget is closed.
 */
void MainWindow::openPinWindow() {
    auto *pinWindow = new PinWindow;
    pinWindow->setAttribute(Qt::WA_DeleteOnClose);
    pinWindow->show();
}

/**
 * @brief Opens the training project.
 * 
 * Allows the user to retrain the model so their edits to the databases affect the model. While the model 
 * is training, a progress bar is displayed to the user and deleted upon completion.
 */
void MainWindow::openTrainProject() {
    QString exePath = QCoreApplication::applicationDirPath() + "/OpenCVProjectTrain";
    auto *trainProcess = new QProcess(this);
    trainProgressBar->setRange(0, 0);
    connect(trainProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, trainProcess](int exitCode, QProcess::ExitStatus exitStatus) {
                Q_UNUSED(exitCode);
                Q_UNUSED(exitStatus);
                trainProgressBar->setRange(0, 100);
                trainProgressBar->setValue(100);
                trainProcess->deleteLater();
            });
    trainProcess->start(exePath, QStringList());
    if (!trainProcess->waitForStarted()) {
        qDebug() << "Failed to start " << exePath;
        trainProgressBar->setRange(0, 100);
        trainProgressBar->setValue(0);
        trainProcess->deleteLater();
    }
}

/**
 * @brief Calls addFace() of faceManager
 * 
 * Enables the user to add a new face and/or employee by stopping the timer, and calling addFace() from faceManager.
 * Upon completion, timer resumes.
 */
void MainWindow::addFace() {
    timer->stop();
    faceManager->addFace();
    timer->start(30);
}

/**
 * @brief Calls deleteFace() of faceManager
 * 
 * Enables the user to delete a face and/or employee by stopping the timer, and calling deleteFace() from faceManager.
 * Upon completion, timer resumes.
 */
void MainWindow::deleteFace() {
    timer->stop();
    faceManager->deleteFace();
    timer->start(30);
}