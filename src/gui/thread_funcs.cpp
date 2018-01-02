/*
	*** Thread
	*** src/gui/thread_funcs.cpp
	Copyright T. Youngs 2007-2018

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

#include "gui/thread.hui"
#include "main/duq.h"

// Constructor
DUQThread::DUQThread(QObject* parent, DUQ& duq) : QThread(parent), duq_(duq)
{
	nIterationsToRun_ = 1;
}

// Destructor
DUQThread::~DUQThread()
{
}

void DUQThread::run()
{
	while (nIterationsToRun_ > 0)
	{
		duq_.iterate(1);
		if (nIterationsToRun_ > 0) --nIterationsToRun_;
		emit iterated();
	}

	emit iterationsComplete();
}

void DUQThread::iterate(int nIterations)
{
	nIterationsToRun_ = nIterations;
	start();
}
