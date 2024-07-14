#include "sound.h"
#include <QtMultimedia/QAudioFormat>
#include <QIODevice>
#include <QKeyEvent>
#include <QDebug>
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
#include <fstream>
#include <cstdint>
#include <cmath>

Sound::Sound(){
    //ctor
    //change
}
/**

    \brief Écrit les données audio dans un fichier WAV.
    Cette fonction écrit les données audio spécifiées dans un fichier WAV.
*/
void Sound::writeWAV(const std::string& filename, const int16_t* buffer, size_t numSamples, int sampleRate, int numChannels) {
    std::ofstream file(filename, std::ios::binary);

    // Write the RIFF chunk descriptor
    file.write("RIFF", 4);
    uint32_t chunkSize = 36 + numSamples * sizeof(int16_t);
    file.write(reinterpret_cast<const char*>(&chunkSize), 4);
    file.write("WAVE", 4);
    file.write("fmt ", 4);
    uint32_t subChunk1Size = 16;
    file.write(reinterpret_cast<const char*>(&subChunk1Size), 4);
    uint16_t audioFormat = 1; // PCM
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    file.write(reinterpret_cast<const char*>(&numChannels), 2);
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    uint32_t byteRate = sampleRate * numChannels * sizeof(int16_t);
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    uint16_t blockAlign = numChannels * sizeof(int16_t);
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    uint16_t bitsPerSample = 16;
    file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);

    // Write the "data" sub-chunk
    file.write("data", 4);
    uint32_t subChunk2Size = numSamples * sizeof(int16_t);
    file.write(reinterpret_cast<const char*>(&subChunk2Size), 4);
    file.write(reinterpret_cast<const char*>(buffer), subChunk2Size);

    file.close();
}

/**

    \brief Génère un fichier WAV à partir des paramètres audio spécifiés.
    Cette fonction génère un fichier WAV à partir des paramètres audio tels que la fréquence, la durée,
    le taux d'échantillonnage, le type d'onde et le type de filtre spécifiés.
*/

void Sound::generateWAV(const std::string& filename, double frequency, double duration, int sampleRate, WaveType waveType, Filter filterType) {
    // Calculate the number of samples
    size_t numSamples = static_cast<size_t>(duration * sampleRate);

    // Generate the wave data
    std::vector<int16_t> samples(numSamples);
    double offsetFactor = 0.2 * static_cast<double>(this->offsetFilter) / 100.0;
    double omega = (2 * PI * frequency / sampleRate) + (offsetFactor * PI);

    double phaseOffset = offsetFactor * PI;  // Convert offset to radians
   for (size_t i = 0; i < numSamples; i++) {
       double phase = omega * i + phaseOffset;
       double value;
       switch (waveType) {
           case SINE:
               value = std::sin(phase);
               break;
           case SQUARE:
               value = std::sin(phase) >= 0 ? 1 : -1;
               break;
           case TRIANGLE:
               value = 2 / PI * std::asin(std::sin(phase));
               break;
           case SAWTOOTH:
               value = 2 * (frequency * (i / static_cast<double>(sampleRate)) - std::floor(0.5 + frequency * (i / static_cast<double>(sampleRate))));
               break;
       }
       samples[i] = static_cast<int16_t>(value * INT16_MAX);
   }

    if (filterType != Sound::NOFILTER) {
        // Apply the filter to the wave data
        applyFilter(samples, filterType, this->cutoffFilter, this->bandwidthFilter, sampleRate);
        //qDebug() << this->bandwidthFilter;
    }
    // Write the wave data to a WAV file
    writeWAV(filename, samples.data(), numSamples, sampleRate, 1);
}

//Cette fonction a été realisé en grande partie a l'aide de ressources tel que :
//https://www.w3.org/TR/audio-eq-cookbook/
//chatGPT a permis de traduire les formules LPF & HPF en une fonction utilisable
/**

    \brief Calcule les coefficients du filtre à partir des paramètres spécifiés.
    Cette fonction calcule les coefficients du filtre (bCoeffs et aCoeffs) en fonction du type de filtre,
    de la fréquence de coupure, de la bande passante et du taux d'échantillonnage spécifiés.
    */
