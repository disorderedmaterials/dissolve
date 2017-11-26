/*
	*** Messaging Routines - GUI Print Text
	*** src/gui/messenger_printtext.cpp
	Copyright T. Youngs 2012-2017

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

#include "base/messenger.h"
#include "base/lineparser.h"
#include "gui/mainwindow.h"

// Print text
void Messenger::printText(const char* text)
{
	// Print the passed string
	if (redirect_) parser_->writeLineF("%s", text);
	else if (MonitorWindow::messagesBrowser())
	{
		QString s = text;
		if (s.endsWith('\n')) s.chop(1);
		MonitorWindow::messagesBrowser()->append(text);
	}
	else printf("%s", text);
}

