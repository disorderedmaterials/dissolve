// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "generator/addOnSphere.h"
#include "base/randomBuffer.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "generator/regionBase.h"
#include "keywords/bool.h"
#include "keywords/node.h"
#include "keywords/nodeValue.h"
#include "keywords/speciesSite.h"

AddOnSphereGeneratorNode::AddOnSphereGeneratorNode(const SpeciesSite *site, const NodeValue &population,
                                                   const NodeValue &density, Units::DensityUnits densityUnits)
    : AddGeneratorNodeBase(NodeType::AddOnSphere, population, density, densityUnits), speciesSite_(site)
{
    setUpKeywords();
}

// Set up keywords for node
void AddOnSphereGeneratorNode::setUpKeywords()
{
    keywords_.setOrganisation("Options", "Target");
    keywords_.add<SpeciesSiteKeyword>("Site", "Target species site to use as anchor point", speciesSite_, false);
    keywords_.add<NodeValueKeyword>("Radius", "Radius of the sphere upon which molecules will be placed", radius_, this);
    keywords_.add<EnumOptionsKeyword<AddOnSphereGeneratorNode::PointDistributionStyle>>(
        "Distribution", "Method for distributing points on the underlying sphere", pointDistributionStyle_,
        pointDistributionStyles());
    keywords_.add<BoolKeyword>("Orient", "Whether to orient the site on the surface", orient_);
    keywords_.add<EnumOptionsKeyword<Site::SiteAxis>>("Axis", "Site axis to orient along surface tangent", axis_,
                                                      Site::siteAxis());
    keywords_.add<NodeValueKeyword>("Variance", "Random variance to add to distributed points on the sphere", variance_, this);

    setUpBaseKeywords();
}

/*
 * Node Data
 */

// Return enum option info for PointDistributionStyle
EnumOptions<AddOnSphereGeneratorNode::PointDistributionStyle> AddOnSphereGeneratorNode::pointDistributionStyles()
{
    return EnumOptions<AddOnSphereGeneratorNode::PointDistributionStyle>(
        "BoxAction", {{AddOnSphereGeneratorNode::PointDistributionStyle::Random, "Random"},
                      {AddOnSphereGeneratorNode::PointDistributionStyle::Fibonacci, "Fibonacci"}});
}

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool AddOnSphereGeneratorNode::prepare(const GeneratorContext &generatorContext)
{
    if (!speciesSite_)
        return Messenger::error("No target species site specified in AddOnSphere node.\n");

    // Check site multiplicity
    if (speciesSite_->instances().size() > 1)
        return Messenger::error("Site must have a single instance - i.e. be unique in the parent species.\n");

    // Check for axes on the site if orientation was requested
    if (orient_ && !speciesSite_->hasAxes())
        return Messenger::error("Orientation requested, but the selected site has no defined axes.\n");

    return true;
}

// Execute node
bool AddOnSphereGeneratorNode::execute(const GeneratorContext &generatorContext)
{
    // Get target species
    auto *sp = speciesSite_->parent();

    auto ipop = population_.asInteger();
    if (ipop > 0)
        Messenger::print("[AddOnSphere] Adding species '{}' - population is {}.\n", sp->name(), ipop);
    else
    {
        Messenger::print("[AddOnSphere] Population of species '{}' is zero so it will not be added.\n", sp->name());
        return true;
    }

    auto *cfg = generatorContext.configuration();
    const auto *box = cfg->box();

    // Set / adjust target box volume
    adjustBoxVolume(cfg, ipop, sp->nAtoms(SpeciesAtom::Presence::Physical) + sp->nAtoms(SpeciesAtom::Presence::Physical),
                    sp->mass() + sp->mass());

    // Get the positioningType_ type and rotation flag
    Messenger::print("[AddOnSphere] Positioning type is '{}'.\n",
                     AddOnSphereGeneratorNode::positioningTypes().keyword(positioningType_));

    // Checks for regional positioning
    if (positioningType_ == AddGeneratorNode::PositioningType::Region)
    {
        if (!region_->region().isValid())
            return Messenger::error("Region '{}' is invalid, probably because it contains no free space.\n", region_->name());

        Messenger::print("[AddOnSphere] Target region ('{}') covers {:.2f}% of the box volume.\n", region_->name(),
                         region_->region().freeVoxelFraction() * 100.0);
    }

    RandomBuffer randomBuffer(generatorContext.processPool(), ProcessPool::PoolProcessesCommunicator);

    // Set / generate position of sphere centre
    Vec3<double> sphereCentre, fr;
    switch (positioningType_)
    {
        case (AddGeneratorNode::PositioningType::Random):
            sphereCentre = box->getReal({randomBuffer.random(), randomBuffer.random(), randomBuffer.random()});
            break;
        case (AddGeneratorNode::PositioningType::Region):
            sphereCentre = region_->region().randomCoordinate();
            break;
        case (AddGeneratorNode::PositioningType::Central):
            sphereCentre = box->getReal({0.5, 0.5, 0.5});
            break;
        case (AddGeneratorNode::PositioningType::Current):
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Positioning type {} not handled.\n", positioningTypes().keyword(positioningType_))));
    }

    // Add space for the new molecules
    cfg->atoms().reserve(cfg->atoms().size() + ipop * sp->nAtoms());

    // Get the first (only) instance details for the site
    auto siteInstance = speciesSite_->instances().front();

    // Now we add the molecules at points on the sphere
    Vec3<double> rLocal;
    const auto psi = 0.5 * (1.0 + sqrt(5.0));
    const auto r = radius_.asDouble();
    const auto variance = variance_.asDouble();
    for (auto n = 0; n < ipop; ++n)
    {
        // Add the Molecule
        auto mol = cfg->addMolecule(sp);

        // Calculate spherical coordinates of point in radians
        auto theta = 0.0, phi = 0.0;
        switch (pointDistributionStyle_)
        {
            case (PointDistributionStyle::Random):
                theta = randomBuffer.random() * M_PI;
                phi = randomBuffer.random() * 2.0 * M_PI;
                break;
            case (PointDistributionStyle::Fibonacci):
                // Map theta as (i+1)/(N+1) to avoid point at pole and give better distribution
                theta = asin(2.0 * (double(n + 1.0) / (ipop + 1.0)) - 1.0) + M_PI_2;
                phi = 2.0 * M_PI * n / psi;
                break;
            default:
                throw(std::runtime_error("PointDistributionStyle not handled in switch.\n"));
        }

        // Apply variance
        if (variance > 0.0)
        {
            theta += randomBuffer.randomPlusMinusOne() * variance;
            phi += randomBuffer.randomPlusMinusOne() * 2.0 * variance;
        }

        // Calculate cartesian coordinates
        rLocal.set(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        rLocal *= r;

        // Generate a working site from the molecule
        Site site(speciesSite_, {}, mol, siteInstance, box);

        // Locate the site on the sphere
        mol->translate((sphereCentre + rLocal) - site.origin());

        // Orient the site?
        if (orient_)
        {
            // Set up the target matrix
            Matrix3 target;
            target.createFromVector(rLocal.normalised(), axis_);

            // Copy the site matrix and invert it
            Matrix3 source = site.axes();
            source.invert();

            mol->transform(box, target * source, sphereCentre + rLocal);
        }
    }

    Messenger::print("[AddOnSphere] New box density is {:e} atoms/Angstrom**3 ({} g/cm3).\n",
                     cfg->atomicDensity().value_or(0.0), cfg->chemicalDensity().value_or(0.0));

    // We've added new content to the box, so need to update our object relationships
    cfg->updateObjectRelationships();

    return true;
}
