/*
	*** Partials Module Widget - Functions
	*** src/modules/partials/gui/modulewidget_funcs.cpp
	Copyright T. Youngs 2007-2017

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

#include "modules/partials/gui/modulewidget.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/mimetreewidgetitem.h"
#include "main/duq.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
PartialsModuleWidget::PartialsModuleWidget(QWidget* parent, Module* module, DUQ& dUQ) : ModuleWidget(parent), module_(module), dUQ_(dUQ)
{
	// Set up user interface
	ui.setupUi(this);

	// Add a UChromaWidget to the PlotWidget
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(4);
	uChromaView_ = new UChromaViewWidget;
	layout->addWidget(uChromaView_);
	ui.PlotWidget->setLayout(layout);

	// Start a new, empty session
	uChromaView_->startNewSession(true);

	// Set up the view pane
	ViewPane* viewPane = uChromaView_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, Angstroms");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "g(r) / S(Q)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);

	refreshing_ = false;
}

PartialsModuleWidget::~PartialsModuleWidget()
{
}

// Update controls within widget
void PartialsModuleWidget::updateControls()
{
	// Repopulate source tree
	repopulateSourceTree();

	// Ensure that any displayed data is up-to-date
	uChromaView_->refreshReferencedDataSets();

	uChromaView_->updateDisplay();
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool PartialsModuleWidget::writeState(LineParser& parser)
{
	// Write UChromaWidget session
	if (!uChromaView_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool PartialsModuleWidget::readState(LineParser& parser)
{
	// Read UChromaWidget session
	if (!uChromaView_->readSession(parser)) return false;
	
	return true;
}

/*
 * Widgets / Functions
 */

// Add data from PartialSet to tree
void PartialsModuleWidget::addPartialSetToTree(PartialSet& partials, QTreeWidgetItem* topLevelItem, PartialsModuleWidget::DataType rootType, const char* rootTitle, int addAverageContributions)
{
	// Create item to contain data
	QTreeWidgetItem* setItem = new QTreeWidgetItem(rootType);
	setItem->setText(0, rootTitle);
	setItem->setData(0, Qt::UserRole, VariantPointer<PartialSet>(&partials));

	if (topLevelItem) topLevelItem->addChild(setItem);
	else ui.SourcesTree->addTopLevelItem(setItem);

	// Loop over AtomType pairs
	CharString groupName;
	AtomTypeData* atd1 = partials.atomTypes().first();
	for (int n=0; n<partials.atomTypes().nItems(); ++n, atd1 = atd1->next)
	{
		AtomTypeData* atd2 = atd1;
		for (int m=n; m<partials.atomTypes().nItems(); ++m, atd2 = atd2->next)
		{
			// Set group name
			groupName.sprintf("%s-%s", atd1->atomTypeName(), atd2->atomTypeName());

			// Create item to contain full data
			MimeTreeWidgetItem* fullItem = new MimeTreeWidgetItem(setItem, rootType+PartialsModuleWidget::FullData);
			fullItem->setText(0, groupName.get());
 			fullItem->addMimeString(MimeString::UChromaCollectionBlockType, QString("Collection '%1'; Group '%1'; LineStyle 1.0 Solid; DataSet 'Full'; Source XYData %2; EndDataSet; EndCollection").arg(groupName.get(), partials.partial(n,m).objectName()));

			// Add subitems with other data
			// -- Bound partial
			MimeTreeWidgetItem* subItem = new MimeTreeWidgetItem(fullItem, rootType+PartialsModuleWidget::BoundData);
			subItem->setText(0, "Bound");
 			subItem->addMimeString(MimeString::UChromaCollectionBlockType, QString("Collection '%1 Bnd'; Group '%1'; LineStyle 1.0 Dots; DataSet 'Bound'; Source XYData %2; EndDataSet; EndCollection").arg(groupName.get(), partials.boundPartial(n,m).objectName()));

			// -- Unbound partial
			subItem = new MimeTreeWidgetItem(fullItem, rootType+PartialsModuleWidget::UnboundData);
			subItem->setText(0, "Unbound");
 			subItem->addMimeString(MimeString::UChromaCollectionBlockType, QString("Collection '%1 Unb'; Group '%1'; LineStyle 1.0 'Quarter Dash'; DataSet 'Unbound'; Source XYData %2; EndDataSet; EndCollection").arg(groupName.get(), partials.unboundPartial(n,m).objectName()));

			// -- Bragg partial
			subItem = new MimeTreeWidgetItem(fullItem, rootType+PartialsModuleWidget::BraggData);
			subItem->setText(0, "Bragg");
			subItem->addMimeString(MimeString::UChromaCollectionBlockType, QString("Collection '%1 Brg'; Group '%1'; LineStyle 1.0 'Dot Dash 1'; DataSet 'Bragg'; Source XYData %2; EndDataSet; EndCollection").arg(groupName.get(), partials.partial(n,m).objectName()));

			// -- Partials forming full average
			if (addAverageContributions > 0)
			{
				// Construct the Collection block
				QString block = QString("Collection '%1 Avg'; Group '%1'; LineStyle 1.0 Solid;").arg(groupName.get());
				for (int avg=1; avg<addAverageContributions; ++avg) block += QString(" DataSet 'Average %1'; Source XYData %2; Z %3; EndDataSet;").arg(avg).arg(partials.partial(n,m).objectName()).arg(avg*1.0);
				block += " EndCollection";
				subItem = new MimeTreeWidgetItem(fullItem, rootType+PartialsModuleWidget::FullData);
				subItem->setText(0, "Avg");
				subItem->addMimeString(MimeString::UChromaCollectionBlockType, block);
			}
		}
	}
}

