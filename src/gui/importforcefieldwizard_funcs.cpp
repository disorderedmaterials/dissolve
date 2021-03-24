// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/library.h"
#include "data/ff/xml/base.h"
#include "gui/importforcefieldwizard.h"
#include <QFileDialog>
#include <pugixml.hpp>

ImportForcefieldWizard::ImportForcefieldWizard(QWidget *parent, Dissolve &mainDissolveInstance) : ff_(mainDissolveInstance)
{
    ui_.setupUi(this);
    ui_.xmlTree->setModel(&ff_);

    updateNavButtons();
}

void ImportForcefieldWizard::updateNavButtons()
{
    int index = ui_.stackedWidget->currentIndex();
    int count = ui_.stackedWidget->count();

    ui_.backButton->setEnabled(index > 0);

    switch (index)
    {
        case 0:
            ui_.continueButton->setEnabled(true);
            break;
        case 1:
            ui_.continueButton->setEnabled(true);
            break;
        default:
            ui_.continueButton->setEnabled(false);
            break;
    }
}

void ImportForcefieldWizard::nextStack()
{
    int index = ui_.stackedWidget->currentIndex();
    int count = ui_.stackedWidget->count();
    index += 1;
    ui_.stackedWidget->setCurrentIndex(index >= count ? count - 1 : index);

    updateNavButtons();

    if (index == 2)
    {
        // auto map = atoms_.toMap();
        // auto xmlFF = std::make_shared<Forcefield_XML>(atoms_.toVector(), bonds_.toVector(map), angles_.toVector(map),
        //                                               torsions_.toVector(map), impropers_.toVector(map));
        // ForcefieldLibrary::registerForcefield(std::static_pointer_cast<Forcefield>(xmlFF));
        this->accept();
    }
}

void ImportForcefieldWizard::prevStack()
{
    int index = ui_.stackedWidget->currentIndex();
    index -= 1;
    ui_.stackedWidget->setCurrentIndex(index < 0 ? 0 : index);

    updateNavButtons();
}

void ImportForcefieldWizard::xmlFileDialog()
{
    auto fileName = QFileDialog::getOpenFileName(this, "Open xml file", "/home/adam", "OpenMM Xml files (*.xml)");
    if (fileName.isEmpty())
        return;

    ui_.lineEdit->setText(fileName);
}

void ImportForcefieldWizard::xmlString(QString fileName)
{
    pugi::xml_document doc;

    auto result = doc.load_file(fileName.toStdString().c_str());
    if (result)
    {
        auto root = doc.root();

        // atoms_.readFile(root);
        // bonds_.readFile(root);
        // angles_.readFile(root);
        // torsions_.readFile(root);
        // impropers_.readFile(root);
        // ui_.atomTable->resizeColumnsToContents();
        // ui_.bondTable->resizeColumnsToContents();
        // ui_.angleTable->resizeColumnsToContents();
        // ui_.torsionTable->resizeColumnsToContents();
        // ui_.improperTable->resizeColumnsToContents();
    }
    updateNavButtons();
    return;
}
