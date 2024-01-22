// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/units.h"
#include "classes/configuration.h"
#include "gui/addConfigurationDialog.h"
#include "gui/helpers/comboPopulator.h"
#include "procedure/nodes/add.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/coordinateSets.h"
#include "procedure/nodes/generalRegion.h"
#include "procedure/nodes/parameters.h"
#include "procedure/nodes/temperature.h"
#include <QInputDialog>
#include <QMessageBox>

AddConfigurationDialog::AddConfigurationDialog(QWidget *parent, Dissolve &dissolve)
    : WizardDialog(parent), addSpeciesInfoModel_(mixSpecies_), dissolve_(dissolve)
{
    ui_.setupUi(this);

    // Set up widgets
    ui_.TargetSpeciesWidget->setSpecies(dissolve_.coreData().species());
    ui_.TargetSpeciesWidget->reset(1, {});

    ui_.BoxASpin->setMinimum(0.0);
    ui_.BoxBSpin->setMinimum(0.0);
    ui_.BoxCSpin->setMinimum(0.0);
    ui_.BoxAlphaSpin->setMinimum(0.0);
    ui_.BoxBetaSpin->setMinimum(0.0);
    ui_.BoxGammaSpin->setMinimum(0.0);
    connect(ui_.BoxASpin, SIGNAL(valueChanged(double)), this, SLOT(boxGeometryParameterChanged(double)));
    connect(ui_.BoxBSpin, SIGNAL(valueChanged(double)), this, SLOT(boxGeometryParameterChanged(double)));
    connect(ui_.BoxCSpin, SIGNAL(valueChanged(double)), this, SLOT(boxGeometryParameterChanged(double)));
    connect(ui_.BoxAlphaSpin, SIGNAL(valueChanged(double)), this, SLOT(boxGeometryParameterChanged(double)));
    connect(ui_.BoxBetaSpin, SIGNAL(valueChanged(double)), this, SLOT(boxGeometryParameterChanged(double)));
    connect(ui_.BoxGammaSpin, SIGNAL(valueChanged(double)), this, SLOT(boxGeometryParameterChanged(double)));

    ui_.AddSpeciesInfoTable->setModel(&addSpeciesInfoModel_);
    connect(&addSpeciesInfoModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)), this,
            SLOT(speciesInfoDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)));
    ComboEnumOptionsPopulator(ui_.SpeciesDensityUnitsCombo, Units::densityUnits());

    // Register pages with the wizard
    registerPage(AddConfigurationDialog::TargetSpeciesPage, "Target Species", AddConfigurationDialog::ConfigurationTypePage);
    registerPage(AddConfigurationDialog::ConfigurationTypePage, "Configuration Type", AddConfigurationDialog::BoxGeometryPage);
    registerPage(AddConfigurationDialog::BoxGeometryPage, "Box Geometry", AddConfigurationDialog::SpeciesInfoPage);
    registerPage(AddConfigurationDialog::SpeciesInfoPage, "Species Information");

    initialise(this, ui_.MainStack, AddConfigurationDialog::TargetSpeciesPage);
}

/*
 * Data
 */

// Determine default species info data
void AddConfigurationDialog::setDefaultSpeciesInfoData()
{
    if (mixSpecies_.empty())
        return;

    // Reset all to defaults
    for (auto &spInfo : mixSpecies_)
        spInfo.reset();

    // Get the total number of species atoms
    auto nTotalSpeciesAtoms = std::accumulate(mixSpecies_.begin(), mixSpecies_.end(), 0,
                                              [](auto acc, const auto &b) { return acc + b.species()->nAtoms(); });

    // Set a sensible initial multiplier
    ui_.SpeciesMultiplierSpin->setValue(DissolveMath::power(10, std::max(0, 3 - int(log10(nTotalSpeciesAtoms)))));
}

/*
 * Wizard
 */

