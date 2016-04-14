//============================================================================
// Name        : HackerRank.cpp
// Author      : Vladimir Khmyrov
// Version     :
// Copyright   : GPL
// Description : Hello World in C++, Ansi-style
//============================================================================

//#include <iostream>
//using namespace std;
//
//int main() {
//	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
//	return 0;
//}


#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>

#include <functional>

using namespace std;


int main_solved(){
    int n;
    cin >> n;
    vector<int> arr(n);
    for(int arr_i = 0;arr_i < n;arr_i++){
       cin >> arr[arr_i];
    }

    std::accumulate(arr.begin(), arr.end(), 0);

    return 0;
}
