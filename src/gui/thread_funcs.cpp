/*
	*** Thread
	*** src/gui/thread_funcs.cpp
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

#include "gui/thread.hui"
#include "gui/gui.h"
#include "main/dissolve.h"

/*
 * Dissolve Thread Worker
 */

// Constructor
DissolveThreadWorker::DissolveThreadWorker(Dissolve& dissolve) : dissolve_(dissolve)
{
	nIterationsToRun_ = 1;
}

// Perform the specified number of iterations (or -1 to keep going)
void DissolveThreadWorker::beginIterating(int nIterations)
{
	nIterationsToRun_ = nIterations;
	keepIterating_ = true;
	while (keepIterating_)
	{
		if (!dissolve_.iterate(1)) keepIterating_ = false;
		if (nIterationsToRun_ > 0) --nIterationsToRun_;
		if (nIterationsToRun_ == 0) keepIterating_ = false;

		emit iterated();

		QCoreApplication::processEvents();
	}

	emit iterationsComplete();
}

// Stop iterating as soon as possible
void DissolveThreadWorker::stopIterating()
{
	keepIterating_ = false;
}

/*
 * Dissolve Thread Controller
 */

// Constructor
DissolveThreadController::DissolveThreadController(DissolveWindow* parentWindow, Dissolve& dissolve, int nIterations)
{
	DissolveThreadWorker* worker = new DissolveThreadWorker(dissolve);
	worker->moveToThread(&workerThread_);

	// Connect signals / slots
	connect(&workerThread_, SIGNAL(finished()), worker, SLOT(deleteLater()));
	connect(this, SIGNAL(workerIterate(int)), worker, SLOT(beginIterating(int)));
	connect(this, SIGNAL(workerStop()), worker, SLOT(stopIterating()));
	connect(worker, SIGNAL(iterated()), parentWindow, SLOT(fullUpdate()), Qt::BlockingQueuedConnection);
	connect(worker, SIGNAL(iterationsComplete()), parentWindow, SLOT(iterationsComplete()));

	workerThread_.start();
}

// Destructor
DissolveThreadController::~DissolveThreadController()
{
	workerThread_.quit();
	workerThread_.wait();
}

// Perform the specified number of main loop iterations
void DissolveThreadController::iterate(int nIterations)
{
	emit workerIterate(nIterations);
}

// Pause any current iterating
void DissolveThreadController::stopIterating()
{
	emit workerStop();
}	
