///////////////////////////////////////////////////////////////////////////////
//Authors: Colton Fuhrmann, Kevin Hilt
//Date: October 8, 2014
//Course: CSC421
//Instructor: Dr. Weiss
//
//Description: The slot member functions of the PhotoAlbum class, which is the
//main application window for this program. These functions are the
//slots to handle user-selected actions sent from UI signals.
//
//Some snippets of code were used from Dr. Weiss' course website and the QT4
//examples directory. The authors are credited inline below whenever this
//is true.
///////////////////////////////////////////////////////////////////////////////

#include "photoalbum.h"
#include "ui_photoalbum.h"
#include "crop.h"

//Constructor - initial set up of the application window
PhotoAlbum::PhotoAlbum(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PhotoAlbum)
{
    ui->setupUi(this);

    //Configure image, date, location, and description labels and hide
    //them from view, since there will be no information to display initially.
    ui->image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->image->setScaledContents(true);
    ui->image->hide();
    ui->date->hide();
    ui->location->hide();
    ui->description->hide();

    //Hide the popup widgets that will be used for several purposes later
    ui->edit_description->setParent(NULL);
    ui->edit_description->hide();
    ui->balance_widget->setParent(NULL);
    ui->balance_widget->hide();
    ui->confirm_save->setParent(NULL);
    ui->confirm_save->hide();

    //Disable menu actions that require an open album
    album_not_open();
}

//Deconstructor - deletes the Ui
PhotoAlbum::~PhotoAlbum()
{
    delete ui;
}

//Called when the main application window is resized
//It simply calls display_photo(), which handles all UI scaling
void PhotoAlbum::resizeEvent(QResizeEvent *)
{

    display_photo(); //display_photo() with new window size
}

//This function sets the stored xml tree to a blank album by removing all the photo tags.
//It then closes the previous album and prompts the user to save this new album.
void PhotoAlbum::on_actionNew_triggered()
{
    QDomDocument blank_album; //Create blank xml structure
    QDomElement album_tag = blank_album.createElement("album");
    blank_album.appendChild(album_tag); //Append the album tag
    album_xml = blank_album; //Replace album_xml

    on_actionClose_triggered(); //Close previous album
    on_actionSave_As_triggered(); //Prompt the user to save the new album

    //Enable only menu actions for an album with no photos
    album_no_photos();

    //Display confirmation status
    QString message = "Created album " + album_filename;
    ui->statusBar->showMessage(message, 3000);
}

//Called when the user selects Save from the menu
//This function saves the album to the path already stored in album_filename
void PhotoAlbum::on_actionSave_triggered()
{
    //Create a file with the selected path
    QFile file(album_filename);
    if(!file.open(QFile::WriteOnly))
    {
        qDebug() << "Error opening file.\n";
    }

    save_xml(&file); //Save the file to the selected location

    //Display confirmation status
    QString message = "Saved album to " + album_filename;
    ui->statusBar->showMessage(message, 3000);
}

//Called when the user selects Save As from the menu
//This function saves the album to the path specified by the user
void PhotoAlbum::on_actionSave_As_triggered()
{
    //Code from qt4/examples/xml/dombookmarks
    QString fileName =
                QFileDialog::getSaveFileName(this, tr("Save XML File"),
                                             QDir::currentPath(),
                                             tr("XML Files (*.xml)"));
        if (fileName.isEmpty())
            return;

        //Append file extension if necessary
        if(!fileName.endsWith(".xml"))
        {
            fileName.append(".xml");
        }

        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("SAX Bookmarks"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString()));
            return;
        }

        save_xml(&file);
        album_filename = fileName; //Update album to newly saved file

        //Display confirmation status
        QString message = "Saved album to " + album_filename;
        ui->statusBar->showMessage(message, 3000);
}

//Called when the user selects About from the menu
void PhotoAlbum::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About PhotoAlbum"),
             tr("This application allows for photo editing and organization in an "
                "xml album. See Help>Usage for more information."));
}

