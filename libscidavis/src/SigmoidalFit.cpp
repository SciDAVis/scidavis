/***************************************************************************
    File                 : SigmoidalFit.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Sigmoidal (Boltzmann) Fit class

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#include "SigmoidalFit.h"
#include "fit_gsl.h"

#include <QMessageBox>

#include <vector>
using namespace std;

SigmoidalFit::SigmoidalFit(ApplicationWindow *parent, Graph *g) : Fit(parent, g)
{
    init();
}

SigmoidalFit::SigmoidalFit(ApplicationWindow *parent, Graph *g, const QString &curveTitle)
    : Fit(parent, g)
{
    init();
    setDataFromCurve(curveTitle);
}

SigmoidalFit::SigmoidalFit(ApplicationWindow *parent, Graph *g, const QString &curveTitle,
                           double start, double end)
    : Fit(parent, g)
{
    init();
    setDataFromCurve(curveTitle, start, end);
}

void SigmoidalFit::init()
{
    setObjectName("Boltzmann");
    d_f = boltzmann_f;
    d_df = boltzmann_df;
    d_fdf = boltzmann_fdf;
    d_fsimplex = boltzmann_d;
    d_p = 4;
    d_min_points = d_p;
    d_param_init = gsl_vector_alloc(d_p);
    gsl_vector_set_all(d_param_init, 1.0);
    covar = gsl_matrix_alloc(d_p, d_p);
    d_results.resize(d_p);
    d_param_explain << tr("(init value)") << tr("(final value)") << tr("(center)")
                    << tr("(time constant)");
    d_param_names << "A1"
                  << "A2"
                  << "x0"
                  << "dx";
    d_explanation = tr("Boltzmann (Sigmoidal) Fit");
    d_formula = "(A1-A2)/(1+exp((x-x0)/dx))+A2";
}

bool SigmoidalFit::calculateFitCurveData(const vector<double> &par, std::vector<double> &X,
                                         std::vector<double> &Y)
{
    generateX(X);
    for (int i = 0; i < d_points; i++)
        Y[i] = (par[0] - par[1]) / (1 + exp((X[i] - par[2]) / par[3])) + par[1];
    return true;
}

void SigmoidalFit::guessInitialValues()
{
    gsl_vector_view x = gsl_vector_view_array(d_x.data(), d_x.size());
    gsl_vector_view y = gsl_vector_view_array(d_y.data(), d_y.size());

    double min_out, max_out;
    gsl_vector_minmax(&y.vector, &min_out, &max_out);

    gsl_vector_set(d_param_init, 0, min_out);
    gsl_vector_set(d_param_init, 1, max_out);
    gsl_vector_set(d_param_init, 2, gsl_vector_get(&x.vector, d_x.size() / 2));
    gsl_vector_set(d_param_init, 3, 1.0);
}
