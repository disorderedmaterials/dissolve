// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/isotopes.h"
#include "gui/delegates/isotopecombo.hui"
#include "templates/list.h"
#include "templates/variantpointer.h"

IsotopeComboDelegate::IsotopeComboDelegate(QObject *parent) : QItemDelegate(parent) {}

IsotopeComboDelegate::~IsotopeComboDelegate() {}

// Create editor
QWidget *IsotopeComboDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Create editor widget (in this case a combo box) and add the available options
    auto *editor = new QComboBox(parent);

    // Get the data from the model index - it should be an Isotope*
    Isotope *isotope = VariantPointer<Isotope>(index.data(Qt::UserRole));
    if (isotope)
    {
        // Populate combo with all possible Isotopes for this element
        ListIterator<Isotope> isotopeIterator(Isotopes::isotopes(isotope->Z()));
        while (Isotope *tope = isotopeIterator.iterate())
            editor->addItem(textForIsotope(tope));
    }
    else
        Messenger::error("IsotopeComboDelegate::createEditor() - Did not find an Isotope* in the associated QModelIndex.\n");

    return editor;
}

// Set initial value in editor
void IsotopeComboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // Grab (cast) the QComboBox
    auto *comboBox = static_cast<QComboBox *>(editor);

    // Get the data from the model index - it should be an Isotope*
    Isotope *isotope = VariantPointer<Isotope>(index.data(Qt::UserRole));
    if (isotope)
    {
        comboBox->setCurrentIndex(isotope->index());
    }
    else
        Messenger::error("IsotopeComboDelegate::createEditor() - Did not find an Isotope* in the associated QModelIndex.\n");
}

// Get value from editing widget, and set back in model
void IsotopeComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // Grab (cast) the QComboBox
    auto *comboBox = static_cast<QComboBox *>(editor);

    // Get existing Isotope
    Isotope *isotope = VariantPointer<Isotope>(index.data(Qt::UserRole));
    if (isotope)
    {
        // Get parent element, and find index of new Isotope
        isotope = Isotopes::isotopeAtIndex(isotope->Z(), comboBox->currentIndex());

        // Set the Isotope pointer in the model
        model->setData(index, VariantPointer<Isotope>(isotope), Qt::UserRole);
        model->setData(index, comboBox->currentText(), Qt::EditRole);
    }
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
QString IsotopeComboDelegate::textForIsotope(Isotope *isotope)
{
    if (!isotope)
        return "NULL";

    if (isotope->A() == 0)
        return QString("Natural (bc = %1)").arg(isotope->boundCoherent());
    else
        return QString("%1 (bc = %2)").arg(isotope->A()).arg(isotope->boundCoherent());
}
