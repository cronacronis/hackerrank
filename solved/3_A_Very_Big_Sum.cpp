#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main(){
    int n;
    cin >> n;
    vector<int> arr(n);
    for(int arr_i = 0;arr_i < n;arr_i++){
       cin >> arr[arr_i];
    }

    unsigned long int init = 0;
    unsigned long int res = accumulate(arr.begin(), arr.end(), init);

    cout << res << endl;

    return 0;
}
