#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "sound.h"
#include <QWidget>
#include <QWidget>
#include <QKeyEvent>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
//#include <QtMultimedia/QAudioSink>
#include <QTimer>
#include <QBuffer>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPushButton>
#include <QLabel>

/**
 * Cette classe contient les definitions de composants et les evenements permettant les interactions entre le widget et les contrôles.
 *
 *
 */

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);

    bool hasChanged = false;

    QPushButton* applyEffects;

    QLabel* inputLabel;

    Sound* sound;

protected:
    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void handleNoteOn(quint8 noteNumber, quint8 velocity, double frequency);
    void handleNoteOff(quint8 noteNumber);

private:

signals:
    void hasChangedChanged();

public slots:
    void updateButtonStyle();

};

#endif // MAINWIDGET_H
