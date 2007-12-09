// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
Gwenview: an image viewer
Copyright 2007 Aurélien Gâteau

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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

*/
#ifndef SAVEBAR_H
#define SAVEBAR_H

// Qt

// KDE

// Local
#include <lib/slidecontainer.h>

class KUrl;

namespace Gwenview {


class SaveBarPrivate;
class SaveBar : public SlideContainer {
	Q_OBJECT
public:
	SaveBar(QWidget* parent);
	~SaveBar();

	void setCurrentUrl(const KUrl&);

	/**
	 * Keep save bar hidden even if there are modifications.
	 * Useful for fullscreen mode.
	 */
	void setForceHide(bool);

	bool saveAll();

Q_SIGNALS:
	void requestSave(const KUrl&);
	void goToUrl(const KUrl&);

private:
	SaveBarPrivate* const d;

private Q_SLOTS:
	void updateContent();
	void triggerAction(const QString& action);
};


} // namespace

#endif /* SAVEBAR_H */
