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
void EQSEQ::updateSeq()
{

    this->SEQ = BJLUND::bjlund(this->pulses, this->steps); // generate sequence
    if (this->shift > 0)                                   // rotate sequence
    {
        int shift = this->shift > this->SEQ.size() ? this->shift % this->SEQ.size() : this->shift;
        std::rotate(this->SEQ.begin(), this->SEQ.begin() + this->SEQ.size() - shift, this->SEQ.end());
    }
    this->_step = 0;
}
void EQSEQ::print()
{
    BJLUND::printResults(this->SEQ);
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
                this->sendNote(0x90, this->ch - 1, this->note, this->getVel()); // send note on
            this->lastNote = this->note;
            if (this->enabled)
            {
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

        if (_step > this->SEQ.size() - 1)
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
    if (ts - this->__OFF > 0 && this->__OFF > 0) // send note off
    {
        this->sendNote(0x80, this->ch - 1, this->lastNote, 0);
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
        div = 12; // dotted 8th
        break;

    case 3:
        div = 8;
        break;

    case 4:
        div = 4;
        break;

    case 5:
        div = 2;
        break;

    case 6:
        div = 1;
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

void EQSEQ::reset()
{
    this->killHanging();
    this->_step = 0;
}

void EQSEQ::killHanging() // stops any playing note used before any operation that could cause stuck notes..
{
    if (this->__OFF > 0)
    {
        this->sendNote(0x80, this->ch - 1, this->lastNote, 0); // kill hanging notes
        this->__OFF = 0;
    }
}
int EQSEQ::getVel() // computes step velocity based on base velocity and humanization factor.
{
    if (this->velh == 0)
        return this->vel;

    srand(time(NULL));
    int mul = ((rand() % 10)) % 2 == 0 ? 1 : -1;
    int add = this->velh > 0 ? rand() % this->velh : 0;
    return limit(this->vel + (add * mul), 10, 127);
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