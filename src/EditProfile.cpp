#include "EditProfile.h"
#include <QFile>
#include <QTextStream>
#include <QLabel>
#include <QFrame>
#include <QDebug>
#include <QPushButton>
#include <QScrollArea>
#include <QDir>
#include <QPixmap>
#include <QSplitter>
#include <QMessageBox>
#include <QComboBox>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDateTime>

/**
* @brief This class provides functionality to edit a user's job status and access levels.
* @file EditProfile.cpp
*/

/// @brief The constructor sets up the GUI's layout
/**
 * @param parent The parent widget for the GUI
 *
 * This constructor initializes the GUI layout, including the sidebar, middle section, and right section.
 * It also displays the profile details (name, date joined, highest access level, and job status) in the GUI.
 * The GUI includes buttons on the sidebar for navigating back and refreshing the dataset.
 * The middle section displays the profile details, and the right section contains a scroll area for profile buttons.
 *
 * @author Kevin Russel
 */
EditProfile::EditProfile(QWidget *parent) : QWidget(parent) {
    setFixedSize(800, 600);
    // Set global dark mode for the entire widget
    setStyleSheet("background-color: #121212; color: #ffffff;");

    // Initialize labels with white text
    imageLabel = new QLabel();
    imageLabel->setFixedSize(400, 400);
    imageLabel->setAlignment(Qt::AlignCenter);

    nameLabel = new QLabel("Name: ");
    nameLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");
    dateLabel = new QLabel("Date Joined: ");
    dateLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");
    jobLabel = new QLabel("Job Status: ");
    jobLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");
    accessLabel = new QLabel("Highest Level Access: ");
    accessLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");

    // Main layout
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(10);

    // Left sidebar (dark background)
    auto *sidebar = new QWidget();
    sidebar->setFixedWidth(200);
    sidebar->setStyleSheet("background-color: #2c2c2c;");
    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->addWidget(CreateTopSection());
    sidebarLayout->addStretch();

    // Middle section (dark background)
    auto *middleSection = new QWidget();
    middleSection->setStyleSheet("background-color: #1e1e1e;");
    auto *middleLayout = new QVBoxLayout(middleSection);

    // Profile picture area (centered)
    auto *pictureArea = new QWidget();
    auto *pictureLayout = new QHBoxLayout(pictureArea);
    pictureLayout->addStretch();
    pictureLayout->addWidget(imageLabel);
    pictureLayout->addStretch();

    // Profile details (below picture)
    auto *detailsArea = new QWidget();
    auto *detailsLayout = new QVBoxLayout(detailsArea);
    detailsLayout->addWidget(nameLabel);
    detailsLayout->addWidget(dateLabel);
    detailsLayout->addWidget(jobLabel);
    detailsLayout->addWidget(accessLabel);
    detailsLayout->addStretch();

    middleLayout->addWidget(pictureArea);
    middleLayout->addWidget(detailsArea);
    middleLayout->addStretch();

    // Right section (dark background)
    auto *rightSection = new QWidget();
    rightSection->setFixedWidth(200);
    rightSection->setStyleSheet("background-color: #2c2c2c;");
    auto *rightLayout = new QVBoxLayout(rightSection);
    auto *settingsLabel = new QLabel("<h2>Settings</h2>");
    settingsLabel->setStyleSheet("color: white;");
    rightLayout->addWidget(settingsLabel);
    auto *editAccessButton = new QPushButton("Edit Access Level");
    editAccessButton->setStyleSheet("background-color: #3a3a3a; color: white;");
    rightLayout->addWidget(editAccessButton);
    connect(editAccessButton, &QPushButton::clicked, this, &EditProfile::showAccessLevelPopup);

    rightLayout->addStretch();

    // Add the "Back" button at the bottom right
    auto *backButton = new QPushButton("Back", this);
    backButton->setStyleSheet("background-color: #3a3a3a; color: white; padding: 5px;");
    rightLayout->addWidget(backButton, 0, Qt::AlignRight);
    connect(backButton, &QPushButton::clicked, this, &EditProfile::onBackButtonClicked);

    // Add the "Refresh CSV" button under the Back button
    auto *refreshButton = new QPushButton("Reset CSV", this);
    refreshButton->setStyleSheet("background-color: #3a3a3a; color: white; padding: 5px;");
    rightLayout->addWidget(refreshButton, 0, Qt::AlignRight);
    connect(refreshButton, &QPushButton::clicked, this, &EditProfile::refreshCSV);

    // Combine sections into main layout
    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(middleSection, 1);
    mainLayout->addWidget(rightSection);

    setLayout(mainLayout);
}

