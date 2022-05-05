// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/datamanagerdialog.h"
#include "gui/gui.h"
#include "main/dissolve.h"
#include <QFileDialog>
#include <QInputDialog>

void DissolveWindow::on_SimulationCheckAction_triggered(bool checked)
{
    clearMessages();

    dissolve_.prepare();

    fullUpdate();
}

void DissolveWindow::on_SimulationRunAction_triggered(bool checked) { setupIteration(-1); }

void DissolveWindow::on_SimulationRunForAction_triggered(bool checked)
{
    // Get the number of iterations to run
    bool ok;
    int nIterations =
        QInputDialog::getInt(this, "Iterate Simulation...", "Enter the number of iterations to run", 10, 1, 1000000, 10, &ok);
    if (!ok)
        return;
    setupIteration(nIterations);
}

void DissolveWindow::on_SimulationStepAction_triggered(bool checked) { setupIteration(1); }

void DissolveWindow::on_SimulationStepFiveAction_triggered(bool checked) { setupIteration(5); }

void DissolveWindow::on_SimulationStopAction_triggered(bool checked) { emit(stopIterating()); }

void DissolveWindow::on_SimulationSetRestartFileFrequencyAction_triggered(bool checked)
{
    auto ok{false};
    auto newFrequency =
        QInputDialog::getInt(this, "Set Restart File Frequency", "Enter the new frequency at which to write the restart file",
                             dissolve_.restartFileFrequency(), 0, 10000000, 5, &ok);
    if (!ok)
        return;
    dissolve_.setRestartFileFrequency(newFrequency);
}

void DissolveWindow::on_SimulationSaveRestartPointAction_triggered(bool checked)
{
    // Get filename for restart point
    QString filename =
        QFileDialog::getSaveFileName(this, "Select Output File", QDir::currentPath(), "Restart Files (*.restart)");
    if (filename.isEmpty())
        return;

    if (dissolve_.saveRestart(qPrintable(filename)))
        statusBar()->showMessage(QString("Saved restart point to '%1'.").arg(filename), 3000);
    else
        statusBar()->showMessage(QString("ERROR: Failed to save restart point to '%1'.").arg(filename), 3000);
}

void DissolveWindow::on_SimulationDataManagerAction_triggered(bool checked)
{
    DataManagerDialog dataManagerDialog(this, dissolve_, referencePoints_, dissolve_.processingModuleData());
    dataManagerDialog.exec();
}

void DissolveWindow::on_SimulationClearModuleDataAction_triggered(bool checked) { clearModuleData(); }
