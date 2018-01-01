/*
	*** UChromaBase - Signalling
	*** src/gui/uchroma/uchromabase_signal.cpp
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

#include "gui/uchroma/uchromabase.h"

// Send Collection-related signal
UChromaSignal::SignalAction UChromaBase::sendSignal(UChromaSignal::CollectionSignal signal, Collection* collection)
{
	// Send to ViewPanes in current ViewLayout....
	int nAccepted = 0;
	bool unique = false;
	for (ViewPane* pane = viewLayout_.panes(); pane != NULL; pane = pane->next)
	{
		UChromaSignal::SignalAction result = pane->processCollectionSignal(signal, collection);
		if (result == UChromaSignal::IgnoreSignal) continue;
		++nAccepted;

		// If the object wants sole acceptance of this signal, break now
		if (result == UChromaSignal::UniqueAcceptSignal)
		{
			unique = true;
			break;
		}
	}
	if (nAccepted == 0) return UChromaSignal::IgnoreSignal;
	else return (unique ? UChromaSignal::UniqueAcceptSignal : UChromaSignal::AcceptSignal);
}
