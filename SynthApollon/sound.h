#ifndef SOUND_H
#define SOUND_H

//#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QBuffer>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QDebug>
#include <QTimer>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <cmath>

/**
 * Cette classe est utilisé pour le traitement d'audio, generation de son/fichiers, et autres.
 *
 *
 */

class Sound : public QObject {
    Q_OBJECT

public:
    Sound();

    Sound(QObject *parent = nullptr) : QObject(parent) {
            // Create a QBuffer to hold the sine wave data
            buffer = new QBuffer(this);
            buffer->open(QIODevice::ReadWrite);

            // Create a QAudioFormat for the sine wave data
            QAudioFormat format;
            format.setSampleRate(SAMPLE_RATE);
            format.setChannelCount(1);
            format.setSampleSize(32);
            format.setCodec("audio/pcm");
            format.setByteOrder(QAudioFormat::LittleEndian);
            format.setSampleType(QAudioFormat::SignedInt);

            QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

            if (!info.isFormatSupported(format))
                format = info.nearestFormat(format);

            // Create a QAudioOutput for the sine wave data
            audioOutput = new QAudioOutput(format, this);
            audioOutput->setBufferSize(buffer->size());

            qDebug() << info.supportedSampleRates();
        }

    // Sample rate (samples per second)
    const int SAMPLE_RATE = 192000 ;

    // Duration of the sine wave (in seconds)
    const double DURATION = 1.0;

    // Frequency of the sine wave (in Hz)
    double FREQUENCY = 440.0;

    //PI
    const double PI = 3.14159265358979323846;

    enum WaveType {
        SINE,
        SQUARE,
        TRIANGLE,
        SAWTOOTH
    };

    enum Instrument {
        SYNTHESIZER,
        PIANO,
        ORGAN,
        GUITAR
    };

    enum Filter {
        LOWPASS,
        HIGHPASS,
        BANDPASS,
        NOFILTER
    };

    int cutoffFilter = 1000;
    double bandwidthFilter = 0;
    int gainFilter = 0;
    int offsetFilter = 0;

    QMap<double, QString> notes = {
        {27.50, "A0"},
        {29.14, "A0#"},
        {30.87, "B0"},
        {32.70, "C1"},
        {34.65, "C1#"},
        {36.71, "D1"},
        {38.89, "D1#"},
        {41.20, "E1"},
        {43.65, "F1"},
        {46.25, "F1#"},
        {49.00, "G1"},
        {51.91, "G1#"},
        {55.00, "A1"},
        {58.27, "A1#"},
        {61.74, "B1"},
        {65.41, "C2"},
        {69.30, "C2#"},
        {73.42, "D2"},
        {77.78, "D2#"},
        {82.41, "E2"},
        {87.31, "F2"},
        {92.50, "F2#"},
        {98.00, "G2"},
        {103.83, "G2#"},
        {110.00, "A2"},
        {116.54, "A#2"},
        {123.47, "B2"},
        {130.81, "C3"},
        {138.59, "C3#"},
        {146.83, "D3"},
        {155.56, "D3#"},
        {164.81, "E3"},
        {174.61, "F3"},
        {185.00, "F3#"},
        {196.00, "G3"},
        {207.65, "G3#"},
        {220.00, "A3"},
        {233.08, "A3#"},
        {246.94, "B3"},
        {261.63, "C4"},
        {277.18, "C4#"},
        {293.66, "D4"},
        {311.13, "D4#"},
        {329.63, "E4"},
        {349.23, "F4"},
        {369.99, "F4#"},
        {392.00, "G4"},
        {415.30, "G4#"},
        {440.00, "A4"},
        {466.16, "A4#"},
        {493.88, "B4"},
        {523.25, "C5"},
        {554.37, "C5#"},
        {587.33, "D5"},
        {622.25, "D5#"},
        {659.25, "E5"},
        {698.46, "F5"},
        {739.99, "F5#"},
        {783.99, "G5"},
        {830.61, "G5#"},
        {880.00, "A5"},
        {932.33, "A5#"},
        {987.77, "B5"},
        {1046.50, "C6"},
        {1108.73, "C6#"},
        {1174.66, "D6"},
        {1244.51, "D6#"},
        {1318.51, "E6"},
        {1396.91, "F6"},
        {1479.98, "F6#"},
        {1567.98, "G6"},
        {1661.22, "G6#"},
        {1760.00, "A6"},
        {1864.66, "A6#"},
        {1975.53, "B6"},
        {2093.00, "C7"},
        {2217.46, "C7#"},
        {2349.32, "D7"},
        {2489.02, "D7#"},
        {2637.02, "E7"},
        {2793.83, "F7"},
        {2959.96, "F7#"},
        {3135.96, "G7"},
        {3322.44, "G7#"},
        {3520.00, "A7"},
        {3729.31, "A7#"},
        {3951.07, "B7"},
        {4186.01, "C8"}
    };

    WaveType currentWave;
    Instrument currentInstrument;
    Filter currentFilter;
    int currentGain = 0;

    bool isPlaying = false;

    void play(QKeyEvent* event, int octave, double frequency, QString prefix);
    void playMIDI(double frequency, int velocity, int octave, QString prefix);
    double calculateFrequency(int key);
    void calculateFilterCoeffs(Filter filterType, double cutoffFreq, double bandwidth, int sampleRate, std::vector<double>& bCoeffs, std::vector<double>& aCoeffs);
    void generateAudioData(const std::string& instrumentPrefix, WaveType waveType, Filter filter);
    void applyFilter(std::vector<int16_t>& samples, Filter filterType, double cutoffFreq, double bandwidth, int sampleRate);
    void writeWAV(const std::string& filename, const int16_t* buffer, size_t numSamples, int sampleRate, int numChannels);
    void generateWAV(const std::string& filename, double frequency, double duration, int sampleRate, WaveType waveType, Filter filterType);
    void handleAudioStateChanged(QAudio::State state);
    void playerMediaStatusChanged(QMediaPlayer::MediaStatus status);
    bool compareDoubles(double d1, double d2, double tolerance);

    void setFrequency(int frequency)
    {
        this->FREQUENCY = frequency;
    }

    QTimer *timer = new QTimer(this);



    QBuffer *buffer;
    QAudioOutput *audioOutput;

    // playlist sounds
    QMediaPlayer* player = new QMediaPlayer(this);
    QMediaPlaylist* playlist = new QMediaPlaylist(player);

    QAudioOutput *m_output;
    QIODevice *m_device;

};

#endif // SOUND_H
