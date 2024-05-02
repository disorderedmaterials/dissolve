// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/ui_selectRestartFileDialog.h"
#include <QDialog>
#include <QFileSystemModel>

// Forward Declarations
class Dissolve;

// Select Restart File Dialog
class SelectRestartFileDialog : public QDialog
{
    Q_OBJECT

    public:
    SelectRestartFileDialog(QWidget *parent);
    ~SelectRestartFileDialog() = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::SelectRestartFileDialog ui_;

    private Q_SLOTS:
    void itemDoubleClicked(const QModelIndex &index);
    void itemSelectionChanged(const QItemSelection &currentSelection, const QItemSelection &oldSelection);
    void pathLoadingComplete(const QString &path);
    void on_ShowBackupFilesCheck_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);
    void on_SelectButton_clicked(bool checked);

    /*
     * Dialog
     */
    private:
    // File system model
    QFileSystemModel fileModel_;

    public:
    // Return restart file to load, based on supplied input file name
    QString getRestartFileName(const QString &inputFileName);
    // Select existing restart file to load, based on supplied input file name
    QString selectRestartFileName(const QString &inputFileName);
};
