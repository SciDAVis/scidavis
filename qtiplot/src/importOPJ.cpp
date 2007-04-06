/***************************************************************************
    File                 : importOPJ.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Origin project import class
                           
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
#include "importOPJ.h"
#include <OPJFile.h>

#include <QRegExp>
#include <QMessageBox>

ImportOPJ::ImportOPJ(ApplicationWindow *app, const QString& filename) :
		mw(app)
{	
OPJFile opj((const char *)filename.latin1());
parse_error = opj.Parse();
importTables(opj);
}

bool ImportOPJ::importTables(OPJFile opj) 
{
	int visible_count=0;
	for (int s=0; s<opj.numSpreads(); s++) 
	{	
		int nr_cols = opj.numCols(s);
		int maxrows = opj.maxRows(s);

		Table *table = (opj.spreadHidden(s)||opj.spreadLoose(s)) ? mw->newHiddenTable(opj.spreadName(s), opj.spreadLabel(s), maxrows, nr_cols) 
										: mw->newTable(opj.spreadName(s), maxrows, nr_cols);
		if (!table)
			return false;

		table->setWindowLabel(opj.spreadLabel(s));
		for (int j=0; j<nr_cols; j++) 
		{
			QString name(opj.colName(s,j));
			table->setColName(j, name.replace(QRegExp(".*_"),""));
			table->setCommand(j, QString(opj.colCommand(s,j)));
			table->setColComment(j, QString(opj.colComment(s,j)));
			table->changeColWidth(opj.colWidth(s,j)*7, j);//approximately col_width_in_pixel(QtiPlot)/col_width_in_character(Origin)=7 - need to fix

			if (QString(opj.colType(s,j)) == "X")
				table->setColPlotDesignation(j, Table::X);
			else if (QString(opj.colType(s,j)) == "Y")
				table->setColPlotDesignation(j, Table::Y);
			else if (QString(opj.colType(s,j)) == "Z")
				table->setColPlotDesignation(j, Table::Z);
			else
				table->setColPlotDesignation(j, Table::None);

			for (int i=0; i<opj.numRows(s,j); i++) 
			{
				if(strcmp(opj.colType(s,j),"LABEL")&&opj.colValueType(s,j)!=1) 
				{// number
					double val = opj.Data(s,j)[i];		
					if(fabs(val)>0 && fabs(val)<2.0e-300)// empty entry
						continue;
					
					table->setText(i, j, QString::number(val));
				}
				else// label? doesn't seem to work
					table->setText(i, j, QString(opj.SData(s,j,i)));
			}

			QString format;
			switch(opj.colValueType(s,j))
			{
			case 0: //Numeric
			case 6: //Text&Numeric
				int f;
				switch(opj.colValueTypeSpec(s,j))
				{
				case 0: //Decimal 1000
					f=1;
					break;
				case 1: //Scientific
					f=2;
					break;
				case 2: //Engeneering
				case 3: //Decimal 1,000
					f=0;
					break;
				}
				table->setColNumericFormat(f, opj.colDecPlaces(s,j), j);
				break;
			case 1: //Text
				table->setTextFormat(j);
				break;
			case 2: // Date
				switch(opj.colValueTypeSpec(s,j))
				{
				case 0:
				case 9:
				case 10:
					format="dd.MM.yyyy";
					break;
				case 2:
					format="MMM d";
					break;
				case 3:
					format="M/d";
					break;
				case 4:
					format="d";
					break;
				case 5:
				case 6:
					format="ddd";
					break; 
				case 7:
					format="yyyy";
					break;
				case 8:
					format="yy";
					break;
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
					format="yyMMdd";
					break;
				case 16:
				case 17:
					format="MMM";
					break;
				case 19:
					format="M-d-yyyy";
					break;
				default:
					format="dd.MM.yyyy";
				}
				table->setDateFormat(format, j);
				break;
			case 3: // Time
				switch(opj.colValueTypeSpec(s,j))
				{
				case 0:
					format="hh:mm";
					break;
				case 1:
					format="hh";
					break;
				case 2:
					format="hh:mm:ss";
					break;
				case 3:
					format="hh:mm:ss.zzz";
					break;
				case 4:
					format="hh ap";
					break;
				case 5:
					format="hh:mm ap";
					break;
				case 6:
					format="mm:ss";
					break;
				case 7:
					format="mm:ss.zzz";
					break;
				case 8:
					format="hhmm";
					break;
				case 9:
					format="hhmmss";
					break;
				case 10:
					format="hh:mm:ss.zzz";
					break;
				}
				table->setTimeFormat(format, j);
				break;
			case 4: // Month
				switch(opj.colValueTypeSpec(s,j))
				{
				case 0:
				case 2:
					format="shortMonthName";
					break;
				case 1:
					format="longMonthName";
					break;
				}
				table->setMonthFormat(format, j);
				break;
			case 5: // Day
				switch(opj.colValueTypeSpec(s,j))
				{
				case 0:
				case 2:
					format="shortDayName";
					break;
				case 1:
					format="longDayName";
					break;
				}
				table->setDayFormat(format, j);
				break;
			}
		}


		if(!(opj.spreadHidden(s)||opj.spreadLoose(s)))
		{
			table->showNormal();

			//cascade the tables
			int dx=table->verticalHeaderWidth();
			int dy=table->parentWidget()->frameGeometry().height() - table->height();
			table->parentWidget()->move(QPoint(visible_count*dx,visible_count*dy));
			visible_count++;
		}
	}

//TO DO: import matrices
return true;
}
