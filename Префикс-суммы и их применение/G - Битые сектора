#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

int main()
{
	int n, k;
	cin >> n >> k;

	vector<int> A(n + 2);
	vector<int> Pref_l(n + 2);
	vector<int> Pref_r(n + 2);

	string s;
	cin >> s;

	int day = 50000000;
	s = '.' + s + '.';
	// . не влияют на работу
	for (int i = 1; i <= n; i++) {
		if (s[i] == '*') {
			day = 0;
		}
		else {
			day++;
		}
		Pref_l[i] = day;
	}

	day = 50000000;
	for (int i = n; i >= 1; i--) {
		if (s[i] == '*') {
			day = 0;
		}
		else {
			day++;
		}
		Pref_r[i] = day;
	}

	for (int i = 1; i <= n; i++) {
		if (min(Pref_l[i], Pref_r[i]) <= k) {
			cout << '*';
		}
		else {
			cout << '.';
		}
	}
}
