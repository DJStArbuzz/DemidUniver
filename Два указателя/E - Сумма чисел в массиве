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
	bool flag = false;
	vector<long long> list(n + 1);
	for (int i = 1; i <= n; i++) {
		cin >> list[i];
		if (list[i] > 0) {
			flag = true;
		}
	}

	if (flag == false) {
		int id = 1;
		int el = -1e7;
		
		for (int i = 1; i <= n; i++) {
			if (list[i] > el) {
				id = i;
				el = list[i];
			}
		}

		cout << id << " " << id;
	}
	else {
		long long left = 0, right = 0, l = 1;
		long long maxSum = -1;
		long long sum = 0;

		for (int r = 1; r <= n; r++) {
			sum += list[r];
			while (sum <= 0 && l <= r) {
				sum -= list[l];
				l++;
			}

			if (sum > maxSum) {
				maxSum = sum;
				left = l;
				right = r;
			}
		}

		cout << left << " " << right;
	}
}
