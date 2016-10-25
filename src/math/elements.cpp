/*
	*** Element definitions
	*** src/base/elements.cpp
	Copyright T. Youngs 2012-2016

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/elements.h"
#include "main/duq.h"
#include "base/sysfunc.h"
#include "ff/forcefield.h"
#include "classes/forcefieldatom.h"
#include "base/atom.h"

// Class return function
ElementMap &elements()
{
	static ElementMap elementMap_;
	return elementMap_;
}

// Definitions
#define LANTHANIDES 98
#define ACTINIDES 99

// ZMapping types
const char* ZMapTypeKeywords[ElementMap::nZMapTypes] = { "Alpha", "FirstAlpha", "SingleAlpha", "Name", "Numeric", "FF", "Auto" };
ElementMap::ZMapType ElementMap::zMapType(const char* s, bool reportError)
{
	ElementMap::ZMapType zm = (ElementMap::ZMapType) enumSearch("element mapping style", ElementMap::nZMapTypes, ZMapTypeKeywords, s);
	if ((zm == nZMapTypes) && reportError) enumPrintValid(ElementMap::nZMapTypes, ZMapTypeKeywords);
	return zm;
}
const char* ElementMap::zMapType(ElementMap::ZMapType zm)
{
	return ZMapTypeKeywords[zm];
}

/*
// Default Element Data
// Element radii taken from:
//	"Covalent radii revisited", 
//	B. Cordero, V. Gómez, A. E. Platero-Prats, M. Revés, J. Echeverría, E. Cremades, F. Barragán and S. Alvarez
//	Dalton Trans., 2008 (DOI: 10.1039/b801115j)
//	Notes: High-Spin radii taken for 1st transition elements, sp3 value taken for Carbon. Bk and beyond set to 1.50.
//
// Lutetium and Lawrencium are assigned to group 3, with Lanthanum and Actinium the first elements in the lanthanide and actinide series.
*/

