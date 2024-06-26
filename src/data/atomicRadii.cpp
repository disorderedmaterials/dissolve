// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/atomicRadii.h"
#include "base/messenger.h"
#include <vector>

namespace AtomicRadii
{

/*
 * Atomic Radius Data
 *
 * Source: "Covalent radii revisited",
 *      B. Cordero, V. Gómez, A. E. Platero-Prats, M. Revés, J. Echeverría, E. Cremades, F. Barragán and S. Alvarez
 *      Dalton Trans., 2008 (DOI: https://dx.doi.org/10.1039/b801115j)
 *      Notes: High-Spin radii taken for 1st transition elements, sp3 value taken for Carbon. Bk and beyond set to 1.50.
 */

constexpr std::pair<int, double> radiusData_[] = {
    {Elements::Unknown, 0.00}, {Elements::H, 0.31},  {Elements::He, 0.28}, {Elements::Li, 1.28}, {Elements::Be, 0.96},
    {Elements::B, 0.84},       {Elements::C, 0.76},  {Elements::N, 0.71},  {Elements::O, 0.66},  {Elements::F, 0.57},
    {Elements::Ne, 0.58},      {Elements::Na, 1.66}, {Elements::Mg, 1.41}, {Elements::Al, 1.21}, {Elements::Si, 1.11},
    {Elements::P, 1.07},       {Elements::S, 1.05},  {Elements::Cl, 1.02}, {Elements::Ar, 1.06}, {Elements::K, 2.03},
    {Elements::Ca, 1.76},      {Elements::Sc, 1.70}, {Elements::Ti, 1.60}, {Elements::V, 1.53},  {Elements::Cr, 1.39},
    {Elements::Mn, 1.61},      {Elements::Fe, 1.52}, {Elements::Co, 1.50}, {Elements::Ni, 1.24}, {Elements::Cu, 1.32},
    {Elements::Zn, 1.22},      {Elements::Ga, 1.22}, {Elements::Ge, 1.20}, {Elements::As, 1.19}, {Elements::Se, 1.20},
    {Elements::Br, 1.20},      {Elements::Kr, 1.16}, {Elements::Rb, 2.20}, {Elements::Sr, 1.95}, {Elements::Y, 1.90},
    {Elements::Zr, 1.75},      {Elements::Nb, 1.64}, {Elements::Mo, 1.54}, {Elements::Tc, 1.47}, {Elements::Ru, 1.46},
    {Elements::Rh, 1.42},      {Elements::Pd, 1.39}, {Elements::Ag, 1.45}, {Elements::Cd, 1.44}, {Elements::In, 1.42},
    {Elements::Sn, 1.39},      {Elements::Sb, 1.39}, {Elements::Te, 1.38}, {Elements::I, 1.39},  {Elements::Xe, 1.40},
    {Elements::Cs, 2.44},      {Elements::Ba, 2.15}, {Elements::La, 2.07}, {Elements::Ce, 2.04}, {Elements::Nd, 2.01},
    {Elements::Pm, 1.99},      {Elements::Sm, 1.98}, {Elements::Eu, 1.98}, {Elements::Gd, 1.96}, {Elements::Tb, 1.94},
    {Elements::Dy, 1.92},      {Elements::Ho, 1.92}, {Elements::Er, 1.89}, {Elements::Tm, 1.90}, {Elements::Yb, 1.87},
    {Elements::Lu, 1.87},      {Elements::Hf, 1.75}, {Elements::Ta, 1.70}, {Elements::W, 1.62},  {Elements::Re, 1.51},
    {Elements::Os, 1.44},      {Elements::Ir, 1.41}, {Elements::Pt, 1.36}, {Elements::Au, 1.36}, {Elements::Hg, 1.32},
    {Elements::Tl, 1.45},      {Elements::Pb, 1.46}, {Elements::Bi, 1.48}, {Elements::Po, 1.40}, {Elements::At, 1.50},
    {Elements::Rn, 1.50},      {Elements::Fr, 2.60}, {Elements::Ra, 2.21}, {Elements::Ac, 2.15}, {Elements::Th, 2.06},
    {Elements::U, 1.96},       {Elements::Np, 1.90}, {Elements::Pu, 1.87}, {Elements::Am, 1.80}, {Elements::Cm, 1.69},
    {Elements::Bk, 1.50},      {Elements::Cf, 1.50}, {Elements::Es, 1.50}, {Elements::Fm, 1.50}, {Elements::Md, 1.50},
    {Elements::No, 1.50},      {Elements::Lr, 1.50}, {Elements::Db, 1.50}, {Elements::Sg, 1.50}, {Elements::Bh, 1.50},
    {Elements::Hs, 1.50},      {Elements::Mt, 1.50}, {Elements::Rg, 1.50}, {Elements::Cn, 1.50}, {Elements::Nh, 1.50},
    {Elements::Fl, 1.50},      {Elements::Mc, 1.50}, {Elements::Lv, 1.50}, {Elements::Ts, 1.50}, {Elements::Og, 1.50},
    {Elements::Phantom, 1.0}};

// Return radius for specified Z
double radius(Elements::Element Z) { return radiusData_[Z].second; }

} // namespace AtomicRadii
