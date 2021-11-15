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
    KeywordBase::KeywordDataType type = keyword->type();

    // Try to create a suitable widget
    if (type == KeywordBase::AtomTypeVectorData)
    {
        auto *atomTypeVectorWidget = new AtomTypeVectorKeywordWidget(nullptr, keyword, coreData);
        connect(atomTypeVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = atomTypeVectorWidget;
        base = atomTypeVectorWidget;
    }
    else if (type == KeywordBase::BoolData)
    {
        BoolKeywordWidget *boolWidget = new BoolKeywordWidget(nullptr, keyword, coreData);
        connect(boolWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = boolWidget;
        base = boolWidget;
    }
    else if (type == KeywordBase::ConfigurationVectorData)
    {
        ConfigurationVectorKeywordWidget *configurationRefListWidget =
            new ConfigurationVectorKeywordWidget(nullptr, keyword, coreData);
        connect(configurationRefListWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = configurationRefListWidget;
        base = configurationRefListWidget;
    }
    else if (type == KeywordBase::DoubleData)
    {
        DoubleKeywordWidget *doubleWidget = new DoubleKeywordWidget(nullptr, keyword, coreData);
        connect(doubleWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = doubleWidget;
        base = doubleWidget;
    }
    else if (type == KeywordBase::EnumOptionsData)
    {
        EnumOptionsKeywordWidget *enumOptionsWidget = new EnumOptionsKeywordWidget(nullptr, keyword, coreData);
        connect(enumOptionsWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = enumOptionsWidget;
        base = enumOptionsWidget;
    }
    else if (type == KeywordBase::ExpressionVariableVectorData)
    {
        ExpressionVariableVectorKeywordWidget *expressionVariableVectorWidget =
            new ExpressionVariableVectorKeywordWidget(nullptr, keyword, coreData);
        connect(expressionVariableVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = expressionVariableVectorWidget;
        base = expressionVariableVectorWidget;
    }
    else if (type == KeywordBase::FileAndFormatData)
    {
        FileAndFormatKeywordWidget *fileAndFormatWidget = new FileAndFormatKeywordWidget(nullptr, keyword, coreData);
        connect(fileAndFormatWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = fileAndFormatWidget;
        base = fileAndFormatWidget;
    }
    else if (type == KeywordBase::Function1DData)
    {
        Function1DKeywordWidget *function1DWidget = new Function1DKeywordWidget(nullptr, keyword, coreData);
        connect(function1DWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = function1DWidget;
        base = function1DWidget;
    }
    else if (type == KeywordBase::IntegerData)
    {
        IntegerKeywordWidget *intWidget = new IntegerKeywordWidget(nullptr, keyword, coreData);
        connect(intWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = intWidget;
        base = intWidget;
    }
    else if (type == KeywordBase::IsotopologueSetData)
    {
        IsotopologueSetKeywordWidget *isotopologueSetWidget = new IsotopologueSetKeywordWidget(nullptr, keyword, coreData);
        connect(isotopologueSetWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = isotopologueSetWidget;
        base = isotopologueSetWidget;
    }
    else if (type == KeywordBase::ModuleData)
    {
        ModuleKeywordWidget *moduleWidget = new ModuleKeywordWidget(nullptr, keyword, coreData);
        connect(moduleWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = moduleWidget;
        base = moduleWidget;
    }
    else if (type == KeywordBase::ModuleRefListData)
    {
        ModuleVectorKeywordWidget *ModuleVectorWidget = new ModuleVectorKeywordWidget(nullptr, keyword, coreData);
        connect(ModuleVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = ModuleVectorWidget;
        base = ModuleVectorWidget;
    }
    else if (type == KeywordBase::NodeData)
    {
        NodeKeywordWidget *nodeWidget = new NodeKeywordWidget(nullptr, keyword, coreData);
        connect(nodeWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = nodeWidget;
        base = nodeWidget;
    }
    else if (type == KeywordBase::NodeAndIntegerData)
    {
        NodeAndIntegerKeywordWidget *nodeAndIntegerWidget = new NodeAndIntegerKeywordWidget(nullptr, keyword, coreData);
        connect(nodeAndIntegerWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = nodeAndIntegerWidget;
        base = nodeAndIntegerWidget;
    }
    else if (type == KeywordBase::NodeValueData)
    {
        NodeValueKeywordWidget *nodeValueWidget = new NodeValueKeywordWidget(nullptr, keyword, coreData);
        connect(nodeValueWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = nodeValueWidget;
        base = nodeValueWidget;
    }
    else if (type == KeywordBase::NodeValueEnumOptionsData)
    {
        NodeValueEnumOptionsKeywordWidget *nodeValueEnumOptionsWidget =
            new NodeValueEnumOptionsKeywordWidget(nullptr, keyword, coreData);
        connect(nodeValueEnumOptionsWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = nodeValueEnumOptionsWidget;
        base = nodeValueEnumOptionsWidget;
    }
    else if (type == KeywordBase::NodeVectorData)
    {
        NodeVectorKeywordWidget *nodeVectorWidget = new NodeVectorKeywordWidget(nullptr, keyword, coreData);
        connect(nodeVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = nodeVectorWidget;
        base = nodeVectorWidget;
    }
    else if (type == KeywordBase::RangeData)
    {
        RangeKeywordWidget *rangeWidget = new RangeKeywordWidget(nullptr, keyword, coreData);
        connect(rangeWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = rangeWidget;
        base = rangeWidget;
    }
    else if (type == KeywordBase::SpeciesData)
    {
        SpeciesKeywordWidget *speciesWidget = new SpeciesKeywordWidget(nullptr, keyword, coreData);
        connect(speciesWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = speciesWidget;
        base = speciesWidget;
    }
    else if (type == KeywordBase::SpeciesSiteData)
    {
        SpeciesSiteKeywordWidget *speciesSiteWidget = new SpeciesSiteKeywordWidget(nullptr, keyword, coreData);
        connect(speciesSiteWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = speciesSiteWidget;
        base = speciesSiteWidget;
    }
    else if (type == KeywordBase::SpeciesSiteVectorData)
    {
        SpeciesSiteVectorKeywordWidget *speciesSiteVectorWidget =
            new SpeciesSiteVectorKeywordWidget(nullptr, keyword, coreData);
        connect(speciesSiteVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = speciesSiteVectorWidget;
        base = speciesSiteVectorWidget;
    }
    else if (type == KeywordBase::SpeciesVectorData)
    {
        SpeciesVectorKeywordWidget *speciesVectorWidget = new SpeciesVectorKeywordWidget(nullptr, keyword, coreData);
        connect(speciesVectorWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = speciesVectorWidget;
        base = speciesVectorWidget;
    }
    else if (type == KeywordBase::StringData)
    {
        StringKeywordWidget *charWidget = new StringKeywordWidget(nullptr, keyword, coreData);
        connect(charWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = charWidget;
        base = charWidget;
    }
    else if (type == KeywordBase::Vec3DoubleData)
    {
        Vec3DoubleKeywordWidget *vec3DoubleWidget = new Vec3DoubleKeywordWidget(nullptr, keyword, coreData);
        connect(vec3DoubleWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = vec3DoubleWidget;
        base = vec3DoubleWidget;
    }
    else if (type == KeywordBase::Vec3IntegerData)
    {
        Vec3IntegerKeywordWidget *vec3IntWidget = new Vec3IntegerKeywordWidget(nullptr, keyword, coreData);
        connect(vec3IntWidget, SIGNAL(keywordValueChanged(int)), this, SLOT(keywordDataChanged(int)));
        widget = vec3IntWidget;
        base = vec3IntWidget;
    }
    else if (type == KeywordBase::Vec3NodeValueData)
    {
        Vec3NodeValueKeywordWidget *vec3NodeValueWidget = new Vec3NodeValueKeywordWidget(nullptr, keyword, coreData);
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
void KeywordsWidget::setUp(KeywordList &keywords, const CoreData &coreData)
{
    // Clear existing item groups....
    while (count() > 0)
        removeItem(0);
    keywordWidgets_.clear();

    for (auto &[groupName, keyNames] : keywords.displayGroups())
    {
        // Create a new QWidget and layout for our widgets
        QWidget *groupWidget = new QWidget;
        QFormLayout *groupLayout = new QFormLayout(groupWidget);

        // Loop over keywords in the group and add them to our groupbox
        for (const auto &keyName : keyNames)
        {
            // Find the keyword in the main map
            auto *keyword = keywords.find(keyName);
            if (!keyword)
                throw(std::runtime_error(
                    fmt::format("Keyword '{}' is listed in group '{}', but it could not be found.\n", keyName, groupName)));

            // Create / setup the keyword widget
            auto *widget = createKeywordWidget(keywordWidgets_, keyword, coreData);

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
        addItem(groupWidget, QString::fromStdString(std::string(groupName)));
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
