/*
	*** dUQ Flags
	*** src/main/flags.cpp
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

#include "main/flags.h"

// Static Members
bool Flags::flags_[Flags::nFlags], Flags::modificationFlag_;
int Flags::count_;

// Constructor
Flags::Flags()
{
}

// Clear flags, excluding modificationFlag_
void Flags::clearFlags()
{
	for (int n=0; n<Flags::nFlags; ++n) flags_[n] = false;
}

// Clear all flags, including modificationFlag_
void Flags::clearAll()
{
	for (int n=0; n<Flags::nFlags; ++n) flags_[n] = false;
	modificationFlag_ = false;
	count_ = 0;
}

// Set specified flag
void Flags::wave(int flags)
{
	for (int n=0; n<Flags::nFlags; ++n)
	{
		if ((flags&(1<<n)))
		{
			flags_[n] = true;
			++count_;
		}
	}
	modificationFlag_ = true;
}

// Return if any of (or'd) flags are currently 'on'
bool Flags::isOn(int flags)
{
	for (int n=0; n<Flags::nFlags; ++n) if ((flags&(1<<n)) && flags_[n]) return true;
	return false;
}

// Return whether any flag is on
bool Flags::anyFlag()
{
	for (int n=0; n<Flags::nFlags; ++n) if (flags_[n]) return true;
	return false;
}

// Return modification flag
bool Flags::modificationFlag()
{
	return modificationFlag_;
}

// Return flag count
int Flags::count()
{
	return count_;
}
