// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "gui/createNanotubeSpeciesDialog.h"
#include "classes/empiricalFormula.h"
#include "gui/helpers/comboPopulator.h"
#include <numeric>

// Useful constants
const auto root3 = sqrt(3.0);
const auto oneSixthPi = M_PI / 6.0; // == 30 degrees

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

    calculateParameters();

    regenerate();
}

/*
 * Data
 */

// Calculate parameters
void CreateNanotubeSpeciesDialog::calculateParameters()
{
    /*
     * Procedure based on "Determination of the chiral indices (n,m) of carbon nanotubes by electron diffraction",
     * Lu-Chan Qin, Phys. Chem. Chem. Phys. 2007, *9*, 31-48. https://doi.org/10.1039/B614121H
     */

    // Get n,m values for tube
    n_ = ui_.NSpin->value();
    m_ = ui_.MSpin->value();

    // Determine a0, the principal unit length of the structure, from the specified bond distance. See Figure 1a, noting that
    // a1 == a2 == a0.
    const auto r = ui_.BondLengthSpin->value();
    a0_ = r * root3;

    // Set principal vectors for reference
    va1_ = Vec3<double>(a0_, 0.0, 0.0);
    va2_ = Vec3<double>(a0_ * sin(oneSixthPi), a0_ * cos(oneSixthPi), 0.0);

    // Calculate repeat dimensions of the (n,m) sheet
    // -- A is magnitude of vecA (equation 1) and represents our X dimension
    A_ = a0_ * sqrt(n_ * n_ + m_ * m_ + n_ * m_);
    // -- c is the magnitude of the vector perpendicular to vecA, along the tube axis, and represents our Y dimension
    const auto M = std::gcd(2 * n_ + m_, n_ + 2 * m_);
    c_ = sqrt(3.0) * A_ / M;

    // Determine angle alpha, describing the tilt of the sheet away from va1 (equation 3)
    alpha_ = atan((sqrt(3.0) * m_) / (2 * n_ + m_));

    // The radius of the tube is then the circumference (== A) divided by 2PI
    radius_ = A_ / (2.0 * M_PI);

    dy_ = va2_.y / cos(alpha_);
    H_ = round(c_ / dy_);
}

