// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/render/renderableconfiguration.h"
#include "base/lineparser.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "data/elementcolours.h"
#include "gui/render/renderablegroupmanager.h"
#include "gui/render/view.h"

RenderableConfiguration::RenderableConfiguration(const Configuration *source)
    : Renderable(Renderable::ConfigurationRenderable, "UNUSED"), source_(source)
{
    // Set defaults
    displayStyle_ = LinesStyle;
    linesAtomRadius_ = 0.05;
    spheresAtomRadius_ = 0.3;
    spheresBondRadius_ = 0.1;

    // Create basic primitives
    atomPrimitive_ = createPrimitive(GL_TRIANGLES, false);
    atomPrimitive_->sphere(1.0, 8, 10);
    bondPrimitive_ = createPrimitive(GL_TRIANGLES, false);
    bondPrimitive_ = createPrimitive(GL_TRIANGLES, false);
    bondPrimitive_->cylinder(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1, 8);
    unitCellPrimitive_ = createPrimitive(GL_LINES, false);
    unitCellPrimitive_->wireOrthorhomboid(1.0, 1.0, 1.0, 0.5, 0.5, 0.5);
    lineConfigurationPrimitive_ = createPrimitive(GL_LINES, true);
    lineInteractionPrimitive_ = createPrimitive(GL_LINES, true);
    lineInteractionPrimitive_->setNoInstances();
}

RenderableConfiguration::~RenderableConfiguration() {}

/*
 * Data
 */

// Return version of data
int RenderableConfiguration::dataVersion() { return (source_ ? source_->contentsVersion() : -99); }

/*
 * Transform / Limits
 */

// Transform data according to current settings
void RenderableConfiguration::transformValues()
{
    if (!source_)
        return;

    // If the transformed data are already up-to-date, no need to do anything
    if (valuesTransformDataVersion_ == dataVersion())
        return;

    // Minimum corresponds to lower left corner of the box at {0,0,0}
    limitsMin_.zero();

    // Transform extreme upper right corner from unit to real space to get maxima
    limitsMax_ = source_->box()->getReal(Vec3<double>(1.0, 1.0, 1.0));

    positiveLimitsMin_ = limitsMin_;
    positiveLimitsMax_ = limitsMax_;

    // Update the transformed data 'version'
    valuesTransformDataVersion_ = dataVersion();
}

/*
 * Rendering Primitives
 */

// Recreate necessary primitives / primitive assemblies for the data
void RenderableConfiguration::recreatePrimitives(const View &view, const ColourDefinition &colourDefinition)
{
    Matrix4 A;
    const std::array<float, 4> colourBlack = {0.0, 0.0, 0.0, 1.0};
    Vec3<double> ri, rj;

    // Check data source
    if (!source_)
        return;

    // Clear existing data
    lineConfigurationPrimitive_->forgetAll();
    configurationAssembly_.clear();
    unitCellAssembly_.clear();

    // Grab the Configuration's Box and CellArray
    const auto *box = source_->box();

    // Render according to the current displayStyle
    if (displayStyle_ == LinesStyle)
    {
        // Set basic styling and content for assemblies
        configurationAssembly_.add(false, GL_LINE);
        configurationAssembly_.add(lineConfigurationPrimitive_, A);

        // Draw Atoms
        for (const auto &i : source_->atoms())
        {
            // If the atom has no bonds draw it as a 'cross'
            if (i->speciesAtom()->nBonds() == 0)
            {
                const auto r = i->r();
                auto &colour = ElementColours::colour(i->speciesAtom()->Z());

                lineConfigurationPrimitive_->line(r.x - linesAtomRadius_, r.y, r.z, r.x + linesAtomRadius_, r.y, r.z,
                                                  colour.data());
                lineConfigurationPrimitive_->line(r.x, r.y - linesAtomRadius_, r.z, r.x, r.y + linesAtomRadius_, r.z,
                                                  colour.data());
                lineConfigurationPrimitive_->line(r.x, r.y, r.z - linesAtomRadius_, r.x, r.y, r.z + linesAtomRadius_,
                                                  colour.data());
            }
            else
            {
                // Draw all bonds from this atom
                for (const SpeciesBond &bond : i->speciesAtom()->bonds())
                {
                    // Blindly get partner Atom 'j' - don't check if it is the true partner, only if it is
                    // the same as 'i' (in which case we skip it, ensuring we draw every bond only once)
                    auto partner = i->molecule()->atom(bond.indexJ());
                    if (i == partner)
                        continue;

                    ri = i->r();
                    rj = partner->r();

                    // Determine half delta i-j for bond
                    const auto dij =
                        (source_->cells().minimumImageRequired(*i->cell(), *partner->cell()) ? box->minimumVector(ri, rj)
                                                                                             : rj - ri) *
                        0.5;

                    // Draw bond halves
                    lineConfigurationPrimitive_->line(ri.x, ri.y, ri.z, ri.x + dij.x, ri.y + dij.y, ri.z + dij.z,
                                                      ElementColours::colour(bond.i()->Z()).data());
                    lineConfigurationPrimitive_->line(rj.x, rj.y, rj.z, rj.x - dij.x, rj.y - dij.y, rj.z - dij.z,
                                                      ElementColours::colour(bond.j()->Z()).data());
                }
            }
        }
    }
    else if (displayStyle_ == SpheresStyle)
    {
        // Set basic styling for assemblies
        configurationAssembly_.add(true, GL_FILL);

        // Draw Atoms
        for (const auto i : source_->atoms())
        {
            A.setIdentity();
            A.setTranslation(i->r());
            A.applyScaling(spheresAtomRadius_);

            // The atom itself
            configurationAssembly_.add(atomPrimitive_, A, ElementColours::colour(i->speciesAtom()->Z()));

            // Bonds from this atom
            for (const SpeciesBond &bond : i->speciesAtom()->bonds())
            {
                // Blindly get partner Atom 'j' - don't check if it is the true partner, only if it is the same
                // as 'i' (in which case we skip it, ensuring we draw every bond only once)
                auto partner = i->molecule()->atom(bond.indexJ());
                if (i == partner)
                    continue;

                if (source_->cells().minimumImageRequired(*i->cell(), *partner->cell()))
                    configurationAssembly_.createCylinderBond(
                        bondPrimitive_, i->r(), partner->r(), box->minimumVector(i->r(), partner->r()),
                        ElementColours::colour(i->speciesAtom()->Z()), ElementColours::colour(partner->speciesAtom()->Z()),
                        true, spheresBondRadius_);
                else
                    configurationAssembly_.createCylinderBond(bondPrimitive_, i->r(), partner->r(), partner->r() - i->r(),
                                                              ElementColours::colour(i->speciesAtom()->Z()),
                                                              ElementColours::colour(partner->speciesAtom()->Z()), false,
                                                              spheresBondRadius_);
            }
        }
    }

    // Add unit cell
    A.setIdentity();
    A = source_->box()->axes();
    unitCellAssembly_.add(unitCellPrimitive_, A, colourBlack);
}

