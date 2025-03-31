#ifndef FACEMANAGER_H
#define FACEMANAGER_H

#include <QString>
#include <QStringList>
#include <QWidget>

/// @brief This class manages the addition and deletion of face images for profiles within the dataset.
/**
 * This class allows for the addition and deletion of face images for a preexisting profile, or for new profiles
 * in the dataset. The class uses the Qt GUI libraries to prompt the user for a folder within the dataset directory.
 *
 * @file facemanager.h
 * @author Lukas Bozinov
 */
class FaceManager {
public:
    /// @brief Constructor accepts a parent Qwidget and the path to the dataset folder.
    /**
     * @param parent The parent widget for dialog boxes.
     * @param datasetPath The path to the dataset folder where images will be added or deleted.
     *                  This should be a valid directory path, for obvious reasons.
     *
     * The constructor initialize the Facemanager object with the given parent widget and dataset path.
     */
    FaceManager(QWidget *parent, const QString &datasetPath);

    /// @brief Open a Qtwidget file dialog to select a folder in the dataset to add images to.
    /**
     * This function allows the user to select one or more images to add to a specific folder in the dataset.
     * The user is prompted to select a folder within the dataset directory, and the selected images are copied
     * to that folder. If a file with the same name already exists, it will be replaced.
     */
    void addFace();

    /// @brief Open a Qtwidget file dialog to select a folder in the dataset to delete images from.
    /**
     * This function allows the user to select one or more images to delete from a specific folder in the dataset.
     * The user is prompted to select a folder within the dataset directory, and the selected images are deleted
     * from that folder. If a file with the same name does not exist, it will be ignored.
     */
    void deleteFace();

private:
    QWidget *m_parent;      // Used for dialogs
    QString m_datasetPath;  // E.g., PROJECT_ROOT_DIR + "/dataset"
};

#endif // FACEMANAGER_H