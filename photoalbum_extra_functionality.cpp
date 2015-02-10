///////////////////////////////////////////////////////////////////////////////
//Authors: Colton Fuhrmann, Kevin Hilt
//Date: October 8, 2014
//Course: CSC421
//Instructor: Dr. Weiss
//
//Description: The non-slot member functions of the PhotoAlbum class
//that break out the work from these slots into more manageable and reuseable
//blocks.
//
//Some snippets of code were used from Dr. Weiss' course website and the QT4
//examples directory. The authors are credited inline below whenever this
//is true.
///////////////////////////////////////////////////////////////////////////////

#include "photoalbum.h"
#include "ui_photoalbum.h"
#include "crop.h"

//Custom slot that is called when the user finishes cropping an image
//Receives QRect crop_area as an argument, which is the portion of
//that will overwrite the original image if the user so chooses
void PhotoAlbum::confirm_crop(QRect crop_area)
{
    //Set preview image to be cropped image
    preview_image = current_image.copy(crop_area);

    //Set the message in confirm_save and show it
    QString message = "Do you want the cropped image to overwrite the original image at "
                        + current_photo.firstChild().toElement().text();
    ui->confirm_label->setText(message);
    ui->confirm_save->show();
}

//Called after a user opens an xml album
//This function processes the file as an xml tree and stores it into a
//member QDomDocument.
void PhotoAlbum::process_xml(QIODevice *device, QString filename)
{
    //Parse the xml file and store it into nodes of a QDomDocument
    album_xml.setContent(device, true, NULL, NULL, NULL);

    //Save a reference to the root
    QDomElement album = album_xml.documentElement();
    //Set current_photo to the first photo in the album
    current_photo = album.firstChildElement("photo");

    //If the ablum has no photo tags
    if(current_photo.isNull())
    {
        //Enable only menu actions for an album with no photos
        album_no_photos();
        return;
    }

    //Enable all the menu actions for an open album with at least one photo
    enable_all_menu_actions();

    //Show all the UI labels that will display current_photo's information
    QLabel* labels[4] = {ui->image, ui->date, ui->location, ui->description};
    for(int i = 0; i < 4; i++)
    {
        labels[i]->show();
    }

    //Display confirmation status
    QString message = "Loaded album " + filename;
    ui->statusBar->showMessage(message, 3000);

    display_photo(); //display_photo() populates the UI labels
}

//This function saves the QDomDocument album_xml to an xml file
void PhotoAlbum::save_xml(QIODevice *device)
{
    const int IndentSize = 4;

    QTextStream out(device);
    album_xml.save(out, IndentSize); //Save album_xml to the specified file
}

//This function takes the information stored in the current_photo node
//and displays it in the approriate UI labels at the approriate window size.
void PhotoAlbum::display_photo()
{
    QLabel* labels[4] = {ui->image, ui->date, ui->location, ui->description};

    //child will iterate through the tags in current_photo
    QDomNode child = current_photo.firstChild();
    QDomElement photo_information = child.toElement();

    //Create a pixmap from the image at the path in the <file> tag
    QImage image_pixmap(photo_information.text());
    image_pixmap.convertToFormat(QImage::Format_RGB888);
    current_image = image_pixmap;
    QPixmap p = (QPixmap::fromImage(image_pixmap));

    //Hide the info labels and return if no image is present
    if(current_image.isNull())
    {
        for(int i = 0; i < 4; i++)
        {
            labels[i]->hide();
        }
        return;
    }

    labels[0]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    labels[0]->setScaledContents(true);

    //If the application window height is greater than the height of the iamge,
    //display the image at full size
    if(this->height() - 150 > current_image.height())
    {
        labels[0]->setPixmap(p.scaled(this->width(),current_image.height(),Qt::KeepAspectRatio));
    }
    else //Scale the image to fit the application window
    {
        labels[0]->setPixmap(p.scaled(this->width(),this->height()-155,Qt::KeepAspectRatio));
    }

    labels[0]->show();
    labels[0]->adjustSize();



    //Loop through the tags in current_photo and populate that label in the UI
    int i = 1;
    for(child = child.nextSibling(); !child.isNull(); child = child.nextSibling())
    {
        QDomElement photo_information = child.toElement();
        labels[i]->setText(photo_information.text());
        labels[i]->adjustSize();
        labels[i]->show();
        i++;
    }

    //Display date, location, and description right below the image
    ui->image_info_widget->move(0, labels[0]->height() + 10 );
}

