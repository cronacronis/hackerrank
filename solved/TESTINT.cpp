


#include <iostream>
#include <vector>
#include <algorithm>




using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::accumulate;

int main() {
	int n = 2;
	vector<int> vals(n);


	for(int i = 0; i < n; i++)
	{
		int tmpval;
		cin >> tmpval;
		vals.emplace_back(tmpval);
	}

	cout << accumulate(vals.begin(), vals.end(), 0) << endl;

	return(0);
}
