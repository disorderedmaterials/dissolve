/*
	*** Samples DockWidget
	*** src/gui/samples.h
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

#ifndef DUQ_SAMPLESDIALOG_H
#define DUQ_SAMPLESDIALOG_H

#include "gui/ui_samples.h"
#include "gui/guiduq.uih"
#include "gui/doublespindelegate.uih"

// Forward Declarations
class Sample;
class Species;
class Isotopologue;

/** \addtogroup GUI
 * @{
 */

/*
 * \brief Samples DockWidget
 * \details TODO
 */
class DUQSamplesDockWidget : public QDockWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT


	/*
	// Window Functions
	*/
	public:
	// Constructor / Destructor
	DUQSamplesDockWidget(QWidget* parent, DUQObject& dUQ);
	~DUQSamplesDockWidget();
	// Main form declaration
	Ui::DUQSamplesDockWidget ui;
	// Finalise widgets (things that we couldn't do in Qt Designer)
	void finaliseUi();

	protected:
	// Window close event
	void closeEvent(QCloseEvent* event);
	
	public slots:
	// General refresh function
	void refresh(int targets);


	/*
	 * Variables
	 */
	private:
	// Reference to main dUQ object
	DUQObject& dUQ_;
	// Delegate for AssignedIsotopologuesTree
	DoubleSpinDelegate* isotopologueRelativePopulationDelegate_;
	// Flag to indicate that the dialog is currently being refreshed
	bool refreshing_;


	/*
	 * Private Functions
	 */
	private:
	// Return pointer to selected Sample
	Sample* selectedSample();
	// Return whether species and isotopologue are valid
	bool getSpeciesAndIsotopologue(QStringList data, Species*& species, Isotopologue*& iso);


	/*
	 * Slots
	 */
	private slots:
	// -- Samples list
	void on_SamplesList_currentRowChanged(int row);
	void on_SamplesList_itemChanged(QListWidgetItem* item);
	void on_AddSampleButton_clicked(bool checked);
	void on_RemoveSampleButton_clicked(bool checked);
	void on_CopySampleButton_clicked(bool checked);
	// -- Isotopologues
	void on_AssignedIsotopologuesTree_itemChanged(QTreeWidgetItem* item, int column);
	void on_AddIsotopologueToMixButton_clicked(bool checked);
	void on_RemoveIsotopologueFromMixButton_clicked(bool checked);
	// -- Reference Data Group
	void on_ReferenceDataFileSelectButton_clicked(bool checked);
	void on_ReferenceDataFilenameEdit_textEdited(const QString& text);
	void on_ReferenceDataNormalisationComboCheck_currentIndexChanged(int index);
	void on_ReferenceDataSubtractSelfCheck_clicked(bool checked);

	public slots:
	void mixtureIsotopologueChanged(int index);
};

/** @}*/

#endif
