#include "mainwidget.h"
#include "circletoggle.h"
#include "midihandler.h"

#include <QtWidgets>
#include <QWidget>
#include <QKeyEvent>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
//#include <QtMultimedia/QAudioSink>
#include <QTimer>
#include <QBuffer>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDebug>
#include <QDir>
#include <QAudioRecorder>

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{

    QPixmap bkgnd(":/res/bg");
    this->resize(1280, 720);
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Window, bkgnd);
    this->setPalette(palette);



    QString synthFolderPath = "C:/SynthApollon/Samples/Synth";
    QString pianoFolderPath = "C:/SynthApollon/Samples/Piano";
    QString guitarFolderPath = "C:/SynthApollon/Samples/Guitar";
    QString recordFolderPath = "C:/SynthApollon/Recordings/";
    QFile outputFile;

    // Create the Synth folder if it doesn't exist
    QDir synthDir;
    if (!synthDir.exists(synthFolderPath)) {
        if (synthDir.mkpath(synthFolderPath)) {
            qDebug() << "Synth folder created successfully";
        } else {
            qDebug() << "Failed to create Synth folder";
        }
    } else {
        qDebug() << "Synth folder already exists";
    }

    // Create the Piano folder if it doesn't exist
    QDir pianoDir;
    if (!pianoDir.exists(pianoFolderPath)) {
        if (pianoDir.mkpath(pianoFolderPath)) {
            qDebug() << "Piano folder created successfully";
        } else {
            qDebug() << "Failed to create Piano folder";
        }
    } else {
        qDebug() << "Piano folder already exists";
    }

    // Create the Guitar folder if it doesn't exist
    QDir guitarDir;
    if (!guitarDir.exists(guitarFolderPath)) {
        if (pianoDir.mkpath(guitarFolderPath)) {
            qDebug() << "Guitar folder created successfully";
        } else {
            qDebug() << "Failed to create Guitar folder";
        }
    } else {
        qDebug() << "Guitar folder already exists";
    }

    // Create the QLabel to display the image
    QLabel* synthLabel = new QLabel(this);
    QImage synthImage(":/res/sine");
    synthLabel->setPixmap(QPixmap::fromImage(synthImage));
    synthLabel->setGeometry(60, 180, 200, 200);

    QLabel* instrumentLabel = new QLabel(this);
    QImage pianoImage(":/res/piano");
    instrumentLabel->setPixmap(QPixmap::fromImage(pianoImage));
    instrumentLabel->setGeometry(80, 5, 200, 200);

    QLabel* filterLabel = new QLabel(this);
    QImage filterImage(":/res/low-pass");
    filterLabel->setPixmap(QPixmap::fromImage(filterImage));
    filterLabel->setGeometry(380, 180, 200, 200);

    this->inputLabel = new QLabel(this);
    QImage inputImage(":/res/keyboard");
    inputLabel->setPixmap(QPixmap::fromImage(inputImage));
    inputLabel->setGeometry(400, 10, 200, 200);


    QLabel* textLabel = new QLabel("STEINWAY Model D (XLN Audio)", this);
    textLabel->setGeometry(60, 145, 200, 20);

    QPalette textPalette = textLabel->palette();
    textPalette.setColor(QPalette::WindowText, Qt::white);
    textLabel->setPalette(textPalette);

    // Create the QListWidget
    QListWidget* listWidget = new QListWidget(this);
    listWidget->addItems({"Sine", "Square", "Saw", "Triangle"});
    listWidget->setGeometry(240, 250, 90, 90);


    // Set the QListWidget's text color to white and background to transparent
    QPalette listPalette = listWidget->palette();
    listPalette.setColor(QPalette::Text, Qt::white);
    listWidget->setPalette(listPalette);
    listWidget->setStyleSheet("QWidget{background: #343434}");
    listWidget->setAutoFillBackground(false);
    listWidget->setFrameShape(QFrame::NoFrame);

    // Create the QListWidget for instruments
    QListWidget* instrumentListWidget = new QListWidget(this);
    instrumentListWidget->addItems({"Synthesizer", "Piano", "Organ", "Guitar"});
    instrumentListWidget->setGeometry(240, 75, 90, 90);

    // Set the QListWidget's text color to white and background to transparent
    QPalette instrumentListPalette = instrumentListWidget->palette();
    instrumentListPalette.setColor(QPalette::Text, Qt::white);
    instrumentListWidget->setPalette(instrumentListPalette);
    instrumentListWidget->setStyleSheet("QWidget{background: #343434}");
    instrumentListWidget->setAutoFillBackground(false);
    instrumentListWidget->setFrameShape(QFrame::NoFrame);

    // Create the QListWidget
    QListWidget* filterListWidget = new QListWidget(this);
    filterListWidget->addItems({"low-pass", "high-pass", "band-pass"});
    filterListWidget->setGeometry(565, 250, 90, 90);


    // Set the QListWidget's text color to white and background to transparent
    QPalette filterListPalette = filterListWidget->palette();
    filterListPalette.setColor(QPalette::Text, Qt::white);
    filterListWidget->setPalette(instrumentListPalette);
    filterListWidget->setStyleSheet("QWidget{background: #343434}");
    filterListWidget->setAutoFillBackground(false);
    filterListWidget->setFrameShape(QFrame::NoFrame);

    // Create the QListWidget
    QListWidget* inputListWidget = new QListWidget(this);
    inputListWidget->addItems({"keyboard", "MIDI"});
    inputListWidget->setGeometry(565, 90, 90, 40);
    inputListWidget->setSelectionMode(QAbstractItemView::NoSelection);

    // Set the QListWidget's text color to white and background to transparent
    QPalette inputListPalette = inputListWidget->palette();
    inputListPalette.setColor(QPalette::Text, Qt::white);
    inputListWidget->setPalette(inputListPalette);
    inputListWidget->setStyleSheet("QWidget{background: #343434}");
    inputListWidget->setAutoFillBackground(false);
    inputListWidget->setFrameShape(QFrame::NoFrame);


    CircleToggle* circleToggle = new CircleToggle(this);
    circleToggle->setGeometry(35, 210, 10, 10);
    circleToggle->hideToggle(true);

    CircleToggle* filterToggle = new CircleToggle(this);
    filterToggle->setGeometry(360, 210, 10, 10);
    filterToggle->hideToggle(true);

    // Create the dial widget
    QDial* frequencyDial = new QDial(this);
    frequencyDial->setMinimum(0);
    frequencyDial->setMaximum(20000);
    frequencyDial->setFixedSize(40, 40);
    frequencyDial->setGeometry(730, 70, 50, 50);
    // Create the label for displaying text
    QLabel* frequencyLabel = new QLabel("Cut-off : 1000hz", this);
    frequencyLabel->setGeometry(710, 110, 200, 20);

    QPalette frequencyPalette = frequencyLabel->palette();
    frequencyPalette.setColor(QPalette::WindowText, Qt::white);
    frequencyLabel->setPalette(textPalette);

    // Set the dial's style
    frequencyDial->setStyleSheet("QDial {"
                        "border: 2px solid white;"
                        "background-color: grey;"
                        "}");

    // Create the dial widget
    QDial* bandwidthDial = new QDial(this);
    bandwidthDial->setMinimum(0);
    bandwidthDial->setMaximum(100);
    bandwidthDial->setFixedSize(40, 40);
    bandwidthDial->setGeometry(730, 130, 50, 50);
    // Create the label for displaying text
    QLabel* bandwidthLabel = new QLabel("Bandwidth : 100%", this);
    bandwidthLabel->setGeometry(710, 170, 200, 20);

    QPalette bandwidthPalette = frequencyLabel->palette();
    bandwidthPalette.setColor(QPalette::WindowText, Qt::white);
    bandwidthLabel->setPalette(bandwidthPalette);

    // Set the dial's style
    bandwidthDial->setStyleSheet("QDial {"
                        "border: 2px solid white;"
                        "background-color: grey;"
                        "}");


    // Create the dial widget
    QDial* gainDial = new QDial(this);
    gainDial->setMinimum(0);
    gainDial->setMaximum(50);
    gainDial->setFixedSize(40, 40);
    gainDial->setGeometry(730, 190, 50, 50);
    // Create the label for displaying text
    QLabel* gainLabel = new QLabel("Gain : 0%", this);
    gainLabel->setGeometry(727, 230, 200, 20);

    QPalette gainPalette = gainLabel->palette();
    gainPalette.setColor(QPalette::WindowText, Qt::white);
    gainLabel->setPalette(gainPalette);

    // Set the dial's style
    gainDial->setStyleSheet("QDial {"
                        "border: 2px solid white;"
                        "background-color: grey;"
                        "}");

    // Create the dial widget
    QDial* offsetDial = new QDial(this);
    offsetDial->setMinimum(0);
    offsetDial->setMaximum(100);
    offsetDial->setFixedSize(40, 40);
    offsetDial->setGeometry(730, 250, 50, 50);
    // Create the label for displaying text
    QLabel* offsetLabel = new QLabel("Offset : 0%", this);
    offsetLabel->setGeometry(720, 290, 200, 20);

    QPalette offsetPalette = gainLabel->palette();
    offsetPalette.setColor(QPalette::WindowText, Qt::white);
    offsetLabel->setPalette(offsetPalette);

    // Set the dial's style
    offsetDial->setStyleSheet("QDial {"
                        "border: 2px solid white;"
                        "background-color: grey;"
                        "}");

    QPushButton* recordButton = new QPushButton(this);
    recordButton->setGeometry(5000, 560, 50, 25);
    recordButton->setText("Record");

    QAudioRecorder* audioRecorder = new QAudioRecorder;


    QAudioDeviceInfo targetDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();

    // Check if the target device is valid
    if (!targetDeviceInfo.isNull()) {
        qDebug() << "Device Found: " << targetDeviceInfo.deviceName();
        // Set the desired audio input device
       audioRecorder->setAudioInput("default");
    } else {
        qDebug() << "Target audio output device not found.";
        // Handle the case when the target device is not found
        // ...
    }


    // Set the desired audio input device
    audioRecorder->setAudioInput(targetDeviceInfo.deviceName());


    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/pcm");
    audioSettings.setQuality(QMultimedia::HighQuality);
    audioRecorder->setEncodingSettings(audioSettings);

    // Get a list of all available audio output devices
    QList<QAudioDeviceInfo> audioOutputDevices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);

    for (const QAudioDeviceInfo& deviceInfo : audioOutputDevices) {
        qDebug() << "Device Name:" << deviceInfo.deviceName();
    }



    QObject::connect(recordButton, &QPushButton::clicked, [audioRecorder, recordFolderPath, recordButton]() {
        if(audioRecorder == nullptr) qDebug() << "audioRecorder is null";
        qDebug() << "started";
        if (audioRecorder->state() == QMediaRecorder::RecordingState) {
            // Stop recording
            audioRecorder->stop();
            qDebug() << "Recording stopped";
            recordButton->setText("record");
        } else {
            // Create the Recordings folder if it doesn't exist
            QDir recordDir;
            if (!recordDir.exists(recordFolderPath)) {
                if (recordDir.mkpath(recordFolderPath)) {
                    qDebug() << "Recordings folder created successfully";
                } else {
                    qDebug() << "Failed to create Recordings folder";
                    return;
                }
            } else {
                qDebug() << "Recordings folder already exists";
            }

            // Generate a unique filename for each recording session
            QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".wav";
            QString filePath = recordFolderPath + fileName;

            // Set the output location for recording
            audioRecorder->setOutputLocation(QUrl::fromLocalFile(filePath));

            //audioRecorder->setVolume(1.0);
            // Start recording
            audioRecorder->record();
            qDebug() << "Recording started";
            recordButton->setText("stop");
        }
    });


    QObject::connect(frequencyDial, &QDial::valueChanged, [frequencyLabel, this](int value){
            hasChanged = true;
            emit hasChangedChanged();
            frequencyLabel->setText("Cut-Off : " + QString::number(value) + "hz");
            this->updateButtonStyle();
            sound->cutoffFilter = value;
        });

    QObject::connect(bandwidthDial, &QDial::valueChanged, [bandwidthLabel, this](int value) {
        hasChanged = true;
        emit hasChangedChanged();

        // Map the value from 0-100 to 0-1
        double mappedValue = static_cast<double>(value) / 100.0;

        bandwidthLabel->setText("Bandwidth: " + QString::number(value) + "%");
        this->updateButtonStyle();
        sound->bandwidthFilter = mappedValue;
    });


    QObject::connect(gainDial, &QDial::valueChanged, [gainLabel, this](int value){
        hasChanged = true;
            emit hasChangedChanged();
            gainLabel->setText("Gain: " + QString::number(value*2) + "%");
            this->updateButtonStyle();
            sound->gainFilter = value;
            sound->currentGain = value;
    });

    QObject::connect(offsetDial, &QDial::valueChanged, [offsetLabel, this](int value){
            hasChanged = true;
            emit hasChangedChanged();
            offsetLabel->setText("Offset : " + QString::number(value) + "%");
            this->updateButtonStyle();
            sound->offsetFilter = value;
        });


    connect(listWidget, &QListWidget::itemClicked, [synthLabel, this](QListWidgetItem* item) {
                QString text = item->text();
                if (text == "Sine") {
                    QImage image(":/res/sine");
                    sound->currentWave = Sound::SINE;
                    synthLabel->setPixmap(QPixmap::fromImage(image));
                } else if (text == "Square") {
                    QImage image(":/res/square");
                    sound->currentWave = Sound::SQUARE;
                    synthLabel->setPixmap(QPixmap::fromImage(image));
                } else if (text == "Saw") {
                    QImage image(":/res/saw");
                    sound->currentWave = Sound::SAWTOOTH;
                    synthLabel->setPixmap(QPixmap::fromImage(image));
                } else if (text == "Triangle") {
                    QImage image(":/res/triangle");
                    sound->currentWave = Sound::TRIANGLE;
                    synthLabel->setPixmap(QPixmap::fromImage(image));
                }
                hasChanged = true;
                emit hasChangedChanged();
    });

    connect(filterToggle, &QPushButton::clicked, [filterToggle, filterListWidget, this](){
        if (!filterToggle->isChecked()) {
            sound->currentFilter = Sound::NOFILTER;
            filterListWidget->setCurrentItem(nullptr); // Deselect any selected item
        }
    });


    connect(filterListWidget, &QListWidget::itemClicked, [filterLabel, filterToggle, this](QListWidgetItem* item) {
                filterToggle->setChecked(true);
                filterToggle->hideToggle(false);
                QString text = item->text();
                if (text == "low-pass") {
                    QImage image(":/res/low-pass");
                    sound->currentFilter = Sound::LOWPASS;
                    filterLabel->setPixmap(QPixmap::fromImage(image));
                } else if (text == "high-pass") {
                    QImage image(":/res/high-pass");
                    sound->currentFilter = Sound::HIGHPASS;
                    filterLabel->setPixmap(QPixmap::fromImage(image));
                } else if (text == "band-pass") {
                    QImage image(":/res/band-pass");
                    sound->currentFilter = Sound::BANDPASS;
                    filterLabel->setPixmap(QPixmap::fromImage(image));
                }
                hasChanged = true;
                emit hasChangedChanged();
    });

    connect(instrumentListWidget, &QListWidget::itemClicked, [instrumentLabel, circleToggle, textLabel, this](QListWidgetItem* item) {
        QString text = item->text();
        if (text == "Synthesizer") {
            QImage image(":/res/synth");
            circleToggle->hideToggle(false);
            circleToggle->setChecked(true);
            textLabel->setText("Internal Synthesizer");
            sound->currentInstrument = Sound::SYNTHESIZER;
            instrumentLabel->setPixmap(QPixmap::fromImage(image));
        } else if (text == "Piano") {
            QImage image(":/res/piano");
            circleToggle->hideToggle(true);
            textLabel->setText("STEINWAY Model D (XLN Audio)");
            sound->currentInstrument = Sound::PIANO;
            instrumentLabel->setPixmap(QPixmap::fromImage(image));
        } else if (text == "Organ") {
            QImage image(":/res/organ");
            circleToggle->hideToggle(true);
            textLabel->setText("NA");
            sound->currentInstrument = Sound::ORGAN;
            instrumentLabel->setPixmap(QPixmap::fromImage(image));
        } else if (text == "Guitar") {
            QImage image(":/res/guitar");
            circleToggle->hideToggle(true);
            textLabel->setText("Martin D-41 Acoustic Guitar");
            sound->currentInstrument = Sound::GUITAR;
            instrumentLabel->setPixmap(QPixmap::fromImage(image));
        }
        hasChanged = true;
        emit hasChangedChanged();
    });



    applyEffects = new QPushButton(this);
    applyEffects->setText("Apply Effects");
    applyEffects->setGeometry(185, 8, 80, 22);
    applyEffects->setEnabled(true);

    // Set the QPushButton's text color to white and background to transparent
    QPalette buttonPalette = applyEffects->palette();
    buttonPalette.setColor(QPalette::ButtonText, Qt::white);
    applyEffects->setPalette(buttonPalette);
    applyEffects->setStyleSheet("QPushButton{background: #343434}");



    sound = new Sound(this);
    sound->generateAudioData("syn_", Sound::SQUARE, Sound::LOWPASS);
    sound->currentInstrument = Sound::PIANO;
    sound->currentWave = Sound::SQUARE;
    sound->currentFilter = Sound::NOFILTER;

