// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/importforcefieldwizard.h"
#include <QFileDialog>

ImportForcefieldWizard::ImportForcefieldWizard(QWidget *parent, const Dissolve &mainDissolveInstance)
{
    ui_.setupUi(this);
    ui_.bondTable->setModel(&bonds_);
    ui_.angleTable->setModel(&angles_);
    ui_.atomTable->setModel(&atoms_);
}

void ImportForcefieldWizard::nextStack()
{
    int index = ui_.stackedWidget->currentIndex();
    int count = ui_.stackedWidget->count();
    index += 1;
    ui_.stackedWidget->setCurrentIndex(index >= count ? count - 1 : index);

    ui_.continueButton->setEnabled(index < count - 1);
    ui_.backButton->setEnabled(index > 0);
}

void ImportForcefieldWizard::prevStack()
{
    int index = ui_.stackedWidget->currentIndex();
    int count = ui_.stackedWidget->count();
    index -= 1;
    ui_.stackedWidget->setCurrentIndex(index < 0 ? 0 : index);

    ui_.continueButton->setEnabled(index < count - 1);
    ui_.backButton->setEnabled(index > 0);
}

void ImportForcefieldWizard::xmlFileDialog()
{
    auto fileName = QFileDialog::getOpenFileName(this, "Open xml file", "/home/adam", "OpenMM Xml files (*.xml)");
    if (fileName.isEmpty())
	return;

    ui_.lineEdit->setText(fileName);
    atoms_.readFile(fileName);
    bonds_.readFile(fileName);
    angles_.readFile(fileName);
}
