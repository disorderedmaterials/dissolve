/*
    *** Select Element Dialog
    *** src/gui/selectelementdialog_funcs.cpp
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

#include "gui/selectelementdialog.h"
#include "templates/variantpointer.h"
#include <QRegExp>

SelectElementDialog::SelectElementDialog(QWidget *parent) { ui_.setupUi(this); }

SelectElementDialog::~SelectElementDialog() {}

void SelectElementDialog::on_ElementWidget_elementSelected(bool dummy) { ui_.SelectButton->setEnabled(true); }

void SelectElementDialog::on_ElementWidget_elementDoubleClicked()
{
    if (!ui_.ElementWidget->currentElement())
        return;

    accept();
}

void SelectElementDialog::on_SelectButton_clicked(bool checked) { accept(); }

void SelectElementDialog::on_CancelButton_clicked(bool checked) { reject(); }

// Run the dialog, returning the selected Element
Element *SelectElementDialog::selectElement(Element *currentElement)
{
    ui_.ElementWidget->setCurrentElement(currentElement);

    show();

    if (exec() == QDialog::Accepted)
        return ui_.ElementWidget->currentElement();
    else
        return nullptr;
}
