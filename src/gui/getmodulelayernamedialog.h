// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_getmodulelayernamedialog.h"
#include "templates/list.h"
#include <QDialog>

// Forward Declarations
class CoreData;
class ModuleLayer;

// Get Layer Name Dialog
class GetModuleLayerNameDialog : public QDialog
{
    Q_OBJECT

    public:
    GetModuleLayerNameDialog(QWidget *parent, const List<ModuleLayer> &layers);
    ~GetModuleLayerNameDialog();

    private:
    // Main form declaration
    Ui::GetModuleLayerNameDialog ui_;
    // Layer list
    const List<ModuleLayer> &layers_;
    // Current ModuleLayer that we are renaming
    const ModuleLayer *moduleLayer_;

    public:
    // Run the dialog
    bool get(const ModuleLayer *currentModuleLayer, const QString &currentName);
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
