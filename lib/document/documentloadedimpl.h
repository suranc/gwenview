// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
Gwenview: an image viewer
Copyright 2007 Aurélien Gâteau <agateau@kde.org>

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
#ifndef DOCUMENTLOADEDIMPL_H
#define DOCUMENTLOADEDIMPL_H

// Qt

// KDE

// Local
#include <lib/document/abstractdocumenteditor.h>
#include <lib/document/abstractdocumentimpl.h>

class QIODevice;

namespace Gwenview {


class DocumentLoadedImplPrivate;
class DocumentLoadedImpl : public AbstractDocumentImpl, protected AbstractDocumentEditor {
	Q_OBJECT
public:
	DocumentLoadedImpl(Document*, const QByteArray&);
	~DocumentLoadedImpl();

	// AbstractDocumentImpl
	virtual void init();
	virtual Document::LoadingState loadingState() const;
	virtual bool save(const KUrl&, const QByteArray& format);
	virtual AbstractDocumentEditor* editor();
	virtual QByteArray rawData() const;
	virtual bool isEditable() const;
	//

protected:
	virtual bool saveInternal(QIODevice* device, const QByteArray& format);

	// AbstractDocumentEditor
	virtual void setImage(const QImage&);
	virtual void applyTransformation(Orientation orientation);
	//

private:
	DocumentLoadedImplPrivate* const d;
};


} // namespace

#endif /* DOCUMENTLOADEDIMPL_H */