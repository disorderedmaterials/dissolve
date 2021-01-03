// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "math/matrix3.h"
#include <memory>

// Forward Declarations
class Molecule;

// Site Definition
class Site
{
    public:
    Site(std::shared_ptr<const Molecule> molecule = nullptr, Vec3<double> origin = Vec3<double>());
    virtual ~Site();

    /*
     * Site Definition
     */
    protected:
    // Site origin
    Vec3<double> origin_;
    // Molecule to which site is related (if relevant)
    std::shared_ptr<const Molecule> molecule_;

    public:
    // Return site origin
    const Vec3<double> &origin() const;
    // Return Molecule to which site is related (if relevant)
    std::shared_ptr<const Molecule> molecule() const;
    // Return whether local axes are present
    virtual bool hasAxes() const;
    // Return local axes
    virtual const Matrix3 &axes() const;
};

// Oriented Site Definition
class OrientedSite : public Site
{
    public:
    OrientedSite(std::shared_ptr<const Molecule> molecule = nullptr, Vec3<double> origin = Vec3<double>(),
                 Vec3<double> xAxis = Vec3<double>(), Vec3<double> yAxis = Vec3<double>(), Vec3<double> zAxis = Vec3<double>());
    ~OrientedSite();

    /*
     * Site Definition
     */
    private:
    // Local axes
    Matrix3 axes_;

    public:
    // Axis Enum
    enum SiteAxis
    {
        XAxis = 0,
        YAxis = 1,
        ZAxis = 2
    };
    // Return enum options for SiteAxis
    static EnumOptions<OrientedSite::SiteAxis> siteAxis();
    // Return whether local axes are present
    bool hasAxes() const;
    // Return local axes
    const Matrix3 &axes() const;
};
