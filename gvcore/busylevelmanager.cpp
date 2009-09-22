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

// Qt
#include <qtimer.h>

// KDE
#include <kdebug.h>

// Local
#include "busylevelmanager.moc"
namespace Gwenview {

#undef ENABLE_LOG
#undef LOG
//#define ENABLE_LOG
#ifdef ENABLE_LOG
#define LOG(x) kdDebug() << k_funcinfo << x << endl
#else
#define LOG(x) ;
#endif

BusyLevelManager::BusyLevelManager()
: mCurrentBusyLevel( BUSY_NONE )
{
	connect( &mDelayedBusyLevelTimer, SIGNAL( timeout()),
		this, SLOT( delayedBusyLevelChanged()));
}

BusyLevelManager* BusyLevelManager::instance() {
	static BusyLevelManager manager;
	return &manager;
}

// How the busy level stuff works:
// This system allows suspending less important tasks while more important
// task are active, i.e. no thumbnails are generated when the viewed
// image is being loaded and painted.
// All objects responsible for operations set their busy level
// to the matching value when the operation starts and reset their busy
// level when the operation is done. They all connect to busyLevelChanged()
// signal and suspend their operation if the current busy level is higher
// than the busy level of their operation. If a new operation is started,
// it needs to be immediatelly suspended if the current busy level is higher!
// Note that there can be only one level per object,
// so if one object is responsible for more operations,
// it needs to use helper objects for setBusyLevel().

void BusyLevelManager::setBusyLevel( QObject* obj, BusyLevel level ) {
	LOG("BUSY:" << level << ":" << obj << ":" << obj->className() );
	if( level > BUSY_NONE ) {
		if( mBusyLevels.contains( obj ) && mBusyLevels[ obj ] == level ) return;
		if( !mBusyLevels.contains( obj )) {
			connect( obj, SIGNAL( destroyed( QObject* )), this, SLOT( objectDestroyed( QObject* )));
		}
		mBusyLevels[ obj ] = level;
	} else {
		mBusyLevels.remove( obj );
		disconnect( obj, SIGNAL( destroyed( QObject* )), this, SLOT( objectDestroyed( QObject* )));
	}
	mDelayedBusyLevelTimer.start( 0, true );
}

void BusyLevelManager::objectDestroyed( QObject* obj ) {
	LOG("DESTROYED:" << obj );
	mBusyLevels.remove( obj );
	mDelayedBusyLevelTimer.start( 0, true );
}

void BusyLevelManager::delayedBusyLevelChanged() {
	BusyLevel newLevel = BUSY_NONE;
	for( QMap< QObject*, BusyLevel >::ConstIterator it = mBusyLevels.begin();
		it != mBusyLevels.end();
		++it ) {
		newLevel = QMAX( newLevel, *it );
	}

	if( newLevel != mCurrentBusyLevel ) {
		LOG("CHANGED BUSY:" << newLevel);
		mCurrentBusyLevel = newLevel;
		emit busyLevelChanged( newLevel );
	}
}

BusyLevel BusyLevelManager::busyLevel() const {
	return mCurrentBusyLevel;
}


} // namespace