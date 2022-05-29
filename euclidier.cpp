/*******************************************************************
 The 8 Track Euclidean Sequencer Main Program
 ***************************************************************** */
#include "eqseq.h"
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>
#include "RtMidi.h"
#include <math.h>
using namespace std;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;
void onMIDI(double deltatime, std::vector<unsigned char> *message, void * /*userData*/);
void sendNote(unsigned char type, unsigned char ch, unsigned char note, unsigned char vel);
void listInports();
uint getinPort(std::string str);
uint getOutPort(std::string str);
void doTempo();
void handleClockMessage(int message);
void pulse();
void updateBPM(float bpm);
void clockStart();
long long getUS();
void sendTicks();
void clear();
float BPM = 120.00;
int getOffset();
void clockStop();
unsigned long long now();
unsigned long long ms();
unsigned long long BOOT_TIME;
const bool CONNECT_AKAI_NETWORK = false; // automatically connevt to akai network remote port
const int STEPMAX = 64;                  // number of max steps and pulses

long long tick = 0;

vector<double> pulses;
int limit(int v, int min, int max);
bool paused = false;
bool started = false;
void printAll(bool _clear);
void resync(bool print);

RtMidiIn *midiIn = 0;
RtMidiIn *HWIN = 0;
RtMidiOut *midiOut = 0;

const int SEQS = 8;

EQSEQ *SQ = new EQSEQ[8]; // creante the 8 track sequencer in an array
int main()
{
    clear();
    BOOT_TIME = now();
    midiIn = new RtMidiIn();
    midiIn->setCallback(&onMIDI);
    midiIn->ignoreTypes(false, false, false); // dont ignore clocK

    midiOut = new RtMidiOut();
    if (CONNECT_AKAI_NETWORK)
    {
        HWIN = new RtMidiIn();
        HWIN->setCallback(&onMIDI);
        HWIN->ignoreTypes(false, false, false); // dont ignore clocK

        int op = getOutPort("Akai Network - MIDI");
        if (op != 99)
        {

            midiOut->openPort(op);
            cout << "Opened Network output" << endl;
        }
        else
        {
            midiOut->openVirtualPort("Euclidier");
        }
        int dp = getinPort("Akai Network - MIDI");
        if (dp != 99)
        {
            delete midiIn;
            HWIN->openPort(dp);
            cout << "Opened Network input" << endl;
        }
    }
    else
    { // normal midi
        midiIn->openVirtualPort("Euclidier");
        midiOut->openVirtualPort("Euclidier");
    }

    cout << "Ports opened - Waiting for Midi Clock Input to Start " << endl;
    printAll(false);

    for (int i = 0; i < SEQS; i++) // initialize sequencer parameters
    {

        SQ[i].setPORT(midiOut);
        if (i == 4)
        {
            SQ[i].note = 36;
        }
        if (i > 3)
        {
            SQ[i].ch = 10;
        }
        if (i == 5)
        {
            SQ[i].note = 41;
            SQ[i].pulses = 2;
            SQ[i].shift = 4;
        }
        if (i == 6)
        {
            SQ[i].note = 38;
            SQ[i].steps = 4;
            SQ[i].pulses = 4;
        }
        if (i == 7)
        {
            SQ[i].note = 39;
            SQ[i].pulses = 4;
            SQ[i].shift = 2;
        }
        SQ[i].updateSeq();
    }
    SQ[0].ENABLE(true);

    long long last = 0;

    while (true) // the main loop
    {
        long long us = getUS();
        if (started)
        {
            for (int i = 0; i < SEQS; i++)
            {
                SQ[i].clock(us);
            }
        }

        usleep(100);
    }

    return 0;
}
void printAll(bool _clear = true) // prints the sequence to console.
{
    if (_clear)
        clear();
    cout << "  *********************************" << endl;
    cout << "        <<<  SEQUENCES  >>>  " << endl;
    cout << "  *********************************" << endl;

    for (int i = 0; i < SEQS; i++)
    {

        SQ[i].print();
    }
    cout << endl
         << "  *********************************" << endl
         << endl;
}
void clear()
{
    // CSI[2J clears screen, CSI[H moves the cursor to top-left corner
    cout << "\x1B[2J\x1B[H";
    cout << "  ************************************" << endl;
    cout << "  Euclidier  (Press Ctrl + C to Quit)" << endl;
    cout << "  ************************************" << endl;
}

