// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/site.h"

#include "base/messenger.h"
#include <utility>

/*
 * Site
 */

Site::Site(std::shared_ptr<const Molecule> molecule, Vec3<double> origin)
{
    molecule_ = std::move(molecule);
    origin_ = origin;
}

Site::~Site() = default;

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

OrientedSite::OrientedSite(std::shared_ptr<const Molecule> molecule, Vec3<double> origin, Vec3<double> xAxis,
                           Vec3<double> yAxis, Vec3<double> zAxis)
    : Site(std::move(molecule), origin)
{
    axes_.setColumn(0, xAxis);
    axes_.setColumn(1, yAxis);
    axes_.setColumn(2, zAxis);
}

OrientedSite::~OrientedSite() = default;

// Return whether local axes are present
bool OrientedSite::hasAxes() const { return true; }

// Return enum options for SiteAxis
EnumOptions<OrientedSite::SiteAxis> OrientedSite::siteAxis()
{
    return EnumOptions<OrientedSite::SiteAxis>(
        "SiteAxis", {{OrientedSite::XAxis, "XAxis"}, {OrientedSite::YAxis, "YAxis"}, {OrientedSite::ZAxis, "ZAxis"}});
}

// Return local axes
const Matrix3 &OrientedSite::axes() const { return axes_; }
