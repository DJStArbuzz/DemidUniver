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

	int countElem = 1e7 + 10;
	// Решето Эратосфена
	vector<int> A(countElem);
	vector<int> Pref(countElem);

	for (int i = 2; i < countElem; i++) {
		Pref[i] = Pref[i - 1];
		if (A[i] == 0) {
			Pref[i]++;
			for (int j = 2 * i; j < countElem; j += i) {
				A[j] = 1;
			}
		}
	}

	int l, r;
	for (int i = 0; i < n; i++) {
		cin >> l >> r;
		cout << Pref[r] - Pref[l - 1] << endl;
	}
}
