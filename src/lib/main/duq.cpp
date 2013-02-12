/*
	*** dUQ Main Structure
	*** src/lib/main/duq.cpp
	Copyright T. Youngs 2012-2013

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

#include "main/duq.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/molecule.h"
#include "base/lineparser.h"
#include "base/flag.h"
#include "base/sysfunc.h"
#include "base/comms.h"
#include "math/constants.h"
#include "version.h"
#include <string.h>

/*!
 * \brief Constructor
 * \details Constructor for dUQ. 
 */
DUQ::DUQ()
{
	// System Composition
	multiplier_ = 1;
	density_ = 0.1;
	densityIsAtomic_ = TRUE;
	boxAngles_.set(90.0, 90.0, 90.0);
	relativeBoxLengths_.set(1.0, 1.0, 1.0);
	nonPeriodic_ = FALSE;
	randomConfiguration_ = TRUE;

	// PairPotentials
	pairPotentialDelta_ = 0.01;
	pairPotentialRange_ = 15.0;
	pairPotentialTruncationWidth_ = 2.0;

	// Partials
	partialsChangeCount_ = -1;
	rdfMethod_ = DUQ::SimpleMethod;

	// Setup
	boxNormalisationPoints_ = 50000000;
	qDependentFWHM_ = 0.0;
	qIndependentFWHM_ = 0.02;
	rdfBinWidth_ = 0.025;
	rdfRange_ = -1.0;
	rdfExtensionLimit_ = 0.0;
	rdfSmoothing_ = 1;
	requestedRDFRange_ = -1.0;
	rmseDeltaQ_ = 0.05;
	seed_ = -1;
	temperature_ = 300.0;
	windowFunction_ = Data2D::GaussianWindow;

	// Perturbation
	simplexNMoves_ = 25;
	simplexNCycles_ = 100;
	simplexTemperature_ = 0.01;
	simplexTolerance_ = 1.0e-3;
	simplexParameterOffset_ = 100.0;
	
	// Simulation
	energyChange_ = 0.0;
	energyChanged_ = FALSE;
}

/*!
 * \brief Destructor
 * \details Destructor for dUQ. 
 */
DUQ::~DUQ()
{
	clear();
}

/*!
 * \brief Clear all data
 */
void DUQ::clear()
{
	species_.clear();
	clearModel();
	fileName_.clear();
}

/*
 * Periodic Table Definition
 */

/*!
 * \brief Return PeriodicTable
 */
PeriodicTable &DUQ::periodicTable()
{
	return periodicTable_;
}

/*
 * Signalling
 */

/*!
 * \brief Act on signal provided
 */
CommandReturnValue DUQ::processSignal(DUQ::Signal signal, int data)
{
	CommandReturnValue result = CommandSuccess;
	switch (signal)
	{
		case (DUQ::TerminateSignal):
			msg.print("Received terminate signal...\n");
			result = CommandQuit;
			break;
		default:
			break;
	}
	return result;
}

/*!
 * \brief Process any received signals
 */
CommandReturnValue DUQ::processSignals()
{
	CommandReturnValue result = CommandSuccess;
	if (Comm.master())
	{
		// Loop over received signals
		for (Pair<DUQ::Signal, int>* sig = receivedSignals_.first(); sig != NULL; sig = sig->next)
		{
			// Something to do, so signal slaves that they are about to receive something
			Comm.decide(TRUE);
			
			// Send Signal data to slaves
			int signal = sig->a;
			Comm.broadcast(&signal, 1);
			Comm.broadcast(&sig->b, 1);
			
			// Act on signal
			result = processSignal(sig->a, sig->b);

			// Send initial decision whether to continue to slaves
			if (result != CommandSuccess)
			{
				msg.print("Master is terminating signal processing - reason = %i.\n", result);
				Comm.decide(FALSE);
				int r = result;
				Comm.broadcast(&r, 1);
				break;
			}
		}
		
		// End of list
		Comm.decide(FALSE);
	}
	else
	{
		// Anything to do?
		while (Comm.decision())
		{
			// Receive signal data
			int signal, data;
			Comm.broadcast(&signal, 1);
			Comm.broadcast(&data, 1);

			// Act on signal
			result = processSignal( (DUQ::Signal) signal, data);
			
			// Receive initial continuation decision from master process
			if (!Comm.decision())
			{
				// Receive integer return code (CommandReturnValue)
				int reason;
				Comm.broadcast(&reason, 1);
				msg.print("Slave %i received decision not to continue processing signals - will return %i instead.\n", Comm.rank(), reason);
				return (CommandReturnValue) reason;
			}
		}
	}
	
	return result;
}

/*!
 * \brief Send signal
 * \details Send a signal to somewhere - the default implementation of this virtual function does nothing.
 */
void DUQ::sendSignal(DUQ::Signal signal, int data)
{
	return;
}

/*!
 * \brief Receive signal
 * \details Receive a signal from somewhere, adding the signal and its data to a list for processing at 
 * an appropriate time.
 */
void DUQ::receiveSignal(DUQ::Signal signal, int data)
{
	receivedSignalsMutex_.lock();
	if (Comm.master())
	{
		Pair<DUQ::Signal, int>* sig = receivedSignals_.add();
		sig->a = signal;
		sig->b = data;
	}
	receivedSignalsMutex_.unlock();
}
