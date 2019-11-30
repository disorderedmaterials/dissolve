/*
	*** Text Symbol Definitions
	*** src/gui/viewer/render/symbol.h
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_RENDER_TEXTSYMBOL_H
#define DISSOLVE_RENDER_TEXTSYMBOL_H

#include <QString>

// Forward Declarations
/* none */

// Symbols
class SymbolData
{
	public:
	// Symbol
	enum Symbol
	{
		MultiplySymbol,
		DivisionSymbol,
		AngstromSymbol,
		DegreeSymbol,
		AlphaSymbol,
		BetaSymbol,
		GammaSymbol,
		DeltaSymbol,
		EpsilonSymbol,
		ZetaSymbol,
		EtaSymbol,
		ThetaSymbol,
		IotaSymbol,
		KappaSymbol,
		LambdaSymbol,
		MuSymbol,
		NuSymbol,
		XiSymbol,
		OmicronSymbol,
		PiSymbol,
		RhoSymbol,
		SigmaSymbol,
		TauSymbol,
		UpsilonSymbol,
		PhiSymbol,
		ChiSymbol,
		PsiSymbol,
		OmegaSymbol,
		CapitalAlphaSymbol,
		CapitalBetaSymbol,
		CapitalGammaSymbol,
		CapitalDeltaSymbol,
		CapitalEpsilonSymbol,
		CapitalZetaSymbol,
		CapitalEtaSymbol,
		CapitalThetaSymbol,
		CapitalIotaSymbol,
		CapitalKappaSymbol,
		CapitalLambdaSymbol,
		CapitalMuSymbol,
		CapitalNuSymbol,
		CapitalXiSymbol,
		CapitalOmicronSymbol,
		CapitalPiSymbol,
		CapitalRhoSymbol,
		CapitalSigmaSymbol,
		CapitalTauSymbol,
		CapitalUpsilonSymbol,
		CapitalPhiSymbol,
		CapitalChiSymbol,
		CapitalPsiSymbol,
		CapitalOmegaSymbol,
		nSymbols
	};
	// Unicode Character
	QChar character;
	// Short name of symbol
	QString name;
	// Symbol description
	QString description;


	/*
	 * Singleton
	 */
	public:
	// Static list of symbols
	static SymbolData symbols[];
	// Return named symbol
	static SymbolData* symbol(QString name);
};

#endif
