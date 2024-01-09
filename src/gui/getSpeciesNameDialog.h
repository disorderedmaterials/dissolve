// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/ui_getSpeciesNameDialog.h"
#include <QDialog>

// Forward Declarations
class CoreData;
class Species;

// Get Species Name Dialog
class GetSpeciesNameDialog : public QDialog
{
    Q_OBJECT

    public:
    GetSpeciesNameDialog(QWidget *parent, const CoreData &coreData);
    ~GetSpeciesNameDialog();

    private:
    // Main form declaration
    Ui::GetSpeciesNameDialog ui_;
    // CoreData reference
    const CoreData &coreData_;
    // Current Species that we are renaming
    const Species *species_;

    public:
    // Run the dialog
    bool get(const Species *currentSpecies, const QString &currentName);
    // Return the name string
    QString newName() const;

    /*
     * Slots
     */
    private Q_SLOTS:
    void on_NameEdit_textChanged(const QString text);
    void on_CancelButton_clicked(bool checked);
    void on_OKButton_clicked(bool checked);
};
