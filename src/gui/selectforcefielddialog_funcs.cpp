/*
    *** Select Forcefield Dialog
    *** src/gui/selectforcefielddialog_funcs.cpp
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
#include "gui/selectforcefielddialog.h"
#include "gui/selectforcefieldwidget.h"
#include "templates/variantpointer.h"
#include <QRegExp>

SelectForcefieldDialog::SelectForcefieldDialog(QWidget *parent, const List<Forcefield> &forcefields) { ui_.setupUi(this); }

SelectForcefieldDialog::~SelectForcefieldDialog() {}

void SelectForcefieldDialog::on_ForcefieldWidget_forcefieldSelectionChanged(bool isValid)
{
    ui_.SelectButton->setEnabled(isValid);
}

void SelectForcefieldDialog::on_ForcefieldWidget_forcefieldDoubleClicked()
{
    if (!ui_.ForcefieldWidget->currentForcefield())
        return;

    accept();
}

void SelectForcefieldDialog::on_SelectButton_clicked(bool checked) { accept(); }

void SelectForcefieldDialog::on_CancelButton_clicked(bool checked) { reject(); }

// Run the dialog, returning the selected Forcefield
std::shared_ptr<Forcefield> SelectForcefieldDialog::selectForcefield(std::shared_ptr<Forcefield> currentFF)
{
    ui_.ForcefieldWidget->setCurrentForcefield(currentFF);

    show();

    if (exec() == QDialog::Accepted)
        return ui_.ForcefieldWidget->currentForcefield();
    else
        return nullptr;
}
