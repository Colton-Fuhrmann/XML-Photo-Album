///////////////////////////////////////////////////////////////////////////////
//Authors: Colton Fuhrmann, Kevin Hilt
//Date: October 8, 2014
//Course: CSC421
//Instructor: Dr. Weiss
//
//Description: The class definition for the PhotoAlbum class, which is the
//main application window for this program. Most of the functions are
//slots to handle user-selected actions sent from UI signals, but there are
//some helper functions that break out the work from these slots into
//more manageable and reuseable blocks.
///////////////////////////////////////////////////////////////////////////////

#ifndef PHOTOALBUM_H
#define PHOTOALBUM_H

#include <QMainWindow>
#include <QtGui>
#include <QDomDocument>
#include <QDebug>
#include "crop.h"

namespace Ui {
class PhotoAlbum;
}

class PhotoAlbum : public QMainWindow
{
    Q_OBJECT

public:
    explicit PhotoAlbum(QWidget *parent = 0);

    //Flags to indicate which image operation was selected
    bool is_brighten = false;
    bool is_contrast = false;
    bool is_resize = false;
    bool is_rotate = false;
    bool is_smooth = false;
    bool is_sharpen = false;

    void process_xml(QIODevice *device, QString filename);
    QString album_filename; //Path to the location of the album's xml file

    //Instance of Dr. Weiss' Cropper class from crop.h
    Cropper* crop_window = new Cropper(NULL);

    ~PhotoAlbum();

public slots:
    void confirm_crop(QRect);

private slots:
    void resizeEvent( QResizeEvent *event);

    void on_actionNew_triggered();

    void on_actionSave_triggered();

    void on_actionAbout_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_As_triggered();

    void on_actionPage_Forward_triggered();

    void on_actionPage_Backward_triggered();

    void on_actionQuit_triggered();

    void on_actionClose_triggered();

    void on_actionEdit_Description_triggered();

    void on_edit_buttons_accepted();

    void on_edit_buttons_rejected();

    void on_actionMove_Forward_triggered();

    void on_actionCrop_triggered();

    void on_actionResize_triggered();

    void on_actionRotate_triggered();

    void on_actionContrast_triggered();

    void on_actionBrightness_triggered();

    void on_balance_buttons_accepted();

    void on_balance_buttons_rejected();

    void on_balance_slider_valueChanged(int value);

    void on_confirm_buttons_rejected();

    void on_confirm_buttons_accepted();

    void on_actionAdd_Photo_triggered();

    void on_Delete_Photo_triggered();

    void on_actionMove_Backward_triggered();

    void on_actionUsage_triggered();

    void on_actionNegate_triggered();

    void on_actionSmooth_triggered();

    void on_actionSharpen_triggered();

private:
    Ui::PhotoAlbum *ui;
    QDomDocument album_xml; //Holds the album xml
    QDomElement current_photo; //Node for the currently displayed photo
    QImage current_image; //QImage of the current_photo
    QImage preview_image; //QImage of current_photo + pending image processing

    //Helper, non-slot functions
    void contrast(int value);

    void album_not_open();

    void enable_all_menu_actions();

    void album_no_photos();

    void display_preview_image();

    void brighten(int value);

    void save_xml(QIODevice *device);

    void display_photo();

    void resize_image(int value);

    void rotate(int value);

    void smooth(int value);

    void sharpen(int value);
};

#endif // PHOTOALBUM_H
