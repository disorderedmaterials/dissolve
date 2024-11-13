// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/site.h"

#include "base/messenger.h"
#include <utility>

Site::Site(const SpeciesSite *parent, std::optional<int> uniqueSiteIndex, std::shared_ptr<const Molecule> molecule,
           const Vec3<double> &origin)
    : parent_(parent), uniqueSiteIndex_(uniqueSiteIndex), molecule_(std::move(molecule)), origin_(origin)
{
}

Site::Site(const SpeciesSite *parent, std::optional<int> uniqueSiteIndex, std::shared_ptr<const Molecule> molecule,
           const Matrix3 &axes, const Vec3<double> &origin)
    : parent_(parent), uniqueSiteIndex_(uniqueSiteIndex), molecule_(std::move(molecule)), origin_(origin), axes_(axes),
      hasAxes_(true)
{
}

// Return enum options for SiteAxis
EnumOptions<Site::SiteAxis> Site::siteAxis()
{
    return EnumOptions<Site::SiteAxis>("SiteAxis", {{Site::XAxis, "X"}, {Site::YAxis, "Y"}, {Site::ZAxis, "Z"}});
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
bool Site::hasAxes() const { return hasAxes_; }

// Return local axes
const Matrix3 &Site::axes() const { return axes_; }
