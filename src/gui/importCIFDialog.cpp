// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/importCIFDialog.h"
#include "classes/empiricalFormula.h"
#include "classes/pairIterator.h"
#include "neta/node.h"
#include "procedure/nodes/box.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>

ImportCIFDialog::ImportCIFDialog(QWidget *parent, Dissolve &dissolve)
    : WizardDialog(parent), cifAssemblyModel_(cifHandler_.assemblies()), dissolve_(dissolve)
{
    ui_.setupUi(this);

    // Add spacegroups to combo
    for (auto n = 1; n < SpaceGroups::nSpaceGroupIds; ++n)
        ui_.SpaceGroupsCombo->addItem(
            QString::fromStdString(std::string(SpaceGroups::formattedInformation((SpaceGroups::SpaceGroupId)n))));
    ui_.SpaceGroupsCombo->setCurrentIndex(-1);

    // Set assembly view model
    ui_.AssemblyView->setModel(&cifAssemblyModel_);
    ui_.AssemblyView->update();
    ui_.AssemblyView->expandAll();
    connect(&cifAssemblyModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)), this,
            SLOT(update()));
    connect(&cifAssemblyModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)),
            ui_.AssemblyView, SLOT(expandAll()));

    // Set display configuration
    ui_.StructureViewer->setConfiguration(cifHandler_.supercellConfiguration());
}

// Perform any final actions before the wizard is closed
void ImportCIFDialog::finalise()
{
    Flags<CIFHandler::OutputFlags> outputFlags;
    if (ui_.OutputMolecularRadio->isChecked())
        outputFlags.setFlag(CIFHandler::OutputFlags::OutputMolecularSpecies);
    else if (ui_.OutputFrameworkRadio->isChecked())
        outputFlags.setFlag(CIFHandler::OutputFlags::OutputFramework);
    else if (ui_.OutputSupermoleculeRadio->isChecked())
        outputFlags.setFlag(CIFHandler::OutputFlags::OutputSupermolecule);

    // Output a configuration as well as the species for certain options
    if (outputFlags.isSet(CIFHandler::OutputFlags::OutputMolecularSpecies) ||
        outputFlags.isSet(CIFHandler::OutputFlags::OutputFramework))
    {
        outputFlags.setFlag(CIFHandler::OutputFlags::OutputConfiguration);
    }

    cifHandler_.finalise(dissolve_.coreData(), outputFlags);
}

/*
 * UI
 */

// Update all controls
void ImportCIFDialog::update()
{
    // DATA_ ID
    ui_.InfoDataLabel->setText(QString::fromStdString(cifHandler_.getTagString("DATA_").value_or("<Unknown>")));

    // Chemical Formula
    ui_.InfoChemicalFormulaLabel->setText(QString::fromStdString(cifHandler_.chemicalFormula()));

    // Publication Data
    ui_.InfoPublicationTitleLabel->setText(
        QString::fromStdString(cifHandler_.getTagString("_publ_section_title").value_or("<Unknown>")));
    ui_.InfoPublicationReferenceLabel->setText(QString::fromStdString(fmt::format(
        "{}, {}, <b>{}</b>, {}", cifHandler_.getTagString("_journal_name_full").value_or("???"),
        cifHandler_.getTagString("_journal_year").value_or("???"), cifHandler_.getTagString("_journal_volume").value_or("???"),
        cifHandler_.getTagString("_journal_page_first").value_or("???"))));

    // Spacegroup
    ui_.SpaceGroupsCombo->setCurrentIndex(cifHandler_.spaceGroup() - 1);

    // Bonding
    ui_.BondFromCIFRadio->setEnabled(cifHandler_.hasBondDistances());

    // Assemblies
    ui_.AssemblyView->expandAll();

    // Supercell information -- TODO should become part of config viewer top bar
    auto *supercellConfiguration = cifHandler_.supercellConfiguration();
    ui_.SupercellBoxALabel->setText(QString::number(supercellConfiguration->box()->axisLengths().x) + " &#8491;");
    ui_.SupercellBoxBLabel->setText(QString::number(supercellConfiguration->box()->axisLengths().y) + " &#8491;");
    ui_.SupercellBoxCLabel->setText(QString::number(supercellConfiguration->box()->axisLengths().z) + " &#8491;");
    ui_.SupercellBoxAlphaLabel->setText(QString::number(supercellConfiguration->box()->axisAngles().x) + "&deg;");
    ui_.SupercellBoxBetaLabel->setText(QString::number(supercellConfiguration->box()->axisAngles().y) + "&deg;");
    ui_.SupercellBoxGammaLabel->setText(QString::number(supercellConfiguration->box()->axisAngles().z) + "&deg;");
    auto chemicalDensity = cifHandler_.supercellConfiguration()->chemicalDensity();
    ui_.SupercellDensityLabel->setText(chemicalDensity ? QString::number(*chemicalDensity) + " g cm<sup>3</sup>"
                                                       : "-- g cm<sup>3</sup>");
    ui_.SupercellVolumeLabel->setText(QString::number(supercellConfiguration->box()->volume()) + " &#8491;<sup>3</sup>");
    ui_.SupercellNAtomsLabel->setText(QString::number(supercellConfiguration->nAtoms()));

    // Output
    auto validSpecies = !cifHandler_.molecularSpecies().empty();
    ui_.OutputMolecularRadio->setEnabled(validSpecies);
    ui_.OutputFrameworkRadio->setEnabled(!validSpecies);
    ui_.OutputSupermoleculeRadio->setEnabled(!validSpecies);

    // Update molecular species list
    ui_.OutputMolecularSpeciesList->clear();
    for (auto &molecularSp : cifHandler_.molecularSpecies())
    {
        ui_.OutputMolecularSpeciesList->addItem(QString::fromStdString(std::string(molecularSp.species()->name())));
    }

    // Structure
    ui_.StructureViewer->postRedisplay();
}