/// @brief creates the top scetion of the sidebar
/**
 * @return A pointer to the top section of the sidebar
 *
 * This function creates a scrollable area containing buttons for each name in the CSV file.
 * Each button, when clicked, loads the corresponding profile information.
 * The top section is styled with a dark background and contains a label prompting the user to select a name.
 */
QFrame* EditProfile::CreateTopSection() {
    QString textfilePath = QString(PROJECT_ROOT_DIR) + "/textfiles";

    auto *topSection = new QFrame();
    topSection->setFrameShape(QFrame::Box);
    // Dark background for top section
    topSection->setStyleSheet("background-color: #2c2c2c;");

    // Scroll area for profile buttons
    auto *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background-color: #2c2c2c;");

    auto *scrollContainer = new QWidget();
    auto *scrollLayout = new QVBoxLayout(scrollContainer);
    int numLines = countLinesInFile();

    // Create a button for each name in the file
    for (int i = 0; i < numLines; i++) {
        QString name = getName(i);
        auto *profile = new QPushButton(name + " " + QString::number(i));
        profile->setStyleSheet("background-color: #3a3a3a; border: 1px solid #555555; padding: 5px; color: white;");
        connect(profile, &QPushButton::clicked, this, [this, name]() {
            loadFirstLine(name);
        });
        scrollLayout->addWidget(profile);
    }
    scrollContainer->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContainer);

    auto *topLayout = new QVBoxLayout(topSection);
    auto *prompting = new QLabel("<h2>Names</h2>");
    prompting->setStyleSheet("color: white;");
    topLayout->addWidget(prompting);
    topLayout->addWidget(scrollArea);
    topSection->setLayout(topLayout);

    return topSection;
}

/// @brief creates the middle section of the GUI
/**
 * @return A pointer to the middle section of the GUI
 *
 * This function creates the middle section of the GUI, which contains the profile picture and details.
 * The profile picture is displayed on the left, and the profile details (name, date joined, job status, access level)
 * are displayed on the right.
 */
QFrame* EditProfile::createMiddleSection() {
    auto *middleSection = new QFrame();
    middleSection->setFrameShape(QFrame::Box);

    auto *leftSide = new QWidget();
    auto *leftLayout = new QVBoxLayout(leftSide);
    imageLabel = new QLabel();
    QPixmap pixmap1(QString(PROJECT_ROOT_DIR) + "/dataset/Sample_User_Icon.png");
    imageLabel->setPixmap(pixmap1.scaled(200, 200));
    leftLayout->addWidget(imageLabel);
    leftSide->setLayout(leftLayout);

    auto *rightSide = new QWidget();
    auto *rightLayout = new QVBoxLayout(rightSide);
    rightSide->setLayout(rightLayout);

    auto *middleLayout = new QHBoxLayout(middleSection);
    middleLayout->addWidget(leftSide);
    middleLayout->addWidget(rightSide);
    middleSection->setLayout(middleLayout);

    return middleSection;
}

/// @brief Loads the first line of the CSV file and updates the profile information
/**
 * @param name The name of the user whose profile information is to be loaded
 *
 * This function opens the "names.csv" file and reads the first line. It updates the profile information
 * (name, date joined, job status, access level) based on the data in the CSV file. If no matching images are found,
 * it falls back to a default sample icon.
 */
void EditProfile::loadFirstLine(QString name) {
    QString textfilePath = QString(PROJECT_ROOT_DIR) + "/textfiles";
    QFile file(textfilePath + "/names.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file loadfirst:" << file.errorString();
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        qDebug() << parts;
        if (parts[0] == name) {
            // Find the first image in the matching-name folder
            QString datasetFolder = QString(PROJECT_ROOT_DIR) + "/dataset/" + name;
            QDir dir(datasetFolder);
            QStringList filters;
            filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";
            dir.setNameFilters(filters);
            QStringList imageFiles = dir.entryList(QDir::Files, QDir::Name);
            if (!imageFiles.isEmpty()) {
                // Use the first image in the folder
                QString imagePath = datasetFolder + "/" + imageFiles.first();
                if (QFile::exists(imagePath)) {
                    imageLabel->setPixmap(QPixmap(imagePath).scaled(400, 400, Qt::KeepAspectRatio));
                } else {
                    imageLabel->setPixmap(QPixmap(QString(PROJECT_ROOT_DIR) + "/dataset/Sample_User_Icon.png")
                                           .scaled(400, 400, Qt::KeepAspectRatio));
                }
            } else {
                // If no matching images are found, fall back to the default sample icon
                imageLabel->setPixmap(QPixmap(QString(PROJECT_ROOT_DIR) + "/dataset/Sample_User_Icon.png")
                                       .scaled(400, 400, Qt::KeepAspectRatio));
            }
            // Update profile information
            nameLabel->setText("Name: " + parts[0]);
            dateLabel->setText("Date Joined: " + parts[1]);
            jobLabel->setText("Job Status: " + parts[2]);
            accessLabel->setText("Access Level: " + parts[3]);
            break;
        }
    }
    file.close();
}
/// @brief Gets the name of the user from the CSV file based on the line number
/**
 * @param number The line number in the CSV file
 * @return The name of the user at the specified line number
 *
 * This function opens the "names.csv" file and reads the specified line. It returns the name of the user
 * at that line number. If the file cannot be opened or if the line number is invalid, it returns "None".
 */
