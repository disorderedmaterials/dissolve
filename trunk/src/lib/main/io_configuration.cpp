/*
	*** dUQ I/O - Model
	*** src/lib/main/io_model.cpp
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
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/atomtype.h"
#include "base/lineparser.h"

/*!
 * \brief Save Configuration as xyz
 */
bool DUQ::saveConfigurationXYZ(Configuration& cfg, const char* fileName)
{
	// I/O operation, so Master only...
	if (Comm.slave()) return TRUE;

	// Open file and check that we're OK to proceed writing to it
	LineParser parser;
	msg.print("Writing model as XYZ file '%s'...\n", fileName);

	parser.openOutput(fileName, TRUE);
	if (!parser.isFileGoodForWriting())
	{
		msg.error("Couldn't open file '%s' for writing.\n", fileName);
		return FALSE;
	}

	// Write number of atoms and title
	parser.writeLineF("%i\n", cfg.nAtoms());
	parser.writeLineF("%s\n", fileName_.get());
	
	// Write Atoms
	for (int n=0; n<cfg.nAtoms(); ++n) parser.writeLineF("%-3s   %10.4f  %10.4f  %10.4f\n", PeriodicTable::element(cfg.atom(n).element()).symbol(), cfg.atom(n).r().x, cfg.atom(n).r().y, cfg.atom(n).r().z);

	msg.print("Finished writing model XYZ file.\n");

	// Close file
	parser.closeFiles();

	return TRUE;
}

/*!
 * \brief Save Configuration as CONFIG
 */
bool DUQ::saveConfigurationDLPOLY(Configuration& cfg, const char* fileName)
{
	// I/O operation, so Master only...
	if (Comm.slave()) return TRUE;

	// Open file and check that we're OK to proceed writing to it
	LineParser parser;
	msg.print("Writing model as CONFIG file '%s'...\n", fileName);

	parser.openOutput(fileName, TRUE);
	if (!parser.isFileGoodForWriting())
	{
		msg.error("Couldn't open file '%s' for writing.\n", fileName);
		return FALSE;
	}

	// Write title
	parser.writeLineF("%s\n", fileName_.get());

	// Write keytrj and imcon
	if (cfg.box()->type() == Box::NonPeriodicBox) parser.writeLineF("%10i%10i\n", 0, 0);
	else if (cfg.box()->type() == Box::CubicBox) parser.writeLineF("%10i%10i\n", 0, 1);
	else if (cfg.box()->type() == Box::OrthorhombicBox) parser.writeLineF("%10i%10i\n", 0, 2);
	else parser.writeLineF("%10i%10i\n", 0, 3);
	
	// Write Cell
	if (cfg.box()->type() != Box::NonPeriodicBox)
	{
		Matrix3 axes = cfg.box()->axes();
		parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[0], axes[1], axes[2]);
		parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[3], axes[4], axes[5]);
		parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[6], axes[7], axes[8]);
	}

	// Write Atoms
	for (int n=0; n<cfg.nAtoms(); ++n)
	{
		Atom& i = cfg.atom(n);
		parser.writeLineF("%-6s%10i%20.10f\n%20.12f%20.12f%20.12f\n", i.atomType()->name(), n+1, PeriodicTable::element(i.element()).isotope(0)->atomicWeight(), i.r().x, i.r().y, i.r().z);
	}

	msg.print("Finished writing model XYZ file.\n");

	// Close file
	parser.closeFiles();

	return TRUE;
}
