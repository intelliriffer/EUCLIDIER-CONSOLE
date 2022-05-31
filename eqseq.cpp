/*******************************************************************
 The Euclidean Sequencer Class for single channel Sequencer.
 Author: Amit Talwar <www.amitszone.com // intelliriffer@gmail.com>
 ***************************************************************** */
#include "eqseq.h"
#include <algorithm>

EQSEQ::EQSEQ()
{
    // constructoir
    EQSEQ::updateSeq();
    this->_ready = true;
}
void EQSEQ::sync()
{
    int max = this->loop == 0 ? this->steps : std::min(this->steps, this->loop);
    if (this->autosync || this->_step >= max)
    {

        this->_step = 0;
        this->killHanging();
    }
}
void EQSEQ::updateSeq()
{
    this->sync();
    this->SEQ = BJLUND::bjlund(this->pulses, this->steps); // generate sequence
    if (this->shift > 0)                                   // rotate sequence
    {
        int shift = this->shift > this->SEQ.size() ? this->shift % this->SEQ.size() : this->shift;
        std::rotate(this->SEQ.begin(), this->SEQ.begin() + this->SEQ.size() - shift, this->SEQ.end());
    }

    this->sync();
}
void EQSEQ::print()
{
    vector<int> PSEQ;
    if (this->loop == 0 || this->loop == this->steps)
    {
        for (vector<int>::size_type i = 0; i != this->SEQ.size(); i++)
        {
            PSEQ.push_back(this->SEQ.at(i));
        }
        PSEQ.push_back(4);
        BJLUND::printResults(PSEQ);
        return;
    }
    int lps = this->loop / this->steps;
    for (int l = 0; l != (lps); l++)
    {
        for (vector<int>::size_type i = 0; i != this->SEQ.size(); i++)
        {
            if (l == 0)
            {
                PSEQ.push_back(this->SEQ.at(i));
            }
            else
            {
                PSEQ.push_back(this->SEQ.at(i) ? 2 : 5);
            }

            if (i == this->loop - 1 && this->loop < this->steps)
            {
                PSEQ.push_back(4); // loop point
            }
        }
    }
    for (int l = 0; l < this->loop % this->steps; l++)
    {
        PSEQ.push_back(this->SEQ.at(l) ? 2 : 5);
    }
    PSEQ.push_back(4);
    BJLUND::printResults(PSEQ);
}

void EQSEQ::setBPM(float newbpm)
{
    this->_bpm = newbpm;
}

void EQSEQ::tick(long long _tick, long long ts) // tick is a midi clock pulse (24ppq)
{

    int div = this->getDiv();
    int step = _tick == 0 ? 0 : (_tick + 1) % ((24 * 4 / div));
    if (_tick == 0)
        sstep = 0;
    if (step == 0) // valid timed step
    {
        //= (_tick) / (24 * 4 / div);

        if (this->SEQ.at(_step) == 1) // pulse note (active)
        {
            long long interval = this->interval();

            if (this->enabled)
            {
                if (this->mode == 1) // note mode
                {
                    int note = this->note + (12 * this->octave) + this->xpose;
                    int mul = note > 127 ? -1 : 1;
                    while (note < 0 || note > 127)
                    {
                        note = note + (12 * mul);
                    }
                    this->sendNote(0x90, this->ch - 1, note, this->getVel()); // send note on
                    this->lastNote = note;
                }

                if (this->mode > 1) // cc
                {
                    int CC = limit(this->note, 1, 119);
                    this->lastRandVal = this->getVel();
                    this->sendNote(0xB0, this->ch - 1, CC, this->lastRandVal); // send CC on
                    this->lastNote = CC;
                }

                try
                { // compute the time at which the note off will be sent,
                    float _gate = (float)this->gate / 100;
                    this->__OFF = ts + (long long)(interval * _gate) - 100; // remove 100 microseconds (master pulse)
                }
                catch (std::exception &e)
                {
                    cout << " *** Caught Note OFF Setting Exception";
                    this->__OFF = 0;
                    this->sendNote(0x80, this->ch - 1, this->note, 0); // kill the note
                }
            }
        }

        __lastpulse = ts;
        _step++;
        sstep += 1;

        if (_step == this->SEQ.size())
            _step = 0;
        if (sstep > 0 && this->loop > 0 && sstep % (this->loop) == 0)
        {
            _step = 0;
            //   cout << "sstep " << sstep << endl;
        }
    }
}
void EQSEQ::clock(long long ts) // triggered every 100 microseconds (1/10 ms)
{
    if (!this->_ready)
        return;
    if (ts - this->__OFF > 0 && this->__OFF > 0) // send note off or cc reset
    {
        if (this->mode <= 1)
            this->sendNote(0x80, this->ch - 1, this->lastNote, 0);

        if (this->mode > 1 && this->lastNote > 0 && this->lastNote < 120)
        { // cc

            this->sendNote(0xB0, this->ch - 1, this->lastNote, this->vel); // reset cc
        }
        this->__OFF = 0; // reset note off time
    }
}

