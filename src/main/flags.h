/*
	*** dUQ Flags
	*** src/main/flags.h
	Copyright T. Youngs 2012-2014

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

#ifndef DUQ_FLAGS_H
#define DUQ_FLAGS_H

// Flag Set
class Flags
{
	public:
	// Constructor
	Flags();
	// Flag List
	enum FlagType { SpeciesChanged=1, AtomTypeChanged=2, GrainsChanged=4, IsotopologuesChanged=8, SamplesChanged=16, RDFChanged=32, ReferenceDataChanged=64, SystemChanged=128, PairPotentialChanged=256, IntramolecularChanged=512, nFlags=10 };

	private:
	// Counter of flag wavings
	static int count_;
	// Flag array
	static bool flags_[Flags::nFlags];
	// Modification flag (for input file use)
	static bool modificationFlag_;
	
	public:
	// Clear flags (but not 'modification' flag)
	static void clearFlags();
	// Clear all flags, including modification flag
	static void clearAll();
	// Set specified (or'd combination) of flag(s)
	static void wave(int flags);
	// Return if any of (or'd) flags are currently 'on'
	static bool isOn(int flags);
	// Return whether any flag is on
	static bool anyFlag();
	// Return modification flag
	static bool modificationFlag();
	// Return flag count
	static int count();
};

#endif
