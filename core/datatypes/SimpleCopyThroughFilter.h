/***************************************************************************
    File                 : SimpleCopyThroughFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Benkert
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
    Description          : Filter which copies the provided input unaltered
                           to the output

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
#ifndef SIMPLE_COPY_THROUGH_FILTER_H
#define SIMPLE_COPY_THROUGH_FILTER_H

#include "core/AbstractSimpleFilter.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

/**
 * \brief Filter which copies the provided input unaltered to the output
 *
 * Most of the necessary methods for this filter are already implemented
 * in AbstractSimpleFilter.
 *
 * The difference between this filter and CopyThroughFilter is that
 * this inherits AbstractColumn and thus can be directly used
 * as input for other filters and plot functions. 
 */
class SimpleCopyThroughFilter : public AbstractSimpleFilter
{
	Q_OBJECT

	public:
		//!\name Masking
		//@{
		//! Return whether a certain row is masked
		virtual bool isMasked(int row) const { return d_inputs.value(0) ? d_inputs.at(0)->isMasked(row) : false; }
		//! Return whether a certain interval of rows rows is fully masked
		virtual bool isMasked(Interval<int> i) const { return d_inputs.value(0) ? d_inputs.at(0)->isMasked(i) : false; }
		//! Return all intervals of masked rows
		virtual QList< Interval<int> > maskedIntervals() const 
		{
			return d_inputs.value(0) ? d_inputs.at(0)->maskedIntervals() : QList< Interval<int> >(); 
		}
		//@}

		//! \name XML related functions
		//@{
		//! Save the column as XML
		virtual void save(QXmlStreamWriter * writer) const
		{
			writer->writeStartElement("simple_filter");
			writer->writeAttribute("filter_name", "SimpleCopyThroughFilter");
			writer->writeEndElement();
		}
		//! Load the column from XML
		virtual bool load(QXmlStreamReader * reader)
		{
			QString prefix(tr("XML read error: ","prefix for XML error messages"));
			QString postfix(tr(" (loading failed)", "postfix for XML error messages"));

			if(reader->isStartElement() && reader->name() == "simple_filter") 
			{
				QXmlStreamAttributes attribs = reader->attributes();
				QString str = attribs.value(reader->namespaceUri().toString(), "filter_name").toString();
				if(str != "SimpleCopyThroughFilter")
					reader->raiseError(prefix+tr("incompatible filter type")+postfix);
				reader->readNext(); // read the end element
			}
			else
				reader->raiseError(prefix+tr("no simple filter element found")+postfix);

			return !reader->error();
		}
		//@}

	protected:
		//! All types are accepted.
		virtual bool inputAcceptable(int, AbstractColumn *) 
		{
			return true;
		}

		//!\name signal handlers
		//@{
		virtual void inputMaskingAboutToChange(AbstractColumn*) 
		{ 
			emit d_output_column->maskingAboutToChange(d_output_column); 
		}
		virtual void inputMaskingChanged(AbstractColumn*) 
		{ 
			emit d_output_column->maskingChanged(d_output_column); 
		}
		//@}
};

#endif // ifndef SIMPLE_COPY_THROUGH_FILTER_H
