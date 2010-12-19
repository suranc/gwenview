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
// Self
#include "scrolltool.moc"

// Qt
#include <QApplication>
#include <QMouseEvent>
#include <QScrollBar>

// KDE
#include <kdebug.h>
#include <kstandarddirs.h>

// Local
#include "imageview.h"

namespace Gwenview {

enum State { StateNone, StateZooming, StateDragging };

struct ScrollToolPrivate {
	int mScrollStartX;
	int mScrollStartY;
	State mState;
	QCursor mZoomCursor;
};


ScrollTool::ScrollTool(ImageView* view)
: AbstractImageViewTool(view)
, d(new ScrollToolPrivate) {
	d->mState = StateNone;

	QString path = KStandardDirs::locate("appdata", "cursors/zoom.png");
	QPixmap cursorPixmap = QPixmap(path);
	d->mZoomCursor = QCursor(cursorPixmap);
}


ScrollTool::~ScrollTool() {
	delete d;
}


void ScrollTool::mousePressEvent(QMouseEvent* event) {
	if (event->modifiers() == Qt::ControlModifier) {
		// Ctrl + Left or right button => zoom in or out
		if (event->button() == Qt::LeftButton) {
			emit zoomInRequested(event->pos());
		} else if (event->button() == Qt::RightButton) {
			emit zoomOutRequested(event->pos());
		}
		return;
	}

	if (imageView()->zoomToFit()) {
		return;
	}

	if (event->button() != Qt::LeftButton) {
		return;
	}

	d->mScrollStartY = event->y();
	d->mScrollStartX = event->x();
	d->mState = StateDragging;
	imageView()->viewport()->setCursor(Qt::ClosedHandCursor);
}


void ScrollTool::mouseMoveEvent(QMouseEvent* event) {
	if (d->mState != StateDragging) {
		return;
	}

	QScrollBar* hScrollBar = imageView()->horizontalScrollBar();
	QScrollBar* vScrollBar = imageView()->verticalScrollBar();
	int width = imageView()->width();
	int height = imageView()->height();

	QPoint mousePos = event->pos();

	int deltaX = d->mScrollStartX - mousePos.x();
	int deltaY = d->mScrollStartY - mousePos.y();

	if (mousePos.y() <= 4 && vScrollBar->value() < vScrollBar->maximum() - 10) {
		// wrap mouse from top to bottom
		mousePos.setY(height - 5);
	} else if (mousePos.y() >= height - 4 && vScrollBar->value() > 10) {
		// wrap mouse from bottom to top
		mousePos.setY(5);
	}

	if (mousePos.x() <= 4 && hScrollBar->value() < hScrollBar->maximum() - 10) {
		// wrap mouse from left to right
		mousePos.setX(width - 5);
	} else if (mousePos.x() >= width - 4 && hScrollBar->value() > 10) {
		// wrap mouse from right to left
		mousePos.setX(5);
	}

	if (mousePos != event->pos()) {
		QCursor::setPos(imageView()->mapToGlobal(mousePos));
	}

	d->mScrollStartX = mousePos.x();
	d->mScrollStartY = mousePos.y();
	hScrollBar->setValue(hScrollBar->value() + deltaX);
	vScrollBar->setValue(vScrollBar->value() + deltaY);
}


void ScrollTool::mouseReleaseEvent(QMouseEvent* /*event*/) {
	if (d->mState != StateDragging) {
		return;
	}

	d->mState = StateNone;
	imageView()->viewport()->setCursor(Qt::OpenHandCursor);
}


void ScrollTool::wheelEvent(QWheelEvent* event) {
	if (event->modifiers() & Qt::ControlModifier) {
		// Ctrl + wheel => zoom in or out
		if (event->delta() > 0) {
			emit zoomInRequested(event->pos());
		} else {
			emit zoomOutRequested(event->pos());
		}
		return;
	}

	// Forward events to the scrollbars, like QAbstractScrollArea::wheelEvent()
	// does.
	if (event->orientation() == Qt::Horizontal) {
		QApplication::sendEvent(imageView()->horizontalScrollBar(), event);
	} else {
		QApplication::sendEvent(imageView()->verticalScrollBar(), event);
	}
}


void ScrollTool::keyPressEvent(QKeyEvent* event) {
	if (event->modifiers() == Qt::ControlModifier && d->mState == StateNone) {
		d->mState = StateZooming;
		imageView()->viewport()->setCursor(d->mZoomCursor);
	}
}


void ScrollTool::keyReleaseEvent(QKeyEvent* event) {
	if (!(event->modifiers() & Qt::ControlModifier) && d->mState == StateZooming) {
		d->mState = StateNone;
		imageView()->viewport()->setCursor(Qt::ArrowCursor);
	}
}


void ScrollTool::toolActivated() {
	imageView()->viewport()->setCursor(Qt::OpenHandCursor);
	imageView()->viewport()->installEventFilter(this);
}


void ScrollTool::toolDeactivated() {
	imageView()->viewport()->removeEventFilter(this);
	imageView()->viewport()->setCursor(Qt::ArrowCursor);
}


bool ScrollTool::eventFilter(QObject*, QEvent* event) {
	if (event->type() == QEvent::ContextMenu) {
		// Filter out context menu if Ctrl is down to avoid showing it when
		// zooming out with Ctrl + Right button
		QContextMenuEvent* contextMenuEvent = static_cast<QContextMenuEvent*>(event);
		if (contextMenuEvent->modifiers() == Qt::ControlModifier) {
			return true;
		}
	}

	return false;
}


} // namespace
