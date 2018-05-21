/*
	*** Flow Block Widget - Functions
	*** src/gui/flowblock_funcs.cpp
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

#include "gui/flowblock.h"
#include "gui/gui.h"
#include "gui/keywordwidgets.h"
#include "main/duq.h"
#include "classes/configuration.h"

// Constructor
FlowBlock::FlowBlock(QWidget* parent, DUQWindow* duqWindow, ModuleReference* modRef) : QWidget(parent), duqWindow_(duqWindow), duq_(duqWindow->duq())
{
	// Set up user interface
	ui.setupUi(this);

	ui.KeywordsFrame->setVisible(false);

	moduleReference_ = modRef;

	module_ = moduleReference_ ? moduleReference_->module() : NULL;

	refreshing_ = false;

	initialiseWindow(module_);

	initialiseControls(module_);
}

FlowBlock::~FlowBlock()
{
}

// Initialise window
void FlowBlock::initialiseWindow(Module* module)
{
	// Set information panel contents
	if (module)
	{
		CharString topText("%s (%s)", module->name(), module->uniqueName());
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
void FlowBlock::initialiseControls(Module* module)
{
	if (!module) return;

	// Create keyword widgets in a new grid layout
	QGridLayout* keywordsLayout = new QGridLayout(ui.KeywordsFrame);
	keywordsLayout->setContentsMargins(4,4,4,4);
	keywordsLayout->setSpacing(4);
	int row = 0;
	QWidget* widget;
	KeywordWidgetBase* base;

	// Select source list for keywords that have potentially been replicated / updated there
	GenericList& moduleData = module->configurationLocal() ? module->targetConfigurations().firstItem()->moduleData() : duq_.processingModuleData();

	ListIterator<ModuleKeywordBase> keywordIterator(module->keywords().keywords());
	while (ModuleKeywordBase* keyword = keywordIterator.iterate())
	{
		printf("Keyword: %s\n", keyword->keyword());
		QLabel* nameLabel = new QLabel(keyword->keyword());
		nameLabel->setToolTip(keyword->description());
		keywordsLayout->addWidget(nameLabel, row, 0);

		// The widget to create here depends on the data type of the keyword
		if (keyword->type() == ModuleKeywordBase::IntegerData)
		{
			IntegerKeywordWidget* intWidget = new IntegerKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(intWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = intWidget;
			base = intWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::DoubleData)
		{
			DoubleKeywordWidget* doubleWidget = new DoubleKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(doubleWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = doubleWidget;
			base = doubleWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::CharStringData)
		{
			CharStringKeywordWidget* charWidget = new CharStringKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(charWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = charWidget;
			base = charWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::BoolData)
		{
			BoolKeywordWidget* boolWidget = new BoolKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(boolWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = boolWidget;
			base = boolWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::BroadeningFunctionData)
		{
			BroadeningFunctionKeywordWidget* broadeningFunctionWidget = new BroadeningFunctionKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(broadeningFunctionWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = broadeningFunctionWidget;
			base = broadeningFunctionWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::IsotopologueListData)
		{
			IsotopologueListKeywordWidget* isotopologueListWidget = new IsotopologueListKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(isotopologueListWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
			widget = isotopologueListWidget;
			base = isotopologueListWidget;
		}
		else if (keyword->type() == ModuleKeywordBase::WindowFunctionData)
		{
			WindowFunctionKeywordWidget* windowFunctionWidget = new WindowFunctionKeywordWidget(NULL, keyword, moduleData, module->uniqueName());
			connect(windowFunctionWidget, SIGNAL(keywordValueChanged()), duqWindow_, SLOT(setModified()));
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

// 	// Add a vertical spacer to the end to take up any extra space
// 	keywordsLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0);

// 	// Create module widget
// 	QVBoxLayout* widgetLayout = new QVBoxLayout(ui.WidgetWidget);
// 	widgetLayout->setContentsMargins(0,0,0,0);
// 	widgetLayout->setSpacing(0);
// 	moduleWidget_ = module->createWidget(ui.WidgetWidget, duq_);
// 	if (moduleWidget_ == NULL) Messenger::printVerbose("Module '%s' did not provide a valid controller widget.\n", module->name());
// 	else widgetLayout->addWidget(moduleWidget_);
// 
// 	// Set and disable the panel toggle button if there is no Module widget
// 	if (!moduleWidget_)
// 	{
// 		setPanelState(ModuleControlWidget::OnlyControlsVisible);
// 		ui.TogglePanelsButton->setEnabled(false);
// 	}
}

/*
 * Module / Reference Data
 */

// Return reference for associated Module
ModuleReference* FlowBlock::moduleReference()
{
	return moduleReference_;
}

/*
 * Widget Functions
 */

void FlowBlock::closeEvent(QCloseEvent* event)
{
}

// Update controls within widget
void FlowBlock::updateControls()
{
	if (!module_) return;

}

// Disable sensitive controls within widget, ready for main code to run
void FlowBlock::disableSensitiveControls()
{
	// Disable control buttons
	ui.RunButton->setEnabled(false);
	ui.RemoveButton->setEnabled(false);

// 	if (moduleWidget_) moduleWidget_->disableSensitiveControls();
}

// Enable sensitive controls within widget, ready for main code to run
void FlowBlock::enableSensitiveControls()
{
	// Enable control buttons
	ui.RunButton->setEnabled(true);
	ui.RemoveButton->setEnabled(true);

// 	if (moduleWidget_) moduleWidget_->enableSensitiveControls();
}

void FlowBlock::on_ToggleKeywordsButton_clicked(bool checked)
{
	ui.KeywordsFrame->setVisible(checked);

	emit(settingsToggled());
}

void FlowBlock::on_RemoveButton_clicked(bool checked)
{
	emit (removeModule(moduleReference_));
}

void FlowBlock::on_RunButton_clicked(bool checked)
{
	if (!module_) return;

	module_->executeMainProcessing(duq_, duq_.worldPool());

	emit moduleRun();
}

void FlowBlock::on_EnabledCheck_clicked(bool checked)
{
	if (refreshing_) return;

	if (!module_) return;

	module_->setEnabled(checked);

	duqWindow_->setModified();
}

void FlowBlock::on_FrequencySpin_valueChanged(int value)
{
	if (refreshing_) return;

	if (!module_) return;
	
	module_->setFrequency(value);

	duqWindow_->setModified();
}
