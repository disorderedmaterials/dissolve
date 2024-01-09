// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "gui/getTabNameDialog.h"
#include "gui/mainTab.h"
#include <QPointer>

GetTabNameDialog::GetTabNameDialog(QWidget *parent, const std::vector<MainTab *> &currentTabs) : currentTabs_(currentTabs)
{
    ui_.setupUi(this);
}

GetTabNameDialog::~GetTabNameDialog() {}

// Run the dialog
bool GetTabNameDialog::get(const MainTab *currentTab, const QString &currentName)
{
    currentTab_ = currentTab;

    // Set the current text
    ui_.NameEdit->setText(currentName);

    show();

    return (exec() == QDialog::Accepted);
}

// Return the name string
QString GetTabNameDialog::newName() const { return ui_.NameEdit->text(); }

/*
 * Slots
 */

// Tab name edited
void GetTabNameDialog::on_NameEdit_textChanged(const QString text)
{
    // Make sure the name is valid
    auto nameValid = true;
    if (text.isEmpty())
        nameValid = false;
    else
    {
        for (const auto &tab : currentTabs_)
        {
            if (currentTab_ == tab)
                continue;

            if (tab->title() == text)
            {
                nameValid = false;
                break;
            }
        }
    }

    // Update indicator
    ui_.NameIndicator->setOK(nameValid);

    // Update dialog buttons
    ui_.OKButton->setEnabled(nameValid);
}

void GetTabNameDialog::on_CancelButton_clicked(bool checked) { reject(); }

void GetTabNameDialog::on_OKButton_clicked(bool checked) { accept(); }
