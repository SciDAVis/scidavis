/***************************************************************************
    File                 : Fit.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
    Description          : Abstract base class for data analysis operations

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
#include "Filter.h"
#include "Legend.h"
#include "ColorButton.h"
#include "Table.h"
#include "FunctionCurve.h"
#include "PlotCurve.h"
#include "core/column/Column.h"

#include <QApplication>
#include <QMessageBox>
#include <QLocale>

#include <gsl/gsl_sort.h>

#include <algorithm>
using namespace std;

Filter::Filter(ApplicationWindow *parent, Graph *g, QString name) : QObject(parent)
{
    QObject::setObjectName(name);
    init();
    d_graph = g;
}

Filter::Filter(ApplicationWindow *parent, Table *t, QString name) : QObject(parent)
{
    QObject::setObjectName(name);
    init();
    d_table = t;
}

void Filter::init()
{
    d_curveColor = ColorButton::color(1);
    d_tolerance = 1e-4;
    d_points = 100;
    d_max_iterations = 1000;
    d_curve = 0;
    d_prec = ((ApplicationWindow *)parent())->fit_output_precision;
    d_init_err = false;
    d_sort_data = false;
    d_min_points = 2;
    d_explanation = objectName();
    d_graph = 0;
    d_table = 0;
}

void Filter::setInterval(double from, double to)
{
    if (!d_curve) {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                              tr("Please assign a curve first!"));
        return;
    }
    setDataFromCurve(d_curve->title().text(), from, to);
}

void Filter::setDataCurve(const int curve, double start, double end)
{
    if (start > end)
        qSwap(start, end);

    d_init_err = false;
    d_curve = d_graph->curve(curve);
    if (nullptr == d_curve) {
        d_init_err = true;
        return;
    }
    QList<Graph::AxisType> axType = d_graph->axesType();
    for (const auto &type : axType)
        if (Graph::AxisType::Numeric != type) {
            auto ret = QMessageBox::warning((ApplicationWindow *)parent(),
                                            tr("SciDAVis") + " - " + tr("Warning"),
                                            tr("At least one of the axis is not numerical!"),
                                            QMessageBox::Abort | QMessageBox::Ignore);
            if (QMessageBox::Abort == ret) {
                d_init_err = true;
                return;
            } else
                break;
        }
    if (d_sort_data)
        sortedCurveData(d_curve, start, end, d_x, d_y);
    else
        curveData(d_curve, start, end, d_x, d_y);

    if (!isDataAcceptable()) {
        d_init_err = true;
        return;
    }

    // ensure range is within data range
    if (!d_x.empty()) {
        auto minMax = std::minmax_element(d_x.cbegin(), d_x.cend());
        d_from = *(minMax.first);
        d_to = *(minMax.second);
    }
}

bool Filter::isDataAcceptable() const
{

    if (d_n() < unsigned(d_min_points)) {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                              tr("You need at least %1 points in order to perform this operation!")
                                      .arg(d_min_points));
        return false;
    }
    return true;
}

int Filter::curveIndex(const QString &curveTitle, Graph *const g)
{
    if (curveTitle.isEmpty()) {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("Filter Error"),
                              tr("Please enter a valid curve name!"));
        d_init_err = true;
        return -1;
    }

    if (g)
        d_graph = g;

    if (!d_graph) {
        d_init_err = true;
        return -1;
    }

    return d_graph->curveIndex(curveTitle);
}

bool Filter::setDataFromCurve(const QString &curveTitle, Graph *const g)
{
    int index = curveIndex(curveTitle, g);
    if (index < 0) {
        d_init_err = true;
        return false;
    }

    d_graph->range(index, d_from, d_to);
    setDataCurve(index, d_from, d_to);
    return true;
}

bool Filter::setDataFromCurve(const QString &curveTitle, const double from, const double to,
                              Graph *const g)
{
    int index = curveIndex(curveTitle, g);
    if (index < 0) {
        d_init_err = true;
        return false;
    }

    setDataCurve(index, from, to);
    return true;
}

void Filter::setColor(const QString &colorName)
{
    QColor c = QColor(COLORVALUE(colorName));
    if (colorName == "green")
        c = QColor(Qt::green);
    else if (colorName == "darkYellow")
        c = QColor(Qt::darkYellow);
    if (!ColorButton::isValidColor(c)) {
        QMessageBox::critical(
                (ApplicationWindow *)parent(), tr("Color Name Error"),
                tr("The color name '%1' is not valid, a default color (red) will be used instead!")
                        .arg(colorName));
        d_curveColor = ColorButton::color(1);
        return;
    }

    d_curveColor = c;
}

void Filter::showLegend()
{
    Legend *mrk = d_graph->newLegend(legendInfo());
    if (d_graph->hasLegend()) {
        Legend *legend = d_graph->legend();
        QPoint p = legend->rect().bottomLeft();
        mrk->setOrigin(QPoint(p.x(), p.y() + 20));
    }
    d_graph->replot();
}

bool Filter::run()
{
    if (d_init_err)
        return false;

    //	if (d_n < 0)
    //	{
    //		QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " +
    // tr("Error"), 				tr("You didn't specify a valid data set for this
    // operation!")); return false;
    //	}

    QApplication::setOverrideCursor(Qt::WaitCursor);

    output(); // data analysis and output
    ((ApplicationWindow *)parent())->updateLog(logInfo());

    QApplication::restoreOverrideCursor();
    return true;
}

void Filter::output()
{
    std::vector<double> X(d_points);
    std::vector<double> Y(d_points);

    // do the data analysis
    calculateOutputData(X, Y);

    addResultCurve(X, Y);
}

int Filter::sortedCurveData(QwtPlotCurve const *const c, const double start, const double end,
                            std::vector<double> &x, std::vector<double> &y)
{
    if ((nullptr == c) || c->rtti() != QwtPlotItem::Rtti_PlotCurve)
        return 0;

    // start/end finding only works on nondecreasing data, so sort first
    int datasize = c->dataSize();
    vector<double> xtemp;
    for (int i = 0; i < datasize; i++) {
        xtemp.push_back(c->x(i));
    }
    vector<size_t> p(datasize);
    gsl_sort_index(&p[0], &xtemp[0], 1, datasize);

    // find indices that, when permuted by the sort result, give start and end
    int i_start, i_end;
    for (i_start = 0; i_start < datasize; i_start++)
        if (c->x(static_cast<int>(p[i_start])) >= start)
            break;
    for (i_end = datasize - 1; i_end >= 0; i_end--)
        if (c->x(static_cast<int>(p[i_end])) <= end)
            break;

    // make result arrays
    int n = i_end - i_start + 1;
    x.resize(n);
    y.resize(n);
    for (int j = 0, i = i_start; i <= i_end; i++, j++) {
        x[j] = c->x(static_cast<int>(p[i]));
        y[j] = c->y(static_cast<int>(p[i]));
    }
    return n;
}

int Filter::curveData(QwtPlotCurve const *const c, const double start, const double end,
                      std::vector<double> &x, std::vector<double> &y)
{
    if ((nullptr == c) || c->rtti() != QwtPlotItem::Rtti_PlotCurve)
        return 0;

    int datasize = c->dataSize();
    int i_start = 0, i_end = c->dataSize();
    for (i_start = 0; i_start < datasize; i_start++)
        if (c->x(i_start) >= start)
            break;
    for (i_end = datasize - 1; i_end >= 0; i_end--)
        if (c->x(i_end) <= end)
            break;

    int n = i_end - i_start + 1;
    x.resize(n);
    y.resize(n);

    for (int j = 0, i = i_start; i <= i_end; i++, j++) {
        x[j] = c->x(i);
        y[j] = c->y(i);
    }
    return n;
}

QwtPlotCurve *Filter::addResultCurve(const std::vector<double> &x, const std::vector<double> &y)
{
    ApplicationWindow *app = (ApplicationWindow *)parent();
    const QString tableName = app->generateUniqueName(this->objectName());
    Column *xCol = new Column(tr("1", "filter table x column name"), SciDAVis::ColumnMode::Numeric);
    Column *yCol = new Column(tr("2", "filter table y column name"), SciDAVis::ColumnMode::Numeric);
    xCol->setPlotDesignation(SciDAVis::X);
    yCol->setPlotDesignation(SciDAVis::Y);
    for (int i = 0; i < d_points; i++) {
        xCol->setValueAt(i, x[i]);
        yCol->setValueAt(i, y[i]);
    }
    // first set the values, then add the columns to the table, otherwise, we generate too many undo
    // commands
    Table *t = app->newHiddenTable(tableName,
                                   d_explanation + " " + tr("of") + " " + d_curve->title().text(),
                                   QList<Column *>() << xCol << yCol);

    DataCurve *c = new DataCurve(t, tableName + "_" + xCol->name(), tableName + "_" + yCol->name());
    c->setData(x.data(), y.data(), d_points);
    c->setPen(QPen(d_curveColor, 1));
    d_graph->insertPlotItem(c, Graph::Line);
    d_graph->updatePlot();

    return (QwtPlotCurve *)c;
}