QString EditProfile::getName(int number) {
    QString textfilePath = QString(PROJECT_ROOT_DIR) + "/textfiles";
    QFile file(textfilePath + "/names.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file getname:" << file.errorString();
        return "None";
    }
    QTextStream in(&file);
    if (number == 0) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        file.close();
        return parts[0];
    }
    int linecount = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        if (!line.isEmpty() && linecount == number) {
            file.close();
            return parts[0];
        }
        linecount++;
    }
    file.close();
    return "Unassigned";
}

/// @brief Counts the number of non-empty lines in the CSV file
/**
 * @return The number of non-empty lines in the CSV file. If the CSV cannot be opened, it returns -1.
 *
 * This function first opens the "names.csv" file. If the file cannot open properly, it outputs an error message
 * and returns -1. Otherwise, it reads through the file line by line, counting the number of non-empty lines.
 * The function then returns the count of non-empty lines.
 */
/// @return
int EditProfile::countLinesInFile() {
    QString textfilePath = QString(PROJECT_ROOT_DIR) + "/textfiles";
    QFile file(textfilePath + "/names.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file countlines:" << file.errorString();
        return -1;
    }
    QTextStream in(&file);
    int linecount = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) {
            linecount++;
        }
    }
    qDebug() << "Here is the number of lines" << linecount;
    file.close();
    return linecount;
}

/// @brief Displays a popup showing the the current access level of the user and allows the user to edit it
/**
 * This function creates a popup window that displays the current access level of the user.
 * It allows the user to edit the access level by selecting from a list of options.
 * The selected access level is then saved to the CSV file.
 */
void EditProfile::showAccessLevelPopup() {
    auto *messageBox  = new QMessageBox(this);
    messageBox->setIcon(QMessageBox::Information);
    messageBox->setWindowTitle("Current Access Level");
    messageBox->setText( accessLabel->text());
    QPushButton *okButton = messageBox->addButton(QMessageBox::Ok);
    QPushButton *editButton = messageBox->addButton("Edit", QMessageBox::ActionRole);
    messageBox->resize(400, 200);
    // Dark mode styling for message box
    messageBox->setStyleSheet("background-color: #2c2c2c; color: white;");
    connect(editButton, &QPushButton::clicked, this, &EditProfile::editAccessLevel);
    connect(okButton,  &QPushButton::clicked, messageBox, &QMessageBox::accept);
    messageBox->exec();
}

/// @brief opens a dialog to edit the access level of the user of interest
/**
 * Allows the user to select a new job status and access level from a list of dropdown options. After an
 * option is selected, the function updates the CSV file with the new values.
 */
void EditProfile::editAccessLevel() {
    QDialog dialog(this);
    dialog.setWindowTitle("Access Level Information");
    dialog.setMinimumSize(400, 300);
    // Dark mode styling for dialog
    dialog.setStyleSheet("background-color: #2c2c2c; color: white;");

    auto *layout = new QVBoxLayout(&dialog);

    auto *nameLabelTitle = new QLabel( nameLabel->text());
    auto *dateLabelTitle = new QLabel(dateLabel->text());
    nameLabelTitle->setStyleSheet("color: white;");
    dateLabelTitle->setStyleSheet("color: white;");

    auto *jobLabelTitle = new QLabel("Job Status:");
    jobLabelTitle->setStyleSheet("color: white;");
    auto *jobComboBox = new QComboBox();
    jobComboBox->addItems({"Manager", "Employee", "Admin"});
    jobComboBox->setStyleSheet("background-color: #3a3a3a; color: white;");

    auto *accessLabelTitle = new QLabel("Access Level:");
    accessLabelTitle->setStyleSheet("color: white;");
    auto *accessComboBox = new QComboBox();
    accessComboBox->addItems({"1", "2", "3"});
    accessComboBox->setStyleSheet("background-color: #3a3a3a; color: white;");

    auto *okButton = new QPushButton("OK");
    okButton->setStyleSheet("background-color: #3a3a3a; color: white;");

    layout->addWidget(nameLabelTitle);
    layout->addWidget(dateLabelTitle);
    layout->addWidget(jobLabelTitle);
    layout->addWidget(jobComboBox);
    layout->addWidget(accessLabelTitle);
    layout->addWidget(accessComboBox);
    layout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, [&]() {
        const QString selectedJob = jobComboBox->currentText();
        const QString selectedAccess = accessComboBox->currentText();
        editCSVFile(nameLabel->text(), selectedJob, selectedAccess);
        jobLabel->setText("Job Status: " + selectedJob);
        accessLabel->setText("Access Level: " + selectedAccess);
        dialog.accept();
    });
    dialog.exec();
}

