/*
	*** PairPotential Widget - Functions
	*** src/gui/pairpotentialwidget_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/pairpotentialwidget.h"
#include "gui/gui.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/widgets/subwindow.h"
#include "main/dissolve.h"
#include "classes/pairpotential.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

// Constructor
PairPotentialWidget::PairPotentialWidget(DissolveWindow* dissolveWindow, const char* title) : SubWidget(dissolveWindow, title), dissolve_(dissolveWindow->dissolve())
{
	// Set up user interface
	ui.setupUi(this);

	// Grab our UChromaWidget pointer
	uChromaView_ = ui.PlotWidget;

	// Initialise window contents
	lastPairPotential_ = dissolve_.pairPotentials().first();
	pairPotentialIndex_ = (lastPairPotential_ ? 0 : -1);
	initialiseWindow(lastPairPotential_);
	initialiseControls(lastPairPotential_, true);

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
		CharString topText("%s-%s", pp->atomTypeNameI(), pp->atomTypeNameJ());
		ui.TopLabel->setText(topText.get());

		CharString bottomText("%s %s", PairPotential::shortRangeType(pp->shortRangeType()), pp->includeCoulomb() ? "+ charges" : "(no charges)");
		ui.BottomLabel->setText(bottomText.get());
	}
	else
	{
		ui.TopLabel->setText("?-? / ?");
		ui.BottomLabel->setText("??");
	}

	ui.PreviousPotentialButton->setEnabled(pp && (dissolve_.nPairPotentials() > 1));
	ui.NextPotentialButton->setEnabled(pp && (dissolve_.nPairPotentials() > 1));
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
	emit (windowClosed(title()));
}

// Update controls within widget
void PairPotentialWidget::updateControls()
{
	refreshing_ = true;

	// Ensure that any displayed data are up-to-date
	uChromaView_->refreshReferencedDataSets();
	uChromaView_->updateDisplay();

	// Compare current pair potential being displayed with that retrieved from the main list
	if (lastPairPotential_ != dissolve_.pairPotential(pairPotentialIndex_))
	{
		lastPairPotential_ = pairPotentialIndex_ < 0 ? NULL : dissolve_.pairPotential(pairPotentialIndex_);

		setDataTargets(lastPairPotential_);
		initialiseWindow(lastPairPotential_);
	}

	ui.UAdditionalMagnitudeLabel->setText(lastPairPotential_ ? QString("%1 kJ/mol %2").arg(lastPairPotential_->uAdditional().absIntegral()).arg(QChar(0xc5)) : "N/A");

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
	// Check validity of current pair potential
	PairPotential* pp = dissolve_.pairPotential(pairPotentialIndex_);

	// Write PairPotential target (if there is one)
	if (pp)
	{
		if (!parser.writeLineF("%s %s\n", pp->atomTypeNameI(), pp->atomTypeNameJ())) return false;
	}
	else if (!parser.writeLineF("None\n")) return false;

	// Write uChromaView state
	if (!uChromaView_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool PairPotentialWidget::readState(LineParser& parser)
{
	// Read PairPotential target
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	lastPairPotential_ = NULL;
	pairPotentialIndex_ = -1;
	if ((parser.nArgs() != 1) && (!DissolveSys::sameString(parser.argc(0), "None")))
	{
		// Locate the pair potential
		lastPairPotential_ = dissolve_.pairPotential(parser.argc(0), parser.argc(1));
		if (lastPairPotential_) pairPotentialIndex_ = dissolve_.pairPotentials().indexOf(lastPairPotential_);
	}

	// Initialise window
	initialiseWindow(lastPairPotential_);

	// Reset UChromaView
	uChromaView_->startNewSession(false);

	// We will read the UChroma data from the file, unless the PairPotential could not be found
	if (!lastPairPotential_)
	{
		initialiseControls(NULL, false);
		return false;
	}
	if (!uChromaView_->readSession(parser)) return false;

	initialiseControls(lastPairPotential_, false);

	return true;
}

/*
 * Widget Slots
 */

void PairPotentialWidget::on_PreviousPotentialButton_clicked(bool checked)
{
	// Decrease index of PP
	--pairPotentialIndex_;
	if ((pairPotentialIndex_ < 0) || (pairPotentialIndex_ >= dissolve_.nPairPotentials()))
	{
		// Out-of-range, one way or another, so select the last in the list
		pairPotentialIndex_ = dissolve_.nPairPotentials() - 1;
	}

	lastPairPotential_ = pairPotentialIndex_ == -1 ? NULL : dissolve_.pairPotential(pairPotentialIndex_);

	setDataTargets(lastPairPotential_);
	initialiseWindow(lastPairPotential_);

	updateControls();
}

void PairPotentialWidget::on_NextPotentialButton_clicked(bool checked)
{
	// Increase index of PP
	if (dissolve_.nPairPotentials() == 0) pairPotentialIndex_ = -1;
	else ++pairPotentialIndex_;

	if (pairPotentialIndex_ >= dissolve_.nPairPotentials())
	{
		// Out-of-range, so cycle back to first item in list
		pairPotentialIndex_ = 0;
	}

	lastPairPotential_ = pairPotentialIndex_ == -1 ? NULL : dissolve_.pairPotential(pairPotentialIndex_);

	setDataTargets(lastPairPotential_);
	initialiseWindow(lastPairPotential_);

	updateControls();
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
	viewPane->axes().setRange(0, 0.0, dissolveWindow_->dissolve().pairPotentialRange());
	viewPane->axes().setRange(1, -10.0, 10.0);

	uChromaView_->updateDisplay();
}

void PairPotentialWidget::on_ZeroUAdditionalButton_clicked(bool checked)
{
	// Compare current pair potential being displayed with that retrieved from the main list
	if (lastPairPotential_ != dissolve_.pairPotential(pairPotentialIndex_))
	{
		lastPairPotential_ = pairPotentialIndex_ < 0 ? NULL : dissolve_.pairPotential(pairPotentialIndex_);
	}
	if (!lastPairPotential_) return;

	QMessageBox queryBox;
	queryBox.setText(CharString("You are about to set the %s-%s additional potential to zero.", lastPairPotential_->atomTypeNameI(), lastPairPotential_->atomTypeNameJ()).get());
	queryBox.setInformativeText("Are you sure you want to do this?");
	queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	queryBox.setDefaultButton(QMessageBox::No);
	int ret = queryBox.exec();

	if (ret == QMessageBox::Yes)
	{
		lastPairPotential_->resetUAdditional();

		uChromaView_->refreshReferencedDataSets();
		uChromaView_->updateDisplay();
	}
}
