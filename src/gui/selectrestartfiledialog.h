// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/ui_selectrestartfiledialog.h"
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

    private slots:
    void on_ShowBackupFilesCheck_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);
    void on_SelectButton_clicked(bool checked);
    void itemDoubleClicked(const QModelIndex &index);

    /*
     * Dialog
     */
    private:
    // File system model
    QFileSystemModel fileModel_;

    public:
    // Return restart file to load, based on supplied input file name
    QString getRestartFileName(const QString &inputFileName);
};
