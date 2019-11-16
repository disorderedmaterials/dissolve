/*
	*** Integrator1D Gizmo - Functions
	*** src/gui/gizmos/integrator1d_funcs.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/gizmos/integrator1d.h"
#include "gui/widgets/subwindow.h"
#include "main/dissolve.h"
#include "math/integrator.h"
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

// Constructor
Integrator1DGizmo::Integrator1DGizmo(DissolveWindow* dissolveWindow, const char* title) : SubWidget(dissolveWindow, title), dissolve_(dissolveWindow->dissolve())
{
	// Set up user interface
	ui.setupUi(this);

	// Grab the DataViewer pointer from the 
	dataView_ = ui.PlotWidget->dataViewer();

	// Initialise window contents
	lastPairPotential_ = dissolve_.pairPotentials().first();
	pairPotentialIndex_ = (lastPairPotential_ ? 0 : -1);
	initialiseWindow(lastPairPotential_);
	initialiseControls(lastPairPotential_, true);

	refreshing_ = false;
}

Integrator1DGizmo::~Integrator1DGizmo()
{
}

// Initialise window
void Integrator1DGizmo::initialiseWindow(PairPotential* pp)
{
	// Set information panel contents
	if (pp)
	{
		CharString topText("%s-%s", pp->atomTypeNameI(), pp->atomTypeNameJ());
		ui.TopLabel->setText(topText.get());

		CharString bottomText("%s %s", Forcefield::shortRangeTypes().keyword(pp->shortRangeType()), pp->includeCoulomb() ? "+ charges" : "(no charges)");
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
void Integrator1DGizmo::initialiseControls(PairPotential* pp, bool addDefaults)
{
	// Add data targets
	if (pp && addDefaults)
	{
		// Set up the view
		View& view = dataView_->view();
		view.setViewType(View::FlatXYView);
		view.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
		view.axes().setMax(0, pp ? pp->range() : 15.0);
		view.axes().setTitle(1, "U(r), kJ mol\\sup{-1}");
		view.axes().setRange(1, -5.0, 10.0);

		setDataTargets(pp);
	}

	refreshing_ = true;

	ui.FullEnergyCheck->setChecked(dataView_->isRenderableVisible("Full"));
	ui.OriginalEnergyCheck->setChecked(dataView_->isRenderableVisible("Original"));
	ui.AdditionalEnergyCheck->setChecked(dataView_->isRenderableVisible("Additional"));
	ui.FullForceCheck->setChecked(dataView_->isRenderableVisible("Force"));

	refreshing_ = false;
}

// Set data targets for specified PairPotential
void Integrator1DGizmo::setDataTargets(PairPotential* pp)
{
	// Clear any old renderables
	dataView_->clearRenderables();

	if (pp)
	{
		Renderable* fullPotential = dataView_->createRenderable(Renderable::Data1DRenderable, pp->uFull().objectTag(), "Full");
		fullPotential->setColour(StockColours::BlackStockColour);

		Renderable* originalPotential = dataView_->createRenderable(Renderable::Data1DRenderable, pp->uOriginal().objectTag(), "Original");
		originalPotential->setColour(StockColours::RedStockColour);
		originalPotential->lineStyle().set(1.0, LineStipple::HalfDashStipple);

		Renderable* additionalPotential = dataView_->createRenderable(Renderable::Data1DRenderable, pp->uAdditional().objectTag(), "Additional");
		additionalPotential->setColour(StockColours::BlueStockColour);
		additionalPotential->lineStyle().set(1.0, LineStipple::DotStipple);

		Renderable* dUFull = dataView_->createRenderable(Renderable::Data1DRenderable, pp->dUFull().objectTag(), "Force");
		dUFull->setColour(StockColours::GreenStockColour);
	}
}

/*
 * SubWidget Reimplementations / Definitions
 */

void Integrator1DGizmo::closeEvent(QCloseEvent* event)
{
	emit (windowClosed(title()));
}

// Update controls within widget
void Integrator1DGizmo::updateControls()
{
	refreshing_ = true;

	// Refresh the graph
	dataView_->postRedisplay();

	// Compare current pair potential being displayed with that retrieved from the main list
	if (lastPairPotential_ != dissolve_.pairPotential(pairPotentialIndex_))
	{
		lastPairPotential_ = pairPotentialIndex_ < 0 ? NULL : dissolve_.pairPotential(pairPotentialIndex_);

		setDataTargets(lastPairPotential_);
		initialiseWindow(lastPairPotential_);
	}

	double integral = Integrator::absTrapezoid(lastPairPotential_->uAdditional());
	ui.UAdditionalMagnitudeLabel->setText(lastPairPotential_ ? QString("%1 kJ/mol %2").arg(integral).arg(QChar(0xc5)) : "N/A");

	refreshing_ = false;
}

// Disable sensitive controls within widget
void Integrator1DGizmo::disableSensitiveControls()
{
	ui.ZeroUAdditionalButton->setEnabled(false);
}

// Enable sensitive controls within widget
void Integrator1DGizmo::enableSensitiveControls()
{
	ui.ZeroUAdditionalButton->setEnabled(true);
}

// Return string specifying widget type
const char* Integrator1DGizmo::widgetType()
{
	return "PairPotential";
}

// Write widget state through specified LineParser
bool Integrator1DGizmo::writeState(LineParser& parser)
{
	// Check validity of current pair potential
	PairPotential* pp = dissolve_.pairPotential(pairPotentialIndex_);

	// Write PairPotential target (if there is one)
	if (pp)
	{
		if (!parser.writeLineF("%s %s\n", pp->atomTypeNameI(), pp->atomTypeNameJ())) return false;
	}
	else if (!parser.writeLineF("None\n")) return false;

	// Write DataViewer state
	if (!dataView_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool Integrator1DGizmo::readState(LineParser& parser)
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

	// We will read the DataViewer data from the file, unless the PairPotential could not be found
	if (!lastPairPotential_)
	{
		initialiseControls(NULL, false);
		return false;
	}
	if (!dataView_->readSession(parser)) return false;

	initialiseControls(lastPairPotential_, false);

	return true;
}

/*
 * Widget Slots
 */

void Integrator1DGizmo::on_PreviousPotentialButton_clicked(bool checked)
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

void Integrator1DGizmo::on_NextPotentialButton_clicked(bool checked)
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

void Integrator1DGizmo::on_FullEnergyCheck_clicked(bool checked)
{
	dataView_->setRenderableVisible("Full", checked);
}

void Integrator1DGizmo::on_OriginalEnergyCheck_clicked(bool checked)
{
	dataView_->setRenderableVisible("Original", checked);
}

void Integrator1DGizmo::on_AdditionalEnergyCheck_clicked(bool checked)
{
	dataView_->setRenderableVisible("Additional", checked);
}

void Integrator1DGizmo::on_FullForceCheck_clicked(bool checked)
{
	dataView_->setRenderableVisible("Force", checked);
}

void Integrator1DGizmo::on_ResetGraphButton_clicked(bool checked)
{
	View& view = dataView_->view();
	view.axes().setRange(0, 0.0, dissolveWindow_->dissolve().pairPotentialRange());
	view.axes().setRange(1, -10.0, 10.0);

	dataView_->postRedisplay();
}

void Integrator1DGizmo::on_ZeroUAdditionalButton_clicked(bool checked)
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

		dataView_->postRedisplay();
	}
}