// Send primitives for rendering
const void RenderableConfiguration::sendToGL(const double pixelScaling)
{
    // Set appropriate lighting for the configuration and interaction assemblies
    if (displayStyle_ == LinesStyle)
        glDisable(GL_LIGHTING);
    else
        glEnable(GL_LIGHTING);
    configurationAssembly_.sendToGL(pixelScaling);
    interactionAssembly_.sendToGL(pixelScaling);

    // Draw unit cell
    glDisable(GL_LIGHTING);
    unitCellAssembly_.sendToGL(pixelScaling);
}

/*
 * Style
 */

// Return EnumOptions for ConfigurationDisplayStyle
EnumOptions<RenderableConfiguration::ConfigurationDisplayStyle> RenderableConfiguration::configurationDisplayStyles()
{
    return EnumOptions<RenderableConfiguration::ConfigurationDisplayStyle>(
        "ConfigurationDisplayStyle",
        {{RenderableConfiguration::LinesStyle, "Lines"}, {RenderableConfiguration::SpheresStyle, "Spheres"}});
}

// Set display style for renderable
void RenderableConfiguration::setDisplayStyle(ConfigurationDisplayStyle displayStyle)
{
    displayStyle_ = displayStyle;

    ++styleVersion_;
}

// Return display style for the renderable
RenderableConfiguration::ConfigurationDisplayStyle RenderableConfiguration::displayStyle() const { return displayStyle_; }

/*
 * Style I/O
 */

// Return enum option info for RenderableKeyword
EnumOptions<RenderableConfiguration::ConfigurationStyleKeyword> RenderableConfiguration::configurationStyleKeywords()
{
    return EnumOptions<RenderableConfiguration::ConfigurationStyleKeyword>(
        "ConfigurationStyleKeyword",
        {{RenderableConfiguration::DisplayKeyword, "Display", 1}, {RenderableConfiguration::EndStyleKeyword, "EndStyle"}});
}

// Write style information
bool RenderableConfiguration::writeStyleBlock(LineParser &parser, int indentLevel) const
{
    // Construct indent string
    char *indent = new char[indentLevel * 2 + 1];
    for (auto n = 0; n < indentLevel * 2; ++n)
        indent[n] = ' ';
    indent[indentLevel * 2] = '\0';

    if (!parser.writeLineF("{}{}  {}\n", indent, configurationStyleKeywords().keyword(RenderableConfiguration::DisplayKeyword),
                           configurationDisplayStyles().keyword(displayStyle_)))
        return false;

    return true;
}

// Read style information
bool RenderableConfiguration::readStyleBlock(LineParser &parser)
{
    while (!parser.eofOrBlank())
    {
        // Get line from file
        if (parser.getArgsDelim(LineParser::SemiColonLineBreaks) != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!configurationStyleKeywords().isValid(parser.argsv(0)))
            return configurationStyleKeywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = configurationStyleKeywords().enumeration(parser.argsv(0));
        if (!configurationStyleKeywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            // Display style
            case (RenderableConfiguration::DisplayKeyword):
                if (!configurationDisplayStyles().isValid(parser.argsv(1)))
                    return configurationDisplayStyles().errorAndPrintValid(parser.argsv(1));
                displayStyle_ = configurationDisplayStyles().enumeration(parser.argsv(1));
                break;
            // End of block
            case (RenderableConfiguration::EndStyleKeyword):
                return true;
            // Unrecognised Keyword
            default:
                Messenger::warn("Unrecognised display style keyword for RenderableConfiguration: {}\n", parser.argsv(0));
                return false;
                break;
        }
    }

    return true;
}
