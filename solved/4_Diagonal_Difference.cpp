#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;


int main(){
	int n;
	cin >> n;

//	cout << "n = " << n << endl;

	vector< vector<int> > a(n,vector<int>(n));
	for(int a_i = 0;a_i < n;a_i++){
		for(int a_j = 0;a_j < n;a_j++){
			cin >> a[a_i][a_j];
		}
	}

//	cout << "Calculating..." << endl;

	vector< vector<int> > diagonals(2,vector<int>(0));

	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
//			cout << "i = " << i << " j = " << j << " elem = " << a[i][j] << endl;
			if(j == i)
			{
				diagonals[0].emplace_back(a[i][j]);
//				cout << "added to diag 0" << endl;
			}
			if(j == (n-1) - i)
			{
				diagonals[1].emplace_back(a[i][j]);
//				cout << "added to diag 1" << endl;
			}
		}
	}


//	for (auto e: diagonals[0])
//		std::cout << e << ' ';
//	cout << endl;
//
//	for (auto e: diagonals[1])
//		std::cout << e << ' ';
//	cout << endl;

	int res = abs(accumulate(diagonals[0].begin(), diagonals[0].end(), 0) - accumulate(diagonals[1].begin(), diagonals[1].end(), 0));

	cout << res << endl;

	return 0;
}
