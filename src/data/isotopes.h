// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include "templates/array.h"
#include "templates/list.h"

// Isotopic Neutron Scattering Data
class Isotope : public ListItem<Isotope>
{
    public:
    Isotope(Elements::Element Z = Elements::Unknown, int A = 0, std::string_view spin = "", double mass = 0.0, double bc = 0.0,
            double bi = 0.0, double sc = 0.0, double si = 0.0, double totalxs = 0.0, double absxs = 0.0);
    Isotope &operator=(const Isotope &source);

    /*
     * Isotope Data
     */
    private:
    // Element for isotope
    Elements::Element Z_;
    // Mass number (A) of isotope
    int A_;
    // Isotope mass(given C = 12)
    double mass_;
    // Nuclear spin description
    std::string spin_;
    // Bound coherent scattering length (fm)
    double boundCoherent_;
    // Bound incoherent scattering length (fm)
    double boundIncoherent_;
    // Bound coherent scattering cross section (barn)
    double boundCoherentXS_;
    // Bound incoherent scattering cross section (barn)
    double boundIncoherentXS_;
    // Total bound scattering cross section (barn)
    double totalXS_;
    // Absorption cross section for thermal (2200 m/s) neutron (barn)
    double absorptionXS_;

    public:
    // Return index of isotope in it's Element parent's list
    int index() const;
    // Return element (Z) of Isotope
    Elements::Element Z() const;
    // Return mass number (A) of Isotope
    int A() const;
    // Return isotope mass (given C = 12)
    double mass() const;
    // Return bound coherent scattering length (fm)
    double boundCoherent() const;
    // Return bound incoherent scattering length (fm)
    double boundIncoherent() const;
    // Return bound coherent scattering cross section (barn)
    double boundCoherentXS() const;
    // Return bound incoherent scattering cross section (barn)
    double boundIncoherentXS() const;
    // Return total bound scattering cross section (barn)
    double totalXS() const;
    // Return absorption cross section for thermal (2200 m/s) neutron (barn)
    double absorptionXS() const;
};

// Sears '91 Isotope Data
class Isotopes
{
    private:
    // Isotope data, grouped by element
    static Array<List<Isotope>> isotopesByElementPrivate_;

    private:
    // Return isotope data for specified Element
    static List<Isotope> &isotopesByElement(Elements::Element Z);

    public:
    // Register specified Isotope to given Element
    static void registerIsotope(Isotope *isotope, Elements::Element Z);
    // Return Isotope with specified A (if it exists) for given element
    static Isotope *isotope(Elements::Element Z, int A);
    // Return Isotope with specified index (if it exists) in its parent Element
    static Isotope *isotopeAtIndex(Elements::Element Z, int index);
    // Return List of all Isotopes available for specified Element
    static const List<Isotope> &isotopes(Elements::Element Z);
    // Return natural Isotope for given Element
    static Isotope *naturalIsotope(Elements::Element Z);
};
