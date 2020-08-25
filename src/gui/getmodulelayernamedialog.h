/*
    *** Get ModuleLayer Name Dialog
    *** src/gui/getspeciesnamedialog.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
