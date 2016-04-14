#include <cmath>
#include <cstdio>
//#include <vector>
#include <iostream>
#include <algorithm>

#include <deque>


using namespace std;


int main(){
    int n;
    cin >> n;

    deque<string> dq(n);
    deque<string> fq(n);

    int nspaces = 1;
    std::generate(dq.begin(), dq.end(), [&nspaces]{ return std::string(nspaces++, '#'); });

    int nsharp = n - 1;
    std::generate(fq.begin(), fq.end(), [&nsharp]{ return std::string(nsharp--, ' '); });


    for(int i = 0; i < n; i++)
    {
    	cout << fq[i] << dq[i] << endl;
    }

    return 0;
}




