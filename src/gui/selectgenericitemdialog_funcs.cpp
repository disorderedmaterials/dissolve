// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "gui/selectgenericitemdialog.h"
#include "main/dissolve.h"
#include <QRegExp>

SelectGenericItemDialog::SelectGenericItemDialog(QWidget *parent, Dissolve &dissolve) : dissolve_(dissolve)
{
    ui_.setupUi(this);

    connect(this, SIGNAL(genericItemSelectionChanged(bool)), ui_.SelectButton, SLOT(setEnabled(bool)));
}

SelectGenericItemDialog::~SelectGenericItemDialog() {}

// Update the table of GenericItems, optionally filtering them by name and description
void SelectGenericItemDialog::updateGenericItemTable(GenericItem *current, QString filter)
{
    // Loop over rows in the table
    for (int n = 0; n < ui_.ItemsTable->rowCount(); ++n)
    {
        QTableWidgetItem *item = ui_.ItemsTable->item(n, 0);
        if (!item)
            continue;
        GenericItem *genericItem = VariantPointer<GenericItem>(item->data(Qt::UserRole));
        if (genericItem == current)
        {
            ui_.ItemsTable->setCurrentItem(item);
            emit(genericItemSelectionChanged(true));
        }

        // Check filtering
        if (filter.isEmpty())
            ui_.ItemsTable->setRowHidden(n, false);
        else
        {
            // Check name
            QString name = QString::fromStdString(std::string(genericItem->name()));
            auto inName = name.contains(QRegExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard));

            // Check object tag
            QString objectTag = ui_.ItemsTable->item(n, 2)->text();
            auto inObjectTag = objectTag.contains(QRegExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard));

            // Hide the item?
            auto hide = (!inName) && (!inObjectTag);
            ui_.ItemsTable->setRowHidden(n, hide);

            // If the item was hidden, and it was selected, reset the current index
            if (hide && item->isSelected())
            {
                ui_.ItemsTable->setCurrentItem(nullptr);
                emit(genericItemSelectionChanged(false));
            }
        }
    }
}

void SelectGenericItemDialog::on_FilterEdit_textChanged(const QString &text) { updateGenericItemTable(nullptr, text); }

void SelectGenericItemDialog::on_ItemsTable_currentItemChanged(QTableWidgetItem *currentItem, QTableWidgetItem *prevItem)
{
    if (currentItem)
        emit(genericItemSelectionChanged(true));
    else
        emit(genericItemSelectionChanged(false));
}

void SelectGenericItemDialog::on_ItemsTable_itemDoubleClicked(QTableWidgetItem *w) { accept(); }

void SelectGenericItemDialog::on_SelectButton_clicked(bool checked) { accept(); }

void SelectGenericItemDialog::on_CancelButton_clicked(bool checked) { reject(); }