// Repopulate tree with source data
void PartialsModuleWidget::repopulateSourceTree()
{
	// Clear the tree
	ui.SourcesTree->clear();

	// Add on source data as appropriate - we will need to see what we can find in the relevant Module data
	GenericList& moduleData = module_->configurationLocal() ? module_->targetConfigurations().firstItem()->moduleData() : dUQ_.processingModuleData();

	QTreeWidgetItem* cfgItem, *setItem, *fullItem, *subItem;

	// Loop over Configurations targeted by Module
	RefListIterator<Configuration,bool> configIterator(module_->targetConfigurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		// Add on an item for the Configuration, unless we are local to a configuration in which case we'll add items at the top level
		if (!module_->configurationLocal())
		{
			cfgItem = new QTreeWidgetItem();
			cfgItem->setText(0, cfg->name());
			ui.SourcesTree->addTopLevelItem(cfgItem);
		}
		else cfgItem = NULL;

		// Grab averaging integer
		const int maxSetsInAverage = module_->keywords().asInt("Averaging");

		// Unweighted partial g(r)
		if (cfg->moduleData().contains("UnweightedGR", "Partials"))
		{
			// Get the PartialSet
			PartialSet& unweightedgr = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedGR", "Partials");

			addPartialSetToTree(unweightedgr, cfgItem, PartialsModuleWidget::UnweightedGRData, "Unweighted g(r)", maxSetsInAverage);
		}

		// Unweighted partial S(Q)
		if (cfg->moduleData().contains("UnweightedSQ", "Partials"))
		{
			// Get the PartialSet
			PartialSet& unweightedsq = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedSQ", "Partials");

			addPartialSetToTree(unweightedsq, cfgItem, PartialsModuleWidget::UnweightedSQData, "Unweighted S(Q)");
		}
	}
}

void PartialsModuleWidget::on_SourcesTree_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	if (item == NULL) return;

	// Get current view pane
	ViewPane* viewPane = uChromaView_->currentViewPane();
	printf("ViewPane = %p\n", viewPane);

	// Check the type of the item clicked
	switch (item->type())
	{
		case (PartialsModuleWidget::FullData):
			break;
	}
}
