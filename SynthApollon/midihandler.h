#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#ifdef _WIN32

#include "windows.h"
#include <iostream>
#include <QDebug>
#include <QObject>

// Auteur : Sami Nouidri

// Cette classe a été construite en grande partie grace aux exemples suivants :
// https://www.codeproject.com/Articles/2771/Wrapper-Library-for-Windows-MIDI-API
// http://www.huckert.com/ehuckert/pdf/MidiAPI.pdf
// https://learn.microsoft.com/en-us/windows/win32/multimedia/midi-functions
// https://learn.microsoft.com/en-us/windows/win32/multimedia/about-midi
// ainsi que plusieurs pages stack overflow & chatGPT pour debugger

/**
 * Cette classe est utilisé pour traiter l'input de clavier MIDI en utilisant l'API Win32.
 *
 * Les IFDEF permettent d'eviter la compilation sur les systèmes unix du serveur d'intégration
 */

class MidiHandler : public QObject {
    Q_OBJECT
public:
    MidiHandler() {
        UINT numDevices = midiInGetNumDevs();
        if (numDevices == 0) {
            std::cerr << "No MIDI devices available." << std::endl;
            return;
        }

        std::cout << "Available MIDI devices:" << std::endl;

        for (UINT i = 0; i < numDevices; i++) {
            MIDIINCAPS caps;
            midiInGetDevCaps(i, &caps, sizeof(caps));
            std::cout << "  " << i << ": " << caps.szPname << std::endl;
        }

        UINT deviceNumber = 0;

        if (deviceNumber >= numDevices) {
            std::cerr << "Invalid device number." << std::endl;
            return;
        }

        if (midiInOpen(&midiInHandle, deviceNumber, (DWORD_PTR)&midiInCallback, (DWORD_PTR)this, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
            std::cerr << "Failed to open MIDI device." << std::endl;
            return;
        }

        if (midiInStart(midiInHandle) != MMSYSERR_NOERROR) {
            std::cerr << "Failed to start MIDI input." << std::endl;
            return;
        }

        std::cout << "MIDI input started." << std::endl;
    }

    ~MidiHandler() {
        midiInStop(midiInHandle);
        midiInClose(midiInHandle);
    }

signals:
    void noteOn(int noteNumber, int velocity, double frequency);
    void noteOff(int noteNumber);

private:
     HMIDIIN midiInHandle;

    static void CALLBACK midiInCallback(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
        MidiHandler *handler = reinterpret_cast<MidiHandler *>(dwInstance);
        handler->handleMidiEvent(wMsg, dwParam1, dwParam2);
    }

    void handleMidiEvent(UINT wMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
        if (wMsg == MIM_DATA) {
            // Extract the status byte and data bytes from the MIDI message
            BYTE statusByte = static_cast<BYTE>(dwParam1 & 0xFF);
            BYTE dataByte1 = static_cast<BYTE>((dwParam1 >> 8) & 0xFF);
            BYTE dataByte2 = static_cast<BYTE>((dwParam1 >> 16) & 0xFF);

            // Check if the status byte is a Note On or Note Off message
            if (statusByte >= 0x80 && statusByte <= 0x9F) {
                bool noteActive = (statusByte & 0xF0) == 0x90;
                BYTE noteNumber = dataByte1;
                BYTE velocity = dataByte2;

                if (noteActive && velocity > 0) {
                    // Calculate the frequency of the note played
                    double frequency = 440.0 * pow(2.0, (noteNumber - 69.0) / 12.0);
                    std::cout << "Note On: " << static_cast<int>(noteNumber) << " Velocity: " << static_cast<int>(velocity) << " Frequency: " << frequency << std::endl;
                    emit noteOn(static_cast<int>(noteNumber), static_cast<int>(velocity), frequency);
                } else {
                    std::cout << "Note Off: " << static_cast<int>(noteNumber) << std::endl;
                    emit noteOff(static_cast<int>(noteNumber));
                }
            }

        }
    }

};

#endif //WIN32

#endif
