// vim: set tabstop=4 shiftwidth=4 noexpandtab
/*
Gwenview - A simple image viewer for KDE
Copyright 2000-2004 Aur�ien G�eau

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kimageio.h>
#include <klocale.h>

#include <src/gvjpegformattype.h>
#include <src/gvmainwindow.h>
#include <src/qxcfi.h>

static KCmdLineOptions options[] = {
	{ "f", I18N_NOOP("Start in fullscreen mode"), 0 },
	{ "+[file or folder]", I18N_NOOP("A starting file or folder"), 0 },
	KCmdLineLastOption
};

static const char* version="1.1.0";


int main (int argc, char *argv[]) {
	KAboutData aboutData("gwenview", I18N_NOOP("Gwenview" ),
		version, I18N_NOOP("An image viewer for KDE"), KAboutData::License_GPL,
		"Copyright 2000-2004 Aurélien Gâteau",0,"http://gwenview.sourceforge.net");
	aboutData.addCredit("Frank Becker", I18N_NOOP("Fast JPEG thumbnail generation (v0.13.0)"), "ff@telus.net");
	aboutData.addCredit("Tudor Calin", I18N_NOOP("Address bar (v0.16.0)\nHistory support (v1.0.0)"), "tudor_calin@mymail.ro");
	aboutData.addCredit("Avinash Chopde", I18N_NOOP("File operation patch (v0.9.2)"), "avinash@acm.org");
	aboutData.addCredit("Marco Gazzetta", I18N_NOOP("Fixed crash when trying to generate a thumbnail for a broken JPEG file (v0.16.0)"), "mililani@pobox.com");
	aboutData.addCredit("GeniusR13", I18N_NOOP("Fixed compilation on KDE 3.0 (v0.16.1)"), "geniusr13@gmx.net");
	aboutData.addCredit("Ian Koenig", I18N_NOOP("First RPM spec file"), "iguy@ionsphere.org");
	aboutData.addCredit("Meni Livne", I18N_NOOP("Toolbar layout patch for RTL languages (v0.16.0)"), "livne@kde.org");
	aboutData.addCredit("Lubos Lunak", I18N_NOOP("Patch to generate thumbnails for the visible images first (v1.1.0)"), "l.lunak@suse.cz");
	aboutData.addCredit("Angelo Naselli", I18N_NOOP("Printing support (v1.0.0)"), "random_lx@yahoo.com");
	aboutData.addCredit("Jos van den Oever", I18N_NOOP("File info view (v1.0.0)\nPatch to toggle auto-zoom on click (v1.0.0)"), "jos@vandenoever.info");
	aboutData.addCredit("Andreas Pfaller", I18N_NOOP("Option to prevent Gwenview from automatically loading the first image of a folder (v0.15.0)"), "apfaller@yahoo.com.au");
	aboutData.addCredit("Renchi Raju", I18N_NOOP("Fixed thumbnail generation to share the thumbnail folder of Konqueror v3 (v0.15.0)"), "renchi@green.tam.uiuc.edu");
	aboutData.addCredit("Michael Spanier", I18N_NOOP("Patch for mouse navigation (v0.7.0)"), "mail@michael-spanier.de");
	aboutData.addCredit("Christian A Strømmen", I18N_NOOP("Integration in Konqueror folder context menu"), "number1@realityx.net");

	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options );

	KApplication kapplication;

	KImageIO::registerFormats();
	XCFImageFormat::registerFormat();
	GVJPEGFormatType jpegFormatType;

	if (kapplication.isRestored()) {
		RESTORE(GVMainWindow)
	} else {
		GVMainWindow *mainWindow = new GVMainWindow;
		mainWindow->show();
	}

	return kapplication.exec();
}
