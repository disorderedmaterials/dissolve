// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include <vector>

// Sears '91 Isotope Data
namespace Sears91
{
// Isotopes
enum Isotope
{
    Unknown,
    H_Natural,
    H_1,
    H_2,
    H_3,
    He_Natural,
    He_3,
    He_4,
    Li_Natural,
    Li_6,
    Li_7,
    Be_Natural,
    B_Natural,
    B_10,
    B_11,
    C_Natural,
    C_12,
    C_13,
    N_Natural,
    N_14,
    N_15,
    O_Natural,
    O_16,
    O_17,
    O_18,
    F_Natural,
    Ne_Natural,
    Ne_20,
    Ne_21,
    Ne_22,
    Na_Natural,
    Mg_Natural,
    Mg_24,
    Mg_25,
    Mg_26,
    Al_Natural,
    Si_Natural,
    Si_28,
    Si_29,
    Si_30,
    P_Natural,
    S_Natural,
    S_32,
    S_33,
    S_34,
    S_36,
    Cl_Natural,
    Cl_35,
    Cl_37,
    Ar_Natural,
    Ar_36,
    Ar_38,
    Ar_40,
    K_Natural,
    K_39,
    K_40,
    K_41,
    Ca_Natural,
    Ca_40,
    Ca_42,
    Ca_43,
    Ca_44,
    Ca_46,
    Ca_48,
    Sc_Natural,
    Ti_Natural,
    Ti_46,
    Ti_47,
    Ti_48,
    Ti_49,
    Ti_50,
    V_Natural,
    V_50,
    V_51,
    Cr_Natural,
    Cr_50,
    Cr_52,
    Cr_53,
    Cr_54,
    Mn_Natural,
    Fe_Natural,
    Fe_54,
    Fe_56,
    Fe_57,
    Fe_58,
    Co_Natural,
    Ni_Natural,
    Ni_58,
    Ni_60,
    Ni_61,
    Ni_62,
    Ni_64,
    Cu_Natural,
    Cu_63,
    Cu_65,
    Zn_Natural,
    Zn_64,
    Zn_66,
    Zn_67,
    Zn_68,
    Zn_70,
    Ga_Natural,
    Ga_69,
    Ga_71,
    Ge_Natural,
    Ge_70,
    Ge_72,
    Ge_73,
    Ge_74,
    Ge_76,
    As_Natural,
    Se_Natural,
    Se_74,
    Se_76,
    Se_77,
    Se_78,
    Se_80,
    Se_82,
    Br_Natural,
    Br_79,
    Br_81,
    Kr_Natural,
    Kr_78,
    Kr_80,
    Kr_82,
    Kr_83,
    Kr_84,
    Kr_86,
    Rb_Natural,
    Rb_85,
    Rb_87,
    Sr_Natural,
    Sr_84,
    Sr_86,
    Sr_87,
    Sr_88,
    Y_Natural,
    Zr_Natural,
    Zr_90,
    Zr_91,
    Zr_92,
    Zr_94,
    Zr_96,
    Nb_Natural,
    Mo_Natural,
    Mo_92,
    Mo_94,
    Mo_95,
    Mo_96,
    Mo_97,
    Mo_98,
    Mo_100,
    Tc_Natural,
    Ru_Natural,
    Ru_96,
    Ru_98,
    Ru_99,
    Ru_100,
    Ru_101,
    Ru_102,
    Ru_104,
    Rh_Natural,
    Pd_Natural,
    Pd_102,
    Pd_104,
    Pd_105,
    Pd_106,
    Pd_108,
    Pd_110,
    Ag_Natural,
    Ag_107,
    Ag_109,
    Cd_Natural,
    Cd_106,
    Cd_108,
    Cd_110,
    Cd_111,
    Cd_112,
    Cd_113,
    Cd_114,
    Cd_116,
    In_Natural,
    In_113,
    In_115,
    Sn_Natural,
    Sn_112,
    Sn_114,
    Sn_115,
    Sn_116,
    Sn_117,
    Sn_118,
    Sn_119,
    Sn_120,
    Sn_122,
    Sn_124,
    Sb_Natural,
    Sb_121,
    Sb_123,
    Te_Natural,
    Te_120,
    Te_122,
    Te_123,
    Te_124,
    Te_125,
    Te_126,
    Te_128,
    Te_130,
    I_Natural,
    Xe_Natural,
    Xe_124,
    Xe_126,
    Xe_128,
    Xe_129,
    Xe_130,
    Xe_131,
    Xe_132,
    Xe_134,
    Xe_136,
    Cs_Natural,
    Ba_Natural,
    Ba_130,
    Ba_132,
    Ba_134,
    Ba_135,
    Ba_136,
    Ba_137,
    Ba_138,
    La_Natural,
    La_138,
    La_139,
    Ce_Natural,
    Ce_136,
    Ce_138,
    Ce_140,
    Ce_142,
    Pr_Natural,
    Nd_Natural,
    Nd_142,
    Nd_143,
    Nd_144,
    Nd_145,
    Nd_146,
    Nd_148,
    Nd_150,
    Pm_Natural,
    Sm_Natural,
    Sm_144,
    Sm_147,
    Sm_148,
    Sm_149,
    Sm_150,
    Sm_152,
    Sm_154,
    Eu_Natural,
    Eu_151,
    Eu_153,
    Gd_Natural,
    Gd_152,
    Gd_154,
    Gd_155,
    Gd_156,
    Gd_157,
    Gd_158,
    Gd_160,
    Tb_Natural,
    Dy_Natural,
    Dy_156,
    Dy_158,
    Dy_160,
    Dy_161,
    Dy_162,
    Dy_163,
    Dy_164,
    Ho_Natural,
    Er_Natural,
    Er_162,
    Er_164,
    Er_166,
    Er_167,
    Er_168,
    Er_170,
    Tm_Natural,
    Yb_Natural,
    Yb_168,
    Yb_170,
    Yb_171,
    Yb_172,
    Yb_173,
    Yb_174,
    Yb_176,
    Lu_Natural,
    Lu_175,
    Lu_176,
    Hf_Natural,
    Hf_174,
    Hf_176,
    Hf_177,
    Hf_178,
    Hf_179,
    Hf_180,
    Ta_Natural,
    Ta_180,
    Ta_181,
    W_Natural,
    W_180,
    W_182,
    W_183,
    W_184,
    W_186,
    Re_Natural,
    Re_185,
    Re_187,
    Os_Natural,
    Os_184,
    Os_186,
    Os_187,
    Os_188,
    Os_189,
    Os_190,
    Os_192,
    Ir_Natural,
    Ir_191,
    Ir_193,
    Pt_Natural,
    Pt_190,
    Pt_192,
    Pt_194,
    Pt_195,
    Pt_196,
    Pt_198,
    Au_Natural,
    Hg_Natural,
    Hg_196,
    Hg_198,
    Hg_199,
    Hg_200,
    Hg_201,
    Hg_202,
    Hg_204,
    Tl_Natural,
    Tl_203,
    Tl_205,
    Pb_Natural,
    Pb_204,
    Pb_206,
    Pb_207,
    Pb_208,
    Bi_Natural,
    Ra_Natural,
    Th_Natural,
    Pa_Natural,
    U_Natural,
    U_233,
    U_234,
    U_235,
    U_238,
    Np_Natural,
    Pu_Natural,
    Pu_239,
    Pu_240,
    Pu_242,
    Am_Natural,
    Am_244,
    Am_246,
    Am_248,
    Art_Natural
};

// Isotopic Neutron Scattering Data
class IsotopeData
{
    public:
    IsotopeData(Sears91::Isotope tope, Elements::Element Z, int A, std::string_view spin, double mass, double bc, double bi,
                double sc, double si, double totalxs, double absxs);
    ~IsotopeData() = default;
    IsotopeData(const IsotopeData &source) = default;
    IsotopeData(IsotopeData &&source) = default;
    IsotopeData &operator=(const IsotopeData &source) = default;

