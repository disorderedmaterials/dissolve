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
void CreateNanotubeSpeciesDialog::plotLayer(double z, double tubeRadius, double radialStep, double radialOffset)
{
    for (auto radial = 0; radial < ui_.RadialRingSizeSpin->value(); ++radial)
    {
        auto angle = radial * radialStep + radialOffset;
        species_.addAtom(zA_, {tubeRadius * cos(angle), tubeRadius * sin(angle), z});
        angle += radialStep * 0.5;
        species_.addAtom(
            zB_, {tubeRadius * cos(angle), tubeRadius * sin(angle), z + ui_.BondLengthSpin->value() * cos(60.0 / DEGRAD)});
    }
}

// Regenerate species
void CreateNanotubeSpeciesDialog::regenerate()
{
    species_.clear();

    const auto r = ui_.BondLengthSpin->value();
    const auto axialN = ui_.AxialRingLengthSpin->value();

    const auto radialStep = (M_PI * 2.0) / ui_.RadialRingSizeSpin->value();
    const auto radialHalfStep = radialStep * 0.5;
    const auto delta60 = r * cos(60.0 / DEGRAD);
    const auto ringRadialWidth = r * cos(30.0 / DEGRAD) * 2.0;
    const auto ringAxialLength = r * (cos(60.0 / DEGRAD) * 2.0 + 1.0);
    const auto ringAxialLayerStep = r * (cos(60.0 / DEGRAD) + 1.0);
    const auto tubeRadius = ringRadialWidth / (2.0 * sin(M_PI / ui_.RadialRingSizeSpin->value()));
    std::cout << std::format("Ring dimensions: width = {} length = {}\n", ringRadialWidth, ringAxialLength);

    /*
     *      B--A
     *     /    \
     *    A      B
     *     \    /
     *      B--A
     *
     *
     */
    for (auto axial = 0; axial < axialN; ++axial)
    {
        auto z = axial * ringAxialLayerStep;
        // Create an AB layer
        plotLayer(z, tubeRadius, radialStep, 0.0);
        plotLayer(z + ringAxialLayerStep, tubeRadius, radialStep, radialHalfStep);
    }

    // Finalise the species
    species_.recalculateIntermolecularTerms(1.1);

    ui_.StructureViewer->postRedisplay();
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

void CreateNanotubeSpeciesDialog::on_AxialRingLengthSpin_valueChanged(int value) { regenerate(); }

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
    //    cifHandler_.finalise(dissolve_.coreData(), outputFlags);

    accept();
}

void CreateNanotubeSpeciesDialog::on_CancelButton_clicked(bool checked) { reject(); }
