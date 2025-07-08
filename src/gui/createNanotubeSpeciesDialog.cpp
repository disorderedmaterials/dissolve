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

    ui_.MSpin->setMaximum(ui_.NSpin->value());

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

    /*
     * Procedure based on "Determination of the chiral indices (n,m) of carbon nanotubes by electron diffraction",
     * Lu-Chan Qin, Phys. Chem. Chem. Phys. 2007, *9*, 31-48. https://doi.org/10.1039/B614121H
     */

    const auto root3 = sqrt(3.0);
    const auto oneSixthPi = M_PI / 6.0; // == 30 degrees
    const auto roll = ui_.RollUpCheck->isChecked();

    // Get n,m values for tube
    const auto n = ui_.NSpin->value();
    const auto m = ui_.MSpin->value();

    // Determine a0, the principal unit length of the structure, from the specified bond distance. See Figure 1a, noting that
    // a1 == a2 == a0.
    const auto r = ui_.BondLengthSpin->value();
    const auto a0 = r * root3;

    // Set principal vectors for reference
    const auto va1 = Vec3<double>(a0, 0.0, 0.0);
    const auto va2 = Vec3<double>(a0 * sin(oneSixthPi), a0 * cos(oneSixthPi), 0.0);

    // Calculate repeat dimensions of the (n,m) sheet
    // -- A is magnitude of vecA (equation 1) and represents our X dimension
    auto A = a0 * sqrt(n * n + m * m + n * m);
    // -- c is the magnitude of the vector perpendicular to vecA, along the tube axis, and represents our Y dimension
    auto M = std::gcd(2 * n + m, n + 2 * m);
    auto c = sqrt(3.0) * A / M;

    // Determine angle alpha, describing the tilt of the sheet away from va1 (equation 3)
    auto alpha = atan((sqrt(3.0) * m) / (2 * n + m));

    // The radius of the tube is then the circumference (== A) divided by 2PI
    const auto radius = A / (2.0 * M_PI);

    // Determine a suitable periodic box for the species - we will always create one so that we get proper folding.
    auto offset = Vec3<double>();
    if (roll)
    {
        // Create some extra space and set an offset so the tube is central in XZ
        species_.createBox({radius * 3, c, radius * 3}, {90, 90, 90});
        offset = Vec3<double>(radius * 1.5, 0.0, radius * 1.5);
    }
    else
        species_.createBox({A, c, 3.0}, {90, 90, 90});

    /*
     * Generate the full coordinates of the sheet. This process is based on equations 7 - 9, but does not follow it precisely.
     * Specifically, the innermost loop to translate the primary helix pair is originally stated to run over j (here 'i') from
     * 0 - (m-1) inclusive, but this does not generate the correct number of copies for most cases. Instead, the number of
     * required helix copies 'H' is calculated from c and the rotated y component of va2.
     */
    auto dy = va2.y / cos(alpha);
    auto H = round(c / dy);
    for (auto j = 0; j <= n + m - 1; ++j)
    {
        // Primary helix atoms (equation 7)
        auto x1j = j * a0 * cos(alpha);
        auto y1j = -j * a0 * sin(alpha);

        // Secondary helix atoms (equation 8)
        auto x2j = x1j - (a0 / sqrt(3.0)) * cos(oneSixthPi - alpha);
        auto y2j = y1j - (a0 / sqrt(3.0)) * sin(oneSixthPi - alpha);

        // Create H copies of the helix pairs
        for (auto i = 0; i < H; ++i)
        {
            // Generate translation vector for this copy (equation 9)
            auto delta = Vec3<double>(-i * a0 * sin(oneSixthPi - alpha), i * a0 * cos(oneSixthPi - alpha), 0.0);
            auto r1 = delta + Vec3<double>(x1j, y1j, 0.0);
            auto r2 = delta + Vec3<double>(x2j, y2j, 0.0);

            if (roll)
            {
                // Wrap the X coordinate (== real position on circumference) onto a tube of radius 'radius', applying the
                // offset we set earlier when creating the periodic box so as to put it in the centre of XZ.
                species_.addAtom(zA_, species_.box()->fold(offset + Vec3<double>(radius * sin(2.0 * M_PI * r1.x / A), r1.y,
                                                                                 radius * cos(2.0 * M_PI * r1.x / A))));
                species_.addAtom(zB_, species_.box()->fold(offset + Vec3<double>(radius * sin(2.0 * M_PI * r2.x / A), r2.y,
                                                                                 radius * cos(2.0 * M_PI * r2.x / A))));
            }
            else
            {
                species_.addAtom(zA_, species_.box()->fold(r1));
                species_.addAtom(zB_, species_.box()->fold(r2));
            }
        }
    }

    // Finalise the species
    species_.recalculateIntermolecularTerms(1.1);

    // Update the sheet properties
    ui_.ALabel->setText(QString("%1 \u212B").arg(A, 0, 'f', 3));
    ui_.CLabel->setText(QString("%1 \u212B").arg(c, 0, 'f', 3));
    ui_.AlphaLabel->setText(QString("%1\u00B0").arg(alpha * DEGRAD, 0, 'f', 3));
    ui_.HLabel->setText(QString("%1\u00B0").arg(H));
    ui_.RadiusLabel->setText(QString("%1 \u212B").arg(radius, 0, 'f', 3));

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

void CreateNanotubeSpeciesDialog::on_BondLengthSpin_valueChanged(double value) { regenerate(); }

void CreateNanotubeSpeciesDialog::on_RollCheck_clicked(bool checked) { regenerate(); }

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