long long EQSEQ::interval() // compute the single step duration based on bpm and time division for track
{
    double N = (60000 * 1000 / this->_bpm) * 4;

    int div = this->getDiv();
    // cout << "interval " << (long long)N / div << endl;
    return (long long)N / div;
}

int EQSEQ::getDiv() // return midi time division 1/16,1/4 etc based on midi received valiees 1-6
{
    int div = 16;
    switch (this->div)
    {
    case 0:
    case 1:
        div = 16;
        break;
    case 2:
        div = 8;
        break;

    case 3:
        div = 4;
        break;

    case 4:
        div = 2;
        break;

    case 5:
        div = 1;
        break;

    case 6:
        div = 32;
        break;
    case 7:
        div = 64;
        break;
    case 8: // dotted 16
        div = 12;
        break;
    case 9: // dotted 8
        div = 6;
        break;
    case 10: // // dotted 4
        div = 3;
        break;
    }
    return div;
}

void EQSEQ::setPORT(RtMidiOut *port) // assign selected midi output port
{
    this->midi = port;
}

void EQSEQ::sendNote(unsigned char type, unsigned char ch, unsigned char note, unsigned char vel) // send midi note message to assigned output port
{
    std::vector<unsigned char> messageOut;
    messageOut.push_back(ch + type);
    messageOut.push_back(note);
    messageOut.push_back(vel);
    this->midi->sendMessage(&messageOut);
}

void EQSEQ::ENABLE(bool e)
{
    this->enabled = e;
    killHanging();
}
void EQSEQ::updateCH(int ch)
{
    killHanging();
    this->ch = ch;
}
void EQSEQ::updateDiv(int div)
{
    this->div = div;
}

void EQSEQ::reset(bool force)
{
    if (force)
    {
        this->_step = 0;
        this->killHanging();
        return;
    }

    this->sync();
}

void EQSEQ::killHanging() // stops any playing note used before any operation that could cause stuck notes..
{
    if (this->__OFF > 0)
    {
        if (this->mode <= 1)
            this->sendNote(0x80, this->ch - 1, this->lastNote, 0); // kill hanging notes
        if (this->mode > 1)
            this->sendNote(0xB0, this->ch - 1, this->lastNote, this->vel); // kill hanging notes

        this->__OFF = 0;
    }
}
int EQSEQ::getVel() // computes step velocity based on base velocity and humanization factor.
{
    if (this->mode <= 1) // note mode
    {                    // note mode
        if (this->velh == 0)
            return this->vel;

        srand(time(NULL));
        //        int mul = ((rand() % 10)) % 2 == 0 ? 1 : -1;
        int add = this->velh > 0 ? rand() % this->velh : 0;
        return limit(this->vel + add, 0, 127);
    }
    if (this->mode == 2)
    {
        if (this->velh == 0)
            return this->vel;
        return limit(this->velh, 1, 127);
    }

    { // mode 2  random cc
        if (this->velh == 0)
            return this->vel;
        srand(time(NULL));
        int max = std::max(this->vel, this->velh);
        int min = std::min(this->vel, this->velh);
        int diff = (max - min);
        int val = min + (rand() % diff);
        while (diff > 10 && std::abs(val - diff) < 5)
        {
            val = min + (rand() % diff);
        }
        // int add = this->velh > 0 ? rand() % this->velh : 0;
        return limit(val, 0, 127);
    }
}

int EQSEQ::limit(int v, int min, int max) // Restricts an interger value between limits
{
    if (v < min)
        v = min;
    if (v > max)
        v = max;
    return v;
}
void EQSEQ::setGATE(int value) // set note duration (10-95%)
{
    bool wasEnabled = this->enabled;
    if (wasEnabled)
        this->enabled = false;
    this->gate = value;
    if (wasEnabled)
        this->enabled = true;
    this->killHanging();
}
void EQSEQ::setMode(unsigned char _mode)
{
    _mode = limit(_mode, 1, 3);
    if (_mode == this->mode)
        return;
    this->killHanging();

    this->mode = _mode;
}