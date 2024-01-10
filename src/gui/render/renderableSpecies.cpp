// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/render/renderableSpecies.h"
#include "base/lineParser.h"
#include "classes/atom.h"
#include "data/elementColours.h"
#include "gui/render/renderableConfiguration.h"
#include "gui/render/renderableGroupManager.h"
#include "gui/render/view.h"

RenderableSpecies::RenderableSpecies(const Species *source, std::string_view name)
    : Renderable(Renderable::SpeciesRenderable, name), source_(source)
{
    // Set defaults
    displayStyle_ = SpheresStyle;
    linesAtomRadius_ = 0.05;
    spheresAtomRadius_ = 0.3;
    spheresBondRadius_ = 0.1;

    // Create basic primitives
    atomPrimitive_ = createPrimitive(GL_TRIANGLES, false);
    atomPrimitive_->sphere(1.0, 8, 10);
    selectedAtomPrimitive_ = createPrimitive(GL_LINES, false);
    selectedAtomPrimitive_->sphere(1.25, 8, 10);
    bondPrimitive_ = createPrimitive(GL_TRIANGLES, false);
    bondPrimitive_->cylinder(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1, 8);
    crossBoxPrimitive_ = createPrimitive(GL_LINES, false);
    crossBoxPrimitive_->wireOrthorhomboid(1.0, 1.0, 1.0, 0.0, 0.0);
    lineSpeciesPrimitive_ = createPrimitive(GL_LINES, true);
    lineSelectionPrimitive_ = createPrimitive(GL_LINES, true);
    lineSelectionPrimitive_->setNoInstances();
    lineInteractionPrimitive_ = createPrimitive(GL_LINES, true);
    lineInteractionPrimitive_->setNoInstances();
    unitCellPrimitive_ = createPrimitive(GL_LINES, false);
    unitCellPrimitive_->wireOrthorhomboid(1.0, 1.0, 1.0, 0.5, 0.5, 0.5);

    // Set versions
    selectionPrimitiveVersion_ = -1;
}

RenderableSpecies::~RenderableSpecies() {}

/*
 * Data
 */

// Return version of data
int RenderableSpecies::dataVersion() { return (source_ ? source_->version() : -99); }

/*
 * Transform / Limits
 */

// Transform data according to current settings
void RenderableSpecies::transformValues()
{
    if (!source_)
        return;

    // If the transformed data are already up-to-date, no need to do anything
    if (valuesTransformDataVersion_ == dataVersion())
        return;

    // For periodic systems use the box - otherwise, loop over Atoms seeking extreme x, y, and z values
    if (source_->box()->type() != Box::BoxType::NonPeriodic)
    {
        // Minimum corresponds to lower left corner of the box at {0,0,0}
        limitsMin_.zero();

        // Transform extreme upper right corner from unit to real space to get maxima
        limitsMax_ = source_->box()->getReal(Vec3<double>(1.0, 1.0, 1.0));
    }
    else if (source_->nAtoms() > 0)
    {
        limitsMin_ = source_->atoms()[0].r();
        limitsMax_ = source_->atoms()[0].r();

        for (const auto &i : source_->atoms())
        {
            if (i.r().x < limitsMin_.x)
                limitsMin_.x = i.r().x;
            else if (i.r().x > limitsMax_.x)
                limitsMax_.x = i.r().x;
            if (i.r().y < limitsMin_.y)
                limitsMin_.y = i.r().y;
            else if (i.r().y > limitsMax_.y)
                limitsMax_.y = i.r().y;
            if (i.r().z < limitsMin_.z)
                limitsMin_.z = i.r().z;
            else if (i.r().z > limitsMax_.z)
                limitsMax_.z = i.r().z;
        }

        // Need to add on a little extra to the limits since the atoms have a radius
        limitsMin_ -= 1.0;
        limitsMax_ += 1.0;
    }
    else
    {
        limitsMin_ = -1.0;
        limitsMax_ = 1.0;
    }

    positiveLimitsMin_.x = limitsMin_.x < 0.0 ? 0.01 : limitsMin_.x;
    positiveLimitsMin_.y = limitsMin_.y < 0.0 ? 0.01 : limitsMin_.y;
    positiveLimitsMin_.z = limitsMin_.z < 0.0 ? 0.01 : limitsMin_.z;
    positiveLimitsMax_.x = limitsMax_.x < 0.0 ? 1.0 : limitsMax_.x;
    positiveLimitsMax_.y = limitsMax_.y < 0.0 ? 1.0 : limitsMax_.y;
    positiveLimitsMax_.z = limitsMax_.z < 0.0 ? 1.0 : limitsMax_.z;

    // Update the transformed data 'version'
    valuesTransformDataVersion_ = dataVersion();
}

