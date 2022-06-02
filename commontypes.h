#ifndef __EUCLIDIER_COMMON__
#define __EUCLIDIER_COMMON__
struct lanePatch
{
    bool enabled;
    unsigned char steps;
    unsigned char pulses;
    unsigned char shift;
    unsigned char loop;
    unsigned char gate;
    unsigned char div;
    unsigned char note;
    unsigned char BV;
    unsigned char VA;
    unsigned char type;
    unsigned char ch = 0;
};
struct EUPATCH
{
    lanePatch lane[8];
};
struct EUBANK
{
    EUPATCH patch[128];
};

#endif