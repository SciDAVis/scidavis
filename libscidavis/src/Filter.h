/***************************************************************************
    File                 : Filter.h
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
#ifndef FILTER_H
#define FILTER_H

#include <QObject>

#include "ApplicationWindow.h"

class QwtPlotCurve;
class Graph;
class Table;

//! Abstract base class for data analysis operations
class Filter : public QObject
{
    Q_OBJECT

public:
    Filter(ApplicationWindow *parent, Table *t = 0, QString name = QString());
    Filter(ApplicationWindow *parent, Graph *g = 0, QString name = QString());
    virtual ~Filter() {};
    //! Actually does the job. Should be reimplemented in derived classes.
    virtual bool run();

    virtual void setDataCurve(const int curve, double start, double end);
    bool setDataFromCurve(const QString &curveTitle, Graph *const g = 0);
    bool setDataFromCurve(const QString &curveTitle, const double from, const double to,
                          Graph *const g = 0);

    //! Changes the data range if the source curve was already assigned. Provided for convenience.
    void setInterval(double from, double to);

    //! Sets the tolerance used by the GSL routines
    void setTolerance(double eps) { d_tolerance = eps; };

    //! Sets the color of the output fit curve.
    void setColor(QColor colorId) { d_curveColor = colorId; };

    //! Sets the color of the output fit curve. Provided for convenience. To be used in scripts only!
    void setColor(const QString &colorName);

    //! Sets the number of points in the output curve
    void setOutputPoints(int points) { d_points = points; };

    //! Sets the precision used for the output
    void setOutputPrecision(int digits) { d_prec = digits; };

    //! Sets the maximum number of iterations to be performed during an iterative session
    void setMaximumIterations(int iter) { d_max_iterations = iter; };

    //! Adds a new legend to the plot. Calls virtual legendInfo()
    virtual void showLegend();

    //! Output string added to the plot as a new legend
    virtual QString legendInfo() { return QString(); };

    //! Returns the size of the fitted data set
    int dataSize() { return d_x.size(); };

    bool error() { return d_init_err; };

private:
    void init();

protected:

    //! returns either sorted or unsorted indices for data in range [start;end]
    template <typename T>
    std::vector<size_t> static getIndices(const std::vector<T>& data, const T start, const T end, const bool sorted);

    //! holds either sorted or unsorted indices for further usage
    std::vector<size_t> d_indices;

    virtual bool isDataAcceptable() const;

    //! Adds the result curve to the target output plot window. Creates a hidden table and frees the input data from memory.
    QwtPlotCurve *addResultCurve(const std::vector<double> &x, const std::vector<double> &y);

    //! Performs checks and returns the index of the source data curve if OK, -1 otherwise
    int curveIndex(const QString &curveTitle, Graph *const g);

    //! Output string added to the log pannel of the application
    virtual QString logInfo() { return QString(); };

    //! Performs the data analysis and takes care of the output
    virtual void output();

    //! Calculates the data for the output curve and store it in the X an Y vectors
    virtual void calculateOutputData(std::vector<double> &X, std::vector<double> &Y)
    {
        Q_UNUSED(X)
        Q_UNUSED(Y)
    };

    //! The graph where the result curve should be displayed
    Graph *d_graph;

    //! A table source of data
    Table *d_table;

    //! x data set to be analysed
    std::vector<double> d_x;

    //! y data set to be analysed
    std::vector<double> d_y;

    //! GSL Tolerance, if ever needed...
    double d_tolerance;

    //! Number of result points to be calculated and displayed in the output curve
    int d_points;

    //! Color index of the result curve
    QColor d_curveColor;

    //! Maximum number of iterations per fit
    int d_max_iterations;

    //! The curve to be analysed
    QwtPlotCurve *d_curve;

    //! Precision (number of significant digits) used for the results output
    int d_prec;

    //! Error flag telling if something went wrong during the initialization phase.
    bool d_init_err;

    //! Data interval
    double d_from, d_to;

    //! Specifies if the filter needs sorted data as input
    bool d_sort_data;

    //! Minimum number of data points necessary to perform the operation
    int d_min_points;

    //! String explaining the operation in the comment of the result table and in the project explorer
    QString d_explanation;
};

template <typename T>
std::vector<size_t> Filter::getIndices(const std::vector<T>& data, const T start, const T end, const bool sorted)
{
    std::vector<size_t> result;
    if (data.empty() || end < start)
            return result;
    if (sorted)
    {
        std::vector<std::pair<T,size_t>> tData(data.size());
        size_t ii=0;
        std::generate(tData.begin(), tData.end(), [&](){ auto pair = std::make_pair(data[ii],ii); ++ii; return pair; });
        std::sort(tData.begin(),tData.end(),[](const std::pair<T,size_t>& a, std::pair<T,size_t>& b){ return a.first < b.first;});
        auto first = std::lower_bound(tData.cbegin(),tData.cend(),start,
                                       [](const std::pair<T,size_t>& a, const T& value){return a.first < value;});
        auto last  = std::upper_bound(tData.cbegin(),tData.cend(),end,
                                       [](const T& value, const std::pair<T,size_t>& a){return value < a.first;});
        std::transform(first,last,std::back_inserter(result),[](const std::pair<T,size_t>& a ){return a.second;});
    }
    else
    {
        size_t ii = 0;
        for (auto it = data.cbegin(); data.cend() !=it; ++it)
        if ((start <= *it)&&(end >= *it))
            result.push_back(ii++);
    }
    return result;
}

#endif
