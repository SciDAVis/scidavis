/***************************************************************************
    File                 : globals.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Definition of global constants and enums

 ***************************************************************************/

#include "globals.h"
#include <QMessageBox>
#include <QIcon>
#include <QObject>

//  Don't forget to change the Doxyfile when changing these!
const int SciDAVis::scidavis_version = 0x000100;

const char * SciDAVis::extra_version = "";

const char * SciDAVis::copyright_string = "Developers (alphabetical order):\nKnut Franke\nTilman Hoener zu Siederdissen\n\nDocumentation:\nRoger Gadiou\n\nSpecial thanks to (alphabetical order):\nQuentin Denis\nGudjon I. Gudjonsson\nAlex Kargovsky\nIon Vasilief\n\nThanks to (no particular order):\nthe developers of Qt, Qwt, QwtPlot3D, GSL, muParser, zlib, Python, PyQt, and liborigin\nall bug reporters, translators and other contributors";

const char * SciDAVis::release_date = " 2007-08-05";

int SciDAVis::version()
{
	return scidavis_version;
}

QString SciDAVis::versionString()
{
	return "SciDAVis " + 
			QString::number((scidavis_version & 0xFF0000) >> 16)+"."+ 
			QString::number((scidavis_version & 0x00FF00) >> 8)+"."+
			QString::number(scidavis_version & 0x0000FF);
}
			
QString SciDAVis::extraVersion()
{
	return	QString(extra_version);
}


void SciDAVis::about()
{
	QString text = "<h2>"+ versionString() + extraVersion() + "</h2>";
	text += "<h3>" + QObject::tr("Released") + ": " + QString(SciDAVis::release_date) + "</h3>";
	text +=	"<h3>" + QString(SciDAVis::copyright_string).replace("\n", "<br>") + "</h3>";

	QMessageBox *mb = new QMessageBox();
	mb->setAttribute(Qt::WA_DeleteOnClose);
	mb->setWindowTitle(QObject::tr("About SciDAVis"));
	mb->setWindowIcon(QIcon(":/appicon"));
	mb->setIconPixmap(QPixmap(":/appicon"));
	mb->setText(text);
	mb->exec();
}

QString SciDAVis::copyrightString()
{
	return copyright_string;
}

QString SciDAVis::releaseDateString()
{
	return release_date;
}