void Sound::calculateFilterCoeffs(Filter filterType, double cutoffFreq, double bandwidth, int sampleRate, std::vector<double>& bCoeffs, std::vector<double>& aCoeffs)
{
    double omegaC = 2 * PI * cutoffFreq / sampleRate;
    double omegaB = 2 * PI * bandwidth / sampleRate;
    double alpha = std::sin(omegaB) / (2 * 0.5);


    switch (filterType) {
        case LOWPASS:
            bCoeffs = { (1 - std::cos(omegaC)) / 2, 1 - std::cos(omegaC), (1 - std::cos(omegaC)) / 2 };
            aCoeffs = { 1 + alpha, -2 * std::cos(omegaC), 1 - alpha };
            break;
        case HIGHPASS:
            bCoeffs = { (1 + std::cos(omegaC)) / 2, -(1 + std::cos(omegaC)), (1 + std::cos(omegaC)) / 2 };
            aCoeffs = { 1 + alpha, -2 * std::cos(omegaC), 1 - alpha };
            break;
        case BANDPASS:
            double beta = std::cos(omegaC);
            double gamma = std::tan(omegaB / 2.0);
            double delta = 1.0 / (1.0 + gamma);

            bCoeffs = { gamma * delta, 0, -gamma * delta };
            aCoeffs = { 1 + gamma * delta, -2 * beta * delta, 1 - gamma * delta };
            break;
    }
}

//Cette fonction a été realisé à l'aide des ressources suivantes : 
//https://dsp.stackexchange.com/questions/8693/how-does-a-low-pass-filter-programmatically-work
//https://medium.com/geekculture/digital-filter-design-in-python-and-c-4484b6d6f4a5 
//chatGPT a été utilisé pour implementer une solution de filtrage en utilisant une formule de convolution

/**

    \brief Applique le filtre spécifié aux échantillons audio.
    Cette fonction applique le filtre (filtreType) aux échantillons audio donnés (samples).
    Les paramètres du filtre, tels que la fréquence de coupure et la bande passante,
    sont spécifiés en utilisant le taux d'échantillonnage (sampleRate).
    */

void Sound::applyFilter(std::vector<int16_t>& samples, Filter filterType, double cutoffFreq, double bandwidth, int sampleRate) {
    std::vector<double> bCoeffs, aCoeffs;
    calculateFilterCoeffs(filterType, cutoffFreq, bandwidth, sampleRate, bCoeffs, aCoeffs);

    std::vector<double> inputSamples(samples.begin(), samples.end());
    std::vector<double> outputSamples(inputSamples.size());

    // Apply the filter to the input samples
    for (size_t i = 0; i < inputSamples.size(); i++) {
        double output = bCoeffs[0] * inputSamples[i];
        for (size_t j = 1; j < bCoeffs.size(); j++) {
            if (i >= j) {
                output += bCoeffs[j] * inputSamples[i - j] - aCoeffs[j] * outputSamples[i - j];
            }
        }
        outputSamples[i] = output;
    }

    // Scale the output samples to fit within the [-1, 1] range
    double maxSample = *std::max_element(outputSamples.begin(), outputSamples.end(), [](double a, double b) {
        return std::abs(a) < std::abs(b);
    });
    for (size_t i = 0; i < outputSamples.size(); i++) {
        outputSamples[i] /= maxSample;
    }

    for (size_t i = 0; i < samples.size(); i++) {
        samples[i] = static_cast<int16_t>(outputSamples[i] * INT16_MAX);
    }

    outputSamples.resize(samples.size());
}


/**

    \brief Calcule la fréquence correspondante à une clé spécifiée.
    Cette fonction calcule la fréquence correspondante à une touche donnée.
    */
double Sound::calculateFrequency(int key)
{
        // C du milieu à 261.63 Hz
        double baseFrequency = 261.63;
        int distance = key - Qt::Key_G;

        return baseFrequency * pow(2, distance / 12.0);
}

/**

    \brief Gère l'état de l'audio lorsqu'il change.
    Cette fonction est appelée pour gérer les changements d'état de l'audio.
    */
void Sound::handleAudioStateChanged(QAudio::State state)
{
    if (state == QAudio::IdleState) {
            // Stop playing the sine wave data
            audioOutput->stop();
    }
}

/**

    \brief Gère le changement de statut du lecteur multimédia.
    Cette fonction est appelée pour gérer les changements de statut du lecteur multimédia.
    */
void Sound::playerMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
            QMediaPlayer* player = qobject_cast<QMediaPlayer*>(sender());
            player->deleteLater();
    }
 }

