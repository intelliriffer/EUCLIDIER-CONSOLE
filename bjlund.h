#include <vector>
#include <iostream>

using namespace std;
namespace BJLUND
{
    vector<int> bjlund(int m, int k);
    void printResults(vector<int> &vect);
    void fillVect(vector<vector<int> > &vect, int val, int count);
    void testBJLUND(int k, int m);
    void flatten(vector<vector<int> > &vect);
    int least(vector<vector<int> > &vect);
    vector<vector<int> > leasts(vector<vector<int> > &vect);

}
