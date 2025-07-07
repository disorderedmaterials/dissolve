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

    const auto a0 = 2.46;
    const auto a1 = Vec3<double>(a0, 0.0, 0.0);
    const auto a2 = Vec3<double>(a0*sin(M_PI/6.0), a0*cos(M_PI/6.0), 0.0);
    const auto a3 = a2 - a1;
    const auto n = 5;
    const auto m = 2;

    auto vecA = a1 * n + a2 * m;
    auto A = a0 * sqrt(n*n + m*m + n*m);
    auto alpha = atan((sqrt(3.0) * m) / (2 * n + m));
    const auto radius = A / (2.0 * M_PI);
    std::cout << std::format("Helicity (alpha) = {}\n", alpha);

    // TEST
    auto M = 10;
    auto nc = -((n + 2*m) / M);
    auto mc = ((2*n + m) / M);

    auto c = sqrt(3.0) * A / M;

    std::cout << std::format("Rectangle dims are A = {} c = {}\n", A, c);

    for (auto j = 0; j <= n + m -1; ++j)
    {
        // Primary helix atoms
        auto x1j = j * a0 * cos(alpha);
        auto y1j = -j * a0 * sin(alpha);

        // Secondary helix atoms
        auto x2j = x1j - (a0 / sqrt(3.0)) * cos(M_PI/6.0 - alpha);
        auto y2j = y1j - (a0 / sqrt(3.0)) * sin(M_PI/6.0-alpha);

        for (auto i = 0; i < m; ++i)
        {
            auto delta = Vec3<double>(-i * a0 * sin(M_PI/6.0 - alpha), i * a0 * cos(M_PI/6.0 - alpha), 0.0);
            auto r1 = delta + Vec3<double>(x1j, y1j, 0.0);
            auto r2 = delta + Vec3<double>(x2j, y2j, 0.0);
            species_.addAtom(Elements::C, {radius*sin(2.0 * M_PI * r1.x / A) , r1.y, radius* cos(2.0 * M_PI * r1.x / A)});
            species_.addAtom(Elements::N, {radius*sin(2.0 * M_PI * r2.x / A) , r2.y, radius * cos(2.0 * M_PI * r2.x / A)});
        }
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
