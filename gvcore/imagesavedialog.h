// vim: set tabstop=4 shiftwidth=4 noexpandtab
/*
Gwenview - A simple image viewer for KDE
Copyright 2000-2004 Aur�lien G�teau

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
#ifndef IMAGESAVEDIALOG_H
#define IMAGESAVEDIALOG_H

// KDE includes
#include <kfiledialog.h>

class QString;
class QWidget;

class KURL;

namespace Gwenview {
class ImageSaveDialog : public KFileDialog {
Q_OBJECT
public:
	ImageSaveDialog(KURL& url,const QCString& imageFormat,QWidget* parent);
	QCString imageFormat() const { return mImageFormat; }

protected slots:
	void accept();

private slots:
	void updateImageFormat(const QString&);

private:
	KURL& mURL;
	QCString mImageFormat;
	QMap<QString,QString> mImageFormats;
};

} // namespace
#endif