/*
 * Rendering Primitives
 */

// Recreate necessary primitives / primitive assemblies for the data
void RenderableSpecies::recreatePrimitives(const View &view, const ColourDefinition &colourDefinition)
{
    Matrix4 A;
    const std::array<float, 4> colourBlack = {0.0, 0.0, 0.0, 1.0};

    // Clear existing data
    lineSpeciesPrimitive_->forgetAll();
    lineSelectionPrimitive_->forgetAll();
    speciesAssembly_.clear();
    selectionAssembly_.clear();
    unitCellAssembly_.clear();

    // Check data source
    if (!source_)
        return;

    // Render according to the current displayStyle
    if (displayStyle_ == LinesStyle)
    {
        // Set basic styling and content for assemblies
        speciesAssembly_.add(LineStyle(1.0));
        speciesAssembly_.add(false, GL_LINE);
        speciesAssembly_.add(lineSpeciesPrimitive_, A);

        // Draw Atoms
        for (const auto &i : source_->atoms())
        {
            // Only draw the atom if it has no bonds, in which case draw it as a 'cross'
            if (i.nBonds() != 0)
                continue;

            const auto r = i.r();
            auto &colour = ElementColours::colour(i.Z());

            lineSpeciesPrimitive_->line(r.x - linesAtomRadius_, r.y, r.z, r.x + linesAtomRadius_, r.y, r.z, colour.data());
            lineSpeciesPrimitive_->line(r.x, r.y - linesAtomRadius_, r.z, r.x, r.y + linesAtomRadius_, r.z, colour.data());
            lineSpeciesPrimitive_->line(r.x, r.y, r.z - linesAtomRadius_, r.x, r.y, r.z + linesAtomRadius_, colour.data());
        }

        // Draw bonds
        auto periodic = source_->box()->type() != Box::BoxType::NonPeriodic;
        for (const auto &bond : source_->bonds())
        {
            // Determine half delta i-j for bond
            const auto ri = bond.i()->r();
            const auto rj = bond.j()->r();
            const auto dij = (periodic ? source_->box()->minimumVector(ri, rj) : (rj - ri)) * 0.5;

            // Draw bond halves
            lineSpeciesPrimitive_->line(ri.x, ri.y, ri.z, ri.x + dij.x, ri.y + dij.y, ri.z + dij.z,
                                        ElementColours::colour(bond.i()->Z()).data());
            lineSpeciesPrimitive_->line(rj.x, rj.y, rj.z, rj.x - dij.x, rj.y - dij.y, rj.z - dij.z,
                                        ElementColours::colour(bond.j()->Z()).data());
        }
    }
    else if (displayStyle_ == SpheresStyle)
    {
        // Set basic styling for assemblies
        speciesAssembly_.add(true, GL_FILL);
        selectionAssembly_.add(true, GL_LINE);

        // Draw Atoms
        for (const auto &i : source_->atoms())
        {
            A.setIdentity();
            A.setTranslation(i.r());
            A.applyScaling(spheresAtomRadius_);

            // The atom itself
            speciesAssembly_.add(atomPrimitive_, A, ElementColours::colour(i.Z()));

            // Is the atom selected?
            if (i.isSelected())
                selectionAssembly_.add(selectedAtomPrimitive_, A, colourBlack);
        }

        // Draw bonds
        auto periodic = source_->box()->type() != Box::BoxType::NonPeriodic;
        for (const auto &bond : source_->bonds())
        {
            if (periodic)
                speciesAssembly_.createCylinderBond(
                    bondPrimitive_, bond.i()->r(), bond.j()->r(), source_->box()->minimumVector(bond.i()->r(), bond.j()->r()),
                    ElementColours::colour(bond.j()->Z()), ElementColours::colour(bond.i()->Z()), true, spheresBondRadius_);
            else
                speciesAssembly_.createCylinderBond(bondPrimitive_, bond.i()->r(), bond.j()->r(), bond.j()->r() - bond.i()->r(),
                                                    ElementColours::colour(bond.i()->Z()),
                                                    ElementColours::colour(bond.j()->Z()), false, spheresBondRadius_);
        }
    }

    // Add unit cell
    if (source_->box()->type() != Box::BoxType::NonPeriodic)
    {
        A.setIdentity();
        A = source_->box()->axes();
        unitCellAssembly_.add(unitCellPrimitive_, A, colourBlack);
    }
}