//Called when the user selects Open from the menu
//This function opens the selected xml file as an album
void PhotoAlbum::on_actionOpen_triggered()
{
    //Code from qt4/examples/xml/dombookmarks
    QString filename =
            QFileDialog::getOpenFileName(this, tr("Open XML File"),
                                         QDir::currentPath(),
                                         tr("XML Files (*.xml)"));\

    album_filename = filename; //Save filename in album_filename for future reference
    if (filename.isEmpty())
        return;

    QFile file(filename); //Create a file with the selected path
    process_xml(&file, filename); //Extracts the xml into a QDomDocument object

    //Display confirmation status
    QString message = "Opened album " + album_filename;
    ui->statusBar->showMessage(message, 3000);
}

//Called when the user selects Page Forward
void PhotoAlbum::on_actionPage_Forward_triggered()
{
    //If not already on the last photo
    if(!current_photo.nextSiblingElement("photo").isNull())
    {
        //Increment current_photo to the next photo and redisplay the UI
        current_photo = current_photo.nextSiblingElement("photo");
        display_photo();

        //Display confirmation status
        QString message = "Showing image "
                           + current_photo.firstChild().toElement().text();
        ui->statusBar->showMessage(message, 3000);
    }
}

//Called when the user selects Page Backward
void PhotoAlbum::on_actionPage_Backward_triggered()
{
    //If not already on the first photo
    if(!current_photo.previousSiblingElement("photo").isNull())
    {
        //Decrement current_photo to the previous photo and redisplay the UI
        current_photo = current_photo.previousSiblingElement("photo");
        display_photo();

        //Display confirmation status
        QString message = "Showing image " + current_photo.firstChild().toElement().text();
        ui->statusBar->showMessage(message, 3000);
    }
}

//Called when the user selects Quit from the menu
void PhotoAlbum::on_actionQuit_triggered()
{
    QApplication::quit(); //Kill the application
}

//Called when the user selects Close from the menu
void PhotoAlbum::on_actionClose_triggered()
{
    //Hide the information labels in the UI
    QLabel* labels[4] = {ui->image, ui->date, ui->location, ui->description};
    for(int i = 0; i < 4; i++)
    {
        labels[i]->hide();
    }

    //Disable menu actions that require an open album
    album_not_open();

    //Display confirmation status
    QString message = "Closed album " + album_filename;
    ui->statusBar->showMessage(message, 3000);
}

//Called the user selectes edit Description from the menu
void PhotoAlbum::on_actionEdit_Description_triggered()
{
    QLineEdit* inputs[3]= {ui->date_input, ui->location_input, ui->description_input};
    QDomNode child = current_photo.firstChild();

    //Loop through the tags below <file> and populate the interface
    int i = 0;
    for(child = child.nextSibling(); !child.isNull(); child = child.nextSibling())
    {
        QDomElement photo_information = child.toElement();
        inputs[i]->setText(photo_information.text());
        i++;
    }

    //Show the window on top of the main application window
    ui->edit_description->setWindowFlags(Qt::WindowStaysOnTopHint);
    ui->edit_description->show();
}

//Called when the user selects OK from edit_buttons
void PhotoAlbum::on_edit_buttons_accepted()
{
    QLineEdit* inputs[3]= {ui->date_input, ui->location_input, ui->description_input};
    QLabel* labels[3] = {ui->date, ui->location, ui->description};
    QDomNode child = current_photo.firstChild();
    QString tags[3] = {"date", "location", "description"};

    //Put the input from the fields in edit_description in current_photo
    int i = 0;
    for(child = child.nextSibling(); !child.isNull(); child = child.nextSibling())
    {
        //Create a node for the tag
        QDomElement new_node = album_xml.createElement(tags[i]);
        //Create a text node for the information
        QDomText node_text = album_xml.createTextNode(inputs[i]->text());
        new_node.appendChild(node_text); //Put the text in the new node
        //Replace the tag in current_photo
        current_photo.replaceChild(new_node, child);
        child = new_node; //Reset child, since the old child was overwritten

        //Update the UI labels with the new information
        labels[i]->setText(inputs[i]->text());
        labels[i]->adjustSize();
        i++;
    }

    ui->edit_description->hide();

    //Display confirmation status
    ui->statusBar->showMessage("Photo information updated", 3000);
}