/// @brief Edits the CSV file with the new job status and access level for a given name
/**
 * @param job The new job status to be set for the user
 * @param access The new access level to be set for the user
 *
 * This function takes the new job status and access level as parameters and updates the CSV file
 * accordingly. It searches for the user's name in the CSV file and modifies the corresponding line
 * with the new values. If the user is not found, it outputs an error message.
 * The function also handles file opening and closing, ensuring that the changes are saved correctly.
 */
void EditProfile::editCSVFile(const QString& name, const QString& job, const QString& access) {
    QString textfilePath = QString(PROJECT_ROOT_DIR) + "/textfiles";
    QFile file(textfilePath + "/names.csv");
    QStringList Splitting = name.split(": ");
    const QString& newName = Splitting[1];
    qDebug() << "Here is the name" << newName;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file for reading:" << file.errorString();
        return;
    }
    QTextStream in(&file);
    QStringList lines;
    bool modified = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        if (!parts.isEmpty() && parts[0].trimmed() == newName) {
            if (parts.size() > 3) {
                parts[2] = job;
                parts[3] = access;
                line = parts.join(",");
                modified = true;
            }
        }
        lines.append(line);
    }
    file.close();
    if (!modified) {
        qDebug() << "No matching entry found for:" << newName;
        return;
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug() << "Error opening file for writing:" << file.errorString();
        return;
    }
    QTextStream out(&file);
    qDebug() << "Writing back modified lines.";
    for (const QString &line : lines) {
        qDebug() << "Writing:" << line;
        out << line << "\n";
    }
    file.close();
    qDebug() << "File updated successfully!";
}

// Slot for the Back button
void EditProfile::onBackButtonClicked() {
    // Set flag so that closing from the back button does NOT quit the program
    m_fromBackButton = true;
    close();
}

/// @brief Refreshes the CSV file
/**
* Refreshes the CSV file by scanning the dataset directory and updating the file with folder names and metadata.
*/
void EditProfile::refreshCSV() {
    // Build the dataset folder path
    QString datasetPath = QString(PROJECT_ROOT_DIR) + "/dataset";
    QDir datasetDir(datasetPath);
    if (!datasetDir.exists()) {
        qDebug() << "Dataset folder does not exist:" << datasetPath;
        return;
    }
    // List only directories (excluding . and ..)
    datasetDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QFileInfoList folderList = datasetDir.entryInfoList();

    // Open the names.csv file for writing (overwrite)
    QString csvPath = QString(PROJECT_ROOT_DIR) + "/textfiles/names.csv";
    QFile csvFile(csvPath);
    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
         qDebug() << "Error opening CSV file for writing:" << csvFile.errorString();
         return;
    }
    QTextStream out(&csvFile);
    // For each folder, write: folderName, lastModified, "Employee", 1
    for (const QFileInfo &folderInfo : folderList) {
         QString folderName = folderInfo.fileName();
         QDateTime lastModified = folderInfo.lastModified();
         QString dateStr = lastModified.toString("yyyy-MM-dd HH:mm:ss");
         out << folderName << "," << dateStr << "," << "Employee" << "," << 1 << "\n";
    }
    csvFile.close();
    qDebug() << "CSV refreshed successfully!";
}

/// @brief Handles the close event of the widget
/**
 * @param event The close event
 *
 * This function handles the close event of the widget. If the close event is triggered by the back button,
 * it allows the event to be accepted. Otherwise, it quits the application.
 */
void EditProfile::closeEvent(QCloseEvent *event) {
    if (!m_fromBackButton) {
        QCoreApplication::quit();
    } else {
        event->accept();
    }
}