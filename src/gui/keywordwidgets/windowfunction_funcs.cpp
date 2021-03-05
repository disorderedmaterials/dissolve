// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/windowfunction.hui"
#include <QComboBox>
#include <QHBoxLayout>

WindowFunctionKeywordWidget::WindowFunctionKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Create the necessary subwidgets
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    functionCombo_ = new QComboBox;
    for (auto n = 0; n < WindowFunction::nFunctionTypes; ++n)
        functionCombo_->addItem(
            QString::fromStdString(std::string(WindowFunction::functionType((WindowFunction::FunctionType)n))));
    layout->addWidget(functionCombo_);
    connect(functionCombo_, SIGNAL(currentTextChanged(QString)), this, SLOT(functionComboChanged(QString)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<WindowFunctionKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into WindowFunctionKeyword.\n", keyword->name());
    else
    {
        // Set current information
        setWidgets(keyword_->data());
    }
}

/*
 * Signals / Slots
 */

// Check box state changed
void WindowFunctionKeywordWidget::functionComboChanged(const QString &text)
{
    if (refreshing_)
        return;

    // Get widget data, and set the function type
    WindowFunction windowFunction;
    WindowFunction::FunctionType func = WindowFunction::functionType(qPrintable(text));
    windowFunction.set(func);

    keyword_->setData(windowFunction);

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void WindowFunctionKeywordWidget::updateValue() { setWidgets(keyword_->data()); }

// Set widgets from supplied object
void WindowFunctionKeywordWidget::setWidgets(const WindowFunction &windowFunction)
{
    refreshing_ = true;

    functionCombo_->setCurrentIndex(windowFunction.function());

    refreshing_ = false;
}
