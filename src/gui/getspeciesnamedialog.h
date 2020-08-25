/*
    *** Get Species Name Dialog
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

#include "gui/ui_getspeciesnamedialog.h"
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
    private slots:
    void on_NameEdit_textChanged(const QString text);
    void on_CancelButton_clicked(bool checked);
    void on_OKButton_clicked(bool checked);
};