// Send primitives for rendering
const void RenderableSpecies::sendToGL(const double pixelScaling)
{
    // Set appropriate lighting for the species and interaction assemblies
    if (displayStyle_ == LinesStyle)
        glDisable(GL_LIGHTING);
    else
        glEnable(GL_LIGHTING);
    speciesAssembly_.sendToGL(pixelScaling);
    interactionAssembly_.sendToGL(pixelScaling);

    // Selection assembly is always drawn with no lighting as it comprises only of lines
    glDisable(GL_LIGHTING);
    selectionAssembly_.sendToGL(pixelScaling);

    // Draw unit cell
    unitCellAssembly_.sendToGL(pixelScaling);
}

// Recreate selection Primitive
void RenderableSpecies::recreateSelectionPrimitive()
{
    if (selectionPrimitiveVersion_ == source_->atomSelectionVersion())
        return;

    // Clear existing data
    selectionAssembly_.clear();
    lineSelectionPrimitive_->forgetAll();

    Matrix4 A;

    if (displayStyle_ == LinesStyle)
    {
        // Set basic styling for assemblies
        selectionAssembly_.add(false, GL_LINE);
        selectionAssembly_.add(LineStyle(3.0));
        selectionAssembly_.add(lineSelectionPrimitive_, A);

        // Draw selection
        for (const auto &i : source_->atoms())
        {
            // If not selected, continue
            if (!i.isSelected())
                continue;

            // Get element colour
            auto &colour = ElementColours::colour(i.Z());

            // If the atom has no bonds, draw it as a 'cross', otherwise render all bond halves
            if (i.nBonds() == 0)
            {
                const auto &r = i.r();

                lineSelectionPrimitive_->line(r.x - linesAtomRadius_, r.y, r.z, r.x + linesAtomRadius_, r.y, r.z,
                                              colour.data());
                lineSelectionPrimitive_->line(r.x, r.y - linesAtomRadius_, r.z, r.x, r.y + linesAtomRadius_, r.z,
                                              colour.data());
                lineSelectionPrimitive_->line(r.x, r.y, r.z - linesAtomRadius_, r.x, r.y, r.z + linesAtomRadius_,
                                              colour.data());
            }
            else
            {
                // Draw all bonds from this atom
                for (const SpeciesBond &bond : i.bonds())
                {
                    const auto ri = i.r();
                    const auto dij = (bond.partner(&i)->r() - ri) * 0.5;

                    // Draw bond halves
                    lineSelectionPrimitive_->line(ri.x, ri.y, ri.z, ri.x + dij.x, ri.y + dij.y, ri.z + dij.z, colour.data());
                }
            }
        }
    }
    else if (displayStyle_ == SpheresStyle)
    {
        // Set basic styling
        selectionAssembly_.add(true, GL_LINE);

        for (auto &i : source_->atoms())
        {
            if (!i.isSelected())
                continue;

            A.setIdentity();
            A.setTranslation(i.r());
            A.applyScaling(spheresAtomRadius_);

            selectionAssembly_.add(selectedAtomPrimitive_, A, 0.5, 0.5, 0.5, 1.0);
        }
    }

    selectionPrimitiveVersion_ = source_->atomSelectionVersion();
}

// Clear interaction Primitive
void RenderableSpecies::clearInteractionPrimitive()
{
    interactionAssembly_.clear();
    lineInteractionPrimitive_->forgetAll();
}

