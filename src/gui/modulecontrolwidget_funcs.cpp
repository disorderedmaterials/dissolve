/*
	*** Module Control Widget - Functions
	*** src/gui/modulecontrolwidget_funcs.cpp
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

#include "gui/modulecontrolwidget.h"
#include "gui/gui.h"
#include "gui/keywordwidgets/bool.hui"
#include "gui/keywordwidgets/charstring.hui"
#include "gui/keywordwidgets/double.hui"
#include "gui/keywordwidgets/int.hui"
#include "gui/keywordwidgets/broadeningfunction.h"
#include "gui/keywordwidgets/isotopologuelist.h"
#include "gui/keywordwidgets/windowfunction.hui"
#include "gui/modulewidget.h"
#include "gui/widgets/subwindow.h"
#include "module/module.h"
#include "main/duq.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include <QGridLayout>
#include <QLabel>

// Constructor
ModuleControlWidget::ModuleControlWidget(DUQWindow* duqWindow, Module* module, const char* title, bool showTopControls) : SubWidget(duqWindow, title), module_(module), duqWindow_(duqWindow), duq_(duqWindow->duq())
{
	// Set up user interface
	ui.setupUi(this);

	// Hide the top controls, allowing shift and removal of the Module, if requested
	if (!showTopControls) ui.TopControlsFrame->setVisible(false);

	moduleWidget_ = NULL;

	panelState_ = ModuleControlWidget::ControlsAndWidgetVisible;

	initialiseWindow(module_);

	initialiseControls(module_);

	refreshing_ = false;
}

ModuleControlWidget::~ModuleControlWidget()
{
}

// Initialise window
void ModuleControlWidget::initialiseWindow(Module* module)
{
	// Set information panel contents
	if (module)
	{
		CharString topText("%s (%s) @ %s", module->name(), module->uniqueName(), module->configurationLocal() ? (module->targetConfigurations().first() ? module->targetConfigurations().first()->item->name() : "[NO CONFIG?]") : "Processing");
		ui.TopLabel->setText(topText.get());
		CharString bottomText;
		RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
		while (Configuration* cfg = configIterator.iterate())
		{
			if (bottomText.isEmpty()) bottomText.sprintf("Targets: %s", cfg->name());
			else bottomText.strcatf(", %s", cfg->name());
		}
		ui.BottomLabel->setText(bottomText.get());
	}
	else
	{
		ui.TopLabel->setText("? (?) @ ?");
		ui.BottomLabel->setText("Targets: ?");
	}
}

// Initialise controls
void ModuleControlWidget::initialiseControls(Module* module)
{
	if (!module) return;

	// Create keyword widgets in a new grid layout
	QGridLayout* keywordsLayout = new QGridLayout(ui.OptionsGroup);
	keywordsLayout->setContentsMargins(4,4,4,4);
	keywordsLayout->setSpacing(4);
	int row = 0;
	QWidget* widget;
	KeywordWidgetBase* base;

	ListIterator<ModuleKeywordBase> keywordIterator(module->keywords().keywords());
	while (ModuleKeywordBase* keyword = keywordIterator.iterate())
	{
		QLabel* nameLabel = new QLabel(keyword->keyword());
		nameLabel->setToolTip(keyword->description());
		keywordsLayout->addWidget(nameLabel, row, 0);

		// The widget to create here depends on the data type of the keyword
		if (keyword->type() == ModuleKeywordBase::IntegerData)
		{
			IntegerKeywordWidget* intWidget = new IntegerKeywordWidget(NULL, keyword);
			connect(intWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = intWidget;
			base = intWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::DoubleData)
		{
			DoubleKeywordWidget* doubleWidget = new DoubleKeywordWidget(NULL, keyword);
			connect(doubleWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = doubleWidget;
			base = doubleWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::CharStringData)
		{
			CharStringKeywordWidget* charWidget = new CharStringKeywordWidget(NULL, keyword);
			connect(charWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = charWidget;
			base = charWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::BoolData)
		{
			BoolKeywordWidget* boolWidget = new BoolKeywordWidget(NULL, keyword);
			connect(boolWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = boolWidget;
			base = boolWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::BroadeningFunctionData)
		{
			BroadeningFunctionKeywordWidget* broadeningFunctionWidget = new BroadeningFunctionKeywordWidget(NULL, keyword);
			connect(broadeningFunctionWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = broadeningFunctionWidget;
			base = broadeningFunctionWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::IsotopologueListData)
		{
			IsotopologueListKeywordWidget* isotopologueListWidget = new IsotopologueListKeywordWidget(NULL, keyword);
			connect(isotopologueListWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = isotopologueListWidget;
			base = isotopologueListWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::WindowFunctionData)
		{
			WindowFunctionKeywordWidget* windowFunctionWidget = new WindowFunctionKeywordWidget(NULL, keyword);
			connect(windowFunctionWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = windowFunctionWidget;
			base = windowFunctionWidget;
		}
		else
		{
			widget = NULL;
			base = NULL;
			// ComplexData, BroadeningFunctionData
		}

		// Set tooltip on widget, and add to the layout and our list of controls
		if (widget)
		{
			widget->setToolTip(keyword->description());
			keywordsLayout->addWidget(widget, row, 1);
			keywordWidgets_.add(base);
		}

		++row;
	}

	// Add a vertical spacer to the end to take up any extra space
	keywordsLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0);

	// Create module widget
	QVBoxLayout* widgetLayout = new QVBoxLayout(ui.WidgetWidget);
	widgetLayout->setContentsMargins(0,0,0,0);
	widgetLayout->setSpacing(0);
	moduleWidget_ = module->createWidget(ui.WidgetWidget, duq_);
	if (moduleWidget_ == NULL) Messenger::printVerbose("Module '%s' did not provide a valid controller widget.\n", module->name());
	else widgetLayout->addWidget(moduleWidget_);
}

/*
 * SubWidget Reimplementations / Definitions
 */

