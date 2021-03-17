// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/selectsystemtemplatedialog.h"
#include "gui/systemtemplate.h"
#include "templates/variantpointer.h"
#include <QRegExp>

SelectSystemTemplateDialog::SelectSystemTemplateDialog(QWidget *parent, const List<SystemTemplate> &systemTemplates)
    : systemTemplates_(systemTemplates)
{
    ui_.setupUi(this);

    // Populate the list with available templates
    ListIterator<SystemTemplate> templateIterator(systemTemplates_);
    while (SystemTemplate *sysTemp = templateIterator.iterate())
    {
        QListWidgetItem *item = new QListWidgetItem(QPixmap(sysTemp->iconResource()), sysTemp->name(), ui_.TemplatesList);
        item->setData(Qt::UserRole, VariantPointer<SystemTemplate>(sysTemp));
    }
}

SelectSystemTemplateDialog::~SelectSystemTemplateDialog() {}

// Update the list of system templates, optionally filtering them by name and description
void SelectSystemTemplateDialog::updateTemplatesList(QString filter)
{
    // Loop over items in the list
    for (auto n = 0; n < ui_.TemplatesList->count(); ++n)
    {
        QListWidgetItem *item = ui_.TemplatesList->item(n);
        if (!item)
            continue;
        SystemTemplate *sysTemp = VariantPointer<SystemTemplate>(item->data(Qt::UserRole));

        // Check filtering
        if (filter.isEmpty())
            item->setHidden(false);
        else
        {
            // Check name
            auto inName = sysTemp->name().contains(QRegExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard));

            // Check description
            auto inDescription = sysTemp->description().contains(QRegExp(filter, Qt::CaseInsensitive, QRegExp::Wildcard));

            // Hide the item?
            auto hide = (!inName) && (!inDescription);
            item->setHidden(hide);

            // If the item was hidden, and it was selected, reset the current index
            if (hide && item->isSelected())
                ui_.TemplatesList->setCurrentRow(-1);
        }
    }
}

void SelectSystemTemplateDialog::on_FilterEdit_textChanged(const QString &text) { updateTemplatesList(text); }

void SelectSystemTemplateDialog::on_TemplatesList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
    {
        ui_.TemplateDetailsTextEdit->clear();
        ui_.SelectButton->setEnabled(false);
        return;
    }

    // Get the selected template
    SystemTemplate *sysTemp = VariantPointer<SystemTemplate>(current->data(Qt::UserRole));

    // Update the informational text
    ui_.TemplateDetailsTextEdit->setText(sysTemp->description());

    ui_.SelectButton->setEnabled(true);
}

void SelectSystemTemplateDialog::on_TemplatesList_itemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;

    accept();
}

void SelectSystemTemplateDialog::on_SelectButton_clicked(bool checked) { accept(); }

void SelectSystemTemplateDialog::on_CancelButton_clicked(bool checked) { reject(); }

// Run the dialog, returning the selected SystemTemplate
SystemTemplate *SelectSystemTemplateDialog::selectTemplate()
{
    show();

    if (exec() == QDialog::Accepted)
    {
        QListWidgetItem *item = ui_.TemplatesList->currentItem();
        if (item == nullptr)
            return nullptr;
        return VariantPointer<SystemTemplate>(item->data(Qt::UserRole));
    }
    else
        return nullptr;
}