#ifdef _WIN32
    MidiHandler* midiHandler = new MidiHandler();
#endif
    qDebug() << "Test";

    connect(sound->audioOutput, &QAudioOutput::stateChanged, sound, &Sound::handleAudioStateChanged);

    connect(this, &MainWidget::hasChangedChanged, this, &MainWidget::updateButtonStyle);

    connect(applyEffects, &QPushButton::clicked, [this]() {
        // generate new files
        qDebug() << "clicked";
        hasChanged = false;
        applyEffects->setStyleSheet("QPushButton{background: #343434}");
        sound->generateAudioData("syn_", sound->currentWave, sound->currentFilter);
    });

#ifdef _WIN32
    // Connect the MIDI handler signals to the event handlers
    connect(midiHandler, &MidiHandler::noteOn, this, &MainWidget::handleNoteOn);
    connect(midiHandler, &MidiHandler::noteOff, this, &MainWidget::handleNoteOff);
#endif

    this->setFixedSize(1280, 720);


}

/**
 * Cette évenement gère la pression de touches du clavier d'ordinateur.
 *
 *
 */

void MainWidget::keyPressEvent(QKeyEvent *event)
{
    QString prefix;
    if (event->isAutoRepeat())
            return;

    qDebug() << "pressed";
    sound->isPlaying = true;
    //double frequency = sound->calculateFrequency(event->key());
    switch(sound->currentInstrument)
    {
        case Sound::SYNTHESIZER:
            prefix = "C:/SynthApollon/Samples/Synth/syn_";
        break;

        case Sound::PIANO:
            prefix = "C:/SynthApollon/Samples/Piano/";
        break;

        case Sound::ORGAN:
            prefix = "C:/SynthApollon/Samples/Piano/";
        break;

        case Sound::GUITAR:
            prefix = "C:/SynthApollon/Samples/Guitar/";
        break;

        default:
            prefix = "C:/SynthApollon/Samples/Piano/";
        break;
    }

    sound->play(event, 4, 0.0, prefix);
}

