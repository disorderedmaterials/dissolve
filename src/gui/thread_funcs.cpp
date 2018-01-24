/*
	*** Thread
	*** src/gui/thread_funcs.cpp
	Copyright T. Youngs 2012-2018

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
#include "gui/gui.h"
#include "main/duq.h"

/*
 * DUQ Thread Worker
 */

// Constructor
DUQThreadWorker::DUQThreadWorker(DUQ& duq) : duq_(duq)
{
	nIterationsToRun_ = 1;
}

// Perform the specified number of iterations (or -1 to keep going)
void DUQThreadWorker::beginIterating(int nIterations)
{
	nIterationsToRun_ = nIterations;
	keepIterating_ = true;
	while (keepIterating_)
	{
		duq_.iterate(1);
		if (nIterationsToRun_ > 0) --nIterationsToRun_;
		if (nIterationsToRun_ == 0) keepIterating_ = false;

		emit iterated();

		QCoreApplication::processEvents();
	}

	emit iterationsComplete();
}

// Stop iterating as soon as possible
void DUQThreadWorker::stopIterating()
{
	keepIterating_ = false;
}

/*
 * DUQ Thread Controller
 */

// Constructor
DUQThreadController::DUQThreadController(DUQWindow* parentWindow, DUQ& duq, int nIterations)
{
	DUQThreadWorker* worker = new DUQThreadWorker(duq);
	worker->moveToThread(&workerThread_);

	// Connect signals / slots
	connect(&workerThread_, SIGNAL(finished()), worker, SLOT(deleteLater()));
	connect(this, SIGNAL(workerIterate(int)), worker, SLOT(beginIterating(int)));
	connect(this, SIGNAL(workerStop()), worker, SLOT(stopIterating()));
	connect(worker, SIGNAL(iterated()), parentWindow, SLOT(updateControls()), Qt::BlockingQueuedConnection);
	connect(worker, SIGNAL(iterationsComplete()), parentWindow, SLOT(iterationsComplete()));

	workerThread_.start();
}

// Destructor
DUQThreadController::~DUQThreadController()
{
	workerThread_.quit();
	workerThread_.wait();
}

// Perform the specified number of main loop iterations
void DUQThreadController::iterate(int nIterations)
{
	emit workerIterate(nIterations);
}

// Pause any current iterating
void DUQThreadController::stopIterating()
{
	emit workerStop();
}	
