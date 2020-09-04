/*
    *** Select Element Dialog
    *** src/gui/selectelementdialog.h
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

#include "gui/ui_selectelementdialog.h"
#include <QDialog>

// Forward Declarations
class Dissolve;
class Element;
class SelectElementWidget;

// Select Element Dialog
class SelectElementDialog : public QDialog
{
    Q_OBJECT

    public:
    SelectElementDialog(QWidget *parent);
    ~SelectElementDialog();

    private:
    // Main form declaration
    Ui::SelectElementDialog ui_;

    private slots:
    void on_ElementWidget_elementSelected(bool dummy);
    void on_ElementWidget_elementDoubleClicked();
    void on_SelectButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);

    public:
    // Run the dialog, returning the selected Element
    Element *selectElement(Element *currentElement = nullptr);
};