// Recreate interaction Primitive to display drawing interaction (from existing atom to existing atom)
void RenderableSpecies::recreateDrawInteractionPrimitive(SpeciesAtom *fromAtom, SpeciesAtom *toAtom)
{
    // Clear existing data
    clearInteractionPrimitive();

    Matrix4 A;

    // Render based on the current drawing style
    if (displayStyle_ == LinesStyle)
    {
        // Set basic styling for assembly
        interactionAssembly_.add(false, GL_LINE);
        interactionAssembly_.add(lineInteractionPrimitive_, A);

        // Determine half delta i-j for bond
        const auto ri = fromAtom->r();
        const auto rj = toAtom->r();
        const auto dij = (rj - ri) * 0.5;

        // Draw bond halves
        lineInteractionPrimitive_->line(ri.x, ri.y, ri.z, ri.x + dij.x, ri.y + dij.y, ri.z + dij.z,
                                        ElementColours::colour(fromAtom->Z()).data());
        lineInteractionPrimitive_->line(rj.x, rj.y, rj.z, rj.x - dij.x, rj.y - dij.y, rj.z - dij.z,
                                        ElementColours::colour(toAtom->Z()).data());
    }
    else if (displayStyle_ == SpheresStyle)
    {
        // Set basic styling for assembly
        interactionAssembly_.add(true, GL_FILL);

        // Draw the temporary bond between the atoms
        interactionAssembly_.createCylinderBond(bondPrimitive_, fromAtom->r(), toAtom->r(), toAtom->r() - fromAtom->r(),
                                                ElementColours::colour(fromAtom->Z()), ElementColours::colour(toAtom->Z()),
                                                true, spheresBondRadius_);
    }
}

// Recreate interaction Primitive to display drawing interaction (from existing atom to point)
void RenderableSpecies::recreateDrawInteractionPrimitive(SpeciesAtom *fromAtom, Vec3<double> toPoint,
                                                         Elements::Element toElement)
{
    // Clear existing data
    clearInteractionPrimitive();

    Matrix4 A;

    // Temporary SpeciesAtom
    static SpeciesAtom j;
    j.setZ(toElement);
    j.setCoordinates(toPoint);

    // Render based on the current drawing style
    if (displayStyle_ == LinesStyle)
    {
        // Set basic styling for assembly
        interactionAssembly_.add(false, GL_LINE);
        interactionAssembly_.add(lineInteractionPrimitive_, A);

        // Determine half delta i-j for bond
        const auto ri = fromAtom->r();
        const auto rj = j.r();
        const auto dij = (rj - ri) * 0.5;

        // Draw bond halves
        lineInteractionPrimitive_->line(ri.x, ri.y, ri.z, ri.x + dij.x, ri.y + dij.y, ri.z + dij.z,
                                        ElementColours::colour(fromAtom->Z()).data());
        lineInteractionPrimitive_->line(rj.x, rj.y, rj.z, rj.x - dij.x, rj.y - dij.y, rj.z - dij.z,
                                        ElementColours::colour(j.Z()).data());
    }
    else if (displayStyle_ == SpheresStyle)
    {
        // Set basic styling for assembly
        interactionAssembly_.add(true, GL_FILL);

        // Draw the temporary atom
        A.setTranslation(j.r());
        A.applyScaling(spheresAtomRadius_);
        interactionAssembly_.add(atomPrimitive_, A, ElementColours::colour(j.Z()));

        // Draw the temporary bond between the atoms
        interactionAssembly_.createCylinderBond(bondPrimitive_, fromAtom->r(), j.r(), j.r() - fromAtom->r(),
                                                ElementColours::colour(j.Z()), ElementColours::colour(fromAtom->Z()), true,
                                                spheresBondRadius_);
    }
}

