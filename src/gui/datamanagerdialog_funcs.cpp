/*
	*** Data Manager Dialog
	*** src/gui/datamanagerdialog_funcs.cpp
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

#include "gui/datamanagerdialog.h"
#include "main/dissolve.h"
#include "genericitems/item.h"
#include "templates/variantpointer.h"
#include <QRegExp>

// Constructor
DataManagerDialog::DataManagerDialog(QWidget* parent, Dissolve& dissolve)
{
	ui_.setupUi(this);

	addItemsToTable(ui_.SimulationDataTable, dissolve.processingModuleData().items(), "Main Processing", ":/dissolve/icons/dissolve.png");
	ListIterator<Configuration> configIterator(dissolve.configurations());
	while (Configuration* cfg = configIterator.iterate()) addItemsToTable(ui_.SimulationDataTable, cfg->moduleData().items(), cfg->name(), ":/tabs/icons/tabs_configuration.svg");
}

// Destructor
DataManagerDialog::~DataManagerDialog()
{
}

/*
 * UI
 */

// Append GenericItems to table under specified source
void DataManagerDialog::addItemsToTable(QTableWidget* table, List<GenericItem>& items, const char* locationName, const char* locationIconResource)
{
	// Create icon
	QIcon locationIcon = QPixmap(locationIconResource);

	QTableWidgetItem* item;
	int count = table->rowCount();
	table->setRowCount(count + items.nItems());
	ListIterator<GenericItem> itemIterator(items);
	while (GenericItem* genericItem = itemIterator.iterate())
	{
		// Item name
		item = new QTableWidgetItem(genericItem->name());
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		item->setData(Qt::UserRole, VariantPointer<GenericItem>(genericItem));
		table->setItem(count, 0, item);

		// Item type
		item = new QTableWidgetItem(genericItem->itemClassName());
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		table->setItem(count, 1, item);

		// Version
		item = new QTableWidgetItem(QString::number(genericItem->version()));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		table->setItem(count, 2, item);

		// Location
		item = new QTableWidgetItem(locationName);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		item->setIcon(locationIcon);
		table->setItem(count, 3, item);

		++count;
	}
}

// Update the specified table of GenericItems, optionally filtering them by name and description
void DataManagerDialog::filterTable(QTableWidget* table, GenericItem* current, QString filter)
{
	// Loop over rows in the table
	for (int n=0; n<table->rowCount(); ++n)
	{
		QTableWidgetItem* item = table->item(n, 0);
		if (!item) continue;
		GenericItem* genericItem = VariantPointer<GenericItem>(item->data(Qt::UserRole));
		if (genericItem == current)
		{
			table->setCurrentItem(item);
		}

		// Check filtering
		if (filter.isEmpty()) table->setRowHidden(n, false);
		else
		{
			// Check name
			QString name = genericItem->name();
			bool inName = name.contains(QRegExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard));

			// Hide the item?
			bool hide = (!inName);
			table->setRowHidden(n, hide);

			// If the item was hidden, and it was selected, reset the current index
			if (hide && item->isSelected())
			{
				table->setCurrentItem(NULL);
			}
		}
	}
}


// Simulation Data
void DataManagerDialog::on_SimulationDataFilterEdit_textChanged(const QString& text)
{
	filterTable(ui_.SimulationDataTable, NULL, text);
}

// Dialog
void DataManagerDialog::on_CloseButton_clicked(bool checked)
{
	accept();
}
