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
#include <algorithm>
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
void handleClockMessage(unsigned char message);
void pulse();
void updateBPM(float bpm);
void clockStart();
bool pendingSync = false;
long long getUS();
void sendTicks();
void clear();
bool velSense = true;
bool receiveNotes = true;
bool autosync = true;
float BPM = 120.00;
int getOffset();
unsigned char syncDiv = 4;

void clockStop();
unsigned long long now();
unsigned long long ms();
unsigned long long BOOT_TIME;
unsigned long long sstep;                // sequence step
const bool CONNECT_AKAI_NETWORK = false; // automatically connevt to akai network remote port
const unsigned char STEPMAX = 64;        // number of max steps and pulses
const unsigned VEL_SENSE_MIN = 22;
const unsigned VEL_SENSE_MAX = 127;

long long tick = 0;

vector<double> pulses;
int limit(int v, int min, int max);
bool paused = false;
bool started = false;
void printAll(bool _clear);
void resync(bool force, bool print);
void updateSteps(unsigned char trk, unsigned char VALUE);
void updatePulse(unsigned char trk, unsigned char VALUE);
void updateShift(unsigned char trk, unsigned char VALUE);
void updateLoop(unsigned char trk, unsigned char VALUE);
void updateDiv(unsigned char trk, unsigned char VALUE);

RtMidiIn *midiIn = 0;
RtMidiIn *HWIN = 0;
RtMidiOut *midiOut = 0;
enum syncTypes
{
    STEP,
    PULSE,
    SHIFT,
    LOOP,
    DIV
};
struct syncMessage
{
    unsigned char CC;
    unsigned char TRACK;
    unsigned char VALUE;
    syncTypes TYPE; // 0=step,1=pulse, 2 loop,3=div
};
void queCC(unsigned char CC, unsigned char TRK, unsigned char VALUE, syncTypes type);
vector<syncMessage> QUEUE;
vector<syncMessage> WAIT_QUEUE;
void processMessage(syncMessage M);
void processQ();
bool QPROCESSING = false;

