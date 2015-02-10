///////////////////////////////////////////////////////////////////////////////
//Authors: John M. Weiss, Ph. D.
//Date: October 8, 2014
//Course: CSC421
//Instructor: Dr. Weiss
//
//Description: This file is taken from Dr. Weiss' course website. His original
//comment block is preserved directly below this one. A few additions were
//made to fit this application's needs, namely the change_image() function
//to allow us to change what image the Cropper window was showing. We also
//added a signal emission when the user released the mouseclick.
///////////////////////////////////////////////////////////////////////////////

/*
    ***** crop.cpp *****

Demo program to illustrate low-level keyboard and mouse event handling in Qt.

Author: John M. Weiss, Ph.D.
Written 111012 for CSC421/521 GUI class, SDSM&T Fall 2011.

Modifications:
*/

#include <cstdlib>
#include <QtGui>
#include "crop.h"
#include "photoalbum.h"

Cropper::Cropper( QString fileName )
{
    QImage image( fileName );
    
    setPixmap( QPixmap::fromImage( image ) );
    setWindowTitle( tr( "Image Cropper" ) );

    rubberBand = new QRubberBand( QRubberBand::Rectangle, this );
}

//This changes the image displayed in the Cropper window
void Cropper::change_image(QString path)
{
    QImage image( path );
    resize(image.width(), image.height()); //Resize to image dimensions

    setPixmap( QPixmap::fromImage( image ) );
}

// print button and location of mouse clicks
void Cropper::mousePressEvent( QMouseEvent *event )
{
    origin = event->pos();
    rubberBand->setGeometry( QRect( origin, QSize() ) );
    rubberBand->show();
}

void Cropper::mouseMoveEvent( QMouseEvent *event )
{
    rubberBand->setGeometry( QRect( origin, event->pos() ).normalized() );
}

void Cropper::mouseReleaseEvent( QMouseEvent *event )
{
    QRect crop_area = QRect( origin, event->pos() ).normalized();

    rubberBand->hide();
    setPixmap( pixmap()->copy(crop_area));
    setGeometry(crop_area);

    //Send signal that photo has been cropped and pass new size
    emit crop_release(crop_area);
}

// print keycode of key presses
void Cropper::keyPressEvent( QKeyEvent *event )
{
    if ( event->key() == Qt::Key_Escape )
        exit( 0 );
    qDebug() << "keypress event " << event->key();
}