void onMIDI(double deltatime, std::vector<unsigned char> *message, void * /*userData*/) // handles incomind midi
{

    int byte0 = (int)message->at(0);
    int typ = byte0 & 0xF0;
    uint size = message->size();

    if (size == 1) // system realtime message
    {
        handleClockMessage((int)message->at(0));
        return;
    }
    if (typ == 0xB0) // cc message
    {
        /*
        params
        enabled: true/false
        TIMING : 1-5
        ch: 1-16
        steps: 4-STEMPMAX
        pulses: 2-STEPMAX
        shift : 0-32
        gate : 10-95%
        */

        int CC = (int)message->at(1);
        int VAL = (int)message->at(2);
        int ch = byte0 & 0x0F;
        int trk = CC / 10;
        int cmd = CC % 10;

        if (trk < 8) // if sequncer track messages
        {
            switch (cmd)
            {
            case 1: // enabled
                SQ[trk].ENABLE(VAL > 64);
                break;

            case 2: // note
                SQ[trk].note = VAL;
                //       0 - 127
                break;

            case 3:
                SQ[trk].updateDiv(limit(VAL, 1, 10));
                break;

            case 4:
                if (trk == 6) // ignore cc 64
                    break;
                SQ[trk].steps = limit(VAL, 2, STEPMAX);
                SQ[trk].updateSeq();
                if (SQ[trk].enabled)
                    resync(true);

                break;

            case 5:
                SQ[trk].pulses = limit(VAL, 1, STEPMAX);

                SQ[trk].updateSeq();
                if (SQ[trk].enabled)
                    resync(true);
                break;
            case 6:

                SQ[trk].shift = limit(VAL, 0, STEPMAX);
                SQ[trk].updateSeq();
                if (SQ[trk].enabled)
                    resync(true);
                break;
            case 7:

                if (trk == 0) // ignore cc 64
                    break;

                SQ[trk].setGATE(limit(VAL, 0, 95));
                break;
            case 8:

                SQ[trk].updateCH(limit(VAL, 1, 16));
                break;

            case 9: // edge cases for force
                if (trk == 0)
                {
                    SQ[trk].setGATE(limit(VAL, 0, 95));

                    break;
                }
                if (trk == 6) // track 6 cc64
                {
                    SQ[trk].steps = limit(VAL, 2, STEPMAX);
                    SQ[trk].updateSeq();
                    if (SQ[trk].enabled)
                        resync(true);
                }
            }
        }

        else // trk > 7
        {
            if (trk == 8 && cmd > 0 && cmd <= 8) // velocity messages
            {
                SQ[cmd - 1].vel = VAL;
            }
            if (trk == 9 && cmd > 0 && cmd <= 8) // velocity humanization messages
            {
                SQ[cmd - 1].velh = limit(VAL, 0, 50);
            }
            if (trk == 10 && cmd > 0 && cmd <= 8) // loop steps 1-64
            {
                //  cout << "loop" << endl;
                SQ[cmd - 1].loop = limit(VAL, 0, 64); // 0=off
            }
        }
    }
}

void sendNote(unsigned char type, unsigned char ch, unsigned char note, unsigned char vel)
{
    std::vector<unsigned char> messageOut;
    messageOut.push_back(ch + type);
    messageOut.push_back(note);
    messageOut.push_back(vel);
    midiOut->sendMessage(&messageOut);
}

int limit(int v, int min, int max)
{
    if (v < min)
        v = min;
    if (v > max)
        v = max;
    return v;
}
void resync(bool print) // after parameter update , set all sequences to step 0 so they are in sync
{
    for (int i = 0; i < SEQS; i++)
    {

        SQ[i].reset();
    }
    if (print)
        printAll();
    paused = true;
}

void listInports()
{
    uint nPorts = HWIN->getPortCount();
    for (uint i = 0; i < nPorts; i++)
    {
        std::string portName = HWIN->getPortName(i);

        std::cout << "Port: " << i << " = " << portName << "\n";
    }
}
uint getinPort(std::string str)
{
    uint nPorts = HWIN->getPortCount();
    for (uint i = 0; i < nPorts; i++)
    {
        std::string portName = HWIN->getPortName(i);
        size_t found = portName.find(str);
        if (found != string::npos)
        {
            return i;
        }
    }
    return 99;
}
uint getOutPort(std::string str)
{
    uint nPorts = midiOut->getPortCount();
    for (uint i = 0; i < nPorts; i++)
    {
        std::string portName = midiOut->getPortName(i);
        size_t found = portName.find(str);
        if (found != string::npos)
        {
            return i;
        }
    }
    return 99;
}

void handleClockMessage(int message)
{

    // cout << "message: " << (int)message << endl;
    switch (message)
    {

    case 248:
        pulse();
        return;
    case 250:
        clockStart();
        return;
    case 252:
        clockStop();
        return;
    }
}

void pulse() // used to compute bpm and send clock message to sequencer for sync
{
    sendTicks();
    tick += 1;

    const uint mSize = 12; // fill up averaging buffer before computing final bpm;
    pulses.push_back(ms());
    if (pulses.size() < mSize)
        return;
    if (pulses.size() > mSize)
    {
        pulses.erase(pulses.begin());
    }

    double avg = 0;
    int cnt = 0;
    for (uint i = 1; i < pulses.size(); i++)
    {
        double msd = pulses.at(i) - pulses.at(i - 1);
        double localavg = (60 / ((msd * 24) / 1000));
        avg += localavg;
        cnt++;
    }
    float bpm = (avg / cnt);
    BPM = bpm;
    updateBPM(bpm);
}
void clockStart()
{

    std::cout << "Clock Started"
              << "\n"
              << std::flush;
    tick = 0;
    resync(true);
    started = true;
}
void clockStop()
{
    std::cout << "Clock Stopped"
              << "\n"
              << std::flush;
    started = false;
    resync(false);
}

unsigned long long now() // current time since epoch in ms
{
    return (unsigned long long)duration_cast<milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

unsigned long long ms() // ms elapsed since app started.
{
    return now() - BOOT_TIME;
}
void updateBPM(float bpm) // sends the computed bpm to all sequencers
{
    if (isinf(bpm))
        return;

    for (int i = 0; i < SEQS; i++)
    {
        SQ[i].setBPM(bpm);
    }
}
void sendTicks() // send midi pulse to all sequencers
{
    long long us = getUS();

    for (int i = 0; i < SEQS; i++)
    {

        SQ[i].tick(tick, us);
    }
}

long long getUS() // gets time since epch in microseconds
{
    auto t1 = std::chrono::system_clock::now();
    long long us = duration_cast<microseconds>(t1.time_since_epoch()).count();
    return us;
}