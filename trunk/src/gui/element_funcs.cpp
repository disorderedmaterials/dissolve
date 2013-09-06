/*
	*** Element Select Dialog
	*** src/gui/element_funcs.cpp
	Copyright T. Youngs 2012-2013

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

#include "gui/element.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "base/ptable.h"
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>

// Constructor
DUQElementDialog::DUQElementDialog(QWidget *parent) : QDialog(parent)
{
	ui.setupUi(this);

	// Create periodic table buttons
	QGridLayout* gl = new QGridLayout;
	QLabel* label;
	int n, m, z;
	const float* colour;

	// Create element button array (and buttons)
	elementButtons_ = new QPushButton*[PeriodicTable::nElements()];
	for (n=1; n<PeriodicTable::nElements(); n++)
	{
		elementButtons_[n] = new QPushButton(this);
		elementButtons_[n]->setText(PeriodicTable::element(n).symbol());
		elementButtons_[n]->setMinimumSize(24,24);
		elementButtons_[n]->setMaximumSize(24,24);
		colour = PeriodicTable::element(n).colour();
		elementButtons_[n]->setPalette(QPalette(qRgb(int(colour[0]*255),int(colour[1]*255),int(colour[2]*255))));
		QObject::connect(elementButtons_[n], SIGNAL(clicked(bool)), this, SLOT(ElementButton_clicked(bool)));
	}

	// Nnow add buttons to gridlayout
	// First row - Group Number
	for (n=1; n<19; n++)
	{
		label = new QLabel(this);
		label->setText(itoa(n));
		gl->addWidget(label, 0, n);
		//cs_label(itoa(n),table,n,n+1,0,1);
	}

	// First column - Period number
	for (n=1; n<8; n++)
	{
		label = new QLabel(this);
		label->setText(itoa(n));
		gl->addWidget(label, n, 0);
	}

	// H, He
	gl->addWidget(elementButtons_[1],1,1);
	gl->addWidget(elementButtons_[2],1,18);

	// Groups 1-2 (periods 1-6) [s]
	z = 3;
	for (n=0; n<6; n++)
	{
		gl->addWidget(elementButtons_[z],n+2,1);
		gl->addWidget(elementButtons_[z+1],n+2,2);
		z += 8;
		if (n > 1) z += 10;
		if (n > 3) z += 14;
	}

	// Groups 13-18 (periods 1-6) [p]
	z = 5;
	for (n=0; n<6; n++)
	{
		for (m=0; m<6; m++) gl->addWidget(elementButtons_[z+m],n+2,13+m);
		z += 8;
		if (n > 0) z += 10;
		if (n > 2) z += 14;
	}

	// Groups 3-8 (periods 3-6) [p]
	z = 21;
	for (n=0; n<4; n++)
	{
		for (m=0; m<10; m++) gl->addWidget(elementButtons_[z+m],n+4,3+m);
		if (n == 1) z += 14;
		z += 18;
		if (n > 1) z += 14;
	}

	label = new QLabel(this);
	label->setText("Ln");
	gl->addWidget(label, 9, 0);
	label = new QLabel(this);
	label->setText("An");
	gl->addWidget(label, 10, 0);

	// Lanthanoids and Actinoids
	z = 57;
	for (n=0; n<14; n++)
	{
		gl->addWidget(elementButtons_[z+n],9,3+n);
		gl->addWidget(elementButtons_[z+n+32],10,3+n);
	}

	ui.PeriodicTableGroup->setLayout(gl);
}

DUQElementDialog::~DUQElementDialog()
{
	if (elementButtons_ != NULL) delete[] elementButtons_;
}

// Cancel dialog
void DUQElementDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

// Return clicked element value
void DUQElementDialog::ElementButton_clicked(bool checked)
{
        // Cast sender
        QPushButton *button = qobject_cast<QPushButton*> (sender());
        if (!button)
        {
                printf("DUQElementDialog::ElementButton_clicked - Sender was not a QPushButton.\n");
                reject();
		return;
        }

	int result;
	for (result=1; result<PeriodicTable::nElements(); result++) if (elementButtons_[result] == button) break;
	selectedElement_ = (result != PeriodicTable::nElements() ? result : -1);
	accept();
}

// Select an element
int DUQElementDialog::selectElement()
{
	// Execute the dialog and check on the result
	return (exec() == 1 ? selectedElement_ : -1);
}
