#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include "mainwidget.h"
#include <QBuffer>
#include <QtMultimedia/QAudioOutput>
#include <QTimer>
#include <fstream>
#include <cstdint>
#include <QMediaPlayer>
#include <QMediaPlaylist>

/**
 * Cette classe la fenêtre principal.
 *
 *
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    MainWidget* w = new MainWidget(this);

};
#endif // MAINWINDOW_H
