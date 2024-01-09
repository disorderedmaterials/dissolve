// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/site.h"

#include "base/messenger.h"
#include <utility>

/*
 * Site
 */

Site::Site(const SpeciesSite *parent, std::optional<int> uniqueSiteIndex, std::shared_ptr<const Molecule> molecule,
           Vec3<double> origin)
{
    parent_ = parent;
    uniqueSiteIndex_ = uniqueSiteIndex;
    molecule_ = std::move(molecule);
    origin_ = origin;
}

// Return the parent
const SpeciesSite *Site::parent() const { return parent_; }

// Return the unique site index in the parent
std::optional<int> Site::uniqueSiteIndex() const { return uniqueSiteIndex_; }

// Return site origin
const Vec3<double> &Site::origin() const { return origin_; }

// Return Molecule to which site is related (if relevant)
std::shared_ptr<const Molecule> Site::molecule() const { return molecule_; }

// Return whether local axes are present
bool Site::hasAxes() const { return false; }

// Return local axes
const Matrix3 &Site::axes() const
{
    static Matrix3 dummy;
    Messenger::warn("Returning empty axes for this Site, since it doesn't have any.\n");
    return dummy;
}

/*
 * Oriented Site
 */

OrientedSite::OrientedSite(const SpeciesSite *parent, std::optional<int> uniqueSiteIndex,
                           std::shared_ptr<const Molecule> molecule, Vec3<double> origin, Vec3<double> xAxis,
                           Vec3<double> yAxis, Vec3<double> zAxis)
    : Site(parent, uniqueSiteIndex, std::move(molecule), origin)
{
    axes_.setColumn(0, xAxis);
    axes_.setColumn(1, yAxis);
    axes_.setColumn(2, zAxis);
}

// Return whether local axes are present
bool OrientedSite::hasAxes() const { return true; }

// Return enum options for SiteAxis
EnumOptions<OrientedSite::SiteAxis> OrientedSite::siteAxis()
{
    return EnumOptions<OrientedSite::SiteAxis>(
        "SiteAxis", {{OrientedSite::XAxis, "X"}, {OrientedSite::YAxis, "Y"}, {OrientedSite::ZAxis, "Z"}});
}

// Return local axes
const Matrix3 &OrientedSite::axes() const { return axes_; }
