/*
	*** Select Forcefield Dialog
	*** src/gui/selectforcefielddialog_funcs.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/selectforcefielddialog.h"
#include "gui/selectforcefieldwidget.h"
#include "data/ff.h"
#include "templates/variantpointer.h"
#include <QRegExp>

// Constructor
SelectForcefieldDialog::SelectForcefieldDialog(QWidget* parent, const List<Forcefield>& forcefields)
{
	ui_.setupUi(this);

	// Create and set up a SelectForcefieldWidget within our dialog
	ffWidget_ = new SelectForcefieldWidget(ui_.ForcefieldWidget, forcefields);
	connect(ffWidget_, SIGNAL(forcefieldSelectionChanged(bool)), this, SLOT(currentForcefieldChanged(bool)));
	connect(ffWidget_, SIGNAL(forcefieldDoubleClicked()), this, SLOT(forcefieldDoubleClicked()));
}

// Destructor
SelectForcefieldDialog::~SelectForcefieldDialog()
{
}

void SelectForcefieldDialog::currentForcefieldChanged(bool isValid)
{
	ui_.SelectButton->setEnabled(isValid);
}

void SelectForcefieldDialog::forcefieldDoubleClicked()
{
	if (!ffWidget_->currentForcefield()) return;

	accept();
}

void SelectForcefieldDialog::on_SelectButton_clicked(bool checked)
{
	accept();
}

void SelectForcefieldDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

// Run the dialog, returning the selected Forcefield
Forcefield* SelectForcefieldDialog::selectForcefield(Forcefield* currentFF)
{
	ffWidget_->setCurrentForcefield(currentFF);

	show();

	if (exec() == QDialog::Accepted) return ffWidget_->currentForcefield();
	else return NULL;
}
