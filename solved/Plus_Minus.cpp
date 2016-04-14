/*
 * Plus_Minus.cpp
 *
 *  Created on: 31 Mar 2016
 *      Author: cracs
 *
 *
 *      Print each value on its own line with the fraction of
 *      	positive numbers first,
 *      	negative numbers second,
 *      	and zeroes third.
 *
 */

#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


struct NumCounter {
	int positiv = 0;
	int negativ = 0;
	int zero = 0;

	void incr(int x)
	{
		if(x > 0) positiv += 1;
		if(x == 0) zero += 1;
		if(x < 0) negativ += 1;
	}

	void Print()
	{
		int total = positiv + zero + negativ;
		cout << (double)positiv/total << endl <<  (double)negativ/total << endl <<  (double)zero/total;
	}
};

int main(){
    int n;
    cin >> n;
    vector<int> arr(n);
    for(int arr_i = 0;arr_i < n;arr_i++){
       cin >> arr[arr_i];
    }



    NumCounter aggregated = std::accumulate(arr.begin(), arr.end(), NumCounter(),
    		[](NumCounter aggr, int e )
			{
    			NumCounter aggr_new = aggr;
    			aggr_new.incr(e);
    			return(aggr_new);
			}
    );

    aggregated.Print();

    return 0;
}
