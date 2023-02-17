// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <QObject>

class AddForcefieldDialogModel : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString nextText READ nextText NOTIFY nextTextChanged)
  Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)

 signals:
  void nextTextChanged();
  void indexChanged();
  void accept();
  void cancel();

 private:
  int index_ = 0;

 public:
  QString nextText();
  int index();
  void setIndex(int idx);
};
