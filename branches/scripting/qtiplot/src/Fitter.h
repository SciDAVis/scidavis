#ifndef FITTER_H
#define FITTER_H

#include <qobject.h>
#include <qlibrary.h>

#include "application.h"

class QwtPlotCurve;
class LegendMarker;

class Fitter : public QObject
{
    Q_OBJECT

public:

	typedef double (*fit_function_simplex)(const gsl_vector *, void *);
	typedef int (*fit_function)(const gsl_vector *, void *, gsl_vector *);
	typedef int (*fit_function_df)(const gsl_vector *, void *, gsl_matrix *);
	typedef int (*fit_function_fdf)(const gsl_vector *, void *, gsl_vector *, gsl_matrix *);

	enum Solver{ScaledLevenbergMarquardt, UnscaledLevenbergMarquardt, NelderMeadSimplex};
	enum WeightingMethod{NoWeighting, Instrumental, Statistical, ArbDataset};

    Fitter( ApplicationWindow *parent, Graph *g = 0);
    ~Fitter();

	//! Customs and stores the fit results according to the derived class specifications. Used by exponential fits.
	virtual void storeCustomFitResults(double *par);

	virtual void fit();
	void fitSimplex(gsl_multimin_function f, int &iterations, int &status);
	void fitGSL(gsl_multifit_function_fdf f, int &iterations, int &status);

	bool setWeightingData(WeightingMethod w, const QString& colName = QString::null);

	void setDataFromCurve(const QString& curveTitle);
	void setDataFromCurve(const QString& curveTitle, double from, double to);
	void setDataFromCurve(QwtPlotCurve *curve, int start, int end);

	void setGraph(Graph *g){d_graph = g;};

	void setFormula(const QString& s){d_formula = s;};
	virtual void setParametersList(const QStringList& lst){};

	void initializeParameter(int parIndex, double val){gsl_vector_set(d_param_init, parIndex, val);};
	void setInitialGuesses(double *x_init);
	void setSolver(Solver s){d_solver = s;};

	void setTolerance(double eps){d_tolerance = eps;};
	void setFitCurveColor(int colorId){d_curveColorIndex = colorId;};

	virtual void generateFitCurve(double *par, double *X, double *Y){};
	void setFitCurveParameters(bool generate, int points = 0);

	void setMaximumIterations(int iter){d_max_iterations = iter;};

	//! Output string added to the result log
	QString logFitInfo(double *par, int iterations, int status);

	//! Output string added to the plot as a new legend
	QString legendFitInfo();

	//! Returns the fit results as a string list
	QStringList fitResultsList();

	void showParametersTable(const QString& tableName);
	void showCovarianceMatrix(const QString& matrixName);

protected:
	//! The graph where the result curve should be displayed
	Graph *d_graph;

	fit_function d_f;
	fit_function_df d_df;
	fit_function_fdf d_fdf;
	fit_function_simplex d_fsimplex;

	//! Pointer to the GSL multifit solver
	gsl_multifit_fdfsolver *s;

	//! Pointer to the GSL multifit minimizer (for simplex algorithm)
	gsl_multimin_fminimizer *s_min;

	//! Size of the data arrays
	int d_n;

	//! Number of fit parameters
	int d_p;

	//! Initial guesses for the fit parameters 
	gsl_vector *d_param_init;

	//! x data set to be fitted
	double *d_x;
	
	//! y data set to be fitted
	double *d_y;

	//! weighting data set used for the fit
	double *d_w;

	//! GSL Tolerance
	double d_tolerance;

	//! Names of the fit parameters
	QStringList d_param_names;

	//! Tells weather the result curve has the same x values as the fit data or not
	bool gen_x_data;

	//! Number of result points to de calculated and displayed in the result curve
	int d_result_points;

	//! Color index of the result curve
	int d_curveColorIndex;

	//! Maximum number of iterations per fit
	int d_max_iterations;

	//! Solver type
	Solver d_solver;

	//! The curve to be fitted
	QwtPlotCurve *d_curve; 

	//! The fit formula
	QString d_formula;

	//! The fit type: exponential decay, gauss etc...
	QString d_fit_type;

	//! Covariance matrix
	gsl_matrix *covar;

	//! The kind of weighting to be performed on the data
	WeightingMethod d_weihting;

	//! The name of the weighting dataset
	QString weighting_dataset;

	//! Stores the result parameters
	double *d_results;
};

class ExponentialFitter : public Fitter
{
    Q_OBJECT

public:
	ExponentialFitter( bool expGrowth, ApplicationWindow *parent, Graph *g);

	void storeCustomFitResults(double *par);
	void generateFitCurve(double *par, double *X, double *Y);

private:
	bool is_exp_growth;
};

class TwoExpFitter : public Fitter
{
    Q_OBJECT

public:
	TwoExpFitter(ApplicationWindow *parent, Graph *g);

	void storeCustomFitResults(double *par);
	void generateFitCurve(double *par, double *X, double *Y);
};

class ThreeExpFitter : public Fitter
{
    Q_OBJECT

public:
	ThreeExpFitter(ApplicationWindow *parent, Graph *g);

	void storeCustomFitResults(double *par);
	void generateFitCurve(double *par, double *X, double *Y);
};

class SigmoidalFitter : public Fitter
{
public:
	SigmoidalFitter(ApplicationWindow *parent, Graph *g);

	void generateFitCurve(double *par, double *X, double *Y);
	void guessInitialValues();
};

class GaussFitter : public Fitter
{
public:
	GaussFitter(ApplicationWindow *parent, Graph *g);

	void generateFitCurve(double *par, double *X, double *Y);
	void guessInitialValues();
};

class LorentzFitter : public Fitter
{
public:
	LorentzFitter(ApplicationWindow *parent, Graph *g);

	void generateFitCurve(double *par, double *X, double *Y);
	void guessInitialValues();
};

class NonLinearFitter : public Fitter
{
public:
	NonLinearFitter(ApplicationWindow *parent, Graph *g);

	void setParametersList(const QStringList& lst);
	void generateFitCurve(double *par, double *X, double *Y);
	void setInitialGuesses(const QStringList& lst);
};

class PluginFitter : public Fitter
{
public:
	typedef double (*fitFunctionEval)(double, double *);

	PluginFitter(ApplicationWindow *parent, Graph *g);

	bool load(const QString& pluginName);
	void generateFitCurve(double *par, double *X, double *Y);
	void setInitialGuesses(const QStringList& lst);

private:
	fitFunctionEval f_eval;
};

class MultiPeakFitter : public Fitter
{
public:
		
	enum PeakProfile{Gauss, Lorentz};

	MultiPeakFitter(ApplicationWindow *parent, Graph *g = 0, PeakProfile profile = Gauss, int peaks = 0);

	void generateFitCurve(double *par, double *X, double *Y);
	void guessInitialValues();

	int peaks(){return d_peaks;};

private:
	int d_peaks;
	PeakProfile d_profile;
};
#endif
