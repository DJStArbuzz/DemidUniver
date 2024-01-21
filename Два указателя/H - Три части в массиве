#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <vector>
#include <set>

using namespace std;

int main() {
	int n;
	cin >> n;
	
	vector<long long> A(n + 2);
	vector<long long> B(n + 2);
	vector<long long> C(n + 2);

	for (int i = 1; i <= n; i++) {
		cin >> A[i];
	}

	for (int i = 1; i <= n; i++) {
		B[i] = B[i - 1] + A[i];
	}

	for (int i = n; i >= 1; i--) {
		C[i] = C[i + 1] + A[i];
	}

	int r = n + 1;
	int res = 0;
	long long sum = 0;
	for (int l = 0; l < r; l++) {
		while (B[l] > C[r] && r > 0) {
			r--;
		}
		if (l < r) {
			if (B[l] == C[r]) {
				sum = B[l];
			}
		}
	}
	cout << sum;
}
