#include "bjlund.h"
#include <vector>
#include <iostream>
#include <chrono>
#include <sys/time.h>
#include <ctime>
#include "RtMidi.h"

using namespace std;
class EQSEQ
{
private:
    const int maxSteps = 32;
    vector<int> SEQ;
    float _bpm = 120.00;
    bool _ready = false;
    int _gate = 50;
    long long interval();
    int _step = 0;
    long long __lastpulse = 0;
    long long __OFF = 0;
    int lastNote = 36;
    RtMidiOut *midi = 0;
    void sendNote(unsigned char type, unsigned char ch, unsigned char note, unsigned char vel);
    void killHanging();
    int getDiv();
    int getVel();
    int limit(int v, int min, int max);
    long long sstep = 0;

public:
    int steps = 16;
    int ch = 1;
    int div = 1;
    int pulses = 4;
    int shift = 0;
    int gate = 65;
    int loop = 0;
    int note = 60;
    int vel = 96;
    int velh = 0;
    bool clockSync = true;
    bool enabled = false;
    void update();
    void updateSeq();
    void print();
    void reset();
    EQSEQ();
    void setBPM(float newbpm);
    void setGATE(int value);
    void clock(long long ts);
    void tick(long long _tick, long long ts);
    void setPORT(RtMidiOut *port);
    void ENABLE(bool e);
    void updateCH(int ch);
    void updateDiv(int div);
};
