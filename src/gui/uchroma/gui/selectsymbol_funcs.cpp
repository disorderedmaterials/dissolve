/*
	*** Select Symbol Functions
	*** src/gui/uchroma/gui/selectsymbol_funcs.cpp
	Copyright T. Youngs 2012-2018

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/gui/selectsymbol.h"
#include "gui/uchroma/classes/symbol.h"
#include "templates/variantpointer.h"
#include <QScrollBar>

// Constructor
SelectSymbolDialog::SelectSymbolDialog(QWidget* parent) : QDialog(parent), itemSize_(32)
{
	ui.setupUi(this);

	selectedSymbol_ = QChar('\0');

	ui.SelectButton->setEnabled(false);
}

// Destructor
SelectSymbolDialog::~SelectSymbolDialog()
{
}

/*
 * Symbol
 */

// Return selected symbol
QChar SelectSymbolDialog::selectedSymbol()
{
	return selectedSymbol_;
}

/*
 * Reimplementations
 */

void SelectSymbolDialog::resizeEvent(QResizeEvent* event)
{
	updateTable();
}

/*
 * Slots
 */

void SelectSymbolDialog::on_SearchEdit_textChanged(QString text)
{
	if (text.isEmpty()) return;

	// See if a symbol description matches our search string
	int symbol;
	for (symbol = 0; symbol < SymbolData::nSymbols; ++symbol) if (SymbolData::symbols[symbol].description.contains(text, Qt::CaseInsensitive)) break;
	if (symbol == SymbolData::nSymbols) return;

	// Found a match, so highlight the relevant item in the table
	ui.SymbolTable->clearSelection();
	int nColumns = ui.SymbolTable->columnCount();
	int row = symbol / nColumns, column = symbol%nColumns;
	QTableWidgetItem* item = ui.SymbolTable->item(row, column);
	if (item) item->setSelected(true);
}

void SelectSymbolDialog::on_ClearSearchButton_clicked(bool checked)
{
	ui.SearchEdit->clear();
}

void SelectSymbolDialog::on_SymbolTable_itemSelectionChanged()
{
	if (ui.SymbolTable->selectedItems().count() == 0)
	{
		ui.SelectButton->setEnabled(false);
		return;
	}

	QTableWidgetItem* item = ui.SymbolTable->selectedItems().at(0);
	if (!item) return;

	// Get the Symbol pointer from the selected item
	SymbolData* symbol = VariantPointer<SymbolData>(item->data(Qt::UserRole));
	if (symbol)
	{
		selectedSymbol_ = symbol->character;
		QString hexCode = QString("0x%1").arg(symbol->character.unicode(), 4, 16, QChar('0'));
		ui.SymbolLabel->setText(hexCode + " : " + symbol->description);
	}

	ui.SelectButton->setEnabled(true);
}

void SelectSymbolDialog::on_SymbolTable_itemDoubleClicked(QTableWidgetItem* item)
{
	if (!item) return;

	// Get the character of the selected item
	selectedSymbol_ = item->text().at(0);

	accept();
}

void SelectSymbolDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

void SelectSymbolDialog::on_SelectButton_clicked(bool checked)
{
	accept();
}

/*
 * Update
 */

// Update symbol table
void SelectSymbolDialog::updateTable(bool force)
{
	static int oldNColumns = -1;

	// Recalculate the number of columns it is possible to display in the widget
	// If this is the same as the previous number, then we only need to adjust the widths (possibly)
	QScrollBar* scrollBar = ui.SymbolTable->verticalScrollBar();
	int scrollBarWidth = (scrollBar ? scrollBar->width() : 0);
	int nDisplayColumns = (ui.SymbolTable->width() - scrollBarWidth) / itemSize_;
	int widthRemainder = (ui.SymbolTable->width() - scrollBarWidth) - nDisplayColumns * itemSize_;
	int nNeededRows = SymbolData::nSymbols/nDisplayColumns + (SymbolData::nSymbols%nDisplayColumns == 0 ? 0 : 1);

	if ((nDisplayColumns != oldNColumns) || force)
	{
		// Repopulate table
		ui.SymbolTable->clear();

		// Set row and column counts
		ui.SymbolTable->setColumnCount(nDisplayColumns);
		ui.SymbolTable->setRowCount(nNeededRows);

		// Populate the symbols list
		QTableWidgetItem* item;
		QSize itemSize(itemSize_, itemSize_);
		for (int n=0; n<SymbolData::nSymbols; ++n)
		{
			item = new QTableWidgetItem();
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			item->setText(SymbolData::symbols[n].character);
			item->setTextAlignment(Qt::AlignCenter);
			item->setData(Qt::UserRole, VariantPointer<SymbolData>(&SymbolData::symbols[n]));
			item->setSizeHint(itemSize);
			ui.SymbolTable->setItem(n/nDisplayColumns, n%nDisplayColumns, item);
		}
	}

	// Flag the last column to absorb any extra space
	QHeaderView *header = ui.SymbolTable->horizontalHeader();
	if (header) header->setStretchLastSection(true);

	// Set sizes of all columns except the last
	for (int n=0; n<nDisplayColumns; ++n) ui.SymbolTable->setColumnWidth(n, itemSize_ + widthRemainder/nDisplayColumns);

	oldNColumns = nDisplayColumns;
}