// Recreate interaction Primitive to display drawing interaction (from point to point)
void RenderableSpecies::recreateDrawInteractionPrimitive(Vec3<double> fromPoint, Elements::Element fromElement,
                                                         Vec3<double> toPoint, Elements::Element toElement)
{
    // Clear existing data
    clearInteractionPrimitive();

    Matrix4 A;

    // Temporary SpeciesAtoms
    static SpeciesAtom i, j;
    i.setZ(fromElement);
    i.setCoordinates(fromPoint);
    j.setZ(toElement);
    j.setCoordinates(toPoint);

    // Render based on the current drawing style
    if (displayStyle_ == LinesStyle)
    {
        // Set basic styling for assembly
        interactionAssembly_.add(false, GL_LINE);
        interactionAssembly_.add(lineInteractionPrimitive_, A);

        // Determine half delta i-j for bond
        const auto ri = i.r();
        const auto rj = j.r();
        const auto dij = (rj - ri) * 0.5;

        // Draw bond halves
        lineInteractionPrimitive_->line(ri.x, ri.y, ri.z, ri.x + dij.x, ri.y + dij.y, ri.z + dij.z,
                                        ElementColours::colour(i.Z()).data());
        lineInteractionPrimitive_->line(rj.x, rj.y, rj.z, rj.x - dij.x, rj.y - dij.y, rj.z - dij.z,
                                        ElementColours::colour(j.Z()).data());
    }
    else if (displayStyle_ == SpheresStyle)
    {
        // Set basic styling for assembly
        interactionAssembly_.add(true, GL_FILL);

        // Draw the temporary atoms
        A.setTranslation(i.r());
        A.applyScaling(spheresAtomRadius_);
        interactionAssembly_.add(atomPrimitive_, A, ElementColours::colour(i.Z()));

        A.setIdentity();
        A.setTranslation(j.r());
        A.applyScaling(spheresAtomRadius_);
        interactionAssembly_.add(atomPrimitive_, A, ElementColours::colour(j.Z()));

        // Draw the temporary bond between the atoms
        interactionAssembly_.createCylinderBond(bondPrimitive_, i.r(), j.r(), j.r() - i.r(), ElementColours::colour(i.Z()),
                                                ElementColours::colour(j.Z()), true, spheresBondRadius_);
    }
}

// Recreate interaction Primitive to display delete interaction (from existing atom to existing atom)
void RenderableSpecies::recreateDeleteInteractionPrimitive(SpeciesAtom *fromAtom, SpeciesAtom *toAtom)
{
    // Clear existing data
    clearInteractionPrimitive();

    Matrix4 A;

    A.setTranslation(fromAtom->r());

    // Render based on the current drawing style
    if (displayStyle_ == LinesStyle)
    {
        A.applyScaling(0.1);

        // Set basic styling for assembly
        interactionAssembly_.add(false, GL_LINE);
        interactionAssembly_.add(crossBoxPrimitive_, A);

        if (toAtom)
        {
            A.setTranslation(toAtom->r());
            interactionAssembly_.add(crossBoxPrimitive_, A);
        }
    }
    else if (displayStyle_ == SpheresStyle)
    {
        // Set basic styling for assembly
        interactionAssembly_.add(true, GL_FILL);

        // Draw the indicators
        A.applyScaling(spheresAtomRadius_ * 2.2);
        interactionAssembly_.add(crossBoxPrimitive_, A, 0.0, 0.0, 0.0, 1.0);

        if (toAtom)
        {
            A.setTranslation(toAtom->r());
            interactionAssembly_.add(crossBoxPrimitive_, A, 0.0, 0.0, 0.0, 1.0);
        }
    }
}

/*
 * Style
 */

// Return EnumOptions for SpeciesDisplayStyle
EnumOptions<RenderableSpecies::SpeciesDisplayStyle> RenderableSpecies::speciesDisplayStyles()
{
    return EnumOptions<RenderableSpecies::SpeciesDisplayStyle>(
        "SpeciesDisplayStyle", {{RenderableSpecies::LinesStyle, "Lines"}, {RenderableSpecies::SpheresStyle, "Spheres"}});
}

// Set display style for renderable
void RenderableSpecies::setDisplayStyle(SpeciesDisplayStyle displayStyle)
{
    displayStyle_ = displayStyle;

    ++styleVersion_;
}

// Return display style for the renderable
RenderableSpecies::SpeciesDisplayStyle RenderableSpecies::displayStyle() const { return displayStyle_; }
