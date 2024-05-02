// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/ui_selectGenericItemDialog.h"
#include "main/dissolve.h"
#include <QDialog>

// Forward Declarations
class QTableWidgetItem;
class SelectGenericItemWidget;

// Select GenericItem Dialog
class SelectGenericItemDialog : public QDialog
{
    Q_OBJECT

    public:
    SelectGenericItemDialog(QWidget *parent, Dissolve &dissolve);
    ~SelectGenericItemDialog();

    private:
    // Main form declaration
    Ui::SelectGenericItemDialog ui_;
    // Reference to Dissolve
    Dissolve &dissolve_;

    private:
    // Update and (optionally) filter the list
    void filterItems(QString filterText);

    private Q_SLOTS:
    void on_FilterEdit_textChanged(const QString &text);
    void on_ItemsList_currentItemChanged(QTableWidgetItem *currentItem, QTableWidgetItem *prevItem);
    void on_ItemsList_itemDoubleClicked(QTableWidgetItem *w);
    void on_SelectButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);

    Q_SIGNALS:
    void genericItemSelectionChanged(bool isValid);

    public:
    // Run the dialog, returning the name of the selected GenericItem
    template <class T> QString selectGenericItem(QString currentItem = "")
    {
        // Populate the table with available items of the specified class type
        auto items = dissolve_.processingModuleData().all<T>();

        ui_.ItemsList->clear();
        for (auto &name : items)
            ui_.ItemsList->addItem(QString::fromStdString(std::string(name)));

        show();

        if (exec() == QDialog::Accepted && ui_.ItemsList->currentItem())
            return ui_.ItemsList->currentItem()->text();
        else
            return "";
    }
};
