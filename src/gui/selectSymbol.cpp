// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/selectSymbol.h"
#include "gui/render/symbol.h"
#include <QScrollBar>

SelectSymbolDialog::SelectSymbolDialog(QWidget *parent) : QDialog(parent), itemSize_(32)
{
    ui.setupUi(this);

    selectedSymbol_ = QChar('\0');

    ui.SelectButton->setEnabled(false);
}

SelectSymbolDialog::~SelectSymbolDialog() {}

/*
 * Symbol
 */

// Return selected symbol
QChar SelectSymbolDialog::selectedSymbol() { return selectedSymbol_; }

/*
 * Reimplementations
 */

void SelectSymbolDialog::resizeEvent(QResizeEvent *event) { updateTable(); }

/*
 * Slots
 */

void SelectSymbolDialog::on_SearchEdit_textChanged(QString text)
{
    if (text.isEmpty())
        return;

    // See if a symbol description matches our search string
    auto symbol = SymbolData::firstDescriptionMatch(qPrintable(text));
    if (symbol == SymbolData::nSymbols)
        return;

    // Found a match, so highlight the relevant item in the table
    ui.SymbolTable->clearSelection();
    auto nColumns = ui.SymbolTable->columnCount();
    auto row = symbol / nColumns, column = symbol % nColumns;
    QTableWidgetItem *item = ui.SymbolTable->item(row, column);
    if (item)
        item->setSelected(true);
}

void SelectSymbolDialog::on_ClearSearchButton_clicked(bool checked) { ui.SearchEdit->clear(); }

void SelectSymbolDialog::on_SymbolTable_itemSelectionChanged()
{
    if (ui.SymbolTable->selectedItems().count() == 0)
    {
        ui.SelectButton->setEnabled(false);
        return;
    }

    QTableWidgetItem *item = ui.SymbolTable->selectedItems().at(0);
    if (!item)
        return;

    // Copy the text of the widget item to the symbol label
    ui.SymbolLabel->setText(item->text());

    ui.SelectButton->setEnabled(true);
}

void SelectSymbolDialog::on_SymbolTable_itemDoubleClicked(QTableWidgetItem *item)
{
    if (!item)
        return;

    // Get the character of the selected item
    selectedSymbol_ = item->text().at(0);

    accept();
}

void SelectSymbolDialog::on_CancelButton_clicked(bool checked) { reject(); }

void SelectSymbolDialog::on_SelectButton_clicked(bool checked) { accept(); }

/*
 * Update
 */

// Update symbol table
void SelectSymbolDialog::updateTable(bool force)
{
    static int oldNColumns = -1;

    // Recalculate the number of columns it is possible to display in the widget
    // If this is the same as the previous number, then we only need to adjust the widths (possibly)
    QScrollBar *scrollBar = ui.SymbolTable->verticalScrollBar();
    auto scrollBarWidth = (scrollBar ? scrollBar->width() : 0);
    auto nDisplayColumns = (ui.SymbolTable->width() - scrollBarWidth) / itemSize_;
    auto widthRemainder = (ui.SymbolTable->width() - scrollBarWidth) - nDisplayColumns * itemSize_;
    auto nNeededRows = SymbolData::nSymbols / nDisplayColumns + (SymbolData::nSymbols % nDisplayColumns == 0 ? 0 : 1);

    if ((nDisplayColumns != oldNColumns) || force)
    {
        // Repopulate table
        ui.SymbolTable->clear();

        // Set row and column counts
        ui.SymbolTable->setColumnCount(nDisplayColumns);
        ui.SymbolTable->setRowCount(nNeededRows);

        // Populate the symbols list
        QTableWidgetItem *item;
        QSize itemSize(itemSize_, itemSize_);
        for (auto n = 0; n < SymbolData::nSymbols; ++n)
        {
            item = new QTableWidgetItem();
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setText(QString::fromStdString(std::string(SymbolData::symbol((SymbolData::Symbol)n))));
            item->setTextAlignment(Qt::AlignCenter);
            item->setData(Qt::UserRole, QVariant::fromValue(n));
            item->setSizeHint(itemSize);
            ui.SymbolTable->setItem(n / nDisplayColumns, n % nDisplayColumns, item);
        }
    }

    // Flag the last column to absorb any extra space
    QHeaderView *header = ui.SymbolTable->horizontalHeader();
    if (header)
        header->setStretchLastSection(true);

    // Set sizes of all columns except the last
    for (auto n = 0; n < nDisplayColumns; ++n)
        ui.SymbolTable->setColumnWidth(n, itemSize_ + widthRemainder / nDisplayColumns);

    oldNColumns = nDisplayColumns;
}
