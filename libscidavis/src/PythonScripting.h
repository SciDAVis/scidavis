/***************************************************************************
	File                 : PythonScripting.h
	Project              : SciDAVis
--------------------------------------------------------------------
	Copyright            : (C) 2006 by Knut Franke
	Email (use @ for *)  : knut.franke*gmx.de
	Description          : Execute Python code from within SciDAVis

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
#ifndef PYTHON_SCRIPTING_H
#define PYTHON_SCRIPTING_H

#include "ScriptingEnv.h"
#include "PythonScript.h"
#include <boost/python.hpp>
namespace py=boost::python;

class QObject;
class QString;

typedef struct _object PyObject;

inline py::dict modDict(const char* mod) {
  using namespace py;
  return extract<dict>(import(mod).attr("__dict__"))();
}

class PythonScripting: public ScriptingEnv
{
  Q_OBJECT
  
public:
  static const char *langName;
  PythonScripting() {}
  PythonScripting(ApplicationWindow *parent, bool batch=false);
  ~PythonScripting();
  static ScriptingEnv *constructor(ApplicationWindow *parent, bool batch=false) { return new PythonScripting(parent, batch); }
  bool initialize();
  void redirectStdIO() override;

  void write(const char* text) { emit print(text); }

  //! like str(object) in Python
  /**
   * Convert object to a string.
   * Steals a reference to object if decref is true; borrows otherwise.
   */
  QString toString(PyObject *object, bool decref=false);
  //! evaluate a Python expression
  /**
   * Evaluates code, using argDict (borrowed reference) as local dictionary
   * or an empty one if argDict==NULL. name is the filename Python uses when
   * reporting errors. Returns a new reference; NULL means caller has to do
   * exception handling.
   */
  PyObject* eval(const QString &code, PyObject *argDict=NULL, const char *name="<scidavis>");
  //! execute a sequence of Python statements
  /**
   * Executes code, using argDict (borrowed reference) as local dictionary
   * or an empty one if argDict==NULL. name is the filename Python uses when
   * reporting errors. A false return value means caller has to do exception
   * handling.
   */
  bool exec(const QString &code, PyObject *argDict=NULL, const char *name="<scidavis>");
  QString errorMsg();

  bool isRunning() const;
  Script *newScript(const QString &code, QObject *context, const QString &name="<input>")
  {
    return new PythonScript(this, code, context, name);
  }

  bool setQObject(boost::python::object, const char*, PyObject *dict);
  bool setQObject(const boost::python::object& val, const char *name)
  { return setQObject(val,name,NULL); }
  bool setInt(int, const char*, PyObject *dict=NULL);
  bool setDouble(double, const char*, PyObject *dict=NULL);

  QStringList mathFunctions() const override;
  QString mathFunctionDoc (const QString &name) const override;
  QStringList fileExtensions() const override;

//  PyObject *globalDict() { return globals; }
//  PyObject *sysDict() { return sys; }
  
  
private:
  bool loadInitFile(const QString &path);

  PyObject *globals=nullptr;		// PyDict of global environment
  PyObject *math=nullptr;		// PyDict of math functions
  PyObject *sys=nullptr;		// PyDict of sys module
};

#endif
