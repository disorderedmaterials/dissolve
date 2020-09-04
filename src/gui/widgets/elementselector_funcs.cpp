/*
    *** Element Selector Widget
    *** src/gui/widgets/elementselector_funcs.cpp
    Copyright T. Youngs 2019-2020

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

#include "data/elements.h"
#include "gui/widgets/elementselector.hui"
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>

ElementSelector::ElementSelector(QWidget *parent) : QWidget(parent)
{
    currentElement_ = nullptr;

    // Create grid layout for widget
    auto *gl = new QGridLayout;
    gl->setSpacing(4);
    gl->setMargin(0);

    // Create periodic table buttons
    QLabel *label;
    int n, m, z;
    double *colour;

    // Create element button array (and buttons)
    QToolButton *button;
    QPalette palette = this->palette();
    QColor background, foreground;
    for (n = 0; n < Elements::nElements(); ++n)
    {
        button = new QToolButton(this);
        button->setAutoRaise(true);
        button->setCheckable(true);
        button->setAutoExclusive(true);
        elementButtons_.append(button, &Elements::element(n));
        button->setText(QString::fromStdString(std::string(Elements::symbol(n))));
        button->setMinimumSize(24, 24);
        // 		button->setMaximumSize(24,24);
        button->setToolTip(QString("%1 (%2)").arg(QString::fromStdString(std::string(Elements::name(n))),
                                                  QString::fromStdString(std::string(Elements::symbol(n)))));

        QObject::connect(button, SIGNAL(clicked(bool)), this, SLOT(elementButtonClicked(bool)));
    }

    // Now add widgets to gridlayout
    // First row - Group Number
    for (n = 1; n < 19; n++)
    {
        label = new QLabel(this);
        label->setText(QString::number(n));
        gl->addWidget(label, 0, n);
    }

    // First column - Period number
    for (n = 1; n < 8; n++)
    {
        label = new QLabel(this);
        label->setText(QString::number(n));
        gl->addWidget(label, n, 0);
    }

    // H, He
    gl->addWidget(elementButtons_.item(1), 1, 1);
    gl->addWidget(elementButtons_.item(2), 1, 18);

    // Groups 1-2 (periods 1-6) [s]
    z = 3;
    for (n = 0; n < 6; n++)
    {
        gl->addWidget(elementButtons_.item(z), n + 2, 1);
        gl->addWidget(elementButtons_.item(z + 1), n + 2, 2);
        z += 8;
        if (n > 1)
            z += 10;
        if (n > 3)
            z += 14;
    }

    // Groups 13-18 (periods 1-6) [p]
    z = 5;
    for (n = 0; n < 6; n++)
    {
        for (m = 0; m < 6; m++)
            gl->addWidget(elementButtons_.item(z + m), n + 2, 13 + m);
        z += 8;
        if (n > 0)
            z += 10;
        if (n > 2)
            z += 14;
    }

    // Groups 3-8 (periods 3-6) [p]
    z = 21;
    for (n = 0; n < 4; n++)
    {
        for (m = 0; m < 10; m++)
            gl->addWidget(elementButtons_.item(z + m), n + 4, 3 + m);
        if (n == 1)
            z += 14;
        z += 18;
        if (n > 1)
            z += 14;
    }

    label = new QLabel(this);
    label->setText("Ln");
    gl->addWidget(label, 9, 0);
    label = new QLabel(this);
    label->setText("An");
    gl->addWidget(label, 10, 0);

    // Lanthanoids and Actinoids
    z = 57;
    for (n = 0; n < 14; n++)
    {
        gl->addWidget(elementButtons_.item(z + n), 9, 3 + n);
        gl->addWidget(elementButtons_.item(z + n + 32), 10, 3 + n);
    }

    // Special elements
    gl->addWidget(elementButtons_.item(0), 1, 19);
    elementButtons_.item(0)->setVisible(false);

    setLayout(gl);

    // Set up the timer
    doubleClickTimer_.setSingleShot(true);
    doubleClickTimer_.setInterval(QApplication::doubleClickInterval());
}

ElementSelector::~ElementSelector() {}

/*
 * Widget Functions
 */

// Element button clicked
void ElementSelector::elementButtonClicked(bool checked)
{
    // Cast sender
    auto *button = qobject_cast<QToolButton *>(sender());
    if (!button)
        currentElement_ = nullptr;

    RefDataItem<QToolButton, Element *> *ri = elementButtons_.contains(button);
    currentElement_ = ri ? ri->data() : nullptr;

    // Was this a double-click? Check the timer
    if (doubleClickTimer_.isActive())
        emit(elementDoubleClicked());
    else
    {
        doubleClickTimer_.start();
        emit(elementSelectionChanged());
        emit(elementSelected(true));
    }
}

/*
 * Data
 */

// Set current element
void ElementSelector::setCurrentElement(Element *element)
{
    // Uncheck any current element button before we change
    if (currentElement_)
    {
        QToolButton *button = elementButtons_.itemWithData(currentElement_);
        if (button)
            button->setChecked(false);
    }

    currentElement_ = element;

    // Find and check the related button
    if (currentElement_ != nullptr)
    {
        QToolButton *button = elementButtons_.itemWithData(currentElement_);
        if (button)
            button->setChecked(true);
    }
}

// Return current element
Element *ElementSelector::currentElement() const { return currentElement_; }

/*
 * Static Functions
 */

// Get Element from user via input dialog
Element *ElementSelector::getElement(QWidget *parent, QString title, QString labelText, Element *element, bool *ok,
                                     Qt::WindowFlags flags)
{
    // Create a QDialog for use
    QDialog inputDialog(parent, flags);
    inputDialog.setWindowTitle(title);

    // Create the label
    QLabel *label = new QLabel(labelText, &inputDialog);
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // Create ElementSelector widget
    auto *elementSelector = new ElementSelector(&inputDialog);
    elementSelector->setCurrentElement(element);
    QObject::connect(elementSelector, SIGNAL(elementDoubleClicked()), &inputDialog, SLOT(accept()));

    // Create button box
    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &inputDialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), &inputDialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), &inputDialog, SLOT(reject()));
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(element != nullptr);
    QObject::connect(elementSelector, SIGNAL(elementSelected(bool)), buttonBox->button(QDialogButtonBox::Ok),
                     SLOT(setEnabled(bool)));

    auto *mainLayout = new QVBoxLayout(&inputDialog);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    mainLayout->addWidget(label);
    mainLayout->addWidget(elementSelector);
    mainLayout->addWidget(buttonBox);

    const auto ret = inputDialog.exec();
    if (ok)
        *ok = (ret == QDialog::Accepted);
    return (ret == QDialog::Accepted ? elementSelector->currentElement() : element);
}
