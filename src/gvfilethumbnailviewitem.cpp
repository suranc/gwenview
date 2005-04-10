// vim: set tabstop=4 shiftwidth=4 noexpandtab
/*  Gwenview - A simple image viewer for KDE
    Copyright 2000-2004 Aur�lien G�teau
    This class is based on the KIconViewItem class from KDE libs.
    Original copyright follows.
*/
/* This file is part of the KDE libraries
   Copyright (C) 1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
// Qt includes
#include <qapplication.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpixmap.h>

// KDE includes
#include <kdebug.h>
#include <kwordwrap.h>
#include <kurldrag.h>

// Our includes
#include "gvfilethumbnailview.h"
#include "gvfilethumbnailviewitem.h"

/*
static void printRect(const QString& txt,const QRect& rect) {
	kdWarning() << txt << " : " << rect.x() << "x" << rect.y() << " " << rect.width() << "x" << rect.height() << endl;
}
*/


GVFileThumbnailViewItem::GVFileThumbnailViewItem(QIconView* view,const QString& text,const QPixmap& icon, KFileItem* fileItem)
: QIconViewItem(view,text,icon), mFileItem(fileItem), mWordWrap(0L) {
	calcRect();
}


GVFileThumbnailViewItem::~GVFileThumbnailViewItem() {
	if (mWordWrap) delete mWordWrap;
}


void GVFileThumbnailViewItem::calcRect(const QString& text_) {
	GVFileThumbnailView *view=static_cast<GVFileThumbnailView*>(iconView());
	Q_ASSERT(view);
	if (!view) return;

	QFontMetrics fm = view->fontMetrics();
	QRect itemIconRect = QRect(0,0,0,0);
	QRect itemTextRect = QRect(0,0,0,0);
	QRect itemRect = rect();
	int availableTextWidth=rect().width()
		- (view->itemTextPos()==QIconView::Bottom ? 0 : view->thumbnailSize() );

// Init itemIconRect 
#ifndef QT_NO_PICTURE
	if ( picture() ) {
		QRect br = picture()->boundingRect();
		itemIconRect.setWidth( br.width() );
		itemIconRect.setHeight( br.height() );
	} else
#endif
	{
		// Qt uses unknown_icon if no pixmap. Let's see if we need that - I doubt it
		if (!pixmap()) return;
		itemIconRect.setWidth( pixmap()->width()+2 );
		itemIconRect.setHeight( pixmap()->height()+2 );
	}

// Init itemTextRect 
	QRect r;
	if (iconView()->wordWrapIconText()) {
	// When is text_ set ? Doesn't look like it's ever set.
		QString txt = text_.isEmpty() ? text() : text_;
		QRect outerRect( 0, 0, availableTextWidth,0xFFFFFFFF);

		if (mWordWrap) delete mWordWrap;
		mWordWrap=KWordWrap::formatText( fm, outerRect, AlignHCenter | WordBreak, txt );
		r=mWordWrap->boundingRect();
	} else {
		truncateText(fm);
		r=QRect(0,0,availableTextWidth,fm.height());
	}
	
	// Add the info line height
	r.rBottom()+=fm.height();
	
	if ( r.width() > availableTextWidth ) {
		r.setWidth(availableTextWidth);
	}

	itemTextRect.setWidth( QMAX(r.width(),fm.width("X")) );
	itemTextRect.setHeight( r.height() );

// All this code isn't related to the word-wrap algo...
// Sucks that we have to duplicate it.
	int w = 0;    int h = 0;
	if ( view->itemTextPos() == QIconView::Bottom ) {
		w = QMAX( itemTextRect.width(), itemIconRect.width() );
		h = itemTextRect.height() + itemIconRect.height() + 1;

		itemRect.setWidth( w );
		itemRect.setHeight( h );
		int width = QMAX( w, QApplication::globalStrut().width() ); // see QIconViewItem::width()
		int height = QMAX( h, QApplication::globalStrut().height() ); // see QIconViewItem::height()
		itemTextRect = QRect( ( width - itemTextRect.width() ) / 2, height - itemTextRect.height(),
							itemTextRect.width(), itemTextRect.height() );
		itemIconRect = QRect( ( width - itemIconRect.width() ) / 2, 0,
							itemIconRect.width(), itemIconRect.height() );
	} else {
		h = QMAX( itemTextRect.height(), itemIconRect.height() );
		w = itemTextRect.width() + itemIconRect.width() + 1;

		itemRect.setWidth( w );
		itemRect.setHeight( h );
		int width = QMAX( w, QApplication::globalStrut().width() ); // see QIconViewItem::width()
		int height = QMAX( h, QApplication::globalStrut().height() ); // see QIconViewItem::height()

		itemTextRect = QRect( width - itemTextRect.width(), ( height - itemTextRect.height() ) / 2,
							itemTextRect.width(), itemTextRect.height() );
		if ( itemIconRect.height() > itemTextRect.height() ) // icon bigger than text -> center vertically
			itemIconRect = QRect( 0, ( height - itemIconRect.height() ) / 2,
								itemIconRect.width(), itemIconRect.height() );
		else // icon smaller than text -> center with first line
			itemIconRect = QRect( 0, ( fm.height() - itemIconRect.height() ) / 2,
								itemIconRect.width(), itemIconRect.height() );
	}

// Apply margin on Y axis
	int margin=view->marginSize();
	itemIconRect.moveBy(0,margin/2);
	itemTextRect.moveBy(0,margin/2);
	itemRect.setHeight(itemRect.height()+margin);

// Update rects
	if ( itemIconRect != pixmapRect() )
		setPixmapRect( itemIconRect );
	if ( itemTextRect != textRect() )
		setTextRect( itemTextRect );
	if ( itemRect != rect() )
		setItemRect( itemRect );
}