void ImportCIFDialog::on_InputFileEdit_editingFinished()
{
    // Load the CIF file
    if (!cifHandler_.read(ui_.InputFileEdit->text().toStdString()))
        Messenger::error("Failed to load CIF file '{}'.\n", ui_.InputFileEdit->text().toStdString());
    else
    {
        cifHandler_.generate(updateFlags_);
        update();
    }
}

void ImportCIFDialog::on_InputFileSelectButton_clicked(bool checked)
{
    QString inputFile = QFileDialog::getOpenFileName(this, "Choose CIF file to open", QDir().absolutePath(),
                                                     "CIF files (*.cif);;All files (*)");
    if (inputFile.isEmpty())
        return;

    // Set input file in edit
    ui_.InputFileEdit->setText(inputFile);
    on_InputFileEdit_editingFinished();
}

void ImportCIFDialog::on_SpaceGroupsCombo_currentIndexChanged(int index)
{
    cifHandler_.setSpaceGroup((SpaceGroups::SpaceGroupId)(ui_.SpaceGroupsCombo->currentIndex() + 1));

    update();
}

void ImportCIFDialog::on_NormalOverlapToleranceRadio_clicked(bool checked)
{
    if (checked)
    {
        cifHandler_.setOverlapTolerance(0.1);
        update();
    }
}

void ImportCIFDialog::on_LooseOverlapToleranceRadio_clicked(bool checked)
{
    if (checked)
    {
        cifHandler_.setOverlapTolerance(0.5);
        update();
    }
}

void ImportCIFDialog::on_CalculateBondingRadio_clicked(bool checked)
{
    if (checked)
    {
        cifHandler_.setUseCIFBondingDefinitions(false);
        update();
    }
}

void ImportCIFDialog::on_BondFromCIFRadio_clicked(bool checked)
{
    if (checked)
    {
        cifHandler_.setUseCIFBondingDefinitions(true);
        update();
    }

    update();
}

void ImportCIFDialog::on_BondingPreventMetallicCheck_clicked(bool checked)
{
    if (checked)
        updateFlags_.setFlag(CIFHandler::PreventMetallicBonding);
    else
        updateFlags_.removeFlag(CIFHandler::PreventMetallicBonding);

    update();
}

// Create / check NETA definition for moiety removal
bool ImportCIFDialog::createMoietyRemovalNETA(std::string definition)
{
    auto result = moietyNETA_.create(definition);

    ui_.MoietyNETARemovalIndicator->setOK(result);

    return result;
}

void ImportCIFDialog::on_MoietyRemoveAtomicsCheck_clicked(bool checked)
{
    if (checked)
        updateFlags_.setFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveAtomics);
    else
        updateFlags_.removeFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveAtomics);

    update();
}

void ImportCIFDialog::on_MoietyRemoveWaterCheck_clicked(bool checked)
{
    if (checked)
        updateFlags_.setFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveWater);
    else
        updateFlags_.removeFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveWater);

    update();
}

void ImportCIFDialog::on_MoietyRemoveByNETAGroup_clicked(bool checked)
{
    if (checked)
        updateFlags_.setFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveNETA);
    else
        updateFlags_.removeFlag(CIFHandler::UpdateFlags::CleanMoietyRemoveNETA);

    cifHandler_.setMoietyRemovalNETA(moietyNETA_.definitionString());
    update();
}

void ImportCIFDialog::on_MoietyNETARemovalEdit_textEdited(const QString &text)
{
    createMoietyRemovalNETA(text.toStdString());
    if (moietyNETA_.isValid())
        update();
}

void ImportCIFDialog::on_MoietyNETARemoveFragmentsCheck_clicked(bool checked)
{
    if (checked)
        updateFlags_.setFlag(CIFHandler::UpdateFlags::CleanRemoveBoundFragments);
    else
        updateFlags_.removeFlag(CIFHandler::UpdateFlags::CleanRemoveBoundFragments);

    if (moietyNETA_.isValid())
        update();
}

/*
 * Output Page
 */

void ImportCIFDialog::on_RepeatASpin_valueChanged(int value)
{

    cifHandler_.setSupercellRepeat({ui_.RepeatASpin->value(), ui_.RepeatBSpin->value(), ui_.RepeatCSpin->value()});
    update();
}

void ImportCIFDialog::on_RepeatBSpin_valueChanged(int value)
{

    cifHandler_.setSupercellRepeat({ui_.RepeatASpin->value(), ui_.RepeatBSpin->value(), ui_.RepeatCSpin->value()});
    update();
}

void ImportCIFDialog::on_RepeatCSpin_valueChanged(int value)
{

    cifHandler_.setSupercellRepeat({ui_.RepeatASpin->value(), ui_.RepeatBSpin->value(), ui_.RepeatCSpin->value()});
    update();
}

void ImportCIFDialog::on_OutputMolecularRadio_clicked(bool checked) { update(); }

void ImportCIFDialog::on_OutputFrameworkRadio_clicked(bool checked) { update(); }

void ImportCIFDialog::on_OutputSupermoleculeRadio_clicked(bool checked) { update(); }