//	  Z	Mass  		Name         		Symbol			Group	Radius  RGBA		
Element ElementMap::el[] = {
	{ 0,	0.000,		"Dummy","DUMMY",	"XX","XX",		0,	0.00,	{ 0.5,0.5,0.5,1.0 },
		{ 0,0,0,0,0,0,0,0,0 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 1,	1.008,		"Hydrogen","HYDROGEN",	"H","H",		1,	0.31,	{ 0.87,0.87,0.87,1.0 },
		{ 2,0,32,32,32,32,32,32,32 },	{ 1,0,-1,0,0,0,0,0,0 } },
	{ 2,	4.003,		"Helium","HELIUM",	"He","HE",		18,	0.28,	{ 1.0,0.784,0.784,1.0 },
		{ 8,0,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 3,	6.941,		"Lithium","LITHIUM",	"Li","LI",		1,	1.28,	{ 0.647,0.165,0.165,1.0 },
		{ 2,0,32,32,32,32,32,32,32 },	{ 1,0,0,0,0,0,0,0,0 } },
	{ 4,	9.012,		"Beryllium","BERYLLIUM","Be","BE",		2,	0.96,	{ 0.0,0.0,0.0,1.0 },	
		{ 32,8,4,0,32,32,32,32,32 },	{ 2,1,0,0,0,0,0,0,0 } },
	{ 5,	10.811,		"Boron","BORON",	"B","B",		13,	0.84,	{ 0.0,1.0,0.0,1.0 },	
		{ 32,16,8,0,32,32,32,32,32 },	{ 3,2,1,0,0,0,0,0,0 } },
	{ 6,	12.011,		"Carbon","CARBON",	"C","C",		14,	0.76,	{ 0.0,1.0,0.2,1.0 },	
		{ 32,32,32,16,0,32,32,32,32 },	{ -4,-3,-2,-1,0,1,0,0,0 } },
	{ 7,	14.007,		"Nitrogen","NITROGEN",	"N","N",		15,	0.71,	{ 0.561,0.561,1.0,1.0 },
		{ 32,32,16,0,8,64,32,32,32 },	{ -3,-2,-1,0,1,0,0,0,0 } },
	{ 8,	15.999,		"Oxygen","OXYGEN",	"O","O",		16,	0.66,	{ 1.0,0.0,0.0,1.0 },	
		{ 32,32,0,32,32,32,32,32,32 },	{ -2,-1,0,1,0,0,0,0,0 } },
	{ 9,	18.998,		"Fluorine","FLUORINE",	"F","F",		17,	0.57,	{ 0.784,0.647,0.094,1.0 },
		{ 2,0,32,32,32,32,32,32,32 },	{ -1,0,0,0,0,0,0,0,0 } },
	{ 10,	20.180,		"Neon","NEON",		"Ne","NE",		18,	0.58,	{ 0.0,0.0,0.0,1.0 },	
		{ 8,0,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 11,	22.990,		"Sodium","SODIUM",	"Na","NA",		1,	1.66,	{ 0.0,0.0,1.0,1.0 },	
		{ 2,0,32,32,32,32,32,32,32 },	{ 1,0,0,0,0,0,0,0,0 } },
	{ 12,	24.305,		"Magnesium","MAGNESIUM","Mg","MG",		2,	1.41,	{ 0.0,0.0,0.0,1.0 },	
		{ 8,4,0,32,32,32,32,32,32 },	{ 2,1,0,0,0,0,0,0,0 } },
	{ 13,	26.982,		"Aluminium","ALUMINIUM","Al","AL",		13,	1.21,	{ 0.561,0.561,1.0,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 14,	28.085,		"Silicon","SILICON",	"Si","SI",		14,	1.11,	{ 0.784,0.647,0.094,1.0 },
		{ 32,32,32,32,0,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 15,	30.974,		"Phosphorus","PHOSPHORUS","P","P",		15,	1.07,	{ 1.0,0.647,0.0,1.0 },	
		{ 32,32,32,0,32,0,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 16,	32.066,		"Sulfur","SULFUR",	"S","S",		16,	1.05,	{ 1.0,0.784,0.196,1.0 },
		{ 32,32,0,16,0,16,0,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 17,	35.453,		"Chlorine","CHLORINE",	"Cl","CL",		17,	1.02,	{ 0.0,1.0,0.0,1.0 },	
		{ 0,0,32,32,32,32,32,32,32 },	{ -1,0,0,0,0,0,0,0,0 } },
	{ 18,	39.948,		"Argon","ARGON",	"Ar","AR",		18,	1.06,	{ 0.0,1.0,0.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 19,	39.098,		"Potassium","POTASSIUM","K","K",		1,	2.03,	{ 0.0,1.0,0.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 20,	40.078,		"Calcium","CALCIUM",	"Ca","CA",		2,	1.76,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 21,	44.956,		"Scandium","SCANDIUM",	"Sc","SC",		3,	1.70,	{ 1.0,0.86,0.66,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 22,	47.880,		"Titanium","TITANIUM",	"Ti","TI",		4,	1.60,	{ 0.7,0.7,0.7,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 23,	50.941,		"Vanadium","VANADIUM",	"V","V",		5,	1.53,	{ 0.7,0.7,0.7,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 24,	51.996,		"Chromium","CHROMIUM",	"Cr","CR",		6,	1.39,	{ 0.7,0.7,0.7,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 25,	54.938,		"Manganese","MANGANESE","Mn","MN",		7,	1.61,	{ 0.7,0.7,0.7,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 26,	55.847,		"Iron","IRON",		"Fe","FE",		8,	1.52,	{ 1.0,0.647,0.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 27,	58.933,		"Cobalt","COBALT",	"Co","CO",		9,	1.50,	{ 0.647,0.165,0.165,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 28,	58.693,		"Nickel","NICKEL",	"Ni","NI",		10,	1.24,	{ 0.647,0.165,0.165,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 29,	63.546,		"Copper","COPPER",	"Cu","CU",		11,	1.32,	{ 0.647,0.165,0.165,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 30,	65.390,		"Zinc","ZINC",		"Zn","ZN",		12,	1.22,	{ 0.647,0.165,0.165,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 31,	69.723,		"Gallium","GALLIUM",	"Ga","GA",		13,	1.22,	{ 0.647,0.165,0.165,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 32,	72.610,		"Germanium","GERMANIUM","Ge","GE",		14,	1.20,	{ 0.647,0.165,0.165,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 33,	74.922,		"Arsenic","ARSENIC",	"As","AS",		15,	1.19,	{ 0.647,0.165,0.165,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 34,	78.960,		"Selenium","SELENIUM",	"Se","SE",		16,	1.20,	{ 0.647,0.165,0.165,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 35,	79.904,		"Bromine","BROMINE",	"Br","BR",		17,	1.20,	{ 0.647,0.165,0.165,1.0 },
		{ 0,0,32,32,32,32,32,32,32 },	{ -1,0,0,0,0,0,0,0,0 } },
	{ 36,	83.800,		"Krypton","KRYPTON",	"Kr","KR",		18,	1.16,	{ 0.6,0.6,0.6,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 37,	85.468,		"Rubidium","RUBIDIUM",	"Rb","RB",		1,	2.20,	{ 0.0,1.0,0.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 38,	87.620,		"Strontium","STRONTIUM","Sr","SR",		2,	1.95,	{ 0.0,1.0,0.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 39,	88.906,		"Yttrium","YTTRIUM",	"Y","Y",		3,	1.90,	{ 0.647,0.165,0.165,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 40,	91.224,		"Zirconium","ZIRCONIUM","Zr","ZR",		4,	1.75,	{ 0.784,0.647,0.094,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 41,	92.906,		"Niobium","NIOBIUM",	"Nb","NB",		5,	1.64,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 42,	95.940,		"Molybdenum","MOLYBDENUM","Mo","MO",		6,	1.54,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 43,	97.907,		"Technetium","TECHNETIUM","Tc","TC",		7,	1.47,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 44,	101.070,	"Ruthenium","RUTHENIUM","Ru","RU",		8,	1.46,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 45,	102.906,	"Rhodium","RHODIUM",	"Rh","RH",		9,	1.42,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 46,	106.420,	"Palladium","PALLADIUM","Pd","PD",		10,	1.39,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 47,	107.870,	"Silver","SILVER",	"Ag","AG",		11,	1.45,	{ 0.6,0.6,0.6,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 48,	112.411,	"Cadmium","CADMIUM",	"Cd","CD",		12,	1.44,	{ 0.0,1.0,0.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 49,	114.818,	"Indium","INDIUM",	"In","IN",		13,	1.42,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 50,	118.710,	"Tin","TIN",		"Sn","SN",		14,	1.39,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 51,	121.760,	"Antimony","ANTIMONY",	"Sb","SB",		15,	1.39,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 52,	127.600,	"Tellurium","TELLURIUM","Te","TE",		16,	1.38,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 53,	129.905,	"Iodine","IODINE",	"I","I",		17,	1.39,	{ 1.0,1.0,1.0,1.0 },	
		{ 0,0,32,32,32,32,32,32,32 },	{ -1,0,0,0,0,0,0,0,0 } },
	{ 54,	131.290,	"Xenon","XENON",	"Xe","XE",		18,	1.40,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 55,	132.905,	"Caesium","CAESIUM",	"Cs","CS",		1,	2.44,	{ 0.647,0.165,0.165,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 56,	137.327,	"Barium","BARIUM",	"Ba","BA",		2,	2.15,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 57,	138.905,	"Lanthanum","LANTHANUM","La","LA",		98,	2.07,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 58,	140.115,	"Cerium","CERIUM",	"Ce","CE",		98,	2.04,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 59,	140.908,	"Praesodymium","PRAESODYMIUM","Pr","PR",	98,	2.03,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 60,	144.240,	"Neodymium","NEODYMIUM","Nd","ND",		98,	2.01,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 61,	144.913,	"Prometheum","PROMETHEUM","Pm","PM",		98,	1.99,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 62,	150.360,	"Samarium","SAMARIUM",	"Sm","SM",		98,	1.98,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 63,	151.965,	"Europium","EUROPIUM",	"Eu","EU",		98,	1.98,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 64,	157.250,	"Gadolinium","GADOLINIUM","Gd","GD",		98,	1.96,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 65,	158.925,	"Terbium","TERBIUM",	"Tb","TB",		98,	1.94,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 66,	162.500,	"Dysprosium","DYSPROSIUM","Dy","DY",		98,	1.92,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 67,	164.930,	"Holmium","HOLMIUM",	"Ho","HO",		98,	1.92,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 68,	167.260,	"Erbium","ERBIUM",	"Er","ER",		98,	1.89,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 69,	168.934,	"Thulium","THULIUM",	"Tm","TM",		98,	1.90,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 70,	173.040,	"Ytterbium","YTTERBIUM","Yb","YB",		98,	1.87,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 71,	174.967,	"Lutetium","LUTETIUM",	"Lu","LU",		3,	1.87,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 72,	178.490,	"Hafnium","HAFNIUM",	"Hf","HF",		4,	1.75,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 73,	180.948,	"Tantalum","TANTALUM",	"Ta","TA",		5,	1.70,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 74,	183.840,	"Tungsten","TUNGSTEN",	"W","W",		6,	1.62,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 75,	186.207,	"Rhenium","RHENIUM",	"Re","RE",		7,	1.51,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 76,	190.230,	"Osmium","OSMIUM",	"Os","OS",		8,	1.44,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 77,	192.220,	"Iridium","IRIDIUM",	"Ir","IR",		9,	1.41,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 78,	195.080,	"Platinum","PLATINUM",	"Pt","PT",		10,	1.36,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 79,	196.967,	"Gold","GOLD",		"Au","AU",		11,	1.36,	{ 0.784,0.647,0.094,1.0 },
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 80,	200.590,	"Mercury","MERCURY",	"Hg","HG",		12,	1.32,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 81,	204.383,	"Thallium","THALLIUM",	"Tl","TL",		13,	1.45,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 82,	207.200,	"Lead","LEAD",		"Pb","PB",		14,	1.46,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 83,	208.980,	"Bismuth","BISMUTH",	"Bi","BI",		15,	1.48,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 84,	208.982,	"Polonium","POLONIUM",	"Po","PO",		16,	1.40,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 85,	209.987,	"Astatine","ASTATINE",	"At","AT",		17,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 86,	222.018,	"Radon","RADON",	"Rn","RN",		18,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 87,	223.020,	"Francium","FRANCIUM",	"Fr","FR",		1,	2.60,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 88,	226.025,	"Radon","RADON",	"Ra","RA",		2,	2.21,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 89,	227.028,	"Actinium","ACTINIUM",	"Ac","AC",		99,	2.15,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 90,	232.038,	"Thorium","THORIUM",	"Th","TH",		99,	2.06,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 91,	231.036,	"Protactinium","PROTACTINIUM","Pa","PA",	99,	2.00,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 92,	238.029,	"Uranium","URANIUM",	"U","U",		99,	1.96,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 93,	237.048,	"Neptunium","NEPTUNIUM","Np","NP",		99,	1.90,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 94,	244.064,	"Plutonium","PLUTONIUM","Pu","PU",		99,	1.87,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 95,	243.061,	"Americium","AMERICIUM","Am","AM",		99,	1.80,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 96,	247.070,	"Curium","CURIUM",	"Cm","CM",		99,	1.69,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 97,	247.070,	"Berkelium","BERKELIUM","Bk","BK",		99,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 98,	251.080,	"Californium","CALIFORNIUM","Cf","CF",		99,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 99,	252.083,	"Einsteinium","EINSTEINIUM","Es","ES",		99,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 100,	257.095,	"Fermium","FERMIUM",	"Fm","FM",		99,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 101,	258.100,	"Mendelevium","MENDELEVIUM","Md","MD",		99,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 102,	259.101,	"Nobelium","NOBELIUM",	"No","NO",		99,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 103,	262.110,	"Lawrencium","LAWRENCIUM","Lr","LR",		3,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 104,	261.000,	"Rutherfordium","RUTHERFORDIUM","Rf","RF",	4,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 105,	262.000,	"Dubnium","DUBNIUM",	"Db","DB",		5,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 106,	266.000,	"Seaborgium","SEABORGIUM","Sg","SG",		6,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 107,	264.000,	"Bohrium","BOHRIUM",	"Bh","BH",		7,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 108,	269.000,	"Hassium","HASSIUM",	"Hs","HS",		8,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 109,	268.000,	"Meitnerium","MEITNERIUM","Mt","MT",		9,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 110,	281.000,	"Darmstadtium","DARMSTADTIUM","Ds","DS",	10,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 111,	272.000,	"Roentgenium","ROENTGENIUM","Rg","RG",		11,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 112,	285.000,	"Ununium","UNUNIUM",	"Uub","UUB",		12,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 113,	284.000,	"Ununtrium","UNUNTRIUM","Uut","UUT",		13,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 114,	289.000,	"Ununquadium","UNUNQUADIUM","Uuq","UUQ",	14,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 115,	288.000,	"Ununpentium","UNUNPENTIUM","Uup","UUP",	15,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 116,	292.000,	"Ununhexium","UNUNHEXIUM","Uuh","UUH",		16,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 117,	0.000,		"Ununseptium","UNUNSEPTIUM","Uus","UUS",	17,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } },
	{ 118,	0.000,		"Ununoctium","UNUNOCTIUM","Uuo","UUO",		18,	1.50,	{ 1.0,1.0,1.0,1.0 },	
		{ 32,32,32,32,32,32,32,32,32 },	{ 0,0,0,0,0,0,0,0,0 } }
};

// Set colour component of element
void Element::setColour(int rgb, double value)
{
	colour[rgb] = value;
}

// Set colour component
void Element::setColour(double r, double g, double b, double a)
{
	colour[0] = r;
	colour[1] = g;
	colour[2] = b;
	colour[3] = a;
}

// Return colour in supplied vector
void Element::copyColour(GLfloat* v)
{
	v[0] = (GLfloat) colour[0];
	v[1] = (GLfloat) colour[1];
	v[2] = (GLfloat) colour[2];
	v[3] = (GLfloat) colour[3];
}

/*
 * Element Map
 */

// Constructor
ElementMap::ElementMap()
{
	// Determine number of defined elements, and double-check against MAXELEMENTS constant
	nElements_ = sizeof(el) / sizeof(el[0]);
	if (nElements_ > MAXELEMENTS) printf("Warning - Number of internally-defined elements exceeds MAXELEMENTS constant.\n");
	// Copy default element data to backup storage
	defaultEl = new Element[nElements_];
	int n,i;
	for (i=0; i<nElements_; i++)
	{
		defaultEl[i].atomicRadius = el[i].atomicRadius;
		for (n=0; n<4; ++n) defaultEl[i].colour[n] = el[i].colour[n];
	}
	// Create backup array while we're here as well
	backupEl_ = new Element[nElements_];
}

// Destructor
ElementMap::~ElementMap()
{
	delete[] backupEl_;
	delete[] defaultEl;
}

// Copy current element data to backup structure
void ElementMap::backupData()
{
	int i,n;
	for (i=0; i<nElements_; i++)
	{
		backupEl_[i].atomicRadius = el[i].atomicRadius;
		for (n=0; n<4; ++n) backupEl_[i].colour[n] = el[i].colour[n];
	}
}

// Copy backed up element data to actual element data
void ElementMap::restoreData()
{
	int i,n;
	for (i=0; i<nElements_; i++)
	{
		el[i].atomicRadius = backupEl_[i].atomicRadius;
		for (n=0; n<4; ++n) el[i].colour[n] = backupEl_[i].colour[n];
	}
}

// Return group number of atomic number 'i'
int ElementMap::group(Atom* i)
{
	return group(i->element());
}

// Return atomic mass of atomic number 'i'
double ElementMap::atomicMass(Atom* i)
{
	return atomicMass(i->element());
}

// Return name of atomic number 'i'
const char* ElementMap::name(Atom* i)
{
	return name(i->element());
}

// Return symbol of atomic number 'i'
const char* ElementMap::symbol(Atom* i)
{
	return symbol(i->element());
}

// Return effective radius of atomic number 'i'
double ElementMap::atomicRadius(Atom* i)
{
	return atomicRadius(i->element());
}

// Return bond order penalty for TBO 'bo' of atomic number 'i'
int ElementMap::bondOrderPenalty(Atom* i, int bo)
{
	return bondOrderPenalty(i->element(), bo);
}

// Return the colour of the element
double *ElementMap::colour(Atom* i)
{
	return colour(i->element());
}

// Return number of defined elements
int ElementMap::nElements() const
{
	return nElements_;
}

// Return group number of atomic number 'i'
int ElementMap::group(int i) const
{
	return el[i].group;
}

// Return atomic mass of atomic number 'i'
double ElementMap::atomicMass(int i) const
{
	return el[i].atomicMass;
}

// Return name of atomic number 'i'
const char* ElementMap::name(int i) const
{
	return el[i].name;
}

// Return symbol of atomic number 'i'
const char* ElementMap::symbol(int i) const
{
	return el[i].symbol;
}

// Set radius of atomic number 'i'
void ElementMap::setAtomicRadius(int i, double r)
{
	el[i].atomicRadius = r;
}

// Return effective radius of atomic number 'i'
double ElementMap::atomicRadius(int i) const
{
	return el[i].atomicRadius;
}

// Return bond order penalty for TBO 'bo' of atomic number 'i'
int ElementMap::bondOrderPenalty(int i, int bo) const
{
	return el[i].bondOrderPenalty[bo];
}

// Return the ambient colour of the element
double *ElementMap::colour(int i)
{
	return el[i].colour;
}

// Set ambient colour component of element
void ElementMap::setColour(int i, int rgb, double value)
{
	el[i].colour[rgb] = (GLfloat) value;
}

// Set ambient colour component
void ElementMap::setColour(int i, double r, double g, double b, double a)
{
	el[i].setColour(r, g, b, a);
}

// Return ambient colour in supplied vector
void ElementMap::copyColour(int i, GLfloat* v) const
{
	el[i].copyColour(v);
}

// Convert string from Z to element number
int ElementMap::numberToZ(const char* s) const
{
	// Check that the string is entirely numerical
	bool isnumber = true;
	for (int n=0; s[n] != '\0'; n++)
		if ((s[n] < 48) || (s[n] > 57))
		{
			isnumber = false;
			break;
		}
	if (isnumber)
	{
		// Check range of number before returning
		int i = atoi(s);
		if ((i < 0) || (i > nElements_))
		{
			Messenger::print("Warning: Converted element number is out of range (%i)\n", i);
			i = 0;
		}
		return i;
	}
	else return -1;
}

// Convert string from alpha to element number
int ElementMap::alphaToZ(const char* s) const
{
	// Ignore numbers. Convert up to non-alpha character.
	static Dnchar cleaned;
	cleaned.clear();
	int n;
	for (n=0; s[n] != '\0'; n++)
	{
		if (s[n] == ' ') continue;
		else if (s[n] > 64 && s[n] < 91) cleaned += s[n];
		else if (s[n] > 96 && s[n] < 123) cleaned += toupper(s[n]);
		else if (s[n] == '_') break;
	}
	for (n=0; n<nElements_; n++) if (cleaned == el[n].ucSymbol) return n;
	return -1;
}

// Convert string from first alpha part to element number
int ElementMap::firstAlphaToZ(const char* s) const
{
	// Convert up to non-alpha character.
	static Dnchar cleaned;
	cleaned.clear();
	int n;
	for (n=0; s[n] != '\0'; n++)
	{
		if (s[n] == ' ') continue;
		else if (s[n] > 64 && s[n] < 91) cleaned += s[n];
		else if (s[n] > 96 && s[n] < 123) cleaned += toupper(s[n]);
		else break;
	}
	for (n=0; n<nElements_; n++) if (cleaned == el[n].ucSymbol) return n;
	return -1;
}

// Convert string from first alpha character to element number
int ElementMap::singleAlphaToZ(const char* s) const
{
	// Convert first alpha character.
	char cleaned[2];
	int n;
	cleaned[0] = '\0';
	for (n=0; s[n] != '\0'; n++)
	{
		if (s[n] == ' ') continue;
		else if (s[n] > 64 && s[n] < 91) cleaned[0] = s[n];
		else if (s[n] > 96 && s[n] < 123) cleaned[0] = s[n] - 32;
		else break;
		if (cleaned[0] != '\0') break;
	}
	cleaned[1] = '\0';
	for (n=0; n<nElements_; n++) if (strcmp(el[n].ucSymbol,cleaned) == 0) return n;
	return -1;
}

// Convert string from name to element number
int ElementMap::nameToZ(const char* s) const
{
	// Ignore numbers. Convert up to non-alpha character.
	static Dnchar cleaned;
	cleaned.clear();
	int n;
	for (n=0; s[n] != '\0'; n++)
	{
		if (s[n] > 64 && s[n] < 91) cleaned += s[n];
		else if (s[n] > 96 && s[n] < 123) cleaned += toupper(s[n]);
		else if (s[n] == '_') break;
	}
	for (n=0; n<nElements_; n++) if (cleaned == el[n].ucName) return n;
	return -1;
}

// Convert string from fftype to element number
int ElementMap::ffToZ(const char* s) const
{
	ForcefieldAtom* ffa;
	int result = -1;
	for (Forcefield* ff = duq.forcefields(); ff != NULL; ff = ff->next)
	{
		ffa = ff->findType(s);
		// Found a match, so find out what element it is...
		if (ffa != NULL) result = ffa->neta()->characterElement();
		if (result != -1) break;
	}
	return result;
}

// Search for element named 'query' in the list of known elements
int ElementMap::find(const char* query, ElementMap::ZMapType zmt) const
{
	// Get the element number from the element name provided.
	Messenger::enter("ElementMap::find");
	int result = -1;
	if (query[0] == '\0')
	{
		Messenger::print("Warning: Element search requested on blank string.\n");
		Messenger::exit("ElementMap::find");
		return 0;
	}
	// Convert the query string according to the specified rule
	switch (zmt == ElementMap::nZMapTypes ? prefs.zMapType() : zmt)
	{
		// Automatic determination
		case (ElementMap::AutoZMap):
			// First, try pure numeric conversion
			result = numberToZ(query);
			if (result != -1) break;
			// Then, try alpha conversion
			result = alphaToZ(query);
			if (result != -1) break;
			// Then, try name conversion
			result = nameToZ(query);
			if (result != -1) break;
			// Finally, try FF conversion
			result = ffToZ(query);
			break;
		// Name search
		case (ElementMap::NameZMap):
			result = nameToZ(query);
			break;
		// Search loaded forcefields for atom names
		case (ElementMap::ForcefieldZMap):
			result = ffToZ(query);
			// Attempt an alpha conversion if the FF conversion failed
			if (result == -1) result = alphaToZ(query);
			break;
		// Convert based on alpha-part of atom name only
		case (ElementMap::AlphaZMap):
			result = alphaToZ(query);
			break;
		// Convert based on first alpha-part of atom name only
		case (ElementMap::FirstAlphaZMap):
			result = firstAlphaToZ(query);
			break;
		// Convert based on first alpha-character of atom name only
		case (ElementMap::SingleAlphaZMap):
			result = singleAlphaToZ(query);
			break;
		// Convert based on numeric part only
		case (ElementMap::NumericZMap):
			result = numberToZ(query);
			break;
		default:
			break;
	}
	Messenger::exit("ElementMap::find");
	return ((result == -1) ? 0 : result);
}
