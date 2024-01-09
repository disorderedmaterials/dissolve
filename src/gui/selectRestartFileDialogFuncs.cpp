// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/selectRestartFileDialog.h"
#include <QFileDialog>
#include <QMessageBox>

SelectRestartFileDialog::SelectRestartFileDialog(QWidget *parent) : QDialog(parent)
{
    ui_.setupUi(this);

    fileModel_.setFilter(QDir::Files);
    fileModel_.setNameFilters(QStringList() << "*.restart");
    fileModel_.setNameFilterDisables(false);
    connect(&fileModel_, SIGNAL(directoryLoaded(const QString &)), this, SLOT(pathLoadingComplete(const QString &)));

    ui_.RestartFilesTable->setModel(&fileModel_);
    ui_.RestartFilesTable->setColumnHidden(2, true);

    connect(ui_.RestartFilesTable, SIGNAL(doubleClicked(const QModelIndex &)), this,
            SLOT(itemDoubleClicked(const QModelIndex &)));
    connect(ui_.RestartFilesTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(itemSelectionChanged(const QItemSelection &, const QItemSelection &)));
}

/*
 * UI
 */

void SelectRestartFileDialog::on_ShowBackupFilesCheck_clicked(bool checked)
{
    if (checked)
        fileModel_.setNameFilters(QStringList() << "*.restart"
                                                << "*.restart.prev");
    else
        fileModel_.setNameFilters(QStringList() << "*.restart");

    ui_.RestartFilesTable->setCurrentIndex(QModelIndex());
    ui_.RestartFilesTable->resizeColumnsToContents();
}

void SelectRestartFileDialog::on_CancelButton_clicked(bool checked) { reject(); }

void SelectRestartFileDialog::on_SelectButton_clicked(bool checked) { accept(); }

void SelectRestartFileDialog::itemDoubleClicked(const QModelIndex &index) { accept(); }

void SelectRestartFileDialog::itemSelectionChanged(const QItemSelection &currentSelection, const QItemSelection &oldSelection)
{
    ui_.SelectButton->setDisabled(currentSelection.indexes().isEmpty());
}

void SelectRestartFileDialog::pathLoadingComplete(const QString &path) { ui_.RestartFilesTable->resizeColumnsToContents(); }

/*
 * Dialog
 */

// Return restart file to load, based on supplied input file name
QString SelectRestartFileDialog::getRestartFileName(const QString &inputFileName)
{
    // Get file information for the input file
    QFileInfo inputFileInfo(inputFileName);

    // Update the file model and table to focus on the input file's directory
    auto pathRootIndex = fileModel_.setRootPath(inputFileInfo.path());
    ui_.RestartFilesTable->setRootIndex(pathRootIndex);

    // Does the default restart file exist?
    auto defaultRestartFileExists = QFile::exists(inputFileName + ".restart");

    // Store the total number of other restart files available using QDir's methods (QFileSystemModel populates itself in a
    // separate thread, so we can't rely on its rowCount() function)
    auto nRestartFilesAvailable = inputFileInfo.dir().entryInfoList(QStringList() << "*.restart", QDir::Files).size();

    // Go time - check what we have available, and query the user appropriately
    QMessageBox::StandardButton button;
    if (defaultRestartFileExists)
    {
        // The default restart file exists, and others may be present
        if (nRestartFilesAvailable == 1)
            button = QMessageBox::question(
                this, "Load Associated Restart File?",
                "A restart file associated to this simulation was found - would you like to load it now?",
                QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::Yes);
        else
            button = QMessageBox::question(
                this, "Load Associated Restart File?",
                "A restart file associated to this simulation was found, but additional restart files in the same directory "
                "are "
                "also present.\nWould you like to load the default associated file, or open a different one?",
                QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::Open | QMessageBox::StandardButton::Cancel,
                QMessageBox::StandardButton::Yes);
    }
    else if (nRestartFilesAvailable != 0)
    {
        // The default restart file does not exist, but other files do
        button = QMessageBox::question(this, "Load Alternate Restart File?",
                                       "A restart file associated to this simulation does not exist, but additional restart "
                                       "files in the same directory were found."
                                       "\nWould you like to open one of those files?",
                                       QMessageBox::StandardButton::Open | QMessageBox::StandardButton::Cancel,
                                       QMessageBox::StandardButton::Cancel);
    }
    else
        return {};

    // Act on the button choice
    if (button == QMessageBox::StandardButton::Yes)
        return inputFileName + ".restart";
    else if (button == QMessageBox::StandardButton::Open)
    {
        // Execute the dialog and get a suitable restart file name to load
        show();
        ui_.RestartFilesTable->resizeColumnsToContents();
        ui_.RestartFilesTable->setCurrentIndex(fileModel_.index(0, 0, pathRootIndex));
        auto result = exec();
        if (result == QDialog::Accepted)
            return fileModel_.filePath(fileModel_.index(ui_.RestartFilesTable->currentIndex().row(), 0, pathRootIndex));
        else
            return {};
    }

    return {};
}

// Select existing restart file to load, based on supplied input file name
QString SelectRestartFileDialog::selectRestartFileName(const QString &inputFileName)
{
    // Get file information for the input file
    QFileInfo inputFileInfo(inputFileName);

    // Update the file model and table to focus on the input file's directory
    auto pathRootIndex = fileModel_.setRootPath(inputFileInfo.path());
    ui_.RestartFilesTable->setRootIndex(pathRootIndex);

    // Execute the dialog and get a suitable restart file name to load
    show();
    ui_.RestartFilesTable->resizeColumnsToContents();
    ui_.RestartFilesTable->setCurrentIndex(fileModel_.index(0, 0, pathRootIndex));
    auto result = exec();
    if (result == QDialog::Accepted)
        return fileModel_.filePath(fileModel_.index(ui_.RestartFilesTable->currentIndex().row(), 0, pathRootIndex));
    else
        return {};
}
