/***************************************************************************
    File                 : Correlation.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Numerical correlation of data sets

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
#include "Correlation.h"
#include "MultiLayer.h"
#include "Plot.h"
#include "PlotCurve.h"
#include "ColorButton.h"
#include <QMessageBox>
#include <QLocale>
#include "core/column/Column.h"

#include <gsl/gsl_fft_halfcomplex.h>
#include <vector>

Correlation::Correlation(ApplicationWindow *parent, Table *t, const QString &colName1,
                         const QString &colName2)
    : Filter(parent, t)
{
    setObjectName(tr("Correlation"));
    setDataFromTable(t, colName1, colName2);
}

void Correlation::setDataFromTable(Table *t, const QString &colName1, const QString &colName2)
{
    if (t && d_table != t)
        d_table = t;

    int col1 = d_table->colIndex(colName1);
    int col2 = d_table->colIndex(colName2);

    if (col1 < 0) {
        QMessageBox::warning((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                             tr("The data set %1 does not exist!").arg(colName1));
        d_init_err = true;
        return;
    } else if (col2 < 0) {
        QMessageBox::warning((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                             tr("The data set %1 does not exist!").arg(colName2));
        d_init_err = true;
        return;
    }

    unsigned rows = d_table->numRows();
    size_t td_n = 16; // tmp number of points
    while (td_n < rows)
        td_n *= 2;

    try {
        d_x.resize(td_n);
        d_y.resize(td_n);
    } catch (const std::bad_alloc &e) {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                              tr("Could not allocate memory, operation aborted!\n")
                                      + tr("Allocator returned: ") + e.what());
        d_init_err = true;
        return;
    }

    for (unsigned i = 0; i < rows; i++) {
        d_x[i] = d_table->cell(i, col1);
        d_y[i] = d_table->cell(i, col2);
    }
std::fill(d_x.begin()+rows,d_x.end(),0.0);
std::fill(d_y.begin()+rows,d_y.end(),0.0);
}

void Correlation::output()
{
    // calculate the FFTs of the two functions
    if (gsl_fft_real_radix2_transform(d_x.data(), 1, d_x.size()) == 0
        && gsl_fft_real_radix2_transform(d_y.data(), 1, d_x.size()) == 0) {
        // multiply the FFT by its complex conjugate
        for (unsigned i = 0; i < d_x.size() / 2; i++) {
            if (i == 0 || i == (d_x.size() / 2) - 1)
                d_x[i] *= d_x[i];
            else {
                int ni = d_x.size() - i;
                double dReal = d_x[i] * d_y[i] + d_x[ni] * d_y[ni];
                double dImag = d_x[i] * d_y[ni] - d_x[ni] * d_y[i];
                d_x[i] = dReal;
                d_x[ni] = dImag;
            }
        }
    } else {
        QMessageBox::warning((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                             tr("Error in GSL forward FFT operation!"));
        return;
    }

    gsl_fft_halfcomplex_radix2_inverse(d_x.data(), 1, d_x.size()); // inverse FFT

    addResultCurve();
}

void Correlation::addResultCurve()
{
    ApplicationWindow *app = (ApplicationWindow *)parent();
    if (!app)
        return;

    int rows = d_table->numRows();
    int cols = d_table->numCols();
    int cols2 = cols + 1;
    d_table->addCol();
    d_table->addCol();
    int n = rows / 2;

    std::vector<double> x_temp(rows), y_temp(rows);
    for (int i = 0; i < rows; i++) {
        x_temp[i] = i - n;

        if (i < n)
            y_temp[i] = d_x[d_x.size() - n + i];
        else
            y_temp[i] = d_x[i - n];

        d_table->column(cols)->setValueAt(i, x_temp[i]);
        d_table->column(cols2)->setValueAt(i, y_temp[i]);
    }

    QStringList l = d_table->colNames().filter(tr("Lag"));
    QString id = QString::number((int)l.size() + 1);
    QString label = objectName() + id;

    d_table->setColName(cols, tr("Lag") + id);
    d_table->setColName(cols2, label);
    d_table->setColPlotDesignation(cols, SciDAVis::X);

    MultiLayer *ml = app->newGraph(objectName() + tr("Plot"));
    if (!ml)
        return;

    DataCurve *c = new DataCurve(d_table, d_table->colName(cols), d_table->colName(cols2));
    c->setData(x_temp.data(), y_temp.data(), rows);
    c->setPen(QPen(d_curveColor, 1));
    ml->activeGraph()->insertPlotItem(c, Graph::Line);
    ml->activeGraph()->updatePlot();
}
