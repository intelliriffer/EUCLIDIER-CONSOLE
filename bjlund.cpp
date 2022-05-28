/*******************************************************************
 The Euclidean/BjorkLund Algorithm implementation
 Author: Amit Talwar <www.amitszone.com // intelliriffer@gmail.com>
 ***************************************************************** */
#include "bjlund.h"
using namespace BJLUND;

int leastval = 0;
vector<vector<int> > pops;

vector<int> BJLUND::bjlund(int m, int k)
{
    vector<int> result;
    vector<vector<int> > ones;
    BJLUND::fillVect(ones, 1, m);

    if (m >= k)
    {
        for (int i = 0; i < k; i++)
            result.push_back(1);
        return result;
    }
    vector<vector<int> > zeroes; //(k - m, e0);
    fillVect(zeroes, 0, k - m);

    if (zeroes.size() < ones.size())
    {
        for (vector<vector<int> >::size_type i = 0; i != zeroes.size(); i++)
        {
            ones[i].push_back(0);
        }
    }
    else
    {
        int zC = zeroes.size();

        while (zC > 1)
        {
            int rng = zC >= m ? m : zC;
            for (int i = 0; i < rng; i++)
            {
                ones[i].push_back(0);
            };
            zC = zC > m ? zC - m : 0;
        }

        if (zC == 1)
        {
            ones.push_back(vector<int>(1, 0));
            zC = 0;
        }
    }

    pops = leasts(ones);
    while (pops.size() > 1)
    {
        int rem = 0;
        for (vector<vector<int> >::size_type i = 0; i != pops.size(); i++)
        {

            if (i < ones.size())
            {
                rem++;
                ones[i].insert(ones[i].end(), pops[i].begin(), pops[i].end());
            }
        }

        pops.resize(pops.size() - rem);

        for (vector<vector<int> >::size_type i = 0; i != pops.size(); i++)
        {
            ones.push_back(pops[i]);
        }
        pops = leasts(ones);
    }

    for (vector<vector<int> >::size_type i = 0; i != ones.size(); i++)
    {
        for (vector<int>::size_type j = 0; j != ones[i].size(); j++)
        {
            result.push_back(ones[i][j]);
        }
    }

    return result;
}

vector<vector<int> > BJLUND::leasts(vector<vector<int> > &vect)
{

    int min = least(vect);

    vector<vector<int> > ret;
    if (min == 0)
    {
        ret.empty();
        return ret;
    }

    for (vector<int>::size_type i = min; i != vect.size(); i++)
    {
        ret.push_back(vect[i]);
    }
    vect.resize(min);
    return ret;
}

int BJLUND::least(vector<vector<int> > &vect)
{
    int max = vect.at(0).size();
    int min = 0;
    for (vector<int>::size_type i = 1; i != vect.size() - 1; i++)
    {
        if (vect[i].size() < max && i > min && vect[i].size() != vect[min].size())
        {
            min = i;
        }
    }
    return min;
}
void BJLUND::flatten(vector<vector<int> > &vect)
{
    for (vector<int>::size_type i = 0; i != vect.size(); i++)
    {
        for (vector<int>::size_type j = 0; j != vect[i].size(); j++)
        {
            cout << vect[i][j];
        }
        cout << endl;
    }
}

void BJLUND::printResults(vector<int> &vect)
{
    cout << "  ";
    for (vector<int>::size_type j = 0; j != vect.size(); j++)
    {
        char c = vect[j] ? 'x' : '.';
        cout << c << " ";
    }
    cout << endl;
}
void BJLUND::fillVect(vector<vector<int> > &vect, int val, int count)
{
    vect.empty();
    vector<int> vval(1, val);
    for (int i = 0; i < count; i++)
    {
        vect.push_back(vval);
    }
}
void BJLUND::testBJLUND(int k, int m)
{
    vector<int> b = bjlund(k, m);
    BJLUND::printResults(b);
}