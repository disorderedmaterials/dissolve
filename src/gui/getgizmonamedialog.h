// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_getgizmonamedialog.h"
#include <QDialog>

// Forward Declarations
class Gizmo;

// Get Gizmo Name Dialog
class GetGizmoNameDialog : public QDialog
{
    Q_OBJECT

    public:
    GetGizmoNameDialog(QWidget *parent);
    ~GetGizmoNameDialog();

    private:
    // Main form declaration
    Ui::GetGizmoNameDialog ui_;
    // Current Gizmo that we are renaming
    const Gizmo *currentGizmo_;

    public:
    // Run the dialog
    bool get(const Gizmo *currentGizmo, const QString &currentName);
    // Return the name string
    QString newName() const;

    /*
     * Slots
     */
    private slots:
    void on_NameEdit_textChanged(const QString text);
    void on_CancelButton_clicked(bool checked);
    void on_OKButton_clicked(bool checked);
};
