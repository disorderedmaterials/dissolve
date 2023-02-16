// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/species.h"
#include "gui/addforcefieldtermsdialog.h"
#include "templates/algorithms.h"
#include <QInputDialog>
#include <QQuickItem>
#include <QMessageBox>
#include <set>
#include <QQuickWidget>

AddForcefieldTermsDialog::AddForcefieldTermsDialog(QWidget *parent)
    : QDialog(parent)
{
    QQuickWidget *view = new QQuickWidget(QUrl("main.qml"), this);
    view->setMinimumSize(400,400);
    view->setResizeMode(QQuickWidget::SizeRootObjectToView);
    // QWidget *container = QWidget::createWindowContainer(view, this);

    // some set up if needed
    // container->setMinimumSize(1280, 600);
    // container->setMaximumSize(1280, 600);
    // container->setFocusPolicy(Qt::TabFocus);

    // ..

    auto root =view->rootObject();

    connect(root, SIGNAL(qmlCancel()), this, SLOT(reject()));

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(view);
    setLayout(topLeftLayout);

    // ui->verticalLayout->addWidget(container);
}
