// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/createNanotubeSpeciesDialog.h"
#include "classes/empiricalFormula.h"
#include "gui/helpers/comboPopulator.h"

CreateNanotubeSpeciesDialog::CreateNanotubeSpeciesDialog(QWidget *parent, Dissolve &dissolve)
    : QDialog(parent), selectElementDialog_(this), dissolve_(dissolve)
{
    ui_.setupUi(this);

    Locker refreshLock(widgetsUpdating_);

    ui_.ElementAButton->setText(QString::fromStdString(std::string(Elements::symbol(zA_))));
    ui_.ElementBButton->setText(QString::fromStdString(std::string(Elements::symbol(zB_))));

    // Set display configuration
    ui_.StructureViewer->setSpecies(&species_);

    refreshLock.unlock();

    regenerate();
}

/*
 * Data
 */

// Plot an AB atom layer
void CreateNanotubeSpeciesDialog::plotLayer(double z, double tubeRadius, double radialStep, double radialOffset,
                                            Elements::Element zA, Elements::Element zB)
{
    for (auto radial = 0; radial < ui_.RadialRingSizeSpin->value(); ++radial)
    {
        auto angle = radial * radialStep + radialOffset;

        if (zA != Elements::Unknown)
            species_.addAtom(zA, {tubeRadius * cos(angle), tubeRadius * sin(angle), z});

        angle += radialStep * 0.5;

        if (zB != Elements::Unknown)
            species_.addAtom(
                zB, {tubeRadius * cos(angle), tubeRadius * sin(angle), z + ui_.BondLengthSpin->value() * cos(60.0 / DEGRAD)});
    }
}

// Regenerate species
void CreateNanotubeSpeciesDialog::regenerate()
{
    species_.clear();

    const auto r = ui_.BondLengthSpin->value();
    const auto nAxialRings = ui_.AxialRingLengthSpin->value();

    const auto radialStep = (M_PI * 2.0) / ui_.RadialRingSizeSpin->value();
    const auto radialHalfStep = radialStep * 0.5;
    const auto ringRadialWidth = r * cos(30.0 / DEGRAD) * 2.0;
    const auto ringAxialLayerStep = r * (cos(60.0 / DEGRAD) + 1.0);
    const auto tubeRadius = ringRadialWidth / (2.0 * sin(M_PI / ui_.RadialRingSizeSpin->value()));

    /*
     *      B1--A2
     *     /      \
     *    A1       B2...
     *     \      /
     *      B1--A2
     *
     * Build up the nanotube in layers
     */
    auto radialOffset = 0.0;
    for (auto axial = 0; axial < nAxialRings; ++axial)
    {
        auto z = axial * ringAxialLayerStep;
        // Create an AB layer
        plotLayer(z, tubeRadius, radialStep, radialOffset, zA_, zB_);
        radialOffset += radialHalfStep;
    }

    // Terminate / make periodic
    if (ui_.TypeCombo->currentIndex() == 0)
    {
        // Remove any existing unit cell
        species_.removeBox();

        // Add final terminating layer
        plotLayer(nAxialRings * ringAxialLayerStep, tubeRadius, radialStep, nAxialRings * radialHalfStep, zA_, zB_);

        // Add hydrogen termination layers
        plotLayer(-1.0, tubeRadius, radialStep, 0.0, Elements::H, Elements::Unknown);
        plotLayer(nAxialRings * ringAxialLayerStep + 1.0, tubeRadius, radialStep, nAxialRings * radialHalfStep,
                  Elements::Unknown, Elements::H);
    }
    else
    {
        // Add on a suitable periodic box
        species_.createBox({tubeRadius * 2.0 + 2.0, tubeRadius * 2.0 + 2.0, nAxialRings * ringAxialLayerStep}, {90, 90, 90});
    }

    // Finalise the species
    species_.recalculateIntermolecularTerms(1.1);

    updateWidgets();
}

/*
 * UI
 */

// Update all controls
void CreateNanotubeSpeciesDialog::updateWidgets()
{
    Locker updateLock(widgetsUpdating_);

    // Configuration information
    const auto *box = species_.box();
    ui_.CurrentBoxTypeLabel->setText(QString::fromStdString(std::string(Box::boxTypes().keyword(box->type()))));
    QString boxInfo = QString("<b>A:</b>  %1 &#8491;<br>").arg(box->axisLengths().x);
    boxInfo += QString("<b>B:</b>  %1 &#8491;<br>").arg(box->axisLengths().y);
    boxInfo += QString("<b>C:</b>  %1 &#8491;<br>").arg(box->axisLengths().z);
    boxInfo += QString("<b>&#x3B1;:</b>  %1&#xb0;<br>").arg(box->axisAngles().x);
    boxInfo += QString("<b>&#x3B2;:</b>  %1&#xb0;<br>").arg(box->axisAngles().y);
    boxInfo += QString("<b>&#x3B3;:</b>  %1&#xb0;").arg(box->axisAngles().z);
    ui_.CurrentBoxFrame->setToolTip(boxInfo);

    ui_.AtomPopulationLabel->setText(QString::number(species_.nAtoms()));

    // Structure
    ui_.StructureViewer->postRedisplay();
}

void CreateNanotubeSpeciesDialog::on_AxialRingLengthSpin_valueChanged(int value)
{
    if (widgetsUpdating_.isLocked())
        return;

    // Clamp to even numbers if type == Periodic
    if (ui_.TypeCombo->currentIndex() == 1)
    {
        Locker refreshLock(widgetsUpdating_);
        ui_.AxialRingLengthSpin->setValue(2 * (ui_.AxialRingLengthSpin->value() / 2));
    }

    regenerate();
}

void CreateNanotubeSpeciesDialog::on_RadialRingSizeSpin_valueChanged(int value) { regenerate(); }

void CreateNanotubeSpeciesDialog::on_ElementAButton_clicked(bool checked)
{
    auto Z = selectElementDialog_.selectElement(zA_);
    if (Z == Elements::Unknown)
        return;
    zA_ = Z;

    regenerate();
}

void CreateNanotubeSpeciesDialog::on_ElementBButton_clicked(bool checked)
{
    auto Z = selectElementDialog_.selectElement(zB_);
    if (Z == Elements::Unknown)
        return;
    zB_ = Z;

    regenerate();
}

void CreateNanotubeSpeciesDialog::on_OKButton_clicked(bool checked)
{
    // Copy the species to the main Dissolve instance and set its new name
    auto *sp = dissolve_.coreData().copySpecies(&species_);
    sp->setName(std::format("Nanotube-{}x{}", ui_.AxialRingLengthSpin->value(), ui_.RadialRingSizeSpin->value()));

    accept();
}

void CreateNanotubeSpeciesDialog::on_CancelButton_clicked(bool checked) { reject(); }

void CreateNanotubeSpeciesDialog::on_TypeCombo_currentIndexChanged(int index)
{
    Locker refreshLock(widgetsUpdating_);

    // Limit tube axial length depending on mode requested
    if (index == 0)
    {
        ui_.AxialRingLengthSpin->setSingleStep(1);
        ui_.AxialRingLengthSpin->setMinimum(1);
    }
    else
    {
        ui_.AxialRingLengthSpin->setSingleStep(2);
        ui_.AxialRingLengthSpin->setMinimum(2);
    }

    refreshLock.unlock();

    regenerate();
}