//Called when the user selects Cancel from edit_buttons
void PhotoAlbum::on_edit_buttons_rejected()
{
    //Close the window without doing anything
    ui->edit_description->hide();
}

// This function moves an image ahead in the album. A copy of the
// image is created and then appended after the next image. The
// original image is then deleted.
void PhotoAlbum::on_actionMove_Forward_triggered()
{
    //If this isn't already the last photo
    if(!current_photo.nextSiblingElement("photo").isNull())
    {
        //Create a new photo tag
        QDomElement new_photo = album_xml.createElement("photo");
        QString tags[4]= {"file", "date", "location", "description"};

        int i = 0;
        //Fill new photo nodes with information from current photo nodes
        for(QDomNode child = current_photo.firstChild(); !child.isNull(); child = child.nextSibling())
        {
            QDomNode new_node = album_xml.createElement(tags[i]);
            QDomText node_text = album_xml.createTextNode(child.toElement().text());
            new_node.appendChild(node_text); //Append information in tag
            new_photo.appendChild(new_node); //Append tag to new photo node
            i++;
        }

        QDomNode previous_photo = current_photo; //Previous = photo to be deleted
        current_photo = current_photo.nextSiblingElement("photo"); //Go one photo ahead

        album_xml.documentElement().insertAfter(new_photo, current_photo); //Insert new after one photo ahead
        album_xml.documentElement().removeChild(previous_photo); //Remove the original photo

        //Increment current_photo to be new photo and display confirmation status
        current_photo = current_photo.nextSiblingElement("photo");
        QString message = "Moved picture forward in the album 1 spot.";
        ui->statusBar->showMessage(message, 3000);

        display_photo();

    }
}

// This function moves an image backwards in the album. A copy of the
// image is created and then appended before the previous image. The
// original image is then deleted.
void PhotoAlbum::on_actionMove_Backward_triggered()
{
    //If this isn't already the first photo
    if(!current_photo.previousSiblingElement("photo").isNull())
    {
        //Create a new photo tag
        QDomElement new_photo = album_xml.createElement("photo");
        QString tags[4]= {"file", "date", "location", "description"};

        int i = 0;
        //Fill new photo nodes with information from current photo nodes
        for(QDomNode child = current_photo.firstChild(); !child.isNull(); child = child.nextSibling())
        {
            QDomNode new_node = album_xml.createElement(tags[i]);
            QDomText node_text = album_xml.createTextNode(child.toElement().text());
            new_node.appendChild(node_text); //Append information in tag
            new_photo.appendChild(new_node); //Append tag to new photo node
            i++;
        }

        QDomNode previous_photo = current_photo; //Previous = photo to be deleted
        current_photo = current_photo.previousSiblingElement("photo"); //Go one photo back

        album_xml.documentElement().insertBefore(new_photo, current_photo); //Insert new before one photo back
        album_xml.documentElement().removeChild(previous_photo); //Remove the original photo

        //Increment current_photo to be new photo and display confirmation status
        current_photo = current_photo.previousSiblingElement("photo");
        QString message = "Moved picture backward in the album 1 spot.";
        ui->statusBar->showMessage(message, 3000);

        display_photo();
    }
}

// This function pops up the crop window and fills it with the current image
// that is being viewed.
void PhotoAlbum::on_actionCrop_triggered()
{
    //Connect the crop_release signal from the Cropper to the confirm_crop slot in PhotoAlbum
    QObject::connect(crop_window, SIGNAL(crop_release(QRect)), this, SLOT(confirm_crop(QRect)));

    //Get path to current image
    QString image_path = current_photo.firstChild().toElement().text();
    crop_window->change_image(image_path); //Change image in crop window
    crop_window->show();

}

