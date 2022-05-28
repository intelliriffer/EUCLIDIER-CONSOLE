/*******************************************************************
 The Euclidean/BjorkLund Algorithm Testing module
 ***************************************************************** */
#include "bjlund.h"
void test(int p, int s);
/* uncomment the main block and
run the following command to check the algo outputs
g++ -w  testalgo.cpp bjlund.cpp -o testalgo && ./testalgo

*/
/*int main()
{
    test(2, 5);
    test(3, 7);
    test(4, 9);
    test(5, 11);
    test(5, 16);

    test(2, 3);
    test(3, 4);
    test(3, 5);
    test(3, 8);
    test(4, 7);
    test(4, 11);
    test(5, 6);
    test(5, 7);
    test(5, 9);
    test(5, 12);
    test(7, 8);
    test(7, 16);
    test(11, 24);

    test(5, 8);
    test(7, 12);
    test(9, 16);
    test(13, 24);
    //   failed
}*/

void test(int p, int s)
{
    vector<int> S = BJLUND::bjlund(p, s);
    cout << "BJLUNJ " << p << ", " << s << "   ";
    BJLUND::printResults(S);
    cout << endl;
}
