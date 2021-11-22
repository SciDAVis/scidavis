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
    if ((nullptr == d_curve)|| (d_curve->rtti() != QwtPlotItem::Rtti_PlotCurve)) {
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

    std::vector<double> temp(d_curve->dataSize());
    for (auto ii=0; d_curve->dataSize() > ii; ++ii )
        temp[ii]=d_curve->x(ii);

    d_indices = getIndices(temp, start, end, d_sort_data);

    try
    {
        d_x.resize(d_indices.size());
        d_y.resize(d_indices.size());
    }
    catch (const std::bad_alloc &e) {
        QMessageBox::critical((ApplicationWindow *)parent(), tr("SciDAVis") + " - " + tr("Error"),
                              tr("Could not allocate memory, operation aborted!\n")
                                      + tr("Allocator returned: ") + e.what());
        d_init_err = true;
        return;
    }

    for (auto ii=0u; d_indices.size() > ii; ++ii )
    {
        d_x[ii]=d_curve->x(d_indices[ii]);
        d_y[ii]=d_curve->y(d_indices[ii]);
    }

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
    else
    {
        d_init_err = true;
        return;
    }
}

bool Filter::isDataAcceptable() const
{

    if (d_x.size() < unsigned(d_min_points)) {
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