// If the resize option from the photo editor is selected, this function is
// called. It pops up the balance_widget which allows the user to input
// %resize from a slider or spinbox.
void PhotoAlbum::on_actionResize_triggered()
{
    ui->balance_widget->show();             // pop up slider,scrollbar, and preview image
    is_resize = true;                       // set the is_resize flag so resize_image() will be called later

    // sets widget label and appropriate default value and ranges for slider and spinbox
    ui->balance_label->setText("Resize %:");
    ui->balance_slider->setValue(100);
    ui->balance_spinbox->setValue(100);
    ui->balance_slider->setRange(1, 500);
    ui->balance_spinbox->setRange(1, 500);

    preview_image = current_image.copy();   // copy the current_image into preview_image
    display_preview_image();                // call function to display preview_image which will call resize_image()
}

// If the rotate option from the photo editor is selected, this function is
// called. It pops up the balance_widget which allows the user to input
// degrees rotated from a slider or spinbox.
void PhotoAlbum::on_actionRotate_triggered()
{
    ui->balance_widget->show();             // pop up slider,scrollbar, and preview image
    is_rotate = true;                       // set the is_rotate flag so rotate() will be called later

     // sets widget label and appropriate default value and ranges for slider and spinbox
    ui->balance_label->setText("Rotate");
    ui->balance_slider->setValue(0);
    ui->balance_spinbox->setValue(0);
    ui->balance_slider->setRange(-180, 180);
    ui->balance_spinbox->setRange(-180, 180);

    preview_image = current_image.copy();    // copy the current_image into preview_image
    display_preview_image();                 // call function to display preview_image which will call rotate_image()
}

// If the contrast option from the photo editor is selected, this function is
// called. It pops up the balance_widget which allows the user to input
// contrast value from a slider or spinbox.
void PhotoAlbum::on_actionContrast_triggered()
{
    ui->balance_widget->show();             // pop up slider,scrollbar, and preview image
    is_contrast = true;                    // set the is_contrast flag so contrast() will be called later

    // sets widget label and appropriate default value and ranges for slider and spinbox
    ui->balance_label->setText("Contrast");
    ui->balance_slider->setValue(0);
    ui->balance_spinbox->setValue(0);
    ui->balance_slider->setRange(-127, 127);
    ui->balance_spinbox->setRange(-127, 127);

    preview_image = current_image.copy();      // copy the current_image into preview_image
    display_preview_image();                   // call function to display preview_image which will call contrast_image()
}


// If the contrast option from the photo editor is selected, this function is
// called. It pops up the balance_widget which allows the user to input
// brighten value from a slider or spinbox.
void PhotoAlbum::on_actionBrightness_triggered()
{
    ui->balance_widget->show();             // pop up slider,scrollbar, and preview image
    is_brighten = true;                     // set the is_brighten flag so brighten() will be called later

    ui->balance_label->setText("Brightness");
    ui->balance_slider->setValue(0);
    ui->balance_spinbox->setValue(0);
    ui->balance_slider->setRange(-255, 255);
    ui->balance_spinbox->setRange(-255, 255);

    preview_image = current_image.copy();      // copy the current_image into preview_image
    display_preview_image();                   // call function to display preview_image which will call brighten_image()
}

// If the user hits accept on any image processing done in the balance_buttons widget,
// then pop up a dialog telling the user that changes are permanent. If the user accepts or declines
// then all the image processing flags are set back to false.
void PhotoAlbum::on_balance_buttons_accepted()
{
    //Show dialog asking the user to confirm saving back to file
    QString message = "Are you sure you want to overwrite the original image at " + current_photo.firstChild().toElement().text();
    ui->confirm_label->setText(message);
    ui->confirm_save->show();
    ui->confirm_save->adjustSize();

    // set image processing flags to false
    is_brighten = false;
    is_contrast = false;
    is_resize = false;
    is_rotate = false;
    is_smooth = false;
    is_sharpen = false;

}

// when the user exits or cancels the balance_widget, the widget is hidden and
// the image processing flags set to false
void PhotoAlbum::on_balance_buttons_rejected()
{
    ui->balance_widget->hide();     // hide the balance_widget

    // set image processing flags to false
    is_brighten = false;
    is_contrast = false;
    is_resize = false;
    is_rotate = false;
    is_smooth = false;
    is_sharpen = false;
}