//Disables menu actions that require an open album
void PhotoAlbum::album_not_open()
{
    ui->actionSave->setEnabled(false);
    ui->actionSave_As->setEnabled(false);
    ui->actionAdd_Photo->setEnabled(false);
    ui->Delete_Photo->setEnabled(false);
    ui->actionEdit_Description->setEnabled(false);
    ui->actionPage_Forward->setEnabled(false);
    ui->actionPage_Backward->setEnabled(false);
    ui->actionMove_Forward->setEnabled(false);
    ui->actionMove_Backward->setEnabled(false);
    ui->actionCrop->setEnabled(false);
    ui->actionResize->setEnabled(false);
    ui->actionRotate->setEnabled(false);
    ui->actionBrightness->setEnabled(false);
    ui->actionContrast->setEnabled(false);
    ui->actionNegate->setEnabled(false);
    ui->actionSmooth->setEnabled(false);
    ui->actionSharpen->setEnabled(false);
}

//Disables menu actions for an open album that has no photos
void PhotoAlbum::album_no_photos()
{
    ui->actionSave->setEnabled(true);
    ui->actionSave_As->setEnabled(true);
    ui->actionAdd_Photo->setEnabled(true);
    ui->Delete_Photo->setEnabled(false);
    ui->actionEdit_Description->setEnabled(false);
    ui->actionPage_Forward->setEnabled(false);
    ui->actionPage_Backward->setEnabled(false);
    ui->actionMove_Forward->setEnabled(false);
    ui->actionMove_Backward->setEnabled(false);
    ui->actionCrop->setEnabled(false);
    ui->actionResize->setEnabled(false);
    ui->actionRotate->setEnabled(false);
    ui->actionBrightness->setEnabled(false);
    ui->actionContrast->setEnabled(false);
    ui->actionNegate->setEnabled(false);
    ui->actionSmooth->setEnabled(false);
    ui->actionSharpen->setEnabled(false);
}

//Enables all the menu actions for an open album with at least one photo
void PhotoAlbum::enable_all_menu_actions()
{
    ui->actionSave->setEnabled(true);
    ui->actionSave_As->setEnabled(true);
    ui->actionAdd_Photo->setEnabled(true);
    ui->Delete_Photo->setEnabled(true);
    ui->actionEdit_Description->setEnabled(true);
    ui->actionPage_Forward->setEnabled(true);
    ui->actionPage_Backward->setEnabled(true);
    ui->actionMove_Forward->setEnabled(true);
    ui->actionMove_Backward->setEnabled(true);
    ui->actionCrop->setEnabled(true);
    ui->actionResize->setEnabled(true);
    ui->actionRotate->setEnabled(true);
    ui->actionBrightness->setEnabled(true);
    ui->actionContrast->setEnabled(true);
    ui->actionNegate->setEnabled(true);
    ui->actionSmooth->setEnabled(true);
    ui->actionSharpen->setEnabled(true);
}

// This function receives its input (int value = 1-500), from the balance
// slider and turns it into a percent. The image width and height is
// then resized according to this percentage.
void PhotoAlbum::resize_image(int value)
{
    double percent_resize = double(double(value)/100);  // get the input value as percent
    int h = current_image.height() * percent_resize;    // scale height
    int w = current_image.width() * percent_resize;     // scale width

    // set the preview_image as the current_image scaled
    preview_image = current_image.scaled(w, h, Qt::IgnoreAspectRatio, Qt::FastTransformation);

}

// The current image is simply rotated by the given input value in degrees
// from -180 to 180 using a QTransform.
void PhotoAlbum::rotate(int value)
{
      // create a Qtransform for rotation
      QTransform *t = new QTransform;
      // rotate the transform by (value) degrees
      t->rotate(value);
      // set preview_image to the current_image rotated by (int value) degrees
      preview_image = current_image.transformed(*t);
}

// This code displays the preview_image in a Qlabel in the balance widget.
// This function provides realtime feedback on how the user is changing
// the image via slider/spinbox
void PhotoAlbum::display_preview_image()
{
    QPixmap preview = (QPixmap::fromImage(preview_image));      // convert preview_image to a pixmap

    // set the label so the pixmap expands to label size and is scaled
    ui->balance_preview->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->balance_preview->setScaledContents(true);
    ui->balance_widget->resize(preview_image.width() / 2, preview_image.height() / 2);
    ui->balance_preview->setPixmap(preview.scaled(ui->balance_widget->width() - 100, ui->balance_widget->height() - 100, Qt::KeepAspectRatio));
    ui->balance_preview->adjustSize();
}

