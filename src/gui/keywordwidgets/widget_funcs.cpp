// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "gui/keywordwidgets/widget.hui"
#include "gui/keywordwidgets/widgets.h"
#include "main/dissolve.h"
#include "module/module.h"
#include <QFormLayout>
#include <QLabel>
#include <QToolBox>

KeywordsWidget::KeywordsWidget(QWidget *parent) : QToolBox(parent)
{
    refreshing_ = false;
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
}

KeywordsWidget::~KeywordsWidget() {}

/*
 * Controls
 */

// Create widget for specified keyword
QWidget *KeywordsWidget::createKeywordWidget(RefList<KeywordWidgetBase> &keywordWidgets, KeywordBase *keyword,
                                             const CoreData &coreData)
{
    QWidget *widget = nullptr;
    KeywordWidgetBase *base = nullptr;

    // Ensure that we have the base keyword pointer, and its datatype
    KeywordBase *keywordBase = keyword->base();
    KeywordBase::KeywordDataType type = keywordBase->type();

    // Try to create a suitable widget
    if (type == KeywordBase::AtomTypeVectorData)
    {
        auto *atomTypeVectorWidget = new AtomTypeVectorKeywordWidget(nullptr, keywordBase, coreData);
        connect(atomTypeVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = atomTypeVectorWidget;
        base = atomTypeVectorWidget;
    }
    else if (type == KeywordBase::BoolData)
    {
        BoolKeywordWidget *boolWidget = new BoolKeywordWidget(nullptr, keywordBase, coreData);
        connect(boolWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = boolWidget;
        base = boolWidget;
    }
    else if (type == KeywordBase::ConfigurationVectorData)
    {
        ConfigurationVectorKeywordWidget *configurationRefListWidget =
            new ConfigurationVectorKeywordWidget(nullptr, keywordBase, coreData);
        connect(configurationRefListWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = configurationRefListWidget;
        base = configurationRefListWidget;
    }
    else if (type == KeywordBase::DoubleData)
    {
        DoubleKeywordWidget *doubleWidget = new DoubleKeywordWidget(nullptr, keywordBase, coreData);
        connect(doubleWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = doubleWidget;
        base = doubleWidget;
    }
    else if (type == KeywordBase::EnumOptionsData)
    {
        EnumOptionsKeywordWidget *enumOptionsWidget = new EnumOptionsKeywordWidget(nullptr, keywordBase, coreData);
        connect(enumOptionsWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = enumOptionsWidget;
        base = enumOptionsWidget;
    }
    else if (type == KeywordBase::ExpressionVariableVectorData)
    {
        ExpressionVariableVectorKeywordWidget *expressionVariableVectorWidget =
            new ExpressionVariableVectorKeywordWidget(nullptr, keywordBase, coreData);
        connect(expressionVariableVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = expressionVariableVectorWidget;
        base = expressionVariableVectorWidget;
    }
    else if (type == KeywordBase::FileAndFormatData)
    {
        FileAndFormatKeywordWidget *fileAndFormatWidget = new FileAndFormatKeywordWidget(nullptr, keywordBase, coreData);
        connect(fileAndFormatWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = fileAndFormatWidget;
        base = fileAndFormatWidget;
    }
    else if (type == KeywordBase::Function1DData)
    {
        Function1DKeywordWidget *function1DWidget = new Function1DKeywordWidget(nullptr, keywordBase, coreData);
        connect(function1DWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = function1DWidget;
        base = function1DWidget;
    }
    else if (type == KeywordBase::IntegerData)
    {
        IntegerKeywordWidget *intWidget = new IntegerKeywordWidget(nullptr, keywordBase, coreData);
        connect(intWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = intWidget;
        base = intWidget;
    }
    else if (type == KeywordBase::IsotopologueSetData)
    {
        IsotopologueSetKeywordWidget *isotopologueSetWidget = new IsotopologueSetKeywordWidget(nullptr, keywordBase, coreData);
        connect(isotopologueSetWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = isotopologueSetWidget;
        base = isotopologueSetWidget;
    }
    else if (type == KeywordBase::ModuleData)
    {
        ModuleKeywordWidget *moduleWidget = new ModuleKeywordWidget(nullptr, keywordBase, coreData);
        connect(moduleWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = moduleWidget;
        base = moduleWidget;
    }
    else if (type == KeywordBase::ModuleRefListData)
    {
        ModuleVectorKeywordWidget *ModuleVectorWidget = new ModuleVectorKeywordWidget(nullptr, keywordBase, coreData);
        connect(ModuleVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = ModuleVectorWidget;
        base = ModuleVectorWidget;
    }
    else if (type == KeywordBase::NodeData)
    {
        NodeKeywordWidget *nodeWidget = new NodeKeywordWidget(nullptr, keywordBase, coreData);
        connect(nodeWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = nodeWidget;
        base = nodeWidget;
    }
    else if (type == KeywordBase::NodeAndIntegerData)
    {
        NodeAndIntegerKeywordWidget *nodeAndIntegerWidget = new NodeAndIntegerKeywordWidget(nullptr, keywordBase, coreData);
        connect(nodeAndIntegerWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = nodeAndIntegerWidget;
        base = nodeAndIntegerWidget;
    }
    else if (type == KeywordBase::NodeValueData)
    {
        NodeValueKeywordWidget *nodeValueWidget = new NodeValueKeywordWidget(nullptr, keywordBase, coreData);
        connect(nodeValueWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = nodeValueWidget;
        base = nodeValueWidget;
    }
    else if (type == KeywordBase::NodeValueEnumOptionsData)
    {
        NodeValueEnumOptionsKeywordWidget *nodeValueEnumOptionsWidget =
            new NodeValueEnumOptionsKeywordWidget(nullptr, keywordBase, coreData);
        connect(nodeValueEnumOptionsWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = nodeValueEnumOptionsWidget;
        base = nodeValueEnumOptionsWidget;
    }
    else if (type == KeywordBase::NodeVectorData)
    {
        NodeVectorKeywordWidget *nodeVectorWidget = new NodeVectorKeywordWidget(nullptr, keywordBase, coreData);
        connect(nodeVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = nodeVectorWidget;
        base = nodeVectorWidget;
    }
    else if (type == KeywordBase::RangeData)
    {
        RangeKeywordWidget *rangeWidget = new RangeKeywordWidget(nullptr, keywordBase, coreData);
        connect(rangeWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = rangeWidget;
        base = rangeWidget;
    }
    else if (type == KeywordBase::SpeciesData)
    {
        SpeciesKeywordWidget *speciesWidget = new SpeciesKeywordWidget(nullptr, keywordBase, coreData);
        connect(speciesWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = speciesWidget;
        base = speciesWidget;
    }
    else if (type == KeywordBase::SpeciesSiteData)
    {
        SpeciesSiteKeywordWidget *speciesSiteWidget = new SpeciesSiteKeywordWidget(nullptr, keywordBase, coreData);
        connect(speciesSiteWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = speciesSiteWidget;
        base = speciesSiteWidget;
    }
    else if (type == KeywordBase::SpeciesSiteVectorData)
    {
        SpeciesSiteVectorKeywordWidget *speciesSiteVectorWidget =
            new SpeciesSiteVectorKeywordWidget(nullptr, keywordBase, coreData);
        connect(speciesSiteVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = speciesSiteVectorWidget;
        base = speciesSiteVectorWidget;
    }
    else if (type == KeywordBase::SpeciesVectorData)
    {
        SpeciesVectorKeywordWidget *speciesVectorWidget = new SpeciesVectorKeywordWidget(nullptr, keywordBase, coreData);
        connect(speciesVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = speciesVectorWidget;
        base = speciesVectorWidget;
    }
    else if (type == KeywordBase::StringData)
    {
        StringKeywordWidget *charWidget = new StringKeywordWidget(nullptr, keywordBase, coreData);
        connect(charWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = charWidget;
        base = charWidget;
    }
    else if (type == KeywordBase::Vec3DoubleData)
    {
        Vec3DoubleKeywordWidget *vec3DoubleWidget = new Vec3DoubleKeywordWidget(nullptr, keywordBase, coreData);
        connect(vec3DoubleWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = vec3DoubleWidget;
        base = vec3DoubleWidget;
    }
    else if (type == KeywordBase::Vec3IntegerData)
    {
        Vec3IntegerKeywordWidget *vec3IntWidget = new Vec3IntegerKeywordWidget(nullptr, keywordBase, coreData);
        connect(vec3IntWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = vec3IntWidget;
        base = vec3IntWidget;
    }
    else if (type == KeywordBase::Vec3NodeValueData)
    {
        Vec3NodeValueKeywordWidget *vec3NodeValueWidget = new Vec3NodeValueKeywordWidget(nullptr, keywordBase, coreData);
        connect(vec3NodeValueWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = vec3NodeValueWidget;
        base = vec3NodeValueWidget;
    }

    // Set tooltip on widget (using the description from the keyword pointer passed, rather than its base) and add to the
    // list of widgets
    if (widget)
    {
        widget->setToolTip(QString::fromStdString(std::string(keyword->description())));
        keywordWidgets.append(base);
    }

    return widget;
}

// Set up keyword controls for specified keyword list
void KeywordsWidget::setUp(const KeywordList &keywords, const CoreData &coreData)
{
    // Clear existing item groups....
    while (count() > 0)
        removeItem(0);
    keywordWidgets_.clear();

    // Loop over keyword groups first - we'll keep track of which keywords are not part of a group, and these in an 'Other'
    // tab at the end
    RefList<KeywordBase> remainingKeywords;
    for (auto *keyword : keywords.keywords())
        remainingKeywords.append(keyword);

    for (auto &group : keywords.groups())
    {
        // If this is the 'HIDDEN' group, don't display any of its widgets
        if (DissolveSys::sameString(group.name(), "HIDDEN"))
        {
            // Remove all keywords in this group from the remainingKeywords list
            for (KeywordBase *keyword : group.keywords())
                remainingKeywords.remove(keyword);

            continue;
        }

        // Create a new QWidget and layout for our widgets
        QWidget *groupWidget = new QWidget;
        QFormLayout *groupLayout = new QFormLayout(groupWidget);

        // Loop over keywords in the group and add them to our groupbox
        for (auto *keyword : group.keywords())
        {
            // Create / setup the keyword widget
            QWidget *widget = createKeywordWidget(keywordWidgets_, keyword, coreData);

            // Can now remove this keyword from our reference list
            remainingKeywords.remove(keyword);

            if (!widget)
            {
                // WORKAROUND - Don't raise errors for datastore types (issue #36)
                if ((keyword->type() == KeywordBase::Data1DStoreData) || (keyword->type() == KeywordBase::Data2DStoreData) ||
                    (keyword->type() == KeywordBase::Data3DStoreData))
                    continue;

                Messenger::error("Can't create widget for keyword '{}' ({}).\n", keyword->name(),
                                 KeywordBase::keywordDataType(keyword->type()));
                continue;
            }

            // Create a label and add it and the widget to our layout
            QLabel *nameLabel = new QLabel(QString::fromStdString(std::string(keyword->name())));
            nameLabel->setToolTip(QString::fromStdString(std::string(keyword->description())));
            groupLayout->addRow(nameLabel, widget);
        }

        // Group is finished, so add the widget as a new tab in our QToolBox
        addItem(groupWidget, QString::fromStdString(std::string(group.name())));
    }

    // If there are any 'group-orphaned' keywords, add these at the bottom
    if (remainingKeywords.nItems() > 0)
    {
        // Need a widget with a QFormLayout...
        QWidget *groupWidget = new QWidget;
        QFormLayout *groupLayout = new QFormLayout(groupWidget);

        for (KeywordBase *keyword : remainingKeywords)
        {
            // Create / setup the keyword widget
            QWidget *widget = createKeywordWidget(keywordWidgets_, keyword, coreData);

            if (!widget)
            {
                Messenger::error("Can't create widget for keyword '{}'.\n", keyword->name());
                continue;
            }

            // Create a label and add it and the widget to our layout
            QLabel *nameLabel = new QLabel(QString::fromStdString(std::string(keyword->name())));
            nameLabel->setToolTip(QString::fromStdString(std::string(keyword->description())));
            groupLayout->addRow(nameLabel, widget);
        }

        // Group is finished, so add the widget as a new tab in our QToolBox
        addItem(groupWidget, "Other");
    }
}

// Update controls within widget
void KeywordsWidget::updateControls()
{
    refreshing_ = true;

    // Update all our keyword widgets
    for (KeywordWidgetBase *keywordWidget : keywordWidgets_)
        keywordWidget->updateValue();

    refreshing_ = false;
}

/*
 * Signals / Slots
 */

// Keyword data changed
void KeywordsWidget::keywordDataChanged(int flags)
{
    // Always emit the 'dataModified' signal
    emit(dataModified());

    // Set-up of encompassing class required?
    if (flags & KeywordBase::ModificationRequiresSetUpOption)
        emit(setUpRequired());
}
