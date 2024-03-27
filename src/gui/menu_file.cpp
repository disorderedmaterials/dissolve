// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/selectRestartFileDialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

// Set the input filename and the working directory of dissolve
void DissolveWindow::setInputFilename(const QString filename)
{
    QFileInfo inputFileInfo(filename);
    QDir::setCurrent(inputFileInfo.absoluteDir().absolutePath());

    dissolve_.setInputFilename(qPrintable(filename));
}

// Check whether current input needs to be saved and, if so, if it saved successfully
bool DissolveWindow::checkSaveCurrentInput()
{
    // First, check the modification status of the current session
    if (modified_)
    {
        QMessageBox queryBox;
        queryBox.setText("The current input file is unsaved.");
        queryBox.setInformativeText("Would you like to save it first?");
        queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        queryBox.setDefaultButton(QMessageBox::Cancel);
        auto ret = queryBox.exec();

        if (ret == QMessageBox::Cancel)
            return false;
        else if (ret == QMessageBox::Yes)
        {
            // If an input filename has been set, just overwrite it. If not, request a name first
            if (!dissolve_.hasInputFilename())
            {
                QString newFile = QFileDialog::getSaveFileName(this, "Choose input file name to save", QDir().absolutePath(),
                                                               "Dissolve input files (*.txt)");
                if (newFile.isEmpty())
                    return false;

                setInputFilename(newFile);
            }

            // Save the file
            if (!dissolve_.saveInput(dissolve_.inputFilename()))
                return false;

            modified_ = false;
        }
    }

    // All OK
    return true;
}

// Clear all data and start new simulation afresh
void DissolveWindow::startNew()
{
    refreshing_ = true;

    // Clear any data-related tabs from the UI
    ui_.MainTabs->clearTabs();

    // Clear Dissolve itself
    dissolve_.clear();
    clearMessages();

    dissolveIterating_ = false;
    modified_ = false;

    refreshing_ = false;

    // Explicitly show the main stack page
    ui_.MainStack->setCurrentIndex(1);

    fullUpdate();
}

/*
 * Open Recent Functions
 */

// Set up recent file menu
void DissolveWindow::setUpRecentFileMenu()
{
    QFont font = ui_.SessionMenu->font();
    ui_.FileOpenRecentMenu->setFont(font);

    for (auto i = 0; i < recentFileLimit_; ++i)
    {
        auto *recentFileAction = new QAction(this);
        recentFileAction->setVisible(false);
        QObject::connect(recentFileAction, SIGNAL(triggered(bool)), this, SLOT(recentFileSelected()));
        ui_.FileOpenRecentMenu->addAction(recentFileAction);
        recentFileActionList_.append(recentFileAction);
    }

    updateRecentFileMenu();
}

// Add specified file to recent files list
void DissolveWindow::addRecentFile(const QString &filePath)
{
    // Add new entry to recent files
    QSettings settings;
    QStringList recentFilePaths = settings.value("recentFiles").toStringList();
    recentFilePaths.removeAll(filePath);
    recentFilePaths.prepend(filePath);
    if (recentFilePaths.size() > recentFileLimit_)
        recentFilePaths.erase(recentFilePaths.begin() + recentFileLimit_, recentFilePaths.end());
    settings.setValue("recentFiles", recentFilePaths);

    updateRecentFileMenu();
}

// Update recent files menu
void DissolveWindow::updateRecentFileMenu()
{
    QSettings settings;
    QStringList recentFilePaths = settings.value("recentFiles").toStringList();

    // Fill recent menu
    for (auto i = 0; i < recentFileLimit_; ++i)
    {
        if (i < recentFilePaths.size())
        {
            auto fileInfo = QFileInfo(recentFilePaths.at(i));
            recentFileActionList_.at(i)->setText(fileInfo.fileName() + "    (" + fileInfo.absoluteDir().absolutePath() + ")");
            recentFileActionList_.at(i)->setData(recentFilePaths.at(i));
            recentFileActionList_.at(i)->setVisible(true);
        }
        else
            recentFileActionList_.at(i)->setVisible(false);
    }
}

void DissolveWindow::on_FileNewAction_triggered(bool checked)
{
    if (!checkSaveCurrentInput())
        return;

    startNew();
}