// Return whether progression to the next page from the current page is allowed
bool AddConfigurationDialog::progressionAllowed(int index) const
{
    if (index == AddConfigurationDialog::TargetSpeciesPage)
    {
        // Must have at least one species, and not more than one periodic species
        auto selection = ui_.TargetSpeciesWidget->selection();
        if (selection.size() == 0 ||
            std::count_if(selection.begin(), selection.end(),
                          [](const auto *sp) { return sp->box()->type() != Box::BoxType::NonPeriodic; }) > 1)
            return false;
    }
    else if (index == AddConfigurationDialog::BoxGeometryPage)
        return ui_.DetectedBoxTypeLabel->text() != "???";

    return true;
}

// Perform any necessary actions before moving to the next page
bool AddConfigurationDialog::prepareForNextPage(int currentIndex)
{
    switch (currentIndex)
    {
        case (AddConfigurationDialog::TargetSpeciesPage):
            // Set species
            frameworkSpecies_ = nullptr;
            mixSpecies_.clear();
            addSpeciesInfoModel_.reset();
            for (auto *sp : ui_.TargetSpeciesWidget->selection())
            {
                if (sp->box()->type() != Box::BoxType::NonPeriodic)
                    frameworkSpecies_ = sp;
                else
                    mixSpecies_.emplace_back(sp);
            }
            setDefaultSpeciesInfoData();
            addSpeciesInfoModel_.reset();

            // Enable / disable configuration types based on species choice
            ui_.ConfigurationTypeFrameworkRadio->setEnabled(frameworkSpecies_);
            ui_.ConfigurationTypeMixtureRadio->setEnabled(!frameworkSpecies_);
            if (frameworkSpecies_)
                ui_.ConfigurationTypeFrameworkRadio->setChecked(true);
            else
                ui_.ConfigurationTypeMixtureRadio->setChecked(true);

            // Box geometry is dependent on presence of framework species
            if (frameworkSpecies_)
            {
                ui_.BoxGeometryFixedSizeRadio->setChecked(true);
                auto box = frameworkSpecies_->box();
                ui_.BoxASpin->setValue(box->axisLength(0));
                ui_.BoxBSpin->setValue(box->axisLength(1));
                ui_.BoxCSpin->setValue(box->axisLength(2));
                ui_.BoxAlphaSpin->setValue(box->axisAngle(0));
                ui_.BoxBetaSpin->setValue(box->axisAngle(1));
                ui_.BoxGammaSpin->setValue(box->axisAngle(2));
            }
            else
                ui_.BoxGeometryUndefinedSizeRadio->setChecked(true);
            ui_.BoxGeometryUndefinedSizeRadio->setDisabled(frameworkSpecies_);
            ui_.BoxAxisLengthsGroup->setDisabled(frameworkSpecies_);
            ui_.BoxAxisAnglesGroup->setDisabled(frameworkSpecies_);
            break;
        case (AddConfigurationDialog::BoxGeometryPage):
            updateResultingBoxInfo();

            // Species can't be added by density if using a fixed-geometry box
            ui_.SpeciesDensitySpin->setDisabled(ui_.BoxGeometryFixedSizeRadio->isChecked());
            break;
        default:
            return true;
    }

    return true;
}

// Determine next page for the current page, based on current data
std::optional<int> AddConfigurationDialog::determineNextPage(int currentIndex)
{
    switch (currentIndex)
    {
        default:
            return {};
    }
}

// Perform any necessary actions before moving to the previous page
bool AddConfigurationDialog::prepareForPreviousPage(int currentIndex) { return true; }

