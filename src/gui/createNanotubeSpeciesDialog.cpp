// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/createNanotubeSpeciesDialog.h"
#include "classes/empiricalFormula.h"
#include "gui/helpers/comboPopulator.h"
#include <numeric>

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

// Regenerate species
void CreateNanotubeSpeciesDialog::regenerate()
{
    species_.clear();

    const auto r = ui_.BondLengthSpin->value();

    const auto a0 = 2.46;
    const auto a1 = Vec3<double>(a0, 0.0, 0.0);
    const auto a2 = Vec3<double>(a0*sin(M_PI/6.0), a0*cos(M_PI/6.0), 0.0);
    const auto n = ui_.NSpin->value();
    a2.print();
    const auto m = ui_.MSpin->value();

    auto A = a0 * sqrt(n*n + m*m + n*m);
    auto alpha = atan((sqrt(3.0) * m) / (2 * n + m));
    const auto radius = A / (2.0 * M_PI);
    std::cout << std::format("Helicity (alpha) = {} rad, {} degrees\n", alpha, alpha*DEGRAD);

    // TEST
    auto M = std::gcd(2 * n + m, n + 2 * m);
//    auto nc = -((n + 2*m) / M);
//    auto mc = ((2*n + m) / M);
//    auto vecAPerp = a1 * nc + a2 * mc;
//    auto Aperp = a0 * sqrt(nc*nc + mc*mc + nc*mc);
//
    auto c = sqrt(3.0) * A / M;

    std::cout << std::format("Rectangle dims are A = {} c = {}\n", A, c);
    auto d = std::gcd(n,m);
    auto acc = a0 / sqrt(3.0);
    auto T = 3.0 * acc * sqrt(n*n + n*m + m*m);
    T /= (n-m)%(3*d) == 0 ? 3 * d : d;
    std::cout << std::format("T = {}, acc = {}, d = {}\n", T, acc, d);

    species_.createBox({A, T, 3.0}, {90,90,90});

    // Generate the full coordinates
    auto dy = a2.y/cos(alpha);
    auto H = round(T / dy);
    std::cout << std::format("Repeats H = {}, T/a0CosAlpha = {}, dy = {}\n", H, T / dy, dy);
    for (auto j = 0; j <= n + m -1; ++j)
    {
        // Primary helix atoms
        auto x1j = j * a0 * cos(alpha);
        auto y1j = -j * a0 * sin(alpha);

        // Secondary helix atoms
        auto x2j = x1j - (a0 / sqrt(3.0)) * cos(M_PI/6.0 - alpha);
        auto y2j = y1j - (a0 / sqrt(3.0)) * sin(M_PI/6.0-alpha);

        for (auto i = 0; i < H; ++i)
        {
            auto delta = Vec3<double>(-i * a0 * sin(M_PI/6.0 - alpha), i * a0 * cos(M_PI/6.0 - alpha), 0.0);
            auto r1 = delta + Vec3<double>(x1j, y1j, 0.0);
            auto r2 = delta + Vec3<double>(x2j, y2j, 0.0);

            // Map negative y values back into their positive periodic image
//            if (r1.y < 0.0)
//                r1.y += T * d;
//            if (r2.y < 0.0)
//                r2.y += T * d;

                species_.addAtom(Elements::C,     species_.box()->fold(r1));
                species_.addAtom(Elements::N,     species_.box()->fold(r2));

//            species_.addAtom(Elements::C, {radius*sin(2.0 * M_PI * r1.x / A) , r1.y, radius* cos(2.0 * M_PI * r1.x / A)});
//            species_.addAtom(Elements::N, {radius*sin(2.0 * M_PI * r2.x / A) , r2.y, radius * cos(2.0 * M_PI * r2.x / A)});
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

void CreateNanotubeSpeciesDialog::on_NSpin_valueChanged(int value)
{
    if (widgetsUpdating_.isLocked())
        return;

    // MSpin's upper value is our current value
    ui_.MSpin->setMaximum(value);

    regenerate();
}

void CreateNanotubeSpeciesDialog::on_MSpin_valueChanged(int value) { regenerate(); }

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
    sp->setName(std::format("Nanotube-({},{})", ui_.NSpin->value(), ui_.MSpin->value()));

    accept();
}

void CreateNanotubeSpeciesDialog::on_CancelButton_clicked(bool checked) { reject(); }

void CreateNanotubeSpeciesDialog::on_TypeCombo_currentIndexChanged(int index)
{
    Locker refreshLock(widgetsUpdating_);

//    // Limit tube axial length depending on mode requested
//    if (index == 0)
//    {
//        ui_.AxialRingLengthSpin->setSingleStep(1);
//        ui_.AxialRingLengthSpin->setMinimum(1);
//    }
//    else
//    {
//        ui_.AxialRingLengthSpin->setSingleStep(2);
//        ui_.AxialRingLengthSpin->setMinimum(2);
//    }

    refreshLock.unlock();

    regenerate();
}