// Regenerate species
void CreateNanotubeSpeciesDialog::regenerate()
{
    species_.clear();

    const auto roll = ui_.RollUpCheck->isChecked();
    const auto cFactor = ui_.CFactorSpin->value();

    // Determine a suitable periodic box for the species - we will always create one so that we get proper folding.
    auto offset = Vec3<double>(), cellLengths = Vec3<double>();
    if (roll)
    {
        // Create some extra space and set an offset so the tube is central in XZ
        cellLengths = {radius_ * 3, c_ * cFactor, radius_ * 3};
        offset = Vec3<double>(radius_ * 1.5, 0.0, radius_ * 1.5);
    }
    else
    {
        cellLengths = {A_, c_ * cFactor, sheetZ_};
        offset = Vec3<double>(0.0, 0.0, sheetZ_ * 0.5);
    }

    species_.createBox(cellLengths, {90, 90, 90});

    /*
     * Generate the full coordinates of the sheet. This process is based on equations 7 - 9, but does not follow it precisely.
     * Specifically, the innermost loop to translate the primary helix pair is originally stated to run over j (here 'i') from
     * 0 - (m-1) inclusive, but this does not generate the correct number of copies for most cases. Instead, the number of
     * required helix copies 'H' is calculated from c and the rotated y component of va2 (see earlier).
     */
    for (auto j = 0; j <= n_ + m_ - 1; ++j)
    {
        // Primary helix atoms (equation 7)
        auto x1j = j * a0_ * cos(alpha_);
        auto y1j = -j * a0_ * sin(alpha_);

        // Secondary helix atoms (equation 8)
        auto x2j = x1j - (a0_ / sqrt(3.0)) * cos(oneSixthPi - alpha_);
        auto y2j = y1j - (a0_ / sqrt(3.0)) * sin(oneSixthPi - alpha_);

        // Create H copies of the helix pairs, multiplied by the extension factor along C
        for (auto i = 0; i < H_ * cFactor; ++i)
        {
            // Generate translation vector for this copy (equation 9)
            auto delta = Vec3<double>(-i * a0_ * sin(oneSixthPi - alpha_), i * a0_ * cos(oneSixthPi - alpha_), 0.0);
            auto r1 = delta + Vec3<double>(x1j, y1j, 0.0);
            auto r2 = delta + Vec3<double>(x2j, y2j, 0.0);

            if (roll)
            {
                // Wrap the X coordinate (== real position on circumference) onto a tube of radius 'radius', applying the
                // offset we set earlier when creating the periodic box so as to put it in the centre of XZ.
                species_.addAtom(zA_, species_.box()->fold(offset + Vec3<double>(radius_ * sin(2.0 * M_PI * r1.x / A_), r1.y,
                                                                                 radius_ * cos(2.0 * M_PI * r1.x / A_))));
                species_.addAtom(zB_, species_.box()->fold(offset + Vec3<double>(radius_ * sin(2.0 * M_PI * r2.x / A_), r2.y,
                                                                                 radius_ * cos(2.0 * M_PI * r2.x / A_))));
            }
            else
            {
                species_.addAtom(zA_, species_.box()->fold(r1));
                species_.addAtom(zB_, species_.box()->fold(r2));
            }
        }
    }

    // Finalise the species
    if (ui_.PeriodicRadio->isChecked())
    {
        species_.recalculateIntermolecularTerms(1.1);
    }
    else if (ui_.NonPeriodicRadio->isChecked())
    {
        species_.recalculateIntermolecularTerms(1.1);

        if (ui_.TidyEndsCheck->isChecked())
        {
            // For any atom whose y coordinate is less than half the box B, or any x coordinate less than half the box A,
            // translate it to the opposite side.

            // // Pass over X
            // for (auto &i : species_.atoms())
            //     if (i.nBonds() == 1 && i.r().x < cellLengths.x*0.5)
            //     {
            //         i.setCoordinates(i.r() + Vec3<double>(cellLengths.x, 0.0, 0.0));
            //         // Check bound neighbour - if it has exactly two bonds then we need to move it as well
            //         auto j =i.bonds().front().get().partner(&i);
            //         if (j->nBonds() == 2)
            //             j->setCoordinates(j->r() + Vec3<double>(cellLengths.x, 0.0, 0.0));
            //     }
            // species_.recalculateIntermolecularTerms(1.1);
            //
            // // Pass over Y
            // for (auto &i : species_.atoms())
            //     if (i.nBonds() == 1 && i.r().y < cellLengths.y*0.5)
            //     {
            //         i.setCoordinates(i.r() + Vec3<double>(0.0, cellLengths.y, 0.0));
            //         // Check bound neighbour - if it has exactly two bonds then we need to move it as well
            //         auto j =i.bonds().front().get().partner(&i);
            //         if (j->nBonds() == 2)
            //             j->setCoordinates(j->r() + Vec3<double>(0.0, cellLengths.y, 0.0));
            //     }

            // Find all atoms which have two bonds spanning PBC of the box
            std::map<SpeciesAtom *, std::vector<SpeciesAtom *>> atoms;
            for (auto &i : species_.atoms())
            {
                // Find all bound partners which are across the unit cell
                std::vector<SpeciesAtom *> pbcJ;
                for (auto &b : i.bonds())
                    if ((i.r() - b.get().partner(&i)->r()).magnitude() > 1.44)
                        pbcJ.push_back(b.get().partner(&i));

                if (pbcJ.size() == 2)
                    atoms[&i] = pbcJ;
            }
            std::cout << std::format("There are {} atoms which have two periodic bonds.\n", atoms.size());

            while (atoms.size() > 0)
            {
                auto &&[i, pbcJ] = *atoms.begin();

                std::cout << std::format("-> Atom {} is at {},{},{}\n", i->index(), i->r().x, i->r().y, i->r().z);
                // Determine the fractional average vector to the pbc atoms
                Vec3<double> vFrac;
                for (auto j : pbcJ)
                {
                    vFrac += species_.box()->getFractional(j->r()) -  species_.box()->getFractional(i->r());
                    std::cout << std::format("-> partner {} is at {},{},{}\n", j->index(), j->r().x, j->r().y, j->r().z);
                }
                vFrac.print();

                // Make this a sensible translation vector - divide elements by their abs value to make unitary and round
                for (auto n = 0; n < 3; ++n)
                    vFrac[n] = fabs(vFrac[n]) < 0.5 ? 0.0 : round(vFrac[n] / fabs(vFrac[n]));
                vFrac.print();
                vFrac.multiply(cellLengths);
                vFrac.print();

                // Translate the atom
                i->setCoordinates(i->r() + vFrac);
                std::cout << std::format("-> new i.r() =  {},{},{}\n", i->r().x, i->r().y, i->r().z);

                // Remove this atom from the vector, along with any that were in its pbc neighbour lists
                for (auto j : pbcJ)
                    if (atoms.contains(j))
                        atoms.erase(j);

                atoms.erase(i);
                //
                // // Skip this atom
                // if (!roll && i->r().x < cellLengths.x * 0.5)
                // {
                //     i->setCoordinates(i->r() + Vec3<double>(cellLengths.x, 0.0, 0.0));
                //     // Check bound neighbour - if it has exactly two bonds then we need to move it as well
                //     auto j = i->bonds().front().get().partner(i);
                //     if (j->nBonds() == 2)
                //         j->setCoordinates(j->r() + Vec3<double>(cellLengths.x, 0.0, 0.0));
                // }
                // else if (i->r().y < cellLengths.y * 0.5)
                // {
                //     i->setCoordinates(i->r() + Vec3<double>(0.0, cellLengths.y, 0.0));
                //     // Check bound neighbour - if it has exactly two bonds then we need to move it as well
                //     auto j = i->bonds().front().get().partner(i);
                //     if (j->nBonds() == 2)
                //         j->setCoordinates(j->r() + Vec3<double>(0.0, cellLengths.y, 0.0));
                // }
                //
                // // Recalculate bonding
                // species_.recalculateIntermolecularTerms(1.1);
                //
                // // Remove any atoms in the vector which now have more than one bond
                // atoms.erase(std::remove_if(atoms.begin(), atoms.end(), [](const auto *j) { return j->nBonds() > 1; }),
                //             atoms.end());
                std::cout << std::format("There are now {} atoms to process.\n", atoms.size());
            }
        }

        species_.removeBox();
    }
    else
    {
        species_.recalculateIntermolecularTerms(1.1);
        species_.removeBox();
    }
}

