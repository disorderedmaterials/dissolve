// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <functional>
#include <string_view>
#include <vector>

// Brent's Principal Axis Minimiser
class PrAxisMinimiser
{
    using MinimiserCostFunction = std::function<double(const std::vector<double> &)>;

    public:
    PrAxisMinimiser(MinimiserCostFunction costFunction, bool pokeBeforeCost = false);

    /*
     * Interface
     */
    private:
    // Maximum step size
    double maxStep_;
    // Convergence tolerance
    double tolerance_;
    // Print level
    int printLevel_;
    // Pointer to cost function
    MinimiserCostFunction costFunction_;
    // Where thos poke values into targets before calling the cost function
    bool pokeBeforeCost_;
    // Pointers to double values to be fit
    std::vector<double *> targets_;
    // Whether maximum limits have been set for targets
    std::vector<bool> maximumLimit_;
    // Whether minimum limits have been set for targets
    std::vector<bool> minimumLimit_;
    // Scaling factor for penalties incurred when outside of allowable limit
    const double penaltyFactor_ = 1e5;
    // Minimum limiting values for targets
    std::vector<double> minimumValue_;
    // Maximum limiting values for targets
    std::vector<double> maximumValue_;
    // Integer power of penalty function when outside allowable limit
    const int penaltyPower_ = 2;

    private:
    void pokeValues(const std::vector<double> &values);

    // Calculate cost from specified values, including contributions from any supplied limits
    double cost(const std::vector<double> &alpha);

    // Perform minimisation
    double execute(std::vector<double> &values);

    public:
    // Set maximum step size
    void setMaxStep(double step);
    // Set convergence tolerance
    void setTolerance(double tol);
    // Set print level
    void setPrintLevel(int level);
    // Minimise target parameters
    double minimise();

    // Add pointer as fit target, with limits specified
    void addTarget(double *var, bool minLimit = false, double minValue = 0.0, bool maxLimit = false, double maxValue = 0.0);

    // Add reference as fit target, with limits specified
    void addTarget(double &var, bool minLimit = false, double minValue = 0.0, bool maxLimit = false, double maxValue = 0.0);

    double flin(const std::vector<double> &alpha, int jsearch, double l, int &nf, double v[], double q0[], double q1[],
                double &qd0, double &qd1, double &qa, double &qb, double &qc);

    void minfit(int n, double tol, double a[], double q[]);
    void minny(std::vector<double> &alpha, int jsearch, int nits, double &d2, double &x1, double &f1, bool fk, double t,
               double h, double v[], double q0[], double q1[], int &nl, int &nf, double dmin, double ldt, double &fx,
               double &qa, double &qb, double &qc, double &qd0, double &qd1);
    void print2(int n, double x[], int prin, double fx, int nf, int nl);
    void quad(std::vector<double> &alpha, double t, double h, double v[], double q0[], double q1[], int &nl, int &nf,
              double dmin, double ldt, double &fx, double &qf1, double &qa, double &qb, double &qc, double &qd0, double &qd1);

    double r8_hypot(double x, double y);
    double r8_uniform_01(int &seed);
    void r8mat_print(int m, int n, double a[], std::string_view title);
    void r8mat_print_some(int m, int n, double a[], int ilo, int jlo, int ihi, int jhi, std::string_view title);
    void r8mat_transpose_in_place(int n, double a[]);
    void r8vec_copy(int n, double a1[], double a2[]);
    double r8vec_max(int n, double r8vec[]);
    double r8vec_min(int n, double r8vec[]);
    double r8vec_norm(int n, double a[]);
    void r8vec_print(int n, double a[], std::string_view title);
    void svsort(int n, double d[], double v[]);
    ;

    double praxis(double t0, double h0, std::vector<double> &x, int prin);
};