    private:
    // Isotope
    Sears91::Isotope isotope_;
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
    // Return isotope
    Sears91::Isotope isotope() const;
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

// Return whether Isotope with specified A exists for the given element
bool hasIsotope(Elements::Element Z, int A);
// Return all isotope enums corresponding to element
std::vector<Isotope> isotopes(Elements::Element Z);
// Return isotope enum corresponding element and A (if it exists)
Isotope isotope(Elements::Element Z, int A);
// Return natural isotope for element (if it exists)
Isotope naturalIsotope(Elements::Element Z);
// Return element (Z) of Isotope
Elements::Element Z(Isotope tope);
// Return mass number (A) of specified isotope
int A(Isotope tope);
// Return mass (given C = 12) of specified isotope
double mass(Isotope tope);
// Return bound coherent scattering length (fm) of specified isotope
double boundCoherent(Isotope tope);
// Return bound incoherent scattering length (fm) of specified isotope
double boundIncoherent(Isotope tope);
// Return bound coherent scattering cross section (barn) of specified isotope
double boundCoherentXS(Isotope tope);
// Return bound incoherent scattering cross section (barn) of specified isotope
double boundIncoherentXS(Isotope tope);
// Return total bound scattering cross section (barn) of specified isotope
double totalXS(Isotope tope);
// Return absorption cross section for thermal (2200 m/s) neutron (barn) of specified isotope
double absorptionXS(Isotope tope);
}; // namespace Sears91
