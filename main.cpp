///////////////////////////////////////////////////////////////////////////////
//Authors: Colton Fuhrmann, Kevin Hilt
//Date: October 8, 2014
//Course: CSC421
//Instructor: Dr. Weiss
//
//Description: The main function which simply sets up the application window
//and begins application execution. If the user supplied a command line
//argument, it opens that album when the application window is shown.
///////////////////////////////////////////////////////////////////////////////

#include "photoalbum.h"
#include "crop.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PhotoAlbum w;

    if(argc == 2)
    {
        w.album_filename = argv[1]; //Set album_filename to be passed file
        QFile file(w.album_filename); //Create a file with the selected path
        //Extracts the xml into a QDomDocument object
        w.process_xml(&file, w.album_filename);
    }

    w.showMaximized();

    return a.exec();
}