void DissolveWindow::on_FileOpenAction_triggered(bool checked)
{
    if (!checkSaveCurrentInput())
        return;

    // Request a new file to open
    QString inputFile = QFileDialog::getOpenFileName(this, "Choose input file to open", QDir().absolutePath(),
                                                     "Dissolve input files (*.txt);;All Files (*)");
    if (inputFile.isEmpty())
        return;

    loadInputFile(qPrintable(inputFile), true);

    fullUpdate();
}

void DissolveWindow::recentFileSelected()
{
    if (!checkSaveCurrentInput())
        return;

    auto *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        loadInputFile(action->data().toString().toStdString(), true);

        fullUpdate();
    }
}

void DissolveWindow::on_FileLoadRestartPointAction_triggered(bool checked)
{
    SelectRestartFileDialog selectRestartFileDialog(this);
    auto restartFile =
        selectRestartFileDialog.selectRestartFileName(QString::fromStdString(std::string(dissolve_.inputFilename())));
    if (restartFile.isEmpty())
        return;

    // Warn the user that we're about to clear everything...
    if (QMessageBox::warning(
            this, "Load Restart Point",
            "This will delete all data generated by modules. Configuration data will remain intact unless overwritten by the "
            "contents of the restart file, and the iteration counter may change according to the restart file history."
            "\n\nThis cannot be undone! Proceed?",
            QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
            QMessageBox::StandardButton::No) != QMessageBox::Yes)
        return;

    clearModuleData(false);
    loadRestartFile(restartFile.toStdString());

    fullUpdate();
}

void DissolveWindow::on_FileSaveRestartPointAction_triggered(bool checked)
{
    // Get filename for restart point
    QString filename =
        QFileDialog::getSaveFileName(this, "Select Restart File", QDir::currentPath(), "Restart Files (*.restart)");
    if (filename.isEmpty())
        return;

    if (dissolve_.saveRestart(qPrintable(filename)))
        statusBar()->showMessage(QString("Saved restart point to '%1'.").arg(filename), 3000);
    else
        statusBar()->showMessage(QString("ERROR: Failed to save restart point to '%1'.").arg(filename), 3000);
}

void DissolveWindow::on_FileCloseAction_triggered(bool checked)
{
    if (!checkSaveCurrentInput())
        return;

    refreshing_ = true;

    // Clear any data-related tabs from the UI
    ui_.MainTabs->clearTabs();

    // Clear the messages widget
    clearMessages();

    refreshing_ = false;

    // Clear Dissolve
    dissolve_.clear();
    dissolveIterating_ = false;
    modified_ = false;

    // Set the stack back to the ident page
    ui_.MainStack->setCurrentIndex(0);
    fullUpdate();
}

void DissolveWindow::on_FileSaveAction_triggered(bool checked)
{
    // If the file is not modified, nothing to do.
    if (!modified_)
        return;

    // If an input file name has not been set, get one now
    if (!dissolve_.hasInputFilename())
    {
        QString newFile = QFileDialog::getSaveFileName(this, "Choose input file name to save", QDir().absolutePath(),
                                                       "Dissolve input files (*.txt)");
        if (newFile.isEmpty())
            return;

        setInputFilename(newFile);

        addRecentFile(newFile);
    }

    // Attempt to save the file
    if (!dissolve_.saveInput(dissolve_.inputFilename()))
        return;

    modified_ = false;

    updateStatusBar();

    updateWindowTitle();

    statusBar()->showMessage(
        QString("Input file saved to '%1'.").arg(QString::fromStdString(std::string(dissolve_.inputFilename()))), 3000);
}

void DissolveWindow::on_FileSaveAsAction_triggered(bool checked)
{
    // Get a suitable input file name
    QString newFile = QFileDialog::getSaveFileName(this, "Choose input file name to save", QDir().absolutePath(),
                                                   "Dissolve input files (*.txt)");
    if (newFile.isEmpty())
        return;

    // Attempt to save the file
    if (!dissolve_.saveInput(qPrintable(newFile)))
        return;

    modified_ = false;

    // Update the current working directory to be local to the new input file
    setInputFilename(newFile);

    addRecentFile(newFile);

    updateStatusBar();

    updateWindowTitle();

    statusBar()->showMessage(
        QString("Input file saved to '%1'.").arg(QString::fromStdString(std::string(dissolve_.inputFilename()))), 3000);
}

void DissolveWindow::on_FileQuitAction_triggered(bool checked)
{
    if (!checkSaveCurrentInput())
        return;

    close();
}
