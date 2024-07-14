#include "mainwindow.h"
#include "qdebug.h"

#include <iostream>
#include <QtMath>
#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
//#include <QtMultimedia/QAudioSink>
#include <QTimer>
#include <QBuffer>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
            setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
            setCentralWidget(w);

}


