#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

int main()
{
	int n;
	cin >> n;

	vector<long long> list(n);
	for (int i = 0; i < n; i++) {
		cin >> list[i];
	}

	vector<long long> res;

	for (int i = n - 1; i > 0; i--) {
		res.push_back(list[i] - list[i - 1]);
	}

	reverse(res.begin(), res.end());
	cout << list[0] << " ";
	for (int i = 0; i < n - 1; i++) {
		cout << res[i] << " ";
	}
}