// This is a brighten function for QImages. Most of the code is Dr. Weiss'
// from examples/ip/bright.cpp
// This function receives its user entered input value from the slider/spinbox
// in the balance_widget
void PhotoAlbum::brighten(int value)
{
    // red, green, and blue pixels
    int r, g, b;

    //Dr. Weiss' code from examples/ip/bright.cpp
    // Using the current_image's width and height and pixel values, loop
    // through every pixel and increment by user entered value.
    for ( int x = 0; x < current_image.width(); x++ )
    {
        for ( int y = 0; y < current_image.height(); y++ )
        {
            QRgb p = current_image.pixel( x, y );
            r = qRed( p ) + value;
            if ( r > 255 ) r = 255;
            else if ( r < 0 ) r = 0;
            g = qGreen( p ) + value;
            if ( g > 255 ) g = 255;
            else if ( g < 0 ) g = 0;
            b = qBlue( p ) + value;
            if ( b > 255 ) b = 255;
            else if ( b < 0 ) b = 0;
            // set the new brightened pixel in the preview_image
            preview_image.setPixel( x, y, qRgb( r, g, b ) );
        }
    }
}

// This is a contrast function for QImages. Most of the code is Dr. Weiss'
// from examples/ip/contrast.cpp
// This function receives its user entered input value from the slider/spinbox
// in the balance_widget
void PhotoAlbum::contrast(int value)
{
    // red, green, and blue pixel values
    int r, g, b;

    //Dr. Weiss' code from examples/ip/contrast.cpp
    // Using the current_image's width and height and pixel values, loop
    // through every pixel and increment by user entered value.
    for ( int x = 0; x < preview_image.width(); x++ )
    {
        for ( int y = 0; y < preview_image.height(); y++ )
        {
            QRgb p = current_image.pixel( x, y );
            r = ( qRed( p ) - value) * 2;
            if ( r < 0 ) r = 0;
            else if ( r > 255 ) r = 255;
            g = ( qGreen( p ) - value ) * 2;
            if ( g < 0 ) g = 0;
            else if ( g > 255 ) g = 255;
            b = ( qBlue( p ) - value ) * 2;
            if ( b < 0 ) b = 0;
            else if ( b > 255 ) b = 255;
            // set the new contrasted pixel in the preview_image
            preview_image.setPixel( x, y, qRgb( r, g, b ) );
        }
    }
}


// This is a smooth function for QImages. Most of the code is Dr. Weiss'
// from examples/ip/smooth.cpp
// This function receives its user entered input value from the slider/spinbox
// in the balance_widget. If the user selects x, the image will be smoothed
// x times.
void PhotoAlbum::smooth(int value)
{
  // red, green, and blue pixel values
  int r, g, b;

  preview_image = current_image;

  int i;
  for(i = 0; i < value; i++)
  {
  QImage smooth_image(preview_image);

  for ( int x = 1; x < current_image.width() - 1; x++ )
      {
          for ( int y = 1; y < current_image.height() - 1; y++ )
          {
              r = 0, g = 0, b = 0;
              for ( int m = -1; m <= 1; m++ )
              {
                  for ( int n = -1; n <= 1; n++ )
                  {
                      QRgb p = smooth_image.pixel( x + m, y + n );
                      r += qRed( p );
                      g += qGreen( p );
                      b += qBlue( p );
                  }
              }
              preview_image.setPixel( x, y, qRgb( r / 9, g / 9, b / 9 ) );
          }
      }
  }
}


// This is a sharpen function for QImages. Most of the code is Dr. Weiss'
// from examples/ip/sharpen.cpp
// This function receives its user entered input value from the slider/spinbox
// in the balance_widget.
void PhotoAlbum::sharpen(int value)
{
  // red, green, and blue pixel values
  int r, g, b;

  preview_image = current_image;

  //loop through sharpening the image for whatever user picks on slider/spinbox
  int i;
  for(i = 0; i < value; i++)
  {
  QImage smooth_image(preview_image);

  for ( int x = 1; x < current_image.width() - 1; x++ )
  {
      for ( int y = 1; y < current_image.height() - 1; y++ )
      {
          QRgb p = smooth_image.pixel( x, y );
          r = 5 * qRed( p );
          g = 5 * qGreen( p );
          b = 5 * qBlue( p );
          p = smooth_image.pixel( x - 1, y );
          r -= qRed( p );
          g -= qGreen( p );
          b -= qBlue( p );
          p = smooth_image.pixel( x + 1, y );
          r -= qRed( p );
          g -= qGreen( p );
          b -= qBlue( p );
          p = smooth_image.pixel( x, y - 1 );
          r -= qRed( p );
          g -= qGreen( p );
          b -= qBlue( p );
          p = smooth_image.pixel( x, y + 1 );
          r -= qRed( p );
          g -= qGreen( p );
          b -= qBlue( p );

          if ( r < 0 ) r = 0; else if ( r > 255 ) r = 255;
          if ( g < 0 ) g = 0; else if ( g > 255 ) g = 255;
          if ( b < 0 ) b = 0; else if ( b > 255 ) b = 255;

          preview_image.setPixel( x, y, qRgb( r, g, b ) );
      }
  }

  }

}
