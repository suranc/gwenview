// vim: set tabstop=4 shiftwidth=4 noexpandtab:
// kate: indent-mode csands; indent-width 4; replace-tabs-save off; replace-tabs off; replace-trailing-space-save off; space-indent off; tabs-indents on; tab-width 4;
/*
Gwenview - A simple image viewer for KDE
Copyright 2005 Aurelien Gateau

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
#ifndef VTABWIDGET_H
#define VTABWIDGET_H   

// Qt
#include <qwidget.h>

class QPixmap;

namespace Gwenview {


class VTabWidget : public QWidget {
Q_OBJECT
public:
	VTabWidget(QWidget* parent);
	virtual ~VTabWidget();
	void addTab(QWidget*, const QPixmap&, const QString& label);

private slots:
	void slotClicked(int id);

private:
	struct Private;
	Private* d;
};


} // namespace


#endif /* VTABWIDGET_H */