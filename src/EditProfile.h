#ifndef EDITPROFILE_H
#define EDITPROFILE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>

/// @brief This class provides functionality to edit a user's job status and access levels.
/**
 * The EditProfiles class allows for the modification of User profiles' job status and access levels while providing
 * a UI with Qt to do so. When changes to permissions are made, the dataset is refreshed to reflect the new changes
 * @file EditProfile.h
 * @author Kevin Russel
 */
class EditProfile: public QWidget{
    Q_OBJECT
    public:
    /// @brief The constructor sets up the GUI's layout
    /**
     * @param parent The parent widget for the GUI
     *
     * This constructor initializes the GUI layout, including the sidebar, middle section, and right section.
     * It also displays the profile details (name, date joined, highest access level, and job status) in the GUI.
     * The GUI includes buttons on the sidebar for navigating back and refreshing the dataset.
     * The middle section displays the profile details, and the right section contains a scroll area for profile buttons.
     */
    explicit EditProfile(QWidget *parent = nullptr);

    /// @brief Counts the number of non-empty lines in the CSV file
    /**
     * @return The number of non-empty lines in the CSV file. If the CSV cannot be opened, it returns -1.
     *
     * This function first opens the "names.csv" file. If the file cannot open properly, it outputs an error message
     * and returns -1. Otherwise, it reads through the file line by line, counting the number of non-empty lines.
     * The function then returns the count of non-empty lines.
     */
    /// @return
    int countLinesInFile();

    /// @brief Displays a popup showing the the current access level of the user and allows the user to edit it
    /**
     * This function creates a popup window that displays the current access level of the user.
     * It allows the user to edit the access level by selecting from a list of options.
     * The selected access level is then saved to the CSV file.
     */
    void showAccessLevelPopup();

    /// @brief opens a dialog to edit the access level of the user of interest
    /**
     * Allows the user to select a new job status and access level from a list of dropdown options. After an
     * option is selected, the function updates the CSV file with the new values.
     */
    void editAccessLevel();

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
    static void editCSVFile(const QString&, const QString& job, const QString& access);

    void closeEvent(QCloseEvent *event);

private slots:
    /// @brief Sets a flag to indicate that the back button was clicked and sends a signal to close the window.
    void onBackButtonClicked();

    /// @brief Refreshes the CSV file
    /**
     * Refreshes the CSV file by scanning the dataset directory and updating the file with folder names and metadata.
     */
    void refreshCSV();

private:
    QFrame* createWindowFrame();
    QFrame* CreateTopSection();
    QFrame* createMiddleSection();
    QScrollArea *scrollArea;
    QString getName(int);
    QLabel *firstLineLabel;
    QLabel *imageLabel;
    QLabel* nameLabel;
    QLabel* dateLabel;
    QLabel* jobLabel;
    QLabel* accessLabel;
    void loadFirstLine(QString name);
    bool m_fromBackButton = false;


};

#endif