/*
	*** Select Symbol Functions
	*** src/gui/uchroma/gui/selectsymbol_funcs.cpp
	Copyright T. Youngs 2012-2015

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
#include "templates/variantpointer.h"
#include <QScrollBar>

// Static list of symbols
Symbol Symbol::symbols[] = {
	{ 0x00D7,	"mult", 	"Multiplication Sign" },
	{ 0x00F7,	"div", 		"Division Sign" },
	{ 0x212B,	"angstrom",	"Angstrom" },
	{ 0x00B0,	"degree",	"Degree" },
	{ 0x03B1,	"alpha",	"Greek Small Letter Alpha" },
	{ 0x03B2,	"beta	",	"Greek Small Letter Beta" },
	{ 0x03B3,	"gamma",	"Greek Small Letter Gamma" },
	{ 0x03B4,	"delta",	"Greek Small Letter Delta" },
	{ 0x03B5,	"epsilon",	"Greek Small Letter Epsilon" },
	{ 0x03B6,	"zeta",		"Greek Small Letter Zeta" },
	{ 0x03B7,	"eta",		"Greek Small Letter Eta" },
	{ 0x03B8,	"theta",	"Greek Small Letter Theta" },
	{ 0x03B9,	"iota",		"Greek Small Letter Iota" },
	{ 0x03BA,	"kappa",	"Greek Small Letter Kappa" },
	{ 0x03BB,	"lambda",	"Greek Small Letter Lambda" },
	{ 0x03BC,	"mu",		"Greek Small Letter Mu" },
	{ 0x03BD,	"nu",		"Greek Small Letter Nu" },
	{ 0x03BE,	"xi",		"Greek Small Letter Xi" },
	{ 0x03BF,	"omicron",	"Greek Small Letter Omicron" },
	{ 0x03C0,	"pi",		"Greek Small Letter Pi" },
	{ 0x03C1,	"rho",		"Greek Small Letter Rho" },
	{ 0x03C3,	"sigma",	"Greek Small Letter Sigma" },
	{ 0x03C4,	"tau",		"Greek Small Letter Tau" },
	{ 0x03C5,	"upsilon",	"Greek Small Letter Upsilon" },
	{ 0x03C6,	"phi",		"Greek Small Letter Phi" },
	{ 0x03C7,	"chi",		"Greek Small Letter Chi" },
	{ 0x03C8,	"psi",		"Greek Small Letter Psi" },
	{ 0x03C9,	"omega",	"Greek Small Letter Omega" },
	{ 0x0391,	"Alpha",	"Greek Capital Letter Alpha" },
	{ 0x0392,	"Beta	",	"Greek Capital Letter Beta" },
	{ 0x0393,	"Gamma",	"Greek Capital Letter Gamma" },
	{ 0x0394,	"Delta",	"Greek Capital Letter Delta" },
	{ 0x0395,	"Epsilon",	"Greek Capital Letter Epsilon" },
	{ 0x0396,	"Zeta",		"Greek Capital Letter Zeta" },
	{ 0x0397,	"Eta",		"Greek Capital Letter Eta" },
	{ 0x0398,	"Theta",	"Greek Capital Letter Theta" },
	{ 0x0399,	"Iota",		"Greek Capital Letter Iota" },
	{ 0x039A,	"Kappa",	"Greek Capital Letter Kappa" },
	{ 0x039B,	"Lambda",	"Greek Capital Letter Lambda" },
	{ 0x039C,	"Mu",		"Greek Capital Letter Mu" },
	{ 0x039D,	"Nu",		"Greek Capital Letter Nu" },
	{ 0x039E,	"Xi",		"Greek Capital Letter Xi" },
	{ 0x039F,	"Omicron",	"Greek Capital Letter Omicron" },
	{ 0x03A0,	"Pi",		"Greek Capital Letter Pi" },
	{ 0x03A1,	"Rho",		"Greek Capital Letter Rho" },
	{ 0x03A3,	"Sigma",	"Greek Capital Letter Sigma" },
	{ 0x03A4,	"Tau",		"Greek Capital Letter Tau" },
	{ 0x03A5,	"Upsilon",	"Greek Capital Letter Upsilon" },
	{ 0x03A6,	"Phi",		"Greek Capital Letter Phi" },
	{ 0x03A7,	"Chi",		"Greek Capital Letter Chi" },
	{ 0x03A8,	"Psi",		"Greek Capital Letter Psi" },
	{ 0x03A9,	"Omega",	"Greek Capital Letter Omega" }
};

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
	for (symbol = 0; symbol < Symbol::nSymbols; ++symbol) if (Symbol::symbols[symbol].description.contains(text, Qt::CaseInsensitive)) break;
	if (symbol == Symbol::nSymbols) return;

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
	Symbol* symbol = VariantPointer<Symbol>(item->data(Qt::UserRole));
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
	int nNeededRows = Symbol::nSymbols/nDisplayColumns + (Symbol::nSymbols%nDisplayColumns == 0 ? 0 : 1);

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
		for (int n=0; n<Symbol::nSymbols; ++n)
		{
			item = new QTableWidgetItem();
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			item->setText(Symbol::symbols[n].character);
			item->setTextAlignment(Qt::AlignCenter);
			item->setData(Qt::UserRole, VariantPointer<Symbol>(&Symbol::symbols[n]));
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
