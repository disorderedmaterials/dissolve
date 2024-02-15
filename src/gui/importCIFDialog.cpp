// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/importCIFDialog.h"
#include "base/units.h"
#include "classes/empiricalFormula.h"
#include "classes/pairIterator.h"
#include "gui/helpers/comboPopulator.h"
#include "procedure/nodes/box.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>

ImportCIFDialog::ImportCIFDialog(QWidget *parent, Dissolve &dissolve)
    : WizardDialog(parent), cifAssemblyModel_(cifHandler_.assemblies()), dissolve_(dissolve)
{
    ui_.setupUi(this);

    Locker refreshLock(widgetsUpdating_);

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

    // Populate density units combo
    ComboEnumOptionsPopulator(ui_.DensityUnitsCombo, Units::densityUnits());

    // Set display configuration
    ui_.StructureViewer->setConfiguration(cifHandler_.supercellConfiguration());

    createMoietyRemovalNETA(ui_.MoietyNETARemovalEdit->text().toStdString());
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
    Locker updateLock(widgetsUpdating_);

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

    // Configuration information
    auto *cfg = cifHandler_.supercellConfiguration();
    const auto *box = cfg->box();
    ui_.CurrentBoxTypeLabel->setText(QString::fromStdString(std::string(Box::boxTypes().keyword(box->type()))));
    QString boxInfo = QString("<b>A:</b>  %1 &#8491;<br>").arg(box->axisLengths().x);
    boxInfo += QString("<b>B:</b>  %1 &#8491;<br>").arg(box->axisLengths().y);
    boxInfo += QString("<b>C:</b>  %1 &#8491;<br>").arg(box->axisLengths().z);
    boxInfo += QString("<b>&#x3B1;:</b>  %1&#xb0;<br>").arg(box->axisAngles().x);
    boxInfo += QString("<b>&#x3B2;:</b>  %1&#xb0;<br>").arg(box->axisAngles().y);
    boxInfo += QString("<b>&#x3B3;:</b>  %1&#xb0;").arg(box->axisAngles().z);
    ui_.CurrentBoxFrame->setToolTip(boxInfo);
    updateDensityLabel();
    ui_.AtomPopulationLabel->setText(QString::number(cfg->nAtoms()));
    ui_.MoleculePopulationLabel->setText(QString::number(cfg->nMolecules()));

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

// Update density label
void ImportCIFDialog::updateDensityLabel()
{
    auto *cfg = cifHandler_.supercellConfiguration();
    if (!cfg)
        ui_.DensityUnitsLabel->setText("N/A");
    else
    {
        auto rho = ui_.DensityUnitsCombo->currentIndex() == 0 ? cfg->atomicDensity() : cfg->chemicalDensity();
        ui_.DensityUnitsLabel->setText(rho ? QString::number(*rho) : "--");
    }
}

void ImportCIFDialog::on_InputFileEdit_editingFinished()
{
    // Load the CIF file
    if (!cifHandler_.read(ui_.InputFileEdit->text().toStdString()))
        Messenger::error("Failed to load CIF file '{}'.\n", ui_.InputFileEdit->text().toStdString());
    else
    {
        cifHandler_.generate();
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
    if (widgetsUpdating_)
        return;

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
    cifHandler_.setPreventMetallicBonds(checked);

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
    cifHandler_.setRemoveAtomics(checked);
    update();
}

void ImportCIFDialog::on_MoietyRemoveWaterCheck_clicked(bool checked)
{
    cifHandler_.setRemoveWaterAndCoordinateOxygens(checked);
    update();
}

void ImportCIFDialog::on_MoietyRemoveByNETACheck_clicked(bool checked)
{
    if (ui_.MoietyNETARemovalIndicator->state() != CheckIndicator::OKState)
        return;

    cifHandler_.setRemoveNETA(checked, ui_.MoietyNETARemoveFragmentsCheck->isChecked());
    update();
}

void ImportCIFDialog::on_MoietyNETARemovalEdit_textEdited(const QString &text)
{
    if (!createMoietyRemovalNETA(ui_.MoietyNETARemovalEdit->text().toStdString()))
        return;

    cifHandler_.setRemoveNETA(ui_.MoietyRemoveByNETACheck->isChecked(), ui_.MoietyNETARemoveFragmentsCheck->isChecked());
    cifHandler_.setMoietyRemovalNETA(moietyNETA_.definitionString());
    update();
}

void ImportCIFDialog::on_MoietyNETARemoveFragmentsCheck_clicked(bool checked)
{
    if (ui_.MoietyNETARemovalIndicator->state() != CheckIndicator::OKState)
        return;

    cifHandler_.setRemoveNETA(ui_.MoietyRemoveByNETACheck->isChecked(), checked);
    update();
}

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

void ImportCIFDialog::on_DensityUnitsCombo_currentIndexChanged(int index)
{
    if (widgetsUpdating_)
        return;

    updateDensityLabel();
}

void ImportCIFDialog::on_OutputMolecularRadio_clicked(bool checked) { update(); }

void ImportCIFDialog::on_OutputFrameworkRadio_clicked(bool checked) { update(); }

void ImportCIFDialog::on_OutputSupermoleculeRadio_clicked(bool checked) { update(); }
