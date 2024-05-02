// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QThread>

// Forward Declarations
class Dissolve;
class DissolveWindow;

// Dissolve Thread Worker Class
class DissolveThreadWorker : public QObject
{
    Q_OBJECT

    public:
    DissolveThreadWorker(Dissolve &dissolve);

    private:
    // Whether the iteration loop should keep going
    bool keepIterating_;
    // Reference to Dissolve
    Dissolve &dissolve_;
    // Number of iterations to run
    int nIterationsToRun_;

    public Q_SLOTS:
    // Perform the specified number of iterations (or -1 to keep going)
    void beginIterating(int nIterations);
    // Stop iterating as soon as possible
    void stopIterating();

    Q_SIGNALS:
    void clearMessages();
    void iterated(int iterationsRemaining);
    void iterationsComplete();
};

// Dissolve Thread Controller
class DissolveThreadController : public QObject
{
    Q_OBJECT

    private:
    // Thread for this object
    QThread workerThread_;

    public:
    DissolveThreadController(DissolveWindow *parentWindow, Dissolve &dissolve, int nIterations = -1);
    ~DissolveThreadController();

    public Q_SLOTS:
    // Perform the specified number of main loop iterations
    void iterate(int nIterations);
    // Stop any current iterating
    void stopIterating();

    Q_SIGNALS:
    void workerIterate(int);
    void workerStop();
};
