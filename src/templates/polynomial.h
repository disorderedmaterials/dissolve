// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include <pair>

class Quadratic {
 private:
  double a_, b_, c_;

 public:
  Quadratic(double a=0, double b=0, double c=0);
  bool hasRoots() const;
  std::pair<double, double> roots() const;
  double at(double x) const;
};
