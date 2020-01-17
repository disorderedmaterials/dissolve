/*
	*** Select Forcefield Widget
	*** src/gui/selectforcefieldwidget_funcs.cpp
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

#include "gui/selectforcefieldwidget.h"
#include "data/ff.h"
#include "templates/variantpointer.h"
#include <QRegExp>

// Constructor
SelectForcefieldWidget::SelectForcefieldWidget(QWidget* parent, const List<Forcefield>& forcefields) : QWidget(parent), forcefields_(forcefields)
{
	ui_.setupUi(this);

	refreshing_ = true;

	// Populate the list with available forcefields
	ListIterator<Forcefield> forcefieldIterator(forcefields_);
	while (Forcefield* ff = forcefieldIterator.iterate())
	{
		QListWidgetItem* item = new QListWidgetItem(ff->name(), ui_.ForcefieldsList);
		item->setData(Qt::UserRole, VariantPointer<Forcefield>(ff));
	}

	refreshing_ = false;
}

// Destructor
SelectForcefieldWidget::~SelectForcefieldWidget()
{
}

// Update the list of Forcefields, optionally filtering them by name and description
void SelectForcefieldWidget::updateForcefieldsList(Forcefield* current, QString filter)
{
	// Loop over items in the list
	for (int n=0; n<ui_.ForcefieldsList->count(); ++n)
	{
		QListWidgetItem* item = ui_.ForcefieldsList->item(n);
		if (!item) continue;
		Forcefield* ff = VariantPointer<Forcefield>(item->data(Qt::UserRole));
		if (ff == current)
		{
			ui_.ForcefieldsList->setCurrentRow(n);
			emit(forcefieldSelectionChanged(true));
		}

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
			if (hide && item->isSelected())
			{
				ui_.ForcefieldsList->setCurrentRow(-1);
				emit(forcefieldSelectionChanged(false));
			}
		}
	}
}

void SelectForcefieldWidget::on_FilterEdit_textChanged(const QString& text)
{
	updateForcefieldsList(NULL, text);
}

void SelectForcefieldWidget::on_ForcefieldsList_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (!current)
	{
		ui_.ForcefieldDetailsTextEdit->clear();
		emit(forcefieldSelectionChanged(false));
		return;
	}

	// Get the selected forcefield
	Forcefield* ff = VariantPointer<Forcefield>(current->data(Qt::UserRole));

	// Update the informational text
	ui_.ForcefieldDetailsTextEdit->setText(ff->description());

	emit(forcefieldSelectionChanged(true));
}

void SelectForcefieldWidget::on_ForcefieldsList_itemDoubleClicked(QListWidgetItem* item)
{
	if (!item) return;

	emit(forcefieldDoubleClicked());
}

// Set the current forcefield
void SelectForcefieldWidget::setCurrentForcefield(Forcefield* currentFF)
{
	updateForcefieldsList(currentFF, ui_.FilterEdit->text());
}

// Return the currently-selected Forcefield
Forcefield* SelectForcefieldWidget::currentForcefield() const
{
	QListWidgetItem* item = ui_.ForcefieldsList->currentItem();
	if (item == NULL) return NULL;

	return VariantPointer<Forcefield>(item->data(Qt::UserRole));
}
