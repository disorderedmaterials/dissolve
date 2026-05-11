// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "gui/createGrapheneSpeciesDialog.h"
#include "classes/empiricalFormula.h"
#include "gui/helpers/comboPopulator.h"
#include "math/mathFunc.h"
#include <numeric>

// Useful constants
const auto root3 = sqrt(3.0);
const auto oneSixthPi = M_PI / 6.0; // == 30 degrees

CreateGrapheneSpeciesDialog::CreateGrapheneSpeciesDialog(QWidget *parent, Dissolve &dissolve)
    : QDialog(parent), selectElementDialog_(this), dissolve_(dissolve)
{
    ui_.setupUi(this);

    Locker refreshLock(widgetsUpdating_);

    ui_.ElementAButton->setText(QString::fromStdString(std::string(Elements::symbol(zA_))));
    ui_.ElementBButton->setText(QString::fromStdString(std::string(Elements::symbol(zB_))));
    ui_.TerminateElementButton->setText(QString::fromStdString(std::string(Elements::symbol(zTerminate_))));

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
void CreateGrapheneSpeciesDialog::calculateParameters()
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
    va1_ = Vector3(a0_, 0.0, 0.0);
    va2_ = Vector3(a0_ * sin(oneSixthPi), a0_ * cos(oneSixthPi), 0.0);

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

// Find dangling atoms, defined as those which have two bonds spanning PBC of the box
std::map<SpeciesAtom *, std::vector<SpeciesAtom *>> CreateGrapheneSpeciesDialog::findDanglingAtoms(double localCutoff)
{
    std::map<SpeciesAtom *, std::vector<SpeciesAtom *>> atoms;
    for (auto &i : species_.atoms())
    {
        // Find all bound partners which are across the unit cell
        std::vector<SpeciesAtom *> pbcJ;
        for (auto &b : i.bonds())
        {
            auto *j = b.get().partner(&i);
            if ((i.r() - j->r()).magnitude() > localCutoff)
                pbcJ.push_back(j);
        }

        if (pbcJ.size() == 2)
            atoms[&i] = pbcJ;
    }

    return atoms;
}

// Get vector of atom keys from atom/neighbour map, sorted by position along indicated direction
std::vector<SpeciesAtom *>
CreateGrapheneSpeciesDialog::getSortedByY(const std::map<SpeciesAtom *, std::vector<SpeciesAtom *>> &atoms) const
{
    std::vector<SpeciesAtom *> sorted;
    for (auto &&[i, _] : atoms)
        sorted.push_back(i);

    std::sort(sorted.begin(), sorted.end(), [](const SpeciesAtom *a, const SpeciesAtom *b) { return a->r().y < b->r().y; });

    return sorted;
}

// Recursive branch function
void CreateGrapheneSpeciesDialog::extendBranch(SpeciesAtom *i, const Box *box, Vector3 &vFrac,
                                               std::vector<SpeciesAtom *> &branch, double localCutoff) const
{
    // Get PBC and non-PBC (local) bond partners for this atom and see what type of atom we have...
    std::vector<SpeciesAtom *> pbcJ, localJ;
    for (auto &b : i->bonds())
    {
        auto j = b.get().partner(i);
        if ((i->r() - j->r()).magnitude() > localCutoff)
            pbcJ.push_back(j);
        else if (std::find(branch.begin(), branch.end(), j) == branch.end())
            localJ.push_back(j);
    }

    // If all atoms are local this is a completely internally-bound atom, and we are done here.
    if (pbcJ.size() == 0)
        return;

    // Add ourselves to the branch
    branch.push_back(i);

    // For all pbc atoms, increment the fractional direction
    for (auto j : pbcJ)
        vFrac += (box->getFractional(j->r()) - box->getFractional(i->r()));

    // For local atoms, recurse into them if we haven't visited them already
    for (auto j : localJ)
        extendBranch(j, box, vFrac, branch, localCutoff);
}

// Regenerate species
void CreateGrapheneSpeciesDialog::regenerate()
{
    species_.clear();

    const auto roll = ui_.RollUpCheck->isChecked();
    const auto cFactor = ui_.CFactorSpin->value();

    // Determine a suitable periodic box for the species - we will always create one so that we get proper folding.
    auto offset = Vector3(), cellLengths = Vector3();
    if (roll)
    {
        // Create some extra space and set an offset so the tube is central in XZ
        cellLengths = {radius_ * 3, c_ * cFactor, radius_ * 3};
        offset = Vector3(radius_ * 1.5, 0.0, radius_ * 1.5);
    }
    else
    {
        cellLengths = {A_, c_ * cFactor, sheetZ_};
        offset = Vector3(0.0, 0.0, sheetZ_ * 0.5);
    }
    species_.createBox(cellLengths, {90, 90, 90});

    /*
     * Generate the full coordinates of the sheet. This process is based on equations 7 - 9, but does not follow it precisely.
     * Specifically, the innermost loop to translate the primary helix pair is originally stated to run over j (here 'i') from
     * 0 - (m-1) inclusive, but this does not generate the correct number of copies for most cases. Instead, the number of
     * required helix copies 'H' is calculated from c and the rotated y component of va2 (see earlier).
     */
    for (auto j = 0; j < n_ + m_; ++j)
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
            auto delta = Vector3(-i * a0_ * sin(oneSixthPi - alpha_), i * a0_ * cos(oneSixthPi - alpha_), 0.0);
            auto r1 = delta + Vector3(x1j, y1j, 0.0);
            auto r2 = delta + Vector3(x2j, y2j, 0.0);

            // Get new helix pair coordinates
            Vector3 p1, p2;
            if (roll)
            {
                // Wrap the X coordinate (== real position on circumference) onto a tube of radius 'radius', applying the
                // offset we set earlier when creating the periodic box so as to put it in the centre of XZ.
                p1 = species_.box()->fold(
                    offset + Vector3(radius_ * sin(2.0 * M_PI * r1.x / A_), r1.y, radius_ * cos(2.0 * M_PI * r1.x / A_)));
                p2 = species_.box()->fold(
                    offset + Vector3(radius_ * sin(2.0 * M_PI * r2.x / A_), r2.y, radius_ * cos(2.0 * M_PI * r2.x / A_)));
            }
            else
            {
                p1 = species_.box()->fold(r1);
                p2 = species_.box()->fold(r2);
            }

            // Check for overlaps with existing atoms - easier and more efficient to do this now as we go.
            auto add1 = true, add2 = true;
            for (auto &existing : species_.atoms())
            {
                if (add1)
                    add1 = (p1 - existing.r()).magnitude() > 0.1;
                if (add2)
                    add2 = (p2 - existing.r()).magnitude() > 0.1;
                if (!add1 && !add2)
                    break;
            }
            if (add1)
                species_.addAtom(zA_, p1);
            if (add2)
                species_.addAtom(zB_, p2);
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
            // Find all atoms which have two bonds spanning PBC of the box
            auto localCutoff = ui_.BondLengthSpin->value() * 1.01;
            const auto atoms = findDanglingAtoms(localCutoff);
            auto sorted = getSortedByY(atoms);

            // Pass 1 - Move all atoms along the PBC bond vectors
            while (!sorted.empty())
            {
                auto i = sorted.back();
                auto &pbcJ = atoms.at(i);

                // Determine the fractional average vector to the pbc atoms
                Vector3 vFrac;
                for (auto j : pbcJ)
                    vFrac += species_.box()->getFractional(j->r()) - species_.box()->getFractional(i->r());

                // Make this a sensible translation vector - divide elements by their abs value to make unitary and round
                for (auto n = 0; n < 3; ++n)
                    vFrac[n] = fabs(vFrac[n]) < 0.5 ? 0.0 : round(vFrac[n] / fabs(vFrac[n]));
                vFrac.multiply(cellLengths);

                // Translate the atom
                i->setCoordinates(i->r() + vFrac);

                // Remove this atom from the vector, along with any that were in its pbc neighbour lists
                for (auto j : pbcJ)
                {
                    auto it = std::ranges::find(sorted, j);
                    if (it != sorted.end())
                        sorted.erase(it);
                }

                sorted.pop_back();
            }

            // Pass 2 - We may have generated some "branches" inadvertently, so rebond, find any singly-bound atoms and check
            auto branchEnd = findDanglingAtoms(localCutoff);
            std::vector<int> indicesToRemove;
            for (auto &&[i, _] : branchEnd)
            {
                // For this atom we will step along bonds until we find an atom with three non-PBC bonds - this marks a fully
                // "internal" atom which does not need (and should not be) translated, and the end of the linear branch we're
                // interested in. Along the way we maintain a fractional translation vector based on all PBC bonds we encounter
                // which will inform the direction we might want to move the branch. Or we could just remove it.

                Vector3 vFrac;
                std::vector<SpeciesAtom *> branch;
                extendBranch(i, species_.box(), vFrac, branch, localCutoff);
                auto maxEl = vFrac.absMaxElement();
                Vector3 tVec;
                tVec.set(maxEl, round(vFrac.get(maxEl) / fabs(vFrac.get(maxEl))));
                tVec.multiply(cellLengths);
                ;

                // Translate or remove the branch?
                for (auto j : branch)
                    if (ui_.RemoveBranchesCheck->isChecked())
                        indicesToRemove.push_back(j->index());
                    else
                        j->setCoordinates(j->r() + tVec);
            }

            // Final tidy-up - remove any branch atoms if required
            species_.removeAtoms(indicesToRemove);
        }

        species_.removeBox();
        species_.recalculateIntermolecularTerms(1.1);

        // Terminate?
        if (ui_.TerminateCheck->isChecked())
        {
            auto rT = ui_.TerminateBondLengthSpin->value();
            auto excludeA = !ui_.TerminateACheck->isChecked();
            auto excludeB = !ui_.TerminateBCheck->isChecked();

            // Any atom with just two bonds can be terminated
            for (auto &i : species_.atoms())
            {
                if ((i.nBonds() != 2) || (excludeA && i.Z() == zA_) || (excludeB && i.Z() == zB_))
                    continue;

                // Get average bond vector
                auto v = (i.r() - i.bond(0).partner(&i)->r()) + (i.r() - i.bond(1).partner(&i)->r());
                v.normalise();

                species_.addAtom(zTerminate_, i.r() + v * rT);
            }

            species_.recalculateIntermolecularTerms(1.1);
        }
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
void CreateGrapheneSpeciesDialog::updateWidgets()
{
    Locker updateLock(widgetsUpdating_);

    // Sheet properties
    ui_.ALabel->setText(QString("%1 \u212B").arg(A_, 0, 'f', 3));
    ui_.CLabel->setText(QString("%1 (%2) \u212B").arg(c_, 0, 'f', 3).arg(c_ * ui_.CFactorSpin->value(), 0, 'f', 3));
    ui_.AlphaLabel->setText(QString("%1\u00B0").arg(DissolveMath::toDegrees(alpha_), 0, 'f', 3));
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

void CreateGrapheneSpeciesDialog::on_NSpin_valueChanged(int value)
{
    if (widgetsUpdating_.isLocked())
        return;

    // MSpin's upper value is our current value
    ui_.MSpin->setMaximum(value);

    calculateParameters();
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_MSpin_valueChanged(int value)
{
    calculateParameters();
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_ElementAButton_clicked(bool checked)
{
    auto Z = selectElementDialog_.selectElement(zA_);
    if (Z == Elements::Unknown)
        return;
    zA_ = Z;
    ui_.ElementAButton->setText(QString::fromStdString(std::string(Elements::symbol(zA_))));

    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_ElementBButton_clicked(bool checked)
{
    auto Z = selectElementDialog_.selectElement(zB_);
    if (Z == Elements::Unknown)
        return;
    zB_ = Z;
    ui_.ElementBButton->setText(QString::fromStdString(std::string(Elements::symbol(zB_))));

    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_BondLengthSpin_valueChanged(double value)
{
    calculateParameters();
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_CFactorSpin_valueChanged(int value)
{
    calculateParameters();
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_RollUpCheck_clicked(bool checked)
{
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_PeriodicRadio_clicked(bool checked)
{
    if (widgetsUpdating_)
        return;

    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_PseudoPeriodicRadio_clicked(bool checked)
{
    if (widgetsUpdating_)
        return;
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_NonPeriodicRadio_clicked(bool checked)
{
    if (widgetsUpdating_)
        return;
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_TidyEndsCheck_clicked(bool checked)
{
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_RemoveBranchesCheck_clicked(bool checked)
{
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_TerminateCheck_clicked(bool checked)
{
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_TerminateElementButton_clicked(bool checked)
{
    auto Z = selectElementDialog_.selectElement(zB_);
    if (Z == Elements::Unknown)
        return;
    zTerminate_ = Z;
    ui_.TerminateElementButton->setText(QString::fromStdString(std::string(Elements::symbol(zTerminate_))));

    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_TerminateBondLengthSpin_valueChanged(double value)
{
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_TerminateACheck_clicked(bool checked)
{
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_TerminateBCheck_clicked(bool checked)
{
    regenerate();
    updateWidgets();
}

void CreateGrapheneSpeciesDialog::on_OKButton_clicked(bool checked)
{
    // Copy the species to the main Dissolve instance and set its new name
    auto *sp = dissolve_.coreData().copySpecies(&species_);
    sp->setName(std::format("Nanotube-({},{})", ui_.NSpin->value(), ui_.MSpin->value()));

    accept();
}

void CreateGrapheneSpeciesDialog::on_CancelButton_clicked(bool checked) { reject(); }