void GVFileThumbnailViewItem::truncateText(const QFontMetrics& fm) {
	static QString dots("...");
	GVFileThumbnailView* view = static_cast<GVFileThumbnailView*>( iconView() );
	Q_ASSERT( view );
	if ( !view ) return;

// If the text fit in the width, don't truncate it
	int width=rect().width()
		- (view->itemTextPos()==QIconView::Bottom ? 0 : view->thumbnailSize() );
	if (fm.boundingRect(text()).width()<=width) {
		mTruncatedText=QString::null;
		return;
	}

// Find the number of letters to keep
	mTruncatedText=text();
	width-=fm.width(dots);
	int len=mTruncatedText.length();
	for(;len>0 && fm.width(mTruncatedText,len)>width;--len);

// Truncate the text
	mTruncatedText.truncate(len);
	mTruncatedText+=dots;
}


void GVFileThumbnailViewItem::paintItem(QPainter *p, const QColorGroup &cg) {
	GVFileThumbnailView *view=static_cast<GVFileThumbnailView*>(iconView());
	Q_ASSERT(view);
	if (!view) return;

	p->save();

// Get the rects
	QRect pRect=pixmapRect(false);
	QRect tRect=textRect(false);
	// Do not draw the focus rect behind the info line if it's empty
	if (mInfoText.isNull()) {
		tRect.rBottom()-=view->fontMetrics().height();
	}

// Draw pixmap
	p->drawPixmap( pRect.x()+1, pRect.y()+1, *pixmap() );

// Draw focus
	if ( isSelected() ) {
		p->setPen( QPen( cg.highlight() ) );
		QRect outerRect=pRect | tRect;
		

		p->drawRect(outerRect);
		if (view->itemTextPos()==QIconView::Bottom) {
			p->fillRect( outerRect.x(),tRect.y(),outerRect.width(),tRect.height(), cg.highlight() );
		} else {
			p->fillRect( tRect.x(),outerRect.y(),tRect.width(),outerRect.height(), cg.highlight() );
		}

		p->setPen( QPen( cg.highlightedText() ) );
	} else {
		if ( view->itemTextBackground() != NoBrush ) {
			p->fillRect( tRect, view->itemTextBackground() );
		}
		p->setPen( cg.text() );
	}

// Draw text
	int align = view->itemTextPos() == QIconView::Bottom ? AlignHCenter : AlignAuto;
	align|=AlignTop;
	if (view->shownFileItem() && view->shownFileItem()->extraData(view)==this) {
		p->setPen(view->shownFileItemColor());
	}
	if (view->wordWrapIconText()) {
		if (!mWordWrap) {
			kdWarning() << "KIconViewItem::paintItem called but wordwrap not ready - calcRect not called, or aborted!" << endl;
			return;
		}
		mWordWrap->drawText( p, tRect.x(), tRect.y(), align );
	} else {
		QString str;
		if (mTruncatedText.isNull()) {
			str=text();
		} else {
			str=mTruncatedText;
		}
		p->drawText(tRect, align, str);
	}

	// Draw info line
	align = view->itemTextPos() == QIconView::Bottom ? AlignHCenter : AlignAuto;
	align|=AlignBottom;
	QFont font=p->font();
	if (font.pixelSize()!=-1) {
		font.setPixelSize(font.pixelSize()-2);
	} else {
		font.setPointSize(font.pointSize()-2);
	}
	p->setFont(font);
	p->drawText(tRect, align, mInfoText);

	p->restore();
}


bool GVFileThumbnailViewItem::acceptDrop(const QMimeSource* source) const {
	return KURLDrag::canDecode(source);
}


void GVFileThumbnailViewItem::dropped(QDropEvent* event, const QValueList<QIconDragItem>&) {
	GVFileThumbnailView *view=static_cast<GVFileThumbnailView*>(iconView());
	emit view->dropped(event,mFileItem);
}

void GVFileThumbnailViewItem::setInfoText(const QString& text) {
	mInfoText=text;
}
