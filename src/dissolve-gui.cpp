// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/messenger.h"
#include "base/processpool.h"
#include "gui/gui.h"
#include "main/cli.h"
#include "main/dissolve.h"
#include "main/version.h"
#include <QSurfaceFormat>
#include <clocale>

int main(int args, char **argv)
{
#ifdef PARALLEL
    // Initialise parallel communication
    ProcessPool::initialiseMPI(&args, &argv);
#endif

    // Instantiate main classes
    CoreData coreData;
    Dissolve dissolve(coreData);

    // Parse CLI options
    CLIOptions options;
    if (options.parse(args, argv, true) != CLIOptions::Success)
        return 1;

    // Print GPL license information
    Messenger::print("Dissolve-GUI version {}, Copyright (C) 2021 Team Dissolve and contributors.\n", Version::info());
    Messenger::print("Source repository: {}.\n", Version::repoUrl());
    Messenger::print("Dissolve comes with ABSOLUTELY NO WARRANTY.\n");
    Messenger::print("This is free software, and you are welcome to redistribute it under certain conditions.\n");
    Messenger::print("For more details read the GPL at <http://www.gnu.org/copyleft/gpl.html>.\n");

    // Register master Modules
    Messenger::banner("Available Modules");
    if (!dissolve.registerMasterModules())
    {
        ProcessPool::finalise();
        return 1;
    }

    /*
     * Create and launch GUI
     */

    // Create the main QApplication */
    QApplication app(args, argv);
    QCoreApplication::setOrganizationName("ProjectAten");
    QCoreApplication::setOrganizationDomain("www.projectaten.com");
    QCoreApplication::setApplicationName("Dissolve-GUI");

    // Set native siblings attribute to prevent odd rendering artefacts on some systems
    app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

    // Set high DPI pixmaps
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    // Ensure that the C locale is set, otherwise printf() and friends may not use dot for the radix point
    setlocale(LC_NUMERIC, "C");
    QLocale::setDefault(QLocale::C);

    // Create the main window
    DissolveWindow dissolveWindow(dissolve);

    // If an input file was specified, load it here
    if (options.inputFile())
    {
        if (!dissolveWindow.openLocalFile(options.inputFile().value_or(""), options.restartFilename().value_or(""),
                                          options.ignoreRestartFile(), options.ignoreStateFile()))
        {
            ProcessPool::finalise();
            return 1;
        }

        // Set restart file frequency and whether to write heartbeat file
        if (options.writeNoFiles())
        {
            dissolve.setRestartFileFrequency(0);
            dissolve.setWriteHeartBeat(false);
        }
        else
            dissolve.setRestartFileFrequency(options.restartFileFrequency());

        // Iterate before launching the GUI?
        if (options.nIterations() > 0)
        {
            // Prepare for run
            if (!dissolve.prepare())
                return 1;

            // Run main simulation
            auto result = dissolve.iterate(options.nIterations());
            if (!result)
                return 1;
        }
    }

    // Update and show the main window
    dissolveWindow.fullUpdate();
    dissolveWindow.addOutputHandler();
    dissolveWindow.show();

    auto result = app.exec();

    // Clear all data
    dissolve.clear();

    // End parallel communication
    ProcessPool::finalise();

    // Done.
    return result;
}
