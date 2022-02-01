/***************************************************************************
    File                 : Fit.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Fit base class

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
#include "Fit.h"
#include "fit_gsl.h"
#include "Table.h"
#include "Matrix.h"
#include "QwtErrorPlotCurve.h"
#include "Legend.h"
#include "FunctionCurve.h"
#include "ColorButton.h"
#include "Script.h"
#include "core/column/Column.h"

#include <gsl/gsl_statistics.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_deriv.h>
#include <gsl/gsl_version.h>

#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QLocale>

using namespace std;

Fit::Fit(ApplicationWindow *parent, Graph *g, QString name)
    : Filter(parent, g, name), scripted(ScriptingLangManager::newEnv("muParser", parent))
{
    d_p = 0;
    d_curveColor = ColorButton::color(1);
    d_solver = ScaledLevenbergMarquardt;
    d_tolerance = 1e-4;
    d_gen_function = true;
    d_points = 100;
    d_max_iterations = 1000;
    d_curve = 0;
    d_formula = QString();
    d_explanation = QString();
    d_y_error_source = UnknownErrors;
    d_y_error_dataset = QString();
    d_prec = parent->fit_output_precision;
    d_init_err = false;
    chi_2 = -1;
    d_scale_errors = false;
    d_sort_data = true;
}

vector<double> Fit::fitGslMultifit(int &iterations, int &status)
{
    vector<double> result(d_p);

    // declare input data
    struct FitData data = { d_x.size(), size_t(d_p), d_x.data(), d_y.data(), &d_y_errors[0], this };
    gsl_multifit_function_fdf f;
    f.f = d_f;
    f.df = d_df;
    f.fdf = d_fdf;
    f.n = d_x.size();
    f.p = d_p;
    f.params = &data;

    // initialize solver
    const gsl_multifit_fdfsolver_type *T = nullptr;
    switch (d_solver) {
    case ScaledLevenbergMarquardt:
        T = gsl_multifit_fdfsolver_lmsder;
        break;
    case UnscaledLevenbergMarquardt:
        T = gsl_multifit_fdfsolver_lmder;
        break;
    default:
        break;
    }
    gsl_multifit_fdfsolver *s = gsl_multifit_fdfsolver_alloc(T, d_x.size(), d_p);
    status = gsl_multifit_fdfsolver_set(s, &f, d_param_init);
    if (status)
        return result;

    // iterate solver algorithm
    for (iterations = 0; iterations < d_max_iterations; iterations++) {
        status = gsl_multifit_fdfsolver_iterate(s);
        if (status)
            break;

        status = gsl_multifit_test_delta(s->dx, s->x, d_tolerance, d_tolerance);
        if (status != GSL_CONTINUE)
            break;
    }

    // grab results
    for (unsigned i = 0; i < d_p; i++)
        result[i] = gsl_vector_get(s->x, i);
    gsl_blas_ddot(s->f, s->f, &chi_2);
#if GSL_MAJOR_VERSION < 2
    gsl_multifit_covar(s->J, 0.0, covar);
#else
    {
        gsl_matrix *J = gsl_matrix_alloc(d_x.size(), d_p);
        gsl_multifit_fdfsolver_jac(s, J);
        gsl_multifit_covar(J, 0.0, covar);
        gsl_matrix_free(J);
    }
#endif
    if (d_y_error_source == UnknownErrors) {
        // multiply covar by variance of residuals, which is used as an estimate for the
        // statistical errors (this relies on the Y errors being set to 1.0, so that
        // s->f is properly normalized)
        gsl_matrix_scale(covar, chi_2 / (d_x.size() - d_p));
    }

    // free memory allocated for fitting
    gsl_multifit_fdfsolver_free(s);

    return result;
}

vector<double> Fit::fitGslMultimin(int &iterations, int &status)
{
    vector<double> result(d_p);

    // declare input data
    struct FitData data = { d_x.size(), size_t(d_p), d_x.data(), d_y.data(), &d_y_errors[0], this };
    gsl_multimin_function f;
    f.f = d_fsimplex;
    f.n = d_p;
    f.params = &data;

    // step size (size of the simplex)
    // can be increased for faster convergence
    gsl_vector *ss = gsl_vector_alloc(f.n);
    gsl_vector_set_all(ss, 10.0);

    // initialize minimizer
    const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;
    gsl_multimin_fminimizer *s_min = gsl_multimin_fminimizer_alloc(T, f.n);
    gsl_multimin_fminimizer_set(s_min, &f, d_param_init, ss);

    // iterate minimization algorithm
    for (iterations = 0; iterations < d_max_iterations; iterations++) {
        status = gsl_multimin_fminimizer_iterate(s_min);
        if (status)
            break;

        double size = gsl_multimin_fminimizer_size(s_min);
        status = gsl_multimin_test_size(size, d_tolerance);
        if (status != GSL_CONTINUE)
            break;
    }

    // grab results
    for (unsigned i = 0; i < d_p; i++)
        result[i] = gsl_vector_get(s_min->x, i);
    chi_2 = s_min->fval;
    gsl_matrix *J = gsl_matrix_alloc(d_x.size(), d_p);
    d_df(s_min->x, (void *)f.params, J);
    gsl_multifit_covar(J, 0.0, covar);
    if (d_y_error_source == UnknownErrors) {
        // multiply covar by variance of residuals, which is used as an estimate for the
        // statistical errors (this relies on the Y errors being set to 1.0)
        gsl_matrix_scale(covar, chi_2 / (d_x.size() - d_p));
    }

    // free previously allocated memory
    gsl_matrix_free(J);
    gsl_multimin_fminimizer_free(s_min);
    gsl_vector_free(ss);

    return result;
}

void Fit::setDataCurve(int curve, double start, double end)
{
    Filter::setDataCurve(curve, start, end);

    if (!setYErrorSource(AssociatedErrors, QString(), true))
        setYErrorSource(UnknownErrors);
}

void Fit::setInitialGuesses(double *x_init)
{
    for (unsigned i = 0; i < d_p; i++)
        gsl_vector_set(d_param_init, i, x_init[i]);
}

void Fit::generateFunction(bool yes, int points)
{
    d_gen_function = yes;
    if (d_gen_function)
        d_points = points;
}

QString Fit::logFitInfo(const vector<double> &par, int iterations, int status,
                        const QString &plotName)
{
    QDateTime dt = QDateTime::currentDateTime();
    QString info = "[" + QLocale().toString(dt) + "\t" + tr("Plot") + ": ''" + plotName + "'']\n";
    info += d_explanation + " " + tr("fit of dataset") + ": " + d_curve->title().text();
    if (!d_formula.isEmpty())
        info += ", " + tr("using function") + ": " + d_formula + "\n";
    else
        info += "\n";

    info += tr("Y standard errors") + ": ";
    switch (d_y_error_source) {
    case UnknownErrors:
        info += tr("Unknown");
        break;
    case AssociatedErrors:
        info += tr("Associated dataset (%1)").arg(d_y_error_dataset);
        break;
    case PoissonErrors:
        info += tr("Statistical (assuming Poisson distribution)");
        break;
    case CustomErrors:
        info += tr("Arbitrary Dataset") + ": " + d_y_error_dataset;
        break;
    }
    info += "\n";

    if (is_non_linear) {
        if (d_solver == NelderMeadSimplex)
            info += tr("Nelder-Mead Simplex");
        else if (d_solver == UnscaledLevenbergMarquardt)
            info += tr("Unscaled Levenberg-Marquardt");
        else
            info += tr("Scaled Levenberg-Marquardt");

        info += tr(" algorithm with tolerance = ") + QLocale().toString(d_tolerance) + "\n";
    }

    info += tr("From x") + " = " + QLocale().toString(d_x[0], 'g', 15) + " " + tr("to x") + " = "
            + QLocale().toString(d_x.back(), 'g', 15) + "\n";
    double chi_2_dof = chi_2 / (d_x.size() - d_p);
    for (unsigned i = 0; i < d_p; i++) {
        info += d_param_names[i] + " " + d_param_explain[i] + " = "
                + QLocale().toString(par[i], 'g', d_prec) + " +/- ";
        if (d_scale_errors)
            info += QLocale().toString(sqrt(chi_2_dof * gsl_matrix_get(covar, i, i)), 'g', d_prec)
                    + "\n";
        else
            info += QLocale().toString(sqrt(gsl_matrix_get(covar, i, i)), 'g', d_prec) + "\n";
    }
    info += "--------------------------------------------------------------------------------------"
            "\n";
    info += "Chi^2 = " + QLocale().toString(chi_2, 'g', d_prec) + "\n";

    info += tr("R^2") + " = " + QLocale().toString(rSquare(), 'g', d_prec) + "\n";
    info += "--------------------------------------------------------------------------------------"
            "-\n";
    if (is_non_linear) {
        info += tr("Iterations") + " = " + QString::number(iterations) + "\n";
        info += tr("Status") + " = " + gsl_strerror(status) + "\n";
        info += "----------------------------------------------------------------------------------"
                "-----\n";
    }
    return info;
}

double Fit::rSquare()
{
    double mean = 0.0, tss = 0.0, weights_sum = 0.0;

    if (d_y_error_source == UnknownErrors) {
        std::accumulate(d_y.cbegin(), d_y.cend(), mean);
        mean /= d_y.size();
        for (auto it = d_y.cbegin(); d_y.cend()!=it;++it)
            tss += (*it - mean) * (*it - mean);
    } else {
        for (unsigned i = 0; i < d_y.size(); i++) {
            mean += d_y[i] / (d_y_errors[i] * d_y_errors[i]);
            weights_sum += 1.0 / (d_y_errors[i] * d_y_errors[i]);
        }
        mean /= weights_sum;
        for (unsigned i = 0; i < d_y.size(); i++)
            tss += (d_y[i] - mean) * (d_y[i] - mean) / (d_y_errors[i] * d_y_errors[i]);
    }
    return 1 - chi_2 / tss;
}

QString Fit::legendInfo()
{
    QString info = tr("Dataset") + ": " + d_curve->title().text() + "\n";
    info += tr("Function") + ": " + d_formula + "\n\n";

    double chi_2_dof = chi_2 / (d_x.size() - d_p);
    info += "Chi^2 = " + QLocale().toString(chi_2, 'g', d_prec) + "\n";
    info += tr("R^2") + " = " + QLocale().toString(rSquare(), 'g', d_prec) + "\n";

    for (unsigned i = 0; i < d_p; i++) {
        info += d_param_names[i] + " = " + QLocale().toString(d_results[i], 'g', d_prec) + " +/- ";
        if (d_scale_errors)
            info += QLocale().toString(sqrt(chi_2_dof * gsl_matrix_get(covar, i, i)), 'g', d_prec)
                    + "\n";
        else
            info += QLocale().toString(sqrt(gsl_matrix_get(covar, i, i)), 'g', d_prec) + "\n";
    }
    return info;
}

bool Fit::setYErrorSource(ErrorSource err, const QString &colName, bool fail_silently)
{
    try
    {
        d_y_errors.resize(d_x.size());
    }
    catch (const std::bad_alloc& e)
    {
        if (!fail_silently)
         QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                              tr("Could not allocate memory, operation aborted!\n")
                                      + tr("Allocator returned: ") + e.what());
        d_init_err = true;
        return false;
    }

    d_y_error_source = err;
    switch (d_y_error_source) {
    case UnknownErrors: {
        d_y_error_dataset = QString();
        // using 1.0 here is important for correct error estimates,
        // cmp. Fit::fitGslMultifit and Fit::fitGslMultimin
        std::fill(d_y_errors.begin(),d_y_errors.end(),1.0);
        break;
    }
    case AssociatedErrors: {
        bool error = true;
        QwtErrorPlotCurve *er = 0;
        if (d_curve && ((PlotCurve *)d_curve)->type() != Graph::Function) {
            QList<DataCurve *> lst = ((DataCurve *)d_curve)->errorBarsList();
            foreach (DataCurve *c, lst) {
                er = (QwtErrorPlotCurve *)c;
                if (!er->xErrors()) {
                    d_y_error_dataset = er->title().text();
                    error = false;
                    break;
                }
            }
        }
        if (error) {
            if (!fail_silently)
                QMessageBox::critical((ApplicationWindow *)parent(), tr("Error"),
                                      tr("The curve %1 has no associated Y error bars.")
                                              .arg(d_curve->title().text()));
            return false;
        }
        if (er)
            for (unsigned j = 0; j < d_y_errors.size(); j++)
                d_y_errors[j]=er->errorValue(d_indices[j]);
        break;
    }
    case PoissonErrors: {
        d_y_error_dataset = d_curve->title().text();

        for (unsigned i = 0; i < d_y.size(); i++)
            d_y_errors[i] = sqrt(d_y[i]);
    } break;
    case CustomErrors: { // d_y_errors are equal to the values of the arbitrary dataset
        if (colName.isEmpty())
            return false;

        Table *t = ((ApplicationWindow *)parent())->table(colName);
        if (!t)
            return false;

        if (unsigned(t->numRows()) < d_y_errors.size()) {
            if (!fail_silently)
                QMessageBox::critical((ApplicationWindow *)parent(), tr("Error"),
                                      tr("The column %1 has less points than the fitted data set. "
                                         "Please choose another column!")
                                              .arg(colName));
            return false;
        }

        d_y_error_dataset = colName;

        int col = t->colIndex(colName);
        for (unsigned i = 0; i < d_y_errors.size(); i++)
            d_y_errors[i] = t->cell(d_indices[i], col);
    } break;
    }
    return true;
}

Table *Fit::parametersTable(const QString &tableName)
{
    ApplicationWindow *app = (ApplicationWindow *)parent();
    Table *t = app->newTable(tableName, d_p, 3);
    t->setHeader(QStringList() << tr("Parameter") << tr("Value") << tr("Error"));
    t->column(0)->setColumnMode(SciDAVis::ColumnMode::Text);
    t->column(1)->setColumnMode(SciDAVis::ColumnMode::Numeric);
    t->column(2)->setColumnMode(SciDAVis::ColumnMode::Numeric);
    for (unsigned i = 0; i < d_p; i++) {
        t->column(0)->setTextAt(i, d_param_names[i]);
        t->column(1)->setValueAt(i, d_results[i]);
        t->column(2)->setValueAt(i, sqrt(gsl_matrix_get(covar, i, i)));
    }

    t->column(2)->setPlotDesignation(SciDAVis::yErr);
// TODO: replace or remove this
#if 0
	for (int j=0; j<3; j++)
		t->table()->adjustColumn(j);
#endif
    t->showNormal();
    return t;
}

Matrix *Fit::covarianceMatrix(const QString &matrixName)
{
    ApplicationWindow *app = (ApplicationWindow *)parent();
    Matrix *m = app->newMatrix(matrixName, d_p, d_p);
    for (unsigned i = 0; i < d_p; i++) {
        for (unsigned j = 0; j < d_p; j++)
            m->setText(i, j, QLocale().toString(gsl_matrix_get(covar, i, j), 'g', d_prec));
    }
    m->showNormal();
    return m;
}

const vector<double> &Fit::errors()
{
    if (d_result_errors.empty()) {
        d_result_errors.resize(d_p);
        double chi_2_dof = chi_2 / (d_x.size() - d_p);
        for (unsigned i = 0; i < d_p; i++) {
            if (d_scale_errors)
                d_result_errors[i] = sqrt(chi_2_dof * gsl_matrix_get(covar, i, i));
            else
                d_result_errors[i] = sqrt(gsl_matrix_get(covar, i, i));
        }
    }
    return d_result_errors;
}

void Fit::fit()
{
    if (!d_graph || d_init_err)
        return;

    if (d_x.empty()) {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("Fit Error"),
                              tr("You didn't specify a valid data set for this fit operation. "
                                 "Operation aborted!"));
        return;
    }
    if (!d_p) {
        QMessageBox::critical(
                (ApplicationWindow *)parent(), tr("Fit Error"),
                tr("There are no parameters specified for this fit operation. Operation aborted!"));
        return;
    }
    if (unsigned(d_p) > d_x.size()) {
        QMessageBox::critical(
                (ApplicationWindow *)parent(), tr("Fit Error"),
                tr("You need at least %1 data points for this fit operation. Operation aborted!")
                        .arg(d_p));
        return;
    }
    if (d_formula.isEmpty()) {
        QMessageBox::critical(
                (ApplicationWindow *)parent(), tr("Fit Error"),
                tr("You must specify a valid fit function first. Operation aborted!"));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    int status, iterations;
    vector<double> par;
    d_script.reset(scriptEnv->newScript(d_formula, this, metaObject()->className()));
    connect(d_script.get(), SIGNAL(error(const QString &, const QString &, int)), this,
            SLOT(scriptError(const QString &, const QString &, int)));

    if (d_solver == NelderMeadSimplex)
        par = fitGslMultimin(iterations, status);
    else
        par = fitGslMultifit(iterations, status);

    storeCustomFitResults(par);
    if (status == GSL_SUCCESS)
        generateFitCurve(par);

    ApplicationWindow *app = (ApplicationWindow *)parent();
    if (app->writeFitResultsToLog)
        app->updateLog(logFitInfo(d_results, iterations, status, d_graph->parentPlotName()));
    disconnect(d_script.get(), SIGNAL(error(const QString &, const QString &, int)), this,
               SLOT(scriptError(const QString &, const QString &, int)));
    QApplication::restoreOverrideCursor();
}

void Fit::scriptError(const QString &message, const QString &script_name, int line_number)
{
    QMessageBox::critical(qobject_cast<QWidget *>(parent()), tr("Input function error"),
                          QString("%1:%2\n").arg(script_name).arg(line_number) + message);
}

int Fit::evaluate_f(const gsl_vector *x, gsl_vector *f)
{
    for (unsigned i = 0; i < d_p; i++) {
        d_script->setDouble(gsl_vector_get(x, i), d_param_names[i].toUtf8());
    }
    for (unsigned j = 0; j < d_x.size(); j++) {
        if (!d_script->setDouble(d_x[j], "x"))
            return GSL_EINVAL;
        bool success;
        auto y = d_script->eval();
        if (y.isNull())
            return GSL_EINVAL;
        gsl_vector_set(f, j, (y.toDouble(&success) - d_y[j]) / d_y_errors[j]);
        if (!success)
            return GSL_EINVAL;
    }
    return GSL_SUCCESS;
}

double Fit::evaluate_d(const gsl_vector *x)
{
    double result = 0.0;
    for (unsigned i = 0; i < d_p; i++)
        d_script->setDouble(gsl_vector_get(x, i), d_param_names[i].toUtf8());
    for (unsigned j = 0; j < d_x.size(); j++) {
        d_script->setDouble(d_x[j], "x");
        bool success;
        result += pow((d_script->eval().toDouble(&success) - d_y[j]) / d_y_errors[j], 2);
        if (!success)
            return GSL_EINVAL;
    }
    return result;
}

typedef struct
{
    Script *script;
    QString param;
    bool success;
} DiffData;

double Fit::evaluate_df_helper(double x, void *params)
{
    DiffData *data = static_cast<DiffData *>(params);
    data->script->setDouble(x, (data->param).toUtf8());
    bool success;
    double result = data->script->eval().toDouble(&success);
    if (!success) {
        data->script->disconnect(SIGNAL(error(const QString &, const QString &, int)));
        data->success = false;
    }
    return result;
}

int Fit::evaluate_df(const gsl_vector *x, gsl_matrix *J)
{
    double result, abserr;
    gsl_function F;
    F.function = &evaluate_df_helper;
    DiffData data;
    F.params = &data;
    data.script = d_script.get();
    data.success = true;
    for (unsigned i = 0; i < d_p; i++)
        d_script->setDouble(gsl_vector_get(x, i), d_param_names[i].toUtf8());
    for (unsigned i = 0; i < d_x.size(); i++) {
        d_script->setDouble(d_x[i], "x");
        for (unsigned j = 0; j < d_p; j++) {
            data.param = d_param_names[j];
            gsl_deriv_central(&F, gsl_vector_get(x, j), 1e-8, &result, &abserr);
            if (!data.success)
                return GSL_EINVAL;
            gsl_matrix_set(J, i, j, result / d_y_errors[j]);
        }
    }
    return GSL_SUCCESS;
}

void Fit::generateFitCurve(const vector<double> &par)
{
    if (!d_gen_function)
        d_points = d_x.size();

    std::vector<double> X(d_points);
    std::vector<double> Y(d_points);

    if (!calculateFitCurveData(par, X, Y)) {
        QMessageBox::critical(0, tr("Fit failed"), tr("An error occurred during fit!"));
        return;
    }

    if (d_gen_function) {
        insertFitFunctionCurve(objectName() + tr("Fit"), X, Y);
        d_graph->replot();
    } else
        d_graph->addFitCurve(addResultCurve(X, Y));
}

void Fit::insertFitFunctionCurve(const QString &name, std::vector<double> &x,
                                 std::vector<double> &y, int penWidth)
{
    QString title = d_graph->generateFunctionName(name);
    FunctionCurve *c =
            new FunctionCurve((ApplicationWindow *)parent(), FunctionCurve::Normal, title);
    c->setPen(QPen(d_curveColor, penWidth));
    // "Set data by copying x- and y-values from specified memory blocks."
    c->setData(x.data(), y.data(), d_points);
    c->setRange(d_x.front(), d_x.back());

    QString formula;
    for (unsigned j = 0; j < d_p; j++)
        formula += QString("%1=%2\n").arg(d_param_names[j]).arg(d_results[j], 0, 'g', d_prec);
    formula += "\n";
    formula += d_formula;
    c->setFormula(formula);
    d_graph->insertPlotItem(c, Graph::Line);
    d_graph->addFitCurve(c);
}

Fit::~Fit()
{
    if (!d_p)
        return;

    if (is_non_linear)
        gsl_vector_free(d_param_init);

    gsl_matrix_free(covar);
}

void Fit::generateX(std::vector<double> &X) const
{
    if (d_gen_function) {
        double X0 = d_x.front();
        double step = (d_x.back() - X0) / (d_points - 1);
        for (int i = 0; i < d_points; i++)
            X[i] = X0 + i * step;
    } else
        for (int i = 0; i < d_points; i++)
            X[i] = d_x[i];
}
