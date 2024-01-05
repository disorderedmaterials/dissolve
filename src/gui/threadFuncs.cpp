// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/thread.hui"
#include "main/dissolve.h"

/*
 * Dissolve Thread Worker
 */

DissolveThreadWorker::DissolveThreadWorker(Dissolve &dissolve) : dissolve_(dissolve) { nIterationsToRun_ = 1; }

// Perform the specified number of iterations (or -1 to keep going)
void DissolveThreadWorker::beginIterating(int nIterations)
{
    nIterationsToRun_ = nIterations;
    keepIterating_ = true;

    while (keepIterating_)
    {
        // Clear messages browser
        emit(clearMessages());

        if (!dissolve_.iterate(1))
            keepIterating_ = false;
        if (nIterationsToRun_ > 0)
            --nIterationsToRun_;
        if (nIterationsToRun_ == 0)
            keepIterating_ = false;

        emit(iterated(nIterationsToRun_));

        QCoreApplication::processEvents();
    }

    emit(iterationsComplete());
}

// Stop iterating as soon as possible
void DissolveThreadWorker::stopIterating() { keepIterating_ = false; }

/*
 * Dissolve Thread Controller
 */

DissolveThreadController::DissolveThreadController(DissolveWindow *parentWindow, Dissolve &dissolve, int nIterations)
{
    auto *worker = new DissolveThreadWorker(dissolve);
    worker->moveToThread(&workerThread_);

    // Connect signals / slots
    connect(&workerThread_, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(this, SIGNAL(workerIterate(int)), worker, SLOT(beginIterating(int)));
    connect(this, SIGNAL(workerStop()), worker, SLOT(stopIterating()));
    connect(worker, SIGNAL(clearMessages()), parentWindow, SLOT(clearMessages()), Qt::BlockingQueuedConnection);
    connect(worker, SIGNAL(iterated(int)), parentWindow, SLOT(updateWhileRunning(int)), Qt::BlockingQueuedConnection);
    connect(worker, SIGNAL(iterationsComplete()), parentWindow, SLOT(iterationsComplete()));

    workerThread_.start();
}

DissolveThreadController::~DissolveThreadController()
{
    workerThread_.quit();
    workerThread_.wait();
}

// Perform the specified number of main loop iterations
void DissolveThreadController::iterate(int nIterations) { emit(workerIterate(nIterations)); }

// Pause any current iterating
void DissolveThreadController::stopIterating() { emit(workerStop()); }