// This function is called whenever the spinbox or slider is changed in the
// balance widget (spinbox and slider are connected). Depending on the menu
// option selected by the user, a different image processing flag will be set,
// and the associated image processing function is then called with the
// slider/spinbox value as its input.
void PhotoAlbum::on_balance_slider_valueChanged(int value)
{
    // check image processing flag and call the appropriate function
    if(is_brighten)
    {
        brighten(value);

    }
    else if(is_contrast)
    {
        contrast(value);

    }
    else if(is_rotate)
    {
        rotate(value);
    }

    else if(is_resize)
    {
        resize_image(value);
    }
    else if(is_smooth)
    {
        smooth(value);
    }
    else if(is_sharpen)
    {
        sharpen(value);
    }

    // update the preview_image since its been changed by one of the above functions
    display_preview_image();
}

// This function is associated with the confirm_save dialog, and if the user
// rejects to save the image, the dialog, crop_window, and balance_widget are
// all hidden
void PhotoAlbum::on_confirm_buttons_rejected()
{
    // hide the following windows if user rejects to change image
    ui->confirm_save->hide();
    crop_window->hide();
    ui->balance_widget->hide();
}

// When the user accepts at the confirm_save dialog, the preview image is saved
// over the current image in the original file. The save, balance_widget, and
// crop window are then hidden.
void PhotoAlbum::on_confirm_buttons_accepted()
{
    //Save processed image to overwrite original file
    preview_image.save(current_photo.firstChild().toElement().text());

    //Hide balance windows and redisplay image
    ui->confirm_save->hide();
    ui->balance_widget->hide();
    crop_window->hide();
    display_photo();      // redisplay to update view of current image in album


    QString message = "Processed image saved to " + current_photo.firstChild().toElement().text();
    ui->statusBar->showMessage(message, 3000);
}

// This function is called when the user chooses to delete a photo form
// the album. The function will return if no image is present. If the
// photo isn't last in album, set next as current and delete previous.
// If last in album, set previous as current and delete last. If
// image is only image in album, dete and set image to null.
void PhotoAlbum::on_Delete_Photo_triggered()
{
    QDomNode to_be_deleted = current_photo;

    // don't attempt to delete if no image is present
    if( current_photo.isNull())
        return;

    // if photo isn't last in album, set next photo as current, previous
    // will be deleted
    if( !current_photo.nextSibling().isNull() )
    {
        current_photo = current_photo.nextSiblingElement("photo");
    }
    // if photo is last in album, set previous photo as the current, last
    // photo will be deleted
    else if ( !current_photo.previousSibling().isNull() )
    {
        current_photo = current_photo.previousSiblingElement("photo");
    }
    // if photo is last in album, delete it and set image to null
    else
    {
        current_photo.clear();
        album_xml.documentElement().removeChild(to_be_deleted);
        display_photo();

        //Enable only menu actions for an album with no photos
        album_no_photos();
        return;
    }

    // delete the photo user was viewing when they selected delete option
    album_xml.documentElement().removeChild(to_be_deleted);
    display_photo();        // display_photo() to update user album view
}


// This function is called when user wants to add a photo. A file dialog box
// pops up and the user has to select an image file. The file path is saved
// and a QDomElement is created for the new photo. The following tags are
// then added as new nodes to this QDomElement; file, date, location,
// description.  The file tag is filled with the filepath saved from the
// file dialog, and the other tags are made as blank but will be prompted
// to be filled in by the user. The new photo is appended after the photo
// currently being viewed if album contains image(s).
void PhotoAlbum::on_actionAdd_Photo_triggered()
{

    //Code from qt4/examples/xml/dombookmarks
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open Image"), QDir::currentPath(),
        tr("Image Files (*.png *.jpg *.bmp *.ppm)"));

    // if the user doesn't select a file, return
    if (filename.isEmpty())
        return;

    QDomElement new_photo = album_xml.createElement("photo");
    QString tags[4]= {"file", "date", "location", "description"};

    // set photo path with whatever user selected (filename)
    QDomNode new_node = album_xml.createElement(tags[0]);
    QDomText node_text = album_xml.createTextNode(filename);
    new_node.appendChild(node_text);
    new_photo.appendChild(new_node);

    // create date, location, description tags and set as blank
    int i;
    for(i = 1; i < 4; i++)
    {
        QDomNode new_node = album_xml.createElement(tags[i]);
        QDomText node_text = album_xml.createTextNode("");
        new_node.appendChild(node_text);
        new_photo.appendChild(new_node);
    }

    // if no other images exist in album
    if(album_xml.documentElement().firstChildElement("photo").isNull())
    {
        // add the QDomElement new_photo to the xml file
        album_xml.documentElement().appendChild(new_photo);
        // set current photo as the one just appended to xml file
        current_photo = album_xml.documentElement().firstChildElement("photo");
        enable_all_menu_actions();      // turn on menu actions since an image exists in album now
    }
    else
    {
        // insert the new_photo after the photo currently being viewed
        album_xml.documentElement().insertAfter(new_photo, current_photo);
        current_photo = new_photo;  // set new_photo as photo being viewed and display it
    }

    display_photo();        // display_photo() to update the user view of album

    // open up the edit description dialog so user can change date, loc, description
    on_actionEdit_Description_triggered();
}

