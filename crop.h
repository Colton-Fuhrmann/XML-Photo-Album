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
    ***** crop.h *****

Demo program to illustrate low-level keyboard and mouse event handling in Qt.

Author: John M. Weiss, Ph.D.
Written 111012 for CSC421/521 GUI class, SDSM&T Fall 2011.

Modifications:
*/

#ifndef CROP_H
#define CROP_H

#include <QtGui>
#include <QtDebug>
#include <math.h>

// main window class that overrides virtual functions mousePressEvent() and keyPressEvent()
class Cropper : public QLabel
{
    Q_OBJECT

  public:
    Cropper( QString fileName );
    void change_image( QString path );
    void mousePressEvent( QMouseEvent *event );
    void mouseMoveEvent( QMouseEvent *event );
    void mouseReleaseEvent( QMouseEvent *event );
    void keyPressEvent( QKeyEvent *event );

  signals:
    void crop_release(QRect);
    
  private:
    QRubberBand *rubberBand;
    QPoint origin;
};

#endif
