/*
    *** Select Configuration Dialog
    *** src/gui/selectconfigurationdialog_funcs.cpp
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

#include "data/ff.h"
#include "gui/selectconfigurationdialog.h"
#include "gui/selectconfigurationwidget.h"
#include "templates/variantpointer.h"
#include <QRegExp>

SelectConfigurationDialog::SelectConfigurationDialog(QWidget *parent, const CoreData &coreData, QString dialogTitle)
{
    ui_.setupUi(this);

    setWindowTitle(dialogTitle);

    ui_.ConfigurationWidget->setCoreData(&coreData);
}

SelectConfigurationDialog::~SelectConfigurationDialog() {}

void SelectConfigurationDialog::on_ConfigurationWidget_speciesSelectionChanged(bool isValid)
{
    ui_.SelectButton->setEnabled(isValid);
}

void SelectConfigurationDialog::on_ConfigurationWidget_speciesDoubleClicked()
{
    // Check current selection size for validity
    if (ui_.ConfigurationWidget->currentConfiguration().nItems() != 1)
        return;

    accept();
}

void SelectConfigurationDialog::on_SelectButton_clicked(bool checked) { accept(); }

void SelectConfigurationDialog::on_CancelButton_clicked(bool checked) { reject(); }

// Run the dialog, returning a single selected Configuration
Configuration *SelectConfigurationDialog::selectConfiguration()
{
    ui_.ConfigurationWidget->reset(1, 1);

    show();

    if (exec() == QDialog::Accepted)
        return ui_.ConfigurationWidget->currentConfiguration().firstItem();
    else
        return nullptr;
}

// Run the dialog, returning a list of selected Configuration
RefList<Configuration> SelectConfigurationDialog::selectConfiguration(int minConfiguration, int maxConfiguration)
{
    ui_.ConfigurationWidget->reset(minConfiguration, maxConfiguration);

    show();

    if (exec() == QDialog::Accepted)
        return ui_.ConfigurationWidget->currentConfiguration();
    else
        return RefList<Configuration>();
}
