#include "facemanager.h"
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>
#include <QCoreApplication>

/**
 * @brief This class manages the addition and deletion of face images for profiles within the dataset.
 * @file facemanager.cpp
 */

/// @brief Constructor accepts a parent Qwidget and the path to the dataset folder.
/**
 * @param parent The parent widget for dialog boxes.
 * @param datasetPath The path to the dataset folder where images will be added or deleted.
 *                  This should be a valid directory path, for obvious reasons.
 *
 * The constructor initialize the Facemanager object with the given parent widget and dataset path.
 */
FaceManager::FaceManager(QWidget *parent, const QString &datasetPath)
    : m_parent(parent),
      m_datasetPath(datasetPath)
{
}

/// @brief Open a Qtwidget file dialog to select a folder in the dataset to add images to.
/**
 * This function allows the user to select one or more images to add to a specific folder in the dataset.
 * The user is prompted to select a folder within the dataset directory, and the selected images are copied
 * to that folder. If a file with the same name already exists, it will be replaced.
 */
void FaceManager::addFace() {
    // 1) Let the user pick one or more images
    QStringList fileNames = QFileDialog::getOpenFileNames(
        m_parent,
        QObject::tr("Select one or more images to add"),
        QString(), // starting directory (could be adjusted)
        QObject::tr("Images (*.png *.jpg *.jpeg *.bmp *.xpm)")
    );

    if (fileNames.isEmpty()) {
        qDebug() << "No images selected.";
        return;
    }

    // 2) Ask the user to choose (or create) a folder inside the dataset folder
    QString selectedDir = QFileDialog::getExistingDirectory(
        m_parent,
        QObject::tr("Select or create a folder in the dataset directory"),
        m_datasetPath,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (selectedDir.isEmpty()) {
        qDebug() << "No folder selected.";
        return;
    }

    // Optional safety: ensure the selected folder is inside m_datasetPath
    if (!selectedDir.startsWith(m_datasetPath)) {
        QMessageBox::warning(m_parent, QObject::tr("Invalid Folder"),
            QObject::tr("Please select or create a folder inside '%1'.").arg(m_datasetPath));
        return;
    }

    QDir targetDir(selectedDir);
    // 3) Copy the selected images to the target folder
    for (const QString &filePath : fileNames) {
        QFileInfo fileInfo(filePath);
        QString destFilePath = targetDir.filePath(fileInfo.fileName());

        // Handle name collisions: remove existing file
        if (QFile::exists(destFilePath)) {
            QFile::remove(destFilePath);
        }

        bool success = QFile::copy(filePath, destFilePath);
        if (!success) {
            qDebug() << "Failed to copy" << filePath << "to" << destFilePath;
        } else {
            qDebug() << "Copied" << filePath << "to" << destFilePath;
        }
    }

    QMessageBox::information(
        m_parent,
        QObject::tr("Add Face"),
        QObject::tr("Successfully added %1 image(s) to:\n%2")
            .arg(fileNames.size())
            .arg(selectedDir)
    );
}

/// @brief Open a Qtwidget file dialog to select a folder in the dataset to delete images from.
/**
 * This function allows the user to select one or more images to delete from a specific folder in the dataset.
 * The user is prompted to select a folder within the dataset directory, and the selected images are deleted
 * from that folder. If a file with the same name does not exist, it will be ignored.
 */
void FaceManager::deleteFace() {
    // 1) Ask the user to choose a folder in the dataset to delete images from.
    QString selectedDir = QFileDialog::getExistingDirectory(
        m_parent,
        QObject::tr("Select a folder in the dataset to delete images from"),
        m_datasetPath,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (selectedDir.isEmpty()) {
        qDebug() << "No folder selected.";
        return;
    }

    if (!selectedDir.startsWith(m_datasetPath)) {
        QMessageBox::warning(m_parent, QObject::tr("Invalid Folder"),
            QObject::tr("Please select a folder inside '%1'.").arg(m_datasetPath));
        return;
    }

    // 2) Let the user select one or more images within that folder to delete
    QStringList fileNames = QFileDialog::getOpenFileNames(
        m_parent,
        QObject::tr("Select one or more images to delete"),
        selectedDir,
        QObject::tr("Images (*.png *.jpg *.jpeg *.bmp *.xpm)")
    );

    if (fileNames.isEmpty()) {
        qDebug() << "No images selected for deletion.";
        return;
    }

    // 3) Delete the selected images
    int deletedCount = 0;
    for (const QString &filePath : fileNames) {
        if (QFile::exists(filePath)) {
            bool success = QFile::remove(filePath);
            if (success) {
                qDebug() << "Deleted" << filePath;
                deletedCount++;
            } else {
                qDebug() << "Failed to delete" << filePath;
            }
        }
    }

    QMessageBox::information(
        m_parent,
        QObject::tr("Delete Face"),
        QObject::tr("Successfully deleted %1 image(s) from:\n%2")
            .arg(deletedCount)
            .arg(selectedDir)
    );
}