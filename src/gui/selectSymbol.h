// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/ui_selectSymbol.h"
#include <QDialog>
#include <QObject>

// Select Symbol Dialog
class SelectSymbolDialog : public QDialog
{
    Q_OBJECT

    public:
    SelectSymbolDialog(QWidget *parent);
    ~SelectSymbolDialog();
    // Main form declaration
    Ui::SelectSymbolDialog ui;

    /*
     * Symbol Table
     */
    private:
    // Desired minimum size of item in table
    const int itemSize_;
    // Selected equation
    QChar selectedSymbol_;

    public:
    // Return selected character
    QChar selectedSymbol();

    /*
     * Reimplementations
     */
    protected:
    void resizeEvent(QResizeEvent *event);

    /*
     * Slots
     */
    public Q_SLOTS:
    void on_SearchEdit_textChanged(QString text);
    void on_ClearSearchButton_clicked(bool checked);
    void on_SymbolTable_itemSelectionChanged();
    void on_SymbolTable_itemDoubleClicked(QTableWidgetItem *item);
    void on_CancelButton_clicked(bool checked);
    void on_SelectButton_clicked(bool checked);

    /*
     * Update
     */
    public:
    // Update symbol table
    void updateTable(bool force = false);
};
