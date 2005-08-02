// vim: set tabstop=4 shiftwidth=4 noexpandtab:
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

#include "bookmarkviewcontroller.moc"

// Qt
#include <qcursor.h>
#include <qheader.h>
#include <qlistview.h>
#include <qpopupmenu.h>

// KDE
#include <kbookmarkmanager.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kurl.h>

// Local
#include "branchpropertiesdialog.h"

namespace Gwenview {

struct BookmarkItem : public QListViewItem {
	template <class ItemParent>
	BookmarkItem(ItemParent* parent, const KBookmark& bookmark)
	: QListViewItem(parent)
	, mBookmark(bookmark)
	{
		refresh();
	}

	void refresh() {
		setText(0, mBookmark.text() );
		setPixmap(0, SmallIcon(mBookmark.icon()) );
	}

	KBookmark mBookmark;
};


struct BookmarkViewController::Private {
	QListView* mListView;
	KBookmarkManager* mManager;

	template <class ItemParent>
	void addGroup(ItemParent* itemParent, const KBookmarkGroup& group) {
		KBookmark bookmark=group.first();
		BookmarkItem* previousItem=0;
		BookmarkItem* item=0;
		for (;!bookmark.isNull(); bookmark=group.next(bookmark) ) {
			if (bookmark.isSeparator()) continue;
			
			// Create the item and make sure it's placed at the end
			previousItem=item;
			item=new BookmarkItem(itemParent, bookmark);
			if (previousItem) {
				item->moveItem(previousItem);
			}
			
			if (bookmark.isGroup()) {
				addGroup(item, static_cast<const KBookmarkGroup&>(bookmark) );
			}
		}
	}
};


BookmarkViewController::BookmarkViewController(QListView* listView, KBookmarkManager* manager)
: QObject(listView)
{
	d=new Private;
	d->mListView=listView;
	d->mManager=manager;

	d->mListView->header()->hide();
	d->mListView->setRootIsDecorated(true);
	d->mListView->addColumn(QString::null);
	d->mListView->setSorting(-1);

	connect(d->mListView, SIGNAL(clicked(QListViewItem*)),
		this, SLOT(slotOpenBookmark(QListViewItem*)) );
	connect(d->mListView, SIGNAL(returnPressed(QListViewItem*)),
		this, SLOT(slotOpenBookmark(QListViewItem*)) );
	connect(d->mListView, SIGNAL(contextMenuRequested(QListViewItem*, const QPoint&, int)),
		this, SLOT(slotContextMenu(QListViewItem*)) );

	// For now, we ignore the caller parameter and just refresh the full list on update
	connect(d->mManager, SIGNAL(changed(const QString&, const QString&)),
		this, SLOT(fill()) );
	
	fill();
}


BookmarkViewController::~BookmarkViewController() {
	delete d;
}


void BookmarkViewController::fill() {
	d->mListView->clear();
	KBookmarkGroup root=d->mManager->root();
	d->addGroup(d->mListView, root);
}
	

void BookmarkViewController::slotOpenBookmark(QListViewItem* item_) {
	if (!item_) return;
	BookmarkItem* item=static_cast<BookmarkItem*>(item_);
	const KURL& url=item->mBookmark.url();
	if (!url.isValid()) return;
	emit openURL(url);
}


void BookmarkViewController::slotContextMenu(QListViewItem* item) {
	if (!item) return;
	QPopupMenu menu(d->mListView);
	menu.insertItem(SmallIcon("edit"), i18n("Edit"), 
		this, SLOT(editCurrentBookmark()));
	menu.insertItem(SmallIcon("editdelete"), i18n("Delete Bookmark"),
		this, SLOT(deleteCurrentBookmark()));
	menu.exec(QCursor::pos());
}


void BookmarkViewController::editCurrentBookmark() {
	BookmarkItem* item=static_cast<BookmarkItem*>( d->mListView->currentItem() );
	Q_ASSERT(item);
	if (!item) return;
	KBookmark bookmark=item->mBookmark;
	
	BranchPropertiesDialog dialog(d->mListView);
	dialog.setContents(bookmark.icon(), bookmark.text(), bookmark.url().prettyURL());
	if (dialog.exec()==QDialog::Rejected) return;

	QDomElement element=bookmark.internalElement();
	element.setAttribute("icon", dialog.icon());
	element.setAttribute("href", dialog.url());

	// Find title element (or create it if it does not exist)
	QDomElement titleElement;
	QDomNode tmp=element.namedItem("title");
	if (tmp.isNull()) {
		titleElement=element.ownerDocument().createElement("title");
		element.appendChild(titleElement);
	} else {
		titleElement=tmp.toElement();
	}
	Q_ASSERT(!titleElement.isNull());

	// Get title element content (or create)
	QDomText titleText;
	tmp=titleElement.firstChild();
	if (tmp.isNull()) {
		titleText=element.ownerDocument().createTextNode("");
		titleElement.appendChild(titleText);
	} else {
		titleText=tmp.toText();
	}
	Q_ASSERT(!titleText.isNull());

	// Set title (at last!)
	titleText.setData(dialog.title());
	
	KBookmarkGroup group=bookmark.parentGroup();
	d->mManager->emitChanged(group);
}


void BookmarkViewController::deleteCurrentBookmark() {
	BookmarkItem* item=static_cast<BookmarkItem*>( d->mListView->currentItem() );
	Q_ASSERT(item);
	if (!item) return;
	KBookmark bookmark=item->mBookmark;

	int response=KMessageBox::warningContinueCancel(d->mListView,
		"<qt>" + i18n("Are you sure you want to delete the bookmark <b>%1</b>?").arg(bookmark.text()) + "</qt>",
		i18n("Delete Bookmark"),
#if KDE_IS_VERSION(3, 3, 0)
		KStdGuiItem::del()
#else
		KGuiItem( i18n( "&Delete" ), "editdelete", i18n( "Delete Bookmark" ) )
#endif
		);
	if (response==KMessageBox::Cancel) return;
	
	KBookmarkGroup group=bookmark.parentGroup();
	group.deleteBookmark(bookmark);
	d->mManager->emitChanged(group);
}



} // namespace