void ModuleControlWidget::closeEvent(QCloseEvent* event)
{
	emit (windowClosed(title()));
}

// Update controls within widget
void ModuleControlWidget::updateControls()
{
	if (!module_) return;

	refreshing_ = true;

	// Update Control group
	ui.EnabledCheck->setChecked(module_->enabled());
	ui.FrequencySpin->setValue(module_->frequency());
	ui.RunsInLabel->setText(module_->frequencyDetails(duq_.iteration()));

	// Select source list for keywords that have potentially been replicated / updated there
	GenericList& moduleData = module_->configurationLocal() ? module_->targetConfigurations().firstItem()->moduleData() : duq_.processingModuleData();

	RefListIterator<KeywordWidgetBase,bool> keywordIterator(keywordWidgets_);
	while (KeywordWidgetBase* keywordWidget = keywordIterator.iterate()) keywordWidget->updateValue(moduleData, module_->uniqueName());

	// Update control widget
	if (moduleWidget_) moduleWidget_->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within widget, ready for main code to run
void ModuleControlWidget::disableSensitiveControls()
{
	// Disable control buttons
	ui.RunButton->setEnabled(false);

	// Disable groups
	ui.ControlGroup->setEnabled(false);
	ui.OptionsGroup->setEnabled(false);
	ui.TopControlsFrame->setEnabled(false);

	if (moduleWidget_) moduleWidget_->disableSensitiveControls();
}

// Enable sensitive controls within widget, ready for main code to run
void ModuleControlWidget::enableSensitiveControls()
{
	// Enable control buttons
	ui.RunButton->setEnabled(true);

	// Enable groups
	ui.ControlGroup->setEnabled(true);
	ui.OptionsGroup->setEnabled(true);
	ui.TopControlsFrame->setEnabled(true);

	if (moduleWidget_) moduleWidget_->enableSensitiveControls();
}

// Return string specifying widget type
const char* ModuleControlWidget::widgetType()
{
	return "ModuleControl";
}

// Write widget state through specified LineParser
bool ModuleControlWidget::writeState(LineParser& parser)
{
	// Write Module target
	if (!parser.writeLineF("%s\n", module_->uniqueName())) return false;

	// Write state data from ModuleWidget (if one exists)
	if (moduleWidget_ && (!moduleWidget_->writeState(parser))) return false;

	return true;
}

// Read widget state through specified LineParser
bool ModuleControlWidget::readState(LineParser& parser)
{
	// First check if there is a current module pointer
	// It is possible that one has not yet been set, e.g. if we are reading in a SubWindow.
	bool moduleSet = module_;

	// Read PairPotential target
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	module_ = ModuleList::findInstanceByUniqueName(parser.argc(0));

	// If we didn't previously have a module, but now we do, initialise the window and its controls
	// This will also create the moduleWidget_ (in initialiseControls())
	if (module_ && (!moduleSet))
	{
		initialiseWindow(module_);
		initialiseControls(module_);
	}

	// Read state data from ModuleWidget (if one exists)
	if (moduleWidget_ && (!moduleWidget_->readState(parser))) return false;

	return true;
}

/*
 * Functions
 */

// Set panel visibility state
void ModuleControlWidget::setPanelState(ModuleControlWidget::PanelState state)
{
	// If there is no Module widget, disallow the 'widget only' state
	if ((state == ModuleControlWidget::OnlyWidgetVisible) && (!moduleWidget_)) state = ModuleControlWidget::ControlsAndWidgetVisible;

	panelState_ = state;

	// Set visibility of panels
	ui.ControlsWidget->setVisible((panelState_ == ModuleControlWidget::ControlsAndWidgetVisible) || (panelState_ == ModuleControlWidget::OnlyControlsVisible));
	ui.WidgetWidget->setVisible((panelState_ == ModuleControlWidget::ControlsAndWidgetVisible) || (panelState_ == ModuleControlWidget::OnlyWidgetVisible));

	// Set icon in toggle button
	QPixmap pixmap;
	if (panelState_ == ModuleControlWidget::ControlsAndWidgetVisible) pixmap = QPixmap(":/general/icons/general_bothon.svg");
	else if (panelState_ == ModuleControlWidget::OnlyControlsVisible) pixmap = QPixmap(":/general/icons/general_lefton.svg");
	else if (panelState_ == ModuleControlWidget::OnlyWidgetVisible) pixmap = QPixmap(":/general/icons/general_righton.svg");
	QIcon icon(pixmap);
	ui.TogglePanelsButton->setIcon(icon);
}

/*
 * Widget Slots
 */

void ModuleControlWidget::on_ShiftLeftButton_clicked(bool checked)
{
	emit (shiftModuleLeft(module_));
}

void ModuleControlWidget::on_ShiftRightButton_clicked(bool checked)
{
	emit (shiftModuleRight(module_));
}

void ModuleControlWidget::on_RemoveButton_clicked(bool checked)
{
	emit (removeModule(module_));
}

void ModuleControlWidget::on_RunButton_clicked(bool checked)
{
	module_->executeMainProcessing(duq_, duq_.worldPool());

	emit moduleRun();
}

void ModuleControlWidget::on_TogglePanelsButton_clicked(bool checked)
{
	setPanelState((ModuleControlWidget::PanelState) ((panelState_+1)%ModuleControlWidget::nPanelStates));
}

void ModuleControlWidget::on_EnabledCheck_clicked(bool checked)
{
	if (refreshing_) return;

	module_->setEnabled(checked);

	duqWindow_->setModified();
}

void ModuleControlWidget::on_FrequencySpin_valueChanged(int value)
{
	if (refreshing_) return;

	module_->setFrequency(value);

	duqWindow_->setModified();
}