/*
 * UI
 */

// Update all controls
void CreateNanotubeSpeciesDialog::updateWidgets()
{
    Locker updateLock(widgetsUpdating_);

    // Sheet properties
    ui_.ALabel->setText(QString("%1 \u212B").arg(A_, 0, 'f', 3));
    ui_.CLabel->setText(QString("%1 (%2) \u212B").arg(c_, 0, 'f', 3).arg(c_ * ui_.CFactorSpin->value(), 0, 'f', 3));
    ui_.AlphaLabel->setText(QString("%1\u00B0").arg(alpha_ * DEGRAD, 0, 'f', 3));
    ui_.HLabel->setText(QString("%1 (%2)").arg(H_).arg(H_ * ui_.CFactorSpin->value()));
    ui_.RadiusLabel->setText(QString("%1 \u212B").arg(radius_, 0, 'f', 3));

    // Final cell properties
    ui_.CellALabel->setText(QString("%1 \u212B").arg(species_.box()->axisLengths().x, 0, 'f', 3));
    ui_.CellBLabel->setText(QString("%1 \u212B").arg(species_.box()->axisLengths().y, 0, 'f', 3));
    ui_.CellCLabel->setText(QString("%1 \u212B").arg(species_.box()->axisLengths().z, 0, 'f', 3));

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

    calculateParameters();
    regenerate();
    updateWidgets();
}

void CreateNanotubeSpeciesDialog::on_MSpin_valueChanged(int value)
{
    calculateParameters();
    regenerate();
    updateWidgets();
}

void CreateNanotubeSpeciesDialog::on_ElementAButton_clicked(bool checked)
{
    auto Z = selectElementDialog_.selectElement(zA_);
    if (Z == Elements::Unknown)
        return;
    zA_ = Z;

    regenerate();
    updateWidgets();
}

void CreateNanotubeSpeciesDialog::on_ElementBButton_clicked(bool checked)
{
    auto Z = selectElementDialog_.selectElement(zB_);
    if (Z == Elements::Unknown)
        return;
    zB_ = Z;

    regenerate();
    updateWidgets();
}

void CreateNanotubeSpeciesDialog::on_BondLengthSpin_valueChanged(double value)
{
    calculateParameters();
    regenerate();
    updateWidgets();
}

void CreateNanotubeSpeciesDialog::on_CFactorSpin_valueChanged(int value)
{
    calculateParameters();
    regenerate();
    updateWidgets();
}

void CreateNanotubeSpeciesDialog::on_RollUpCheck_clicked(bool checked)
{
    regenerate();
    updateWidgets();
}

void CreateNanotubeSpeciesDialog::on_PeriodicRadio_clicked(bool checked)
{
    if (widgetsUpdating_)
        return;

    regenerate();
    updateWidgets();
}

void CreateNanotubeSpeciesDialog::on_NonPeriodicRadio_clicked(bool checked)
{
    if (widgetsUpdating_)
        return;
    regenerate();
    updateWidgets();
}

void CreateNanotubeSpeciesDialog::on_TidyEndsCheck_clicked(bool checked)
{
    regenerate();
    updateWidgets();
}

void CreateNanotubeSpeciesDialog::on_PseudoPeriodicRadio_clicked(bool checked)
{
    if (widgetsUpdating_)
        return;
    regenerate();
    updateWidgets();
}

void CreateNanotubeSpeciesDialog::on_OKButton_clicked(bool checked)
{
    // Copy the species to the main Dissolve instance and set its new name
    auto *sp = dissolve_.coreData().copySpecies(&species_);
    sp->setName(std::format("Nanotube-({},{})", ui_.NSpin->value(), ui_.MSpin->value()));

    accept();
}

void CreateNanotubeSpeciesDialog::on_CancelButton_clicked(bool checked) { reject(); }
