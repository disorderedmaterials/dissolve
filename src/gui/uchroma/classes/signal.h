/*
	*** Signal
	*** src/gui/uchroma/classes/signal.h
	Copyright T. Youngs 2013-2018

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_UCHROMASIGNAL_H
#define DUQ_UCHROMASIGNAL_H

// Forward Declarations
/* none */

// Signals
class UChromaSignal
{
	public:
	// Signal actions
	enum SignalAction { IgnoreSignal, AcceptSignal, UniqueAcceptSignal };
	// Signals to indicate changes in data structures that may be relevant to other structures
	enum CollectionSignal
	{
		CollectionCreatedSignal,
		CollectionDeletedSignal,
		CollectionSliceChangedSignal,
		CollectionSliceExtractedSignal,
		CollectionDataChangedSignal
	};
};

#endif