// Perform any final actions before the wizard is closed
void AddConfigurationDialog::finalise()
{
    // Create the Configuration and a suitable generator
    auto *newConfiguration = dissolve_.coreData().addConfiguration();
    auto &generator = newConfiguration->generator();

    // Add Temperature node, by default
    generator.createRootNode<TemperatureProcedureNode>({});

    std::shared_ptr<GeneralRegionProcedureNode> regionNode;

    // Add the framework species if present
    if (frameworkSpecies_)
    {
        auto frameworkNode = generator.createRootNode<AddProcedureNode>("Framework", frameworkSpecies_, 1);
        frameworkNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::Set);
        frameworkNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Current);
        frameworkNode->keywords().set("Rotate", false);

        // Add a GeneralRegion node
        regionNode = generator.createRootNode<GeneralRegionProcedureNode>({});
        regionNode->keywords().set("Tolerance", 5.0);
    }
    else
    {
        // No framework - add on a Box spec
        auto boxNode = generator.createRootNode<BoxProcedureNode>({});
        boxNode->keywords().set("Lengths",
                                Vec3<NodeValue>(ui_.BoxASpin->value(), ui_.BoxBSpin->value(), ui_.BoxCSpin->value()));
        boxNode->keywords().set(
            "Angles", Vec3<NodeValue>(ui_.BoxAlphaSpin->value(), ui_.BoxBetaSpin->value(), ui_.BoxGammaSpin->value()));
    }

    // Create parameters node
    auto paramsNode = generator.createRootNode<ParametersProcedureNode>({});
    if (ui_.BoxGeometryUndefinedSizeRadio->isChecked())
        paramsNode->addParameter("rho", ui_.SpeciesDensitySpin->value());
    paramsNode->addParameter("multiplier", ui_.SpeciesMultiplierSpin->value());

    // Add mixture species
    auto rhoString = ui_.BoxGeometryUndefinedSizeRadio->isChecked() ? "rho" : "";
    auto rhoUnits = ui_.SpeciesDensityUnitsCombo->currentIndex() == 0 ? Units::DensityUnits::AtomsPerAngstromUnits
                                                                      : Units::DensityUnits::GramsPerCentimetreCubedUnits;
    for (const auto &spInfo : mixSpecies_)
    {
        // Set the population equation
        auto popString = fmt::format("{}*multiplier", spInfo.requestedPopulation());

        // Add coordinate sets only for suitable species - atomics, those with lone molecule population, or "large" molecules
        // don't get a coordinate sets node
        const auto *sp = spInfo.species();
        std::shared_ptr<AddProcedureNode> addNode;
        if (!spInfo.useCoordinateSets())
            addNode = generator.createRootNode<AddProcedureNode>(sp->name(), sp, NodeValue(popString, paramsNode->parameters()),
                                                                 NodeValue(rhoString, paramsNode->parameters()), rhoUnits);
        else
        {
            auto coordSets = generator.createRootNode<CoordinateSetsProcedureNode>(fmt::format("{}_Sets", sp->name()), sp);

            addNode = generator.createRootNode<AddProcedureNode>(sp->name(), coordSets,
                                                                 NodeValue(popString, paramsNode->parameters()),
                                                                 NodeValue(rhoString, paramsNode->parameters()), rhoUnits);
        }

        // Set other parameters for the Add node
        addNode->keywords().set("Rotate", spInfo.rotate());
        if (frameworkSpecies_)
        {
            addNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);
            addNode->keywords().setEnumeration("Positioning", AddProcedureNode::PositioningType::Region);
            addNode->keywords().set("Region", regionNode);
        }
        else if (ui_.BoxGeometryFixedSizeRadio->isChecked())
            addNode->keywords().setEnumeration("BoxAction", AddProcedureNode::BoxActionStyle::None);
    }
}

/*
 * Target Species Page
 */

void AddConfigurationDialog::on_TargetSpeciesWidget_speciesSelectionChanged(bool isValid) { updateProgressionControls(); }

/*
 * Box Geometry Page
 */

// Update detected box type from current values
void AddConfigurationDialog::updateBoxType()
{
    auto boxType = Box::type(Vec3<double>(ui_.BoxASpin->value(), ui_.BoxBSpin->value(), ui_.BoxCSpin->value()),
                             Vec3<double>(ui_.BoxAlphaSpin->value(), ui_.BoxBetaSpin->value(), ui_.BoxGammaSpin->value()));

    if (boxType)
        ui_.DetectedBoxTypeLabel->setText(QString::fromStdString(Box::boxTypes().keyword(*boxType)));
    else
        ui_.DetectedBoxTypeLabel->setText("???");
}

void AddConfigurationDialog::boxGeometryParameterChanged(double)
{
    updateBoxType();
    updateProgressionControls();
}

/*
 * Species Info Page
 */

