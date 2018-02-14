/*
	*** PairPotential Widget - Functions
	*** src/gui/pairpotentialwidget_funcs.cpp
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

#include "gui/pairpotentialwidget.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/subwindow.h"
#include "main/duq.h"
#include "classes/pairpotential.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

// Constructor
PairPotentialWidget::PairPotentialWidget(QWidget* parent, DUQ& dUQ, const char* title) : SubWidget(parent, title), duq_(dUQ)
{
	// Set up user interface
	ui.setupUi(this);

	// Grab our UChromaWidget pointer
	uChromaView_ = ui.PlotWidget;

	// Initialise window contents
	pairPotential_ = duq_.pairPotentials();
	initialiseWindow(pairPotential_);
	initialiseControls(pairPotential_, true);

	refreshing_ = false;
}

PairPotentialWidget::~PairPotentialWidget()
{
}

// Initialise window
void PairPotentialWidget::initialiseWindow(PairPotential* pp)
{
	// Set information panel contents
	if (pp)
	{
		CharString topText("%s-%s", pairPotential_->atomTypeNameI(), pairPotential_->atomTypeNameJ());
		ui.TopLabel->setText(topText.get());

		CharString bottomText("%s %s", PairPotential::shortRangeType(pairPotential_->shortRangeType()), pairPotential_->includeCoulomb() ? "+ charges" : "(no charges)");
		ui.BottomLabel->setText(bottomText.get());
	}
	else
	{
		ui.TopLabel->setText("?-? / ?");
		ui.BottomLabel->setText("??");
	}

	ui.PreviousPotentialButton->setEnabled(pp && (duq_.nPairPotentials() > 1));
	ui.NextPotentialButton->setEnabled(pp && (duq_.nPairPotentials() > 1));
}

// Initialise controls
void PairPotentialWidget::initialiseControls(PairPotential* pp, bool addDefaults)
{
	// Add data targets
	if (pp && addDefaults)
	{
		// Start a new, empty session
		uChromaView_->startNewSession(true);

		// Set up the view pane
		ViewPane* viewPane = uChromaView_->currentViewPane();
		viewPane->setViewType(ViewPane::FlatXYView);
		viewPane->axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
		viewPane->axes().setMax(0, pp ? pp->range() : 15.0);
		viewPane->axes().setTitle(1, "U(r), kJ mol\\sup{-1}");
		viewPane->axes().setMin(1, -5.0);
		viewPane->axes().setMax(1, 10.0);

		setDataTargets(pp);
	}

	refreshing_ = true;

	ui.FullEnergyCheck->setChecked(uChromaView_->collectionVisible("Full"));
	ui.OriginalEnergyCheck->setChecked(uChromaView_->collectionVisible("Original"));
	ui.AdditionalEnergyCheck->setChecked(uChromaView_->collectionVisible("Additional"));
	ui.FullForceCheck->setChecked(uChromaView_->collectionVisible("Force"));

	refreshing_ = false;
}

// Set data targets for specified PairPotential in UChromaView
void PairPotentialWidget::setDataTargets(PairPotential* pp)
{
	// Clear any old collections
	uChromaView_->clearCollections();

	if (pp)
	{
		CharString blockData;

		// Full potential
		blockData.sprintf("Collection 'Full'; ColourSingle 0 0 0 255; LineStyle 1.0 Solid; DataSet 'Full'; Source XYData %s; EndDataSet; EndCollection", pp->uFull().objectName());
		uChromaView_->addCollectionFromBlock(blockData);

		// Original potential
		blockData.sprintf("Collection 'Original'; ColourSingle 200 0 0 255; LineStyle 1.0 'Half Dash'; DataSet 'Full'; Source XYData %s; EndDataSet; EndCollection", pp->uOriginal().objectName());
		uChromaView_->addCollectionFromBlock(blockData);

		// Additional potential
		blockData.sprintf("Collection 'Additional'; ColourSingle 0 0 200 255; LineStyle 1.0 'Dots'; DataSet 'Full'; Source XYData %s; EndDataSet; EndCollection", pp->uAdditional().objectName());
		uChromaView_->addCollectionFromBlock(blockData);

		// Full potential
		blockData.sprintf("Collection 'Force'; Visible False; ColourSingle 0 200 0 255; LineStyle 1.0 Solid; DataSet 'Full'; Source XYData %s; EndDataSet; EndCollection", pp->dUFull().objectName());
		uChromaView_->addCollectionFromBlock(blockData);
	}
}

/*
 * SubWidget Reimplementations / Definitions
 */

