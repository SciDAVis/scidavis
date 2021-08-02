/***************************************************************************
    File                 : FFT.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical FFT of data sets

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
#include "FFT.h"
#include "MultiLayer.h"
#include "Plot.h"
#include "ColorButton.h"
#include "core/column/Column.h"

#include <QMessageBox>
#include <QLocale>

#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_fft_halfcomplex.h>

FFT::FFT(ApplicationWindow *parent, Table *t, const QString &realColName,
         const QString &imagColName)
    : Filter(parent, t)
{
    init();
    setDataFromTable(t, realColName, imagColName);
}

FFT::FFT(ApplicationWindow *parent, Graph *g, const QString &curveTitle) : Filter(parent, g)
{
    init();
    setDataFromCurve(curveTitle);
    // intersperse 0 imaginary components
    std::vector<double> tmp(2 * d_x.size(), 0.0);
    for (size_t i = 0; i < d_x.size(); ++i)
        tmp[2 * i] = d_y[i];
    d_y = std::move(tmp);
}

void FFT::init()
{
    setObjectName(tr("FFT"));
    d_inverse = false;
    d_normalize = true;
    d_shift_order = true;
    d_real_col = -1;
    d_imag_col = -1;
    d_sampling = 1.0;
}

QList<Column *> FFT::fftTable()
{
    std::vector<double> amp;

    gsl_fft_complex_wavetable *wavetable = nullptr;
    gsl_fft_complex_workspace *workspace = nullptr;

    try {
        amp.resize(d_x.size());
        wavetable = gsl_fft_complex_wavetable_alloc(d_x.size());
        workspace = gsl_fft_complex_workspace_alloc(d_x.size());
        if ((nullptr == wavetable) || (nullptr == workspace))
            throw std::bad_alloc();
    } catch (const std::bad_alloc &) {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                              tr("Could not allocate memory, operation aborted!"));
        if (nullptr != wavetable)
            gsl_fft_complex_wavetable_free(wavetable);
        if (nullptr != workspace)
            gsl_fft_complex_workspace_free(workspace);
        d_init_err = true;
        return QList<Column *>();
    }

    double df = 1.0 / (d_x.size() * d_sampling); // frequency sampling
    double aMax = 0.0; // max amplitude
    QList<Column *> columns;
    if (!d_inverse) {
        columns << new Column(tr("Frequency"), SciDAVis::ColumnMode::Numeric);
        gsl_fft_complex_forward(d_y.data(), 1, d_x.size(), wavetable, workspace);
    } else {
        columns << new Column(tr("Time"), SciDAVis::ColumnMode::Numeric);
        gsl_fft_complex_inverse(d_y.data(), 1, d_x.size(), wavetable, workspace);
    }

    gsl_fft_complex_wavetable_free(wavetable);
    gsl_fft_complex_workspace_free(workspace);

    if (d_shift_order) {
        int n2 = d_x.size() / 2;
        for (int i = 0u; i < d_x.size(); i++) {
            d_x[i] = (i - n2) * df;
            int j = i + d_x.size();
            double aux = d_y[i];
            d_y[i] = d_y[j];
            d_y[j] = aux;
        }
    } else {
        for (size_t i = 0; i < d_x.size(); i++)
            d_x[i] = i * df;
    }

    for (size_t i = 0; i < d_x.size(); i++) {
        size_t i2 = 2 * i;
        double a = sqrt(d_y[i2] * d_y[i2] + d_y[i2 + 1] * d_y[i2 + 1]);
        amp[i] = a;
        if (a > aMax)
            aMax = a;
    }

    columns << new Column(tr("Real"), SciDAVis::ColumnMode::Numeric);
    columns << new Column(tr("Imaginary"), SciDAVis::ColumnMode::Numeric);
    columns << new Column(tr("Amplitude"), SciDAVis::ColumnMode::Numeric);
    columns << new Column(tr("Angle"), SciDAVis::ColumnMode::Numeric);
    for (int i = 0; i < static_cast<int>(d_x.size()); i++) {
        int i2 = 2 * i;
        columns.at(0)->setValueAt(i, d_x[i]);
        columns.at(1)->setValueAt(i, d_y[i2]);
        columns.at(2)->setValueAt(i, d_y[i2 + 1]);
        if (d_normalize)
            columns.at(3)->setValueAt(i, amp[i] / aMax);
        else
            columns.at(3)->setValueAt(i, amp[i]);
        columns.at(4)->setValueAt(i, atan(d_y[i2 + 1] / d_y[i2]));
    }

    columns.at(0)->setPlotDesignation(SciDAVis::X);
    columns.at(1)->setPlotDesignation(SciDAVis::Y);
    columns.at(2)->setPlotDesignation(SciDAVis::Y);
    columns.at(3)->setPlotDesignation(SciDAVis::Y);
    columns.at(4)->setPlotDesignation(SciDAVis::Y);
    return columns;
}

void FFT::output()
{
    QList<Column *> columns;
    if (!d_y.empty())
        columns = fftTable();

    if (!columns.isEmpty())
        output(columns);
}

void FFT::output(QList<Column *> columns)
{
    ApplicationWindow *app = (ApplicationWindow *)parent();
    QString tableName = app->generateUniqueName(objectName());
    Table *t = app->newHiddenTable(tableName, d_explanation, columns);
    MultiLayer *ml = app->multilayerPlot(t, QStringList() << tableName + "_" + tr("Amplitude"), 0);
    if (!ml)
        return;

    Graph *g = ml->activeGraph();
    if (g) {
        g->setCurvePen(0, QPen(d_curveColor, 1));

        Plot *plot = g->plotWidget();
        plot->setTitle(QString());
        if (!d_inverse)
            plot->setAxisTitle(QwtPlot::xBottom, tr("Frequency") + " (" + tr("Hz") + ")");
        else
            plot->setAxisTitle(QwtPlot::xBottom, tr("Time") + +" (" + tr("s") + ")");

        plot->setAxisTitle(QwtPlot::yLeft, tr("Amplitude"));
        plot->replot();
    }
    ml->showMaximized();
}

void FFT::setDataFromTable(Table *t, const QString &realColName, const QString &imagColName)
{
    if (t && d_table != t)
        d_table = t;

    d_real_col = d_table->colIndex(realColName);

    if (!imagColName.isEmpty())
        d_imag_col = d_table->colIndex(imagColName);

    size_t rows = d_table->numRows();
    int n2 = 2 * rows;
    try {
        d_y.resize(n2);
        d_x.resize(rows);
    } catch (const std::bad_alloc &e) {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                              tr("Could not allocate memory, operation aborted!\n")
                                      + tr("Allocator returned: ") + e.what());
        d_init_err = true;
        return;
    }

    for (unsigned i = 0; i < d_x.size(); i++) {
        int i2 = 2 * i;
        d_y[i2] = d_table->cell(i, d_real_col);
        if (d_imag_col >= 0)
            d_y[i2 + 1] = d_table->cell(i, d_imag_col);
    }
}
