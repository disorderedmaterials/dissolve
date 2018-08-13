/*
	*** Module Control Widget - Functions
	*** src/gui/modulecontrolwidget_funcs.cpp
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
#include "main/dissolve.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include <QGridLayout>
#include <QLabel>

// Constructor
ModuleControlWidget::ModuleControlWidget(DissolveWindow* dissolveWindow, Module* module, const char* title, bool showTopControls) : SubWidget(dissolveWindow, title), module_(module), dissolveWindow_(dissolveWindow), dissolve_(dissolveWindow->dissolve())
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

	// Select source list for keywords that have potentially been replicated / updated there
	GenericList& moduleData = module->configurationLocal() ? module->targetConfigurations().firstItem()->moduleData() : dissolve_.processingModuleData();

	ListIterator<ModuleKeywordBase> keywordIterator(module->keywords().keywords());
	while (ModuleKeywordBase* keyword = keywordIterator.iterate())
	{
		QLabel* nameLabel = new QLabel(keyword->keyword());
		nameLabel->setToolTip(keyword->description());
		keywordsLayout->addWidget(nameLabel, row, 0);

		// The widget to create here depends on the data type of the keyword
		if (keyword->type() == ModuleKeywordBase::IntegerData)
		{
			IntegerKeywordWidget* intWidget = new IntegerKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(intWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = intWidget;
			base = intWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::DoubleData)
		{
			DoubleKeywordWidget* doubleWidget = new DoubleKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(doubleWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = doubleWidget;
			base = doubleWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::CharStringData)
		{
			CharStringKeywordWidget* charWidget = new CharStringKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(charWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = charWidget;
			base = charWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::BoolData)
		{
			BoolKeywordWidget* boolWidget = new BoolKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(boolWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = boolWidget;
			base = boolWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::BroadeningFunctionData)
		{
			BroadeningFunctionKeywordWidget* broadeningFunctionWidget = new BroadeningFunctionKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(broadeningFunctionWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = broadeningFunctionWidget;
			base = broadeningFunctionWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::IsotopologueListData)
		{
			IsotopologueListKeywordWidget* isotopologueListWidget = new IsotopologueListKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(isotopologueListWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = isotopologueListWidget;
			base = isotopologueListWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::WindowFunctionData)
		{
			WindowFunctionKeywordWidget* windowFunctionWidget = new WindowFunctionKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(windowFunctionWidget, SIGNAL(keywordValueChanged()), dissolveWindow_, SLOT(setModified()));
			widget = windowFunctionWidget;
			base = windowFunctionWidget;
		}
		else
		{
			widget = NULL;
			base = NULL;
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
	moduleWidget_ = module->createWidget(ui.WidgetWidget, dissolve_);
	if (moduleWidget_ == NULL) Messenger::printVerbose("Module '%s' did not provide a valid controller widget.\n", module->name());
	else widgetLayout->addWidget(moduleWidget_);

	// Set and disable the panel toggle button if there is no Module widget
	if (!moduleWidget_)
	{
		setPanelState(ModuleControlWidget::OnlyControlsVisible);
		ui.TogglePanelsButton->setEnabled(false);
	}
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
	ui.RunsInLabel->setText(module_->frequencyDetails(dissolve_.iteration()));

	RefListIterator<KeywordWidgetBase,bool> keywordIterator(keywordWidgets_);
	while (KeywordWidgetBase* keywordWidget = keywordIterator.iterate()) keywordWidget->updateValue();

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
	if (!module_) return false;

	// Write state data from ModuleWidget (if one exists)
	if (moduleWidget_ && (!moduleWidget_->writeState(parser))) return false;

	return true;
}

// Read widget state through specified LineParser
bool ModuleControlWidget::readState(LineParser& parser)
{
	if (!module_) return false;

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

void ModuleControlWidget::on_RunButton_clicked(bool checked)
{
	if (!module_) return;

	module_->executeProcessing(dissolve_, dissolve_.worldPool());

	updateControls();

	emit moduleRun();
}

void ModuleControlWidget::on_TogglePanelsButton_clicked(bool checked)
{
	setPanelState((ModuleControlWidget::PanelState) ((panelState_+1)%ModuleControlWidget::nPanelStates));
}

void ModuleControlWidget::on_EnabledCheck_clicked(bool checked)
{
	if (refreshing_) return;

	if (!module_) return;

	module_->setEnabled(checked);

	dissolveWindow_->setModified();
}

void ModuleControlWidget::on_FrequencySpin_valueChanged(int value)
{
	if (refreshing_) return;

	if (!module_) return;
	
	module_->setFrequency(value);

	dissolveWindow_->setModified();
}