void PairPotentialWidget::closeEvent(QCloseEvent* event)
{
	emit (windowClosed(pairPotential_));
}

// Update controls within widget
void PairPotentialWidget::updateControls()
{
	refreshing_ = true;

	// Ensure that any displayed data are up-to-date
	uChromaView_->refreshReferencedDataSets();
	uChromaView_->updateDisplay();

	ui.UAdditionalMagnitudeLabel->setText(pairPotential_ ? QString("%1 kJ/mol %2").arg(pairPotential_->uAdditional().absIntegral()).arg(QChar(0xc5)) : "N/A");

	refreshing_ = false;
}


// Disable sensitive controls within widget, ready for main code to run
void PairPotentialWidget::disableSensitiveControls()
{
	ui.ZeroUAdditionalButton->setEnabled(false);
}

// Enable sensitive controls within widget, ready for main code to run
void PairPotentialWidget::enableSensitiveControls()
{
	ui.ZeroUAdditionalButton->setEnabled(true);
}

// Return string specifying widget type
const char* PairPotentialWidget::widgetType()
{
	return "PairPotential";
}

// Write widget state through specified LineParser
bool PairPotentialWidget::writeState(LineParser& parser)
{
	// Write PairPotential target
	if (!parser.writeLineF("%s %s\n", pairPotential_->atomTypeNameI(), pairPotential_->atomTypeNameJ())) return false;

	// Write uChromaView state
	if (!uChromaView_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool PairPotentialWidget::readState(LineParser& parser)
{
	// Read PairPotential target
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	pairPotential_ = duq_.pairPotential(parser.argc(0), parser.argc(1));

	// Initialise window
	initialiseWindow(pairPotential_);

	// Reset UChromaView
	uChromaView_->startNewSession(false);

	// We will read the UChroma data from the file, unless the PairPotential could not be found
	if (!pairPotential_)
	{
		initialiseControls(NULL, false);
		return false;
	}
	if (!uChromaView_->readSession(parser)) return false;

	initialiseControls(pairPotential_, false);

	return true;
}

/*
 * Widget Slots
 */

void PairPotentialWidget::on_PreviousPotentialButton_clicked(bool checked)
{
	if (!pairPotential_) return;

	pairPotential_ = pairPotential_->prev;
	if (pairPotential_ == NULL) pairPotential_ = duq_.pairPotential(duq_.nPairPotentials()-1);

	setDataTargets(pairPotential_);
	initialiseWindow(pairPotential_);
}

void PairPotentialWidget::on_NextPotentialButton_clicked(bool checked)
{
	if (!pairPotential_) return;

	pairPotential_ = pairPotential_->next;
	if (pairPotential_ == NULL) pairPotential_ = duq_.pairPotentials();

	setDataTargets(pairPotential_);
	initialiseWindow(pairPotential_);
}

void PairPotentialWidget::on_FullEnergyCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible("Full", checked);
}

void PairPotentialWidget::on_OriginalEnergyCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible("Original", checked);
}

void PairPotentialWidget::on_AdditionalEnergyCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible("Additional", checked);
}

void PairPotentialWidget::on_FullForceCheck_clicked(bool checked)
{
	uChromaView_->setCollectionVisible("Force", checked);
}

void PairPotentialWidget::on_ResetGraphButton_clicked(bool checked)
{
	ViewPane* viewPane = uChromaView_->currentViewPane();
	viewPane->axes().setRange(0, 0.0, pairPotential_ ? pairPotential_->range() : 15.0);
	viewPane->axes().setRange(1, -10.0, 10.0);

	uChromaView_->updateDisplay();
}

void PairPotentialWidget::on_ZeroUAdditionalButton_clicked(bool checked)
{
	QMessageBox queryBox;
	queryBox.setText(CharString("You are about to set the %s-%s additional potential to zero.", pairPotential_->atomTypeNameI(), pairPotential_->atomTypeNameJ()).get());
	queryBox.setInformativeText("Are you sure you want to do this?");
	queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	queryBox.setDefaultButton(QMessageBox::No);
	int ret = queryBox.exec();

	if (ret == QMessageBox::Yes)
	{
		pairPotential_->resetUAdditional();

		uChromaView_->refreshReferencedDataSets();
		uChromaView_->updateDisplay();
	}
}