/**
 * Cette évenement gère la pression de touches du clavier d'ordinateur lorsque la touche est relachée.
 *
 *
 */

void MainWidget::keyReleaseEvent(QKeyEvent* event)
    {

    QImage inputImage(":/res/keyboard");
    this->inputLabel->setPixmap(QPixmap::fromImage(inputImage));

    if (event->isAutoRepeat())
            return;

        qDebug() << "lifted";
        if (sound->isPlaying) {
            sound->isPlaying = false;
            sound->player->stop();

        }
    }

/**
 * Cette évenement gère l'aspect visuel du temoin "apply effects".
 *
 *
 */

void MainWidget::updateButtonStyle() {
        if (hasChanged) {
            // If hasChanged is true, set the button's style to glow green
            applyEffects->setStyleSheet("QPushButton {background: #2a5b2f; border-color: green; }");
        } else {
            // If hasChanged is false, set the button's style to its default style
            applyEffects->setStyleSheet("QPushButton{background: #343434}");
        }
}
/**
 * Cette évenement gère la pression de touches du clavier MIDI.
 *
 *
 */
#ifdef _WIN32
void MainWidget::handleNoteOn(quint8 noteNumber, quint8 velocity, double frequency) {
        // Update the current note label
        //QString labelText = QString("Current note: %1 (%2 Hz)").arg(noteNumber).arg(frequency, 0, 'f', 2);
        //currentNoteLabel->setText(labelText);

    QString prefix;

    QImage inputImage(":/res/synth");
    this->inputLabel->setPixmap(QPixmap::fromImage(inputImage));

    qDebug() << "pressed EVENT";
    sound->isPlaying = true;
    //double frequency = sound->calculateFrequency(event->key());
    switch(sound->currentInstrument)
    {
        case Sound::SYNTHESIZER:
            prefix = "C:/SynthApollon/Samples/Synth/syn_";
        break;

        case Sound::PIANO:
            prefix = "C:/SynthApollon/Samples/Piano/";
        break;

        case Sound::ORGAN:
            prefix = "C:/SynthApollon/Samples/Piano/";
        break;

        case Sound::GUITAR:
            prefix = "C:/SynthApollon/Samples/Guitar/";
        break;

        default:
            prefix = "C:/SynthApollon/Samples/Piano/";
        break;
    }

    sound->playMIDI(frequency, velocity, 4, prefix);

        // variable de tests
        //int currentNoteNumber = noteNumber;
        //int currentNoteVelocity = velocity;
        //double currentNoteFrequency = frequency;
}

/**
 * Cette évenement gère la pression de touches du clavier MIDI.
 *
 *
 */

void MainWidget::handleNoteOff(quint8 noteNumber) {
        //currentNoteLabel->setText("Current note: None");
        //int currentNoteNumber = 0;
        //int currentNoteVelocity = 0;
        //double currentNoteFrequency = 0.0;

        qDebug() << "lifted";
        if (sound->isPlaying) {
            sound->isPlaying = false;
            sound->player->stop();

        }
}
#endif
