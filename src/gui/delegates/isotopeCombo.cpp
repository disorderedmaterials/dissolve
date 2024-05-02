// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/delegates/isotopeCombo.h"
#include "data/isotopes.h"

IsotopeComboDelegate::IsotopeComboDelegate(QObject *parent) : QItemDelegate(parent) {}

// Create editor
QWidget *IsotopeComboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Create editor widget (in this case a combo box) and add the available options
    auto *editor = new QComboBox(parent);

    // Get the data from the model index
    auto isotope = index.data(Qt::UserRole).value<Sears91::Isotope>();
    auto isotopes = Sears91::isotopes(Sears91::Z(isotope));

    // Populate combo with all possible Isotopes for this element
    for (auto tope : isotopes)
        editor->addItem(textForIsotope(tope), tope);

    return editor;
}

// Set initial value in editor
void IsotopeComboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // Grab (cast) the QComboBox
    auto *comboBox = static_cast<QComboBox *>(editor);

    // Get the data from the model index - it should be an Isotope
    auto isotope = index.data(Qt::UserRole).value<Sears91::Isotope>();
    comboBox->setCurrentIndex(comboBox->findData(isotope));
}

// Get value from editing widget, and set back in model
void IsotopeComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // Grab (cast) the QComboBox
    auto *comboBox = static_cast<QComboBox *>(editor);

    // Get current Isotope in combo
    auto isotope = comboBox->currentData();

    // Set the Isotope pointer in the model
    model->setData(index, QVariant::fromValue(isotope), Qt::UserRole);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

// Update widget geometry
void IsotopeComboDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

/*
 * Helper Functions
 */

// Return text for Isotope specified
QString IsotopeComboDelegate::textForIsotope(Sears91::Isotope isotope)
{
    if (Sears91::A(isotope) == 0)
        return QString("Natural (bc = %1 fm)").arg(Sears91::boundCoherent(isotope));
    else
        return QString("%1 (bc = %2 fm)").arg(Sears91::A(isotope)).arg(Sears91::boundCoherent(isotope));
}