//Called when user clicks Usage from the menu
void PhotoAlbum::on_actionUsage_triggered()
{
    QMessageBox::about(this, tr("PhotoAlbum Usage"),
             tr("This application opens xml photo albums. The xml will be "\
                "formatted as below.\n\n"
                "<album>\n"
                "    <photo>\n"
                "        <file>absolute/path/to/image/file</file>\n"
                "        <date>October 8, 2014</date>\n"
                "        <location>Rapid City, SD</location>\n"
                "        <description>Description of the image.</description>\n"
                "    <photo>\n"
                "    <photo>\n"
                "        <file>another/absoulte/path</file>\n"
                "        <date>October 8, 2014</date>\n"
                "        <location>Rapid City, SD</location>\n"
                "        <description>This is a second image.</description>\n"
                "    <photo>\n"
                "</album>\n\n"
                "Use the commands in the application menu to modify "
                "the xml album or even the image files themselves. Changes "
                "to the xml album structure like adding/deleting a photo "
                "or moving an image forward or backward in the album are "
                "not saved to the xml file until you choose Save or Save As. "
                "Changes to an image like brighten or crop will prompt you "
                "to either confirm or cancel overwriting the original "
                "image file.\n"));
}

// this function pops up a widget asking the user if they want to negate
// the image and save it and negates image if say ok.
void PhotoAlbum::on_actionNegate_triggered()
{
    preview_image = current_image.copy();
    preview_image.invertPixels();

    //Set the message in confirm_save and show it
    QString message = "Do you want to negate the image and overwrite the original image at "
                        + current_photo.firstChild().toElement().text();
    ui->confirm_label->setText(message);
    ui->confirm_save->show();

    display_photo();
}


// If the smooth option from the photo editor is selected, this function is
// called. It pops up the balance_widget which allows the user to input
// how many times an image is smoothed (0-3)
void PhotoAlbum::on_actionSmooth_triggered()
{
    ui->balance_widget->show();             // pop up slider,scrollbar, and preview image
    is_smooth = true;                     // set the is_smooth flag so smooth() will be called later

    ui->balance_label->setText("Smoothness");
    ui->balance_slider->setValue(0);
    ui->balance_spinbox->setValue(0);
    ui->balance_slider->setRange(0, 3);
    ui->balance_spinbox->setRange(0, 3);

    preview_image = current_image.copy();      // copy the current_image into preview_image
    display_preview_image();                   // call function to display preview_image which will call smooth()
}


void PhotoAlbum::on_actionSharpen_triggered()
{
    ui->balance_widget->show();             // pop up slider,scrollbar, and preview image
    is_sharpen = true;                     // set the is_smooth flag so sharpen() will be called later

    ui->balance_label->setText("Sharpness");
    ui->balance_slider->setValue(0);
    ui->balance_spinbox->setValue(0);
    ui->balance_slider->setRange(0, 3);
    ui->balance_spinbox->setRange(0, 3);

    preview_image = current_image.copy();      // copy the current_image into preview_image
    display_preview_image();                   // call function to display preview_image which will call sharpen()
}