/**

    \brief Joue le son correspondant à la touche du clavier.
    Cette fonction est appelée pour jouer le son correspondant à la touche du clavier pressée.
    Elle utilise un objet QMediaPlayer pour jouer le fichier audio associé à la touche en parrallèle des autres.
    */

void Sound::play(QKeyEvent* event, int octave, double frequency, QString prefix)
{
    static QList<QMediaPlayer*> players; //liste de qmediaplayer pour pouvoir jouer simultanement
    QString filename;
    switch (event->key()) {
        case Qt::Key_A:
            filename = prefix + "A" + QString::number(octave) + ".WAV";
            player->setMedia(QMediaContent(QUrl::fromLocalFile(filename)));
            break;

        case Qt::Key_S:
            filename = prefix + "A" + QString::number(octave) + "#.WAV";
            player->setMedia(QMediaContent(QUrl::fromLocalFile(filename)));
            break;

        case Qt::Key_D:
            filename = prefix + "B" + QString::number(octave) + ".WAV";
            player->setMedia(QMediaContent(QUrl::fromLocalFile(filename)));
            break;

        case Qt::Key_F:
            filename = prefix + "C" + QString::number(octave) + ".WAV";
            player->setMedia(QMediaContent(QUrl::fromLocalFile(filename)));
            break;

        case Qt::Key_G:
            filename = prefix + "C" + QString::number(octave) + "#.WAV";
            player->setMedia(QMediaContent(QUrl::fromLocalFile(filename)));
            break;

        case Qt::Key_H:
            filename = prefix + "D" + QString::number(octave) + ".WAV";
            player->setMedia(QMediaContent(QUrl::fromLocalFile(filename)));
            break;

        case Qt::Key_J:
            filename = prefix + "D" + QString::number(octave) + "#.WAV";
            player->setMedia(QMediaContent(QUrl::fromLocalFile(filename)));
            break;

        case Qt::Key_K:
            filename = prefix + "E" + QString::number(octave) + ".WAV";
            player->setMedia(QMediaContent(QUrl::fromLocalFile(filename)));
            break;

        case Qt::Key_L:
            filename = prefix + "F" + QString::number(octave) + ".WAV";
            player->setMedia(QMediaContent(QUrl::fromLocalFile(filename)));
            break;

        default:
            // Nothing
            break;
    }

    //player->setPlaylist(playlist);
    QMediaPlayer* player = new QMediaPlayer;
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &Sound::playerMediaStatusChanged);
    player->setMedia(QMediaContent(QUrl::fromLocalFile(filename)));
    player->setVolume(50 + this->currentGain);
    player->play();
    players.append(player);

}

bool Sound::compareDoubles(double d1, double d2, double tolerance) {
    return std::abs(d1 - d2) < tolerance;
}

/**

    \brief Joue le son MIDI correspondant à une fréquence donnée.
    Cette fonction est appelée pour jouer le son MIDI correspondant à une fréquence donnée.
    */

void Sound::playMIDI(double frequency, int velocity, int octave, QString prefix)
{
    static QList<QMediaPlayer*> players; //liste de qmediaplayer pour pouvoir jouer simultanement
    QString filename;
    for (auto note : this->notes.toStdMap()) {
        if (compareDoubles(frequency, note.first, 1e-2)) {
            filename = prefix + note.second + ".WAV";
            qDebug() << filename;
            break;
        }
    }

    if (filename.isEmpty()) {
        // Frequency not found in the mapping
        qDebug() << "File not found : " << filename;
         return;
    }

    //player->setPlaylist(playlist);
    QMediaPlayer* player = new QMediaPlayer;
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &Sound::playerMediaStatusChanged);
    player->setMedia(QMediaContent(QUrl::fromLocalFile(filename)));
    player->setVolume((velocity / 2) + this->currentGain);
    player->play();
    players.append(player);

}

/**

    \brief Génère les données audio pour toutes les notes.
    Cette fonction génère les données audio pour toutes les notes en utilisant un préfixe d'instrument spécifié.
    */

void Sound::generateAudioData(const std::string& instrumentPrefix, WaveType waveType, Filter filter) {
    // Generate the audio data for all the notes
    for (auto it = this->notes.begin(); it != this->notes.end(); ++it) {
        QString noteName = it.value();
        std::string noteNumber = "C:/SynthApollon/Samples/Synth/" + instrumentPrefix + noteName.toStdString() + ".WAV";
        generateWAV(noteNumber, it.key(), DURATION, SAMPLE_RATE, waveType, filter);
    }
}