// Update resulting box info
void AddConfigurationDialog::updateResultingBoxInfo()
{
    auto nAtoms = frameworkSpecies_ ? frameworkSpecies_->nAtoms() : 0;
    auto nMolecules = frameworkSpecies_ ? 1 : 0;
    auto mass = frameworkSpecies_ ? frameworkSpecies_->mass() : 0.0;
    Vec3<bool> scalableAxes(true, true, true);
    auto rho = ui_.SpeciesDensitySpin->value();
    auto mult = ui_.SpeciesMultiplierSpin->value();
    auto rhoUnits = ui_.SpeciesDensityUnitsCombo->currentIndex() == Units::DensityUnits::AtomsPerAngstromUnits
                        ? Units::DensityUnits::AtomsPerAngstromUnits
                        : Units::DensityUnits::GramsPerCentimetreCubedUnits;

    // Create a temporary box
    auto box = Box::generate(Vec3<double>(ui_.BoxASpin->value(), ui_.BoxBSpin->value(), ui_.BoxCSpin->value()),
                             Vec3<double>(ui_.BoxAlphaSpin->value(), ui_.BoxBetaSpin->value(), ui_.BoxGammaSpin->value()));

    for (auto &spInfo : mixSpecies_)
    {
        // Determine population of species to add
        int population = mult * spInfo.requestedPopulation();
        spInfo.setActualPopulation(population);
        auto nAtomsToAdd = population * spInfo.species()->nAtoms();

        // Scale box (if its geometry is not fixed)
        if (ui_.BoxGeometryUndefinedSizeRadio->isChecked())
        {
            // Get current cell volume
            auto currentVolume = box->volume();

            // Determine volume required to contain the population of the specified Species at the requested density
            auto requiredVolume = 0.0;
            if (rhoUnits == Units::AtomsPerAngstromUnits)
                requiredVolume = nAtomsToAdd / rho;
            else
                requiredVolume = ((spInfo.species()->mass() * population) / AVOGADRO) / (rho / 1.0E24);

            // If the current box has no atoms in it, absorb the current volume rather than adding to it
            if (nAtoms > 0)
                requiredVolume += currentVolume;

            box->scale(box->scaleFactors(requiredVolume, scalableAxes));
        }

        // Increase counters
        nAtoms += nAtomsToAdd;
        nMolecules += population;
        mass += spInfo.species()->mass() * population;
    }

    ui_.ResultingBoxTypeLabel->setText(QString::fromStdString(Box::boxTypes().keyword(box->type())));
    ui_.ResultingBoxALabel->setText(QString::number(box->axisLength(0)));
    ui_.ResultingBoxBLabel->setText(QString::number(box->axisLength(1)));
    ui_.ResultingBoxCLabel->setText(QString::number(box->axisLength(2)));
    ui_.ResultingBoxAlphaLabel->setText(QString::number(box->axisAngle(0)));
    ui_.ResultingBoxBetaLabel->setText(QString::number(box->axisAngle(1)));
    ui_.ResultingBoxGammaLabel->setText(QString::number(box->axisAngle(2)));
    ui_.ResultingAtomPopulation->setText(QString::number(nAtoms));
    ui_.ResultingMoleculePopulation->setText(QString::number(nMolecules));

    // Update the density here (if box is fixed size)
    if (ui_.BoxGeometryFixedSizeRadio->isChecked())
        ui_.SpeciesDensitySpin->setValue(ui_.SpeciesDensityUnitsCombo->currentIndex() == Units::AtomsPerAngstromUnits
                                             ? nAtoms / box->volume()
                                             : (mass / AVOGADRO) / (box->volume() / 1.0E24));

    addSpeciesInfoModel_.reset();
}

void AddConfigurationDialog::on_SpeciesDensitySpin_valueChanged(double value)
{
    if (!frameworkSpecies_)
        updateResultingBoxInfo();
}

void AddConfigurationDialog::on_SpeciesDensityUnitsCombo_currentIndexChanged(int index) { updateResultingBoxInfo(); }

void AddConfigurationDialog::on_SpeciesMultiplierSpin_valueChanged(int value) { updateResultingBoxInfo(); }

void AddConfigurationDialog::speciesInfoDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &)
{
    updateResultingBoxInfo();
}