const unsigned char SEQS = 8;

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
            for (char i = 0; i < SEQS; i++)
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

    unsigned char byte0 = (int)message->at(0);
    unsigned char typ = byte0 & 0xF0;
    uint size = message->size();
    // cout << "message " << byte0 << endl;

    if (size == 1) // system realtime message
    {
        handleClockMessage(message->at(0));
        return;
    }
    if (typ == 0x90 && receiveNotes) // note message
    {

        unsigned char VAL = (int)message->at(2);
        if (VAL < 1) // disregard notes with velocity less than 10
            return;
        unsigned char note = (int)message->at(1);

        unsigned char oct = note / 12;
        unsigned char xpose = note % 12;
        if (oct < 8) // xpose tracks 1-8 {}
        {
            if (SQ[oct].mode > 1)
                return;

            unsigned targetCC = (oct * 10) + 2;
            unsigned ooct = 0;
            if (velSense)
            {
                ooct = VAL < VEL_SENSE_MIN ? -1 : 0;
                ooct = VAL >= VEL_SENSE_MAX ? 1 : ooct;
            }
            SQ[oct].octave = ooct;
            SQ[oct].xpose = xpose;
        }
        if (xpose < 8 && SQ[xpose].mode > 1)
            return;

        if (oct == 8 && xpose < 8) // 96-103
        {
            SQ[xpose].octave = 0;
        }
        if (oct == 9 && xpose < 8) // 108-115
        {
            SQ[xpose].octave = 1;
        }
        if (oct == 10 && xpose < 8) // 127-127
        {

            SQ[xpose].octave = -1;
        }
    }

    if (typ == 0xB0) // cc message
    {

        // cout << "CC" << endl;
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

        unsigned char CC = (int)message->at(1);
        unsigned char VAL = (int)message->at(2);
        unsigned char ch = byte0 & 0x0F;
        unsigned char trk = CC / 10;
        unsigned char cmd = CC % 10;
        if (CC == 100 && VAL > 0 && VAL % 2 == 0)
        {
            for (unsigned char i = 0; i < SEQS; i++)
            {

                SQ[i].octave = 0;
                SQ[i].xpose = 0;
            }

            return;
        }
        if (CC == 119)
        {
            bool wasActive = velSense;
            velSense = limit(VAL, 0, 1);
            if (!velSense && wasActive) // reset the octaves
            {
                for (unsigned char i = 0; i < SEQS; i++)
                {

                    SQ[i].octave = 0;
                }
            }

            return;
        }
        if (CC == 99) // receive notes
        {

            receiveNotes = limit(VAL, 0, 1);

            return;
        }
        if (CC == 80) // autosync
        {

            for (unsigned char i = 0; i < SEQS; i++)
            {

                SQ[i].autosync = limit(VAL, 0, 1);
            }

            return;
        }
        if (CC == 70) // resync
        {

            resync(true, true);

            return;
        }

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
                //  SQ[trk].updateDiv(limit(VAL, 1, 10));
                if (!started)
                {
                    updateDiv(trk, VAL);
                    SQ[trk].updateSeq();
                    printAll();
                    return;
                }
                queCC(CC, trk, VAL, DIV);
                return;

                break;

            case 4:
                if (trk == 6) // ignore cc 64
                    break;
                if (!started)
                {
                    updateSteps(trk, VAL);
                    SQ[trk].updateSeq();
                    printAll();
                    return;
                }
                queCC(CC, trk, VAL, STEP);

                break;

            case 5:

                if (!started)
                {
                    updatePulse(trk, VAL);
                    SQ[trk].updateSeq();
                    printAll();
                    return;
                }
                queCC(CC, trk, VAL, PULSE);
                break;

            case 6:

                if (!started)
                {
                    updateShift(trk, VAL);
                    SQ[trk].updateSeq();
                    printAll();
                    return;
                }
                queCC(CC, trk, VAL, SHIFT);
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
                    if (!started)
                    {
                        updateSteps(trk, VAL);
                        SQ[trk].updateSeq();
                        printAll();
                        return;
                    }
                    queCC(CC, trk, VAL, STEP);
                    return;
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
                SQ[cmd - 1].velh = VAL;
            }
            if (trk == 10 && cmd > 0 && cmd <= 8) // loop steps 1-64
            {
                //  cout << "loop" << endl;
                SQ[cmd - 1].loop = limit(VAL, 0, 64); // 0=off
                printAll();
            }
            if (trk == 11 && cmd > 0 && cmd <= 8) // set Modes 1-3
            {

                SQ[cmd - 1].setMode(VAL); // 0=off
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
void resync(bool force, bool print) // after parameter update , set all sequences to step 0 so they are in sync
{
    for (char i = 0; i < SEQS; i++)
    {

        SQ[i].reset(force);
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

void handleClockMessage(unsigned char message)
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
    sstep++;

    int ystep = tick == 0 ? 0 : (tick + 1) % ((24 * 4 / 4));
    if (ystep == 0)
        processQ();

    const uint mSize = 12; // fill up averaging buffer before computing final bpm;
    pulses.push_back(ms());
    if (pulses.size() < mSize)
        return;
    if (pulses.size() > mSize)
    {
        pulses.erase(pulses.begin());
    }

    double avg = 0;
    unsigned cnt = 0;
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
    sstep = 0;
    resync(true, true);
    started = true;
}
void clockStop()
{
    std::cout << "Clock Stopped"
              << "\n"
              << std::flush;
    started = false;
    resync(true, false);
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

    for (char i = 0; i < SEQS; i++)
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

void queCC(unsigned char CC, unsigned char TRK, unsigned char VALUE, syncTypes type)
{
    int index = -1;
    syncMessage M = {.CC = CC, .TRACK = TRK, .VALUE = VALUE, .TYPE = type};

    if (!QPROCESSING)
    {
        for (vector<syncMessage>::size_type i = 0; i != QUEUE.size(); i++)
        {
            if (QUEUE.at(i).CC == CC)
            {
                QUEUE.at(i).VALUE = VALUE;
                return;
            }
        }

        QUEUE.push_back(M);
        return;
    }
    else // wait que
    {
        for (vector<syncMessage>::size_type i = 0; i != WAIT_QUEUE.size(); i++)
        {
            if (WAIT_QUEUE.at(i).CC == CC)
            {
                WAIT_QUEUE.at(i).VALUE = VALUE;
                return;
            }
        }
        WAIT_QUEUE.push_back(M);
        return;
    }
}

void updateSteps(unsigned char trk, unsigned char VALUE)
{
    SQ[trk].steps = limit(VALUE, 2, STEPMAX);
}
void updatePulse(unsigned char trk, unsigned char VALUE)
{
    SQ[trk].pulses = limit(VALUE, 1, STEPMAX);
}
void updateShift(unsigned char trk, unsigned char VALUE)
{
    SQ[trk].shift = limit(VALUE, 0, STEPMAX);
}
void updateLoop(unsigned char trk, unsigned char VALUE) {}
void updateDiv(unsigned char trk, unsigned char VALUE)
{
    SQ[trk].updateDiv(limit(VALUE, 1, 10));
}
void processQ()
{
    QPROCESSING = true;
    vector<unsigned char> updated;
    for (vector<syncMessage>::size_type i = 0; i != QUEUE.size(); i++)
    {
        processMessage(QUEUE.at(i));
        updated.push_back(QUEUE.at(i).TRACK);
    }
    QUEUE.clear();
    QPROCESSING = false;
    for (vector<syncMessage>::size_type i = 0; i != WAIT_QUEUE.size(); i++)
    {
        processMessage(WAIT_QUEUE.at(i));
        updated.push_back(WAIT_QUEUE.at(i).TRACK);
    }
    WAIT_QUEUE.clear();
    if (updated.size())
    {
        for (unsigned char i = 0; i != 8; i++)
        {
            if (std::find(updated.begin(), updated.end(), i) != updated.end())
            {
                SQ[i].updateSeq();
            }
        }

        // SQ[trk].updateSeq();
        resync(false, true);
        // printAll();
        //  send sync messages
    }
}
void processMessage(syncMessage M)
{
    switch (M.TYPE)
    {
    case STEP:
        updateSteps(M.TRACK, M.VALUE);
        break;
    case PULSE:
        updatePulse(M.TRACK, M.VALUE);
        break;

    case LOOP:
        updateLoop(M.TRACK, M.VALUE);
        break;

    case SHIFT:
        updateShift(M.TRACK, M.VALUE);
        break;

    case DIV:
        updateDiv(M.TRACK, M.VALUE);
        break;
    }
}