// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/ui_selectgenericitemdialog.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"
#include <QDialog>

// Forward Declarations
class GenericItem;
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
    // Append GenericItems to table under specified source
    template <class T> void addToTable(RefList<T> &items, QString source)
    {
        QTableWidgetItem *item;
        int count = ui_.ItemsTable->rowCount();
        ui_.ItemsTable->setRowCount(count + items.nItems());
        for (T *templatedItem : items)
        {
            // Item name
            item = new QTableWidgetItem(QString::fromStdString(std::string(templatedItem->name())));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            item->setData(Qt::UserRole, VariantPointer<T>(templatedItem));
            ui_.ItemsTable->setItem(count, 0, item);

            // Item source
            item = new QTableWidgetItem(source);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ui_.ItemsTable->setItem(count, 1, item);

            // Object tag
            item = new QTableWidgetItem(QString::fromStdString(std::string(templatedItem->objectTag())));
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ui_.ItemsTable->setItem(count, 2, item);

            ++count;
        }
    }

    // Update the table of GenericItems, optionally filtering them by name and description
    void updateGenericItemTable(GenericItem *current, QString filter);

    private slots:
    void on_FilterEdit_textChanged(const QString &text);
    void on_ItemsTable_currentItemChanged(QTableWidgetItem *currentItem, QTableWidgetItem *prevItem);
    void on_ItemsTable_itemDoubleClicked(QTableWidgetItem *w);
    void on_SelectButton_clicked(bool checked);
    void on_CancelButton_clicked(bool checked);

    signals:
    void genericItemSelectionChanged(bool isValid);

    public:
    // Run the dialog, returning the selected GenericItem
    template <class T> T *selectGenericItem(T *currentItem = nullptr)
    {
        // Populate the table with available items of the specified class type
        RefList<T> items;

        // -- Processing Module Data
        items = dissolve_.processingModuleData().items<T>();
        addToTable<T>(items, "Processing");

        show();

        if (exec() == QDialog::Accepted)
        {
            // Get item in first column on the current row
            int row = ui_.ItemsTable->currentRow();
            if (row == -1)
                return nullptr;
            QTableWidgetItem *item = ui_.ItemsTable->item(row, 0);

            // Retrieve the data pointer
            T *dataItem = VariantPointer<T>(item->data(Qt::UserRole));

            return dataItem;
        }
        else
            return nullptr;
    }
};
