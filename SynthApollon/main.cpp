#include "mainwindow.h"
#include "mainwidget.h"
#include "midihandler.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>

/*
TEMPLATE = subdirs
SUBDIRS = \
            TestApollon/TestApollon

CONFIG += ordered
*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MidiHandler midiHandler;

    MainWidget w;

    w.show();
    return a.exec();
}
