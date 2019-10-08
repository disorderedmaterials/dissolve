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
#include "data/ff.h"
#include "templates/variantpointer.h"
#include <QRegExp>

// Constructor
SelectForcefieldDialog::SelectForcefieldDialog(QWidget* parent, const List<Forcefield>& forcefields) : forcefields_(forcefields)
{
	ui_.setupUi(this);

	// Populate the list with available templates
	ListIterator<Forcefield> templateIterator(forcefields_);
	while (Forcefield* ff = templateIterator.iterate())
	{
		QListWidgetItem* item = new QListWidgetItem(ff->name(), ui_.ForcefieldsList);
		item->setData(Qt::UserRole, VariantPointer<Forcefield>(ff));
	}
}

// Destructor
SelectForcefieldDialog::~SelectForcefieldDialog()
{
}

// Update the list of Forcefields, optionally filtering them by name and description
void SelectForcefieldDialog::updateForcefieldsList(Forcefield* current, QString filter)
{
	// Loop over items in the list
	for (int n=0; n<ui_.ForcefieldsList->count(); ++n)
	{
		QListWidgetItem* item = ui_.ForcefieldsList->item(n);
		if (!item) continue;
		Forcefield* ff = VariantPointer<Forcefield>(item->data(Qt::UserRole));
		if (ff == current) ui_.ForcefieldsList->setCurrentRow(n);

		// Check filtering
		if (filter.isEmpty()) item->setHidden(false);
		else
		{
			// Check name
			QString name = ff->name();
			bool inName = name.contains(QRegExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard));

			// Check description
			QString description = ff->description();
			bool inDescription = description.contains(QRegExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard));

			// Hide the item?
			bool hide = (!inName) && (!inDescription);
			item->setHidden(hide);

			// If the item was hidden, and it was selected, reset the current index
			if (hide && item->isSelected()) ui_.ForcefieldsList->setCurrentRow(-1);
		}
	}
}

void SelectForcefieldDialog::on_FilterEdit_textChanged(const QString& text)
{
	updateForcefieldsList(NULL, text);
}

void SelectForcefieldDialog::on_ForcefieldsList_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (!current)
	{
		ui_.ForcefieldDetailsTextEdit->clear();
		ui_.SelectButton->setEnabled(false);
		return;
	}

	// Get the selected template
	Forcefield* ff = VariantPointer<Forcefield>(current->data(Qt::UserRole));

	// Update the informational text
	ui_.ForcefieldDetailsTextEdit->setText(ff->description());

	ui_.SelectButton->setEnabled(true);
}

void SelectForcefieldDialog::on_ForcefieldsList_itemDoubleClicked(QListWidgetItem* item)
{
	if (!item) return;

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
	updateForcefieldsList(currentFF, ui_.FilterEdit->text());

	show();

	if (exec() == QDialog::Accepted)
	{
		QListWidgetItem* item = ui_.ForcefieldsList->currentItem();
		if (item == NULL) return NULL;
		return VariantPointer<Forcefield>(item->data(Qt::UserRole));
	}
	else return NULL;
}
