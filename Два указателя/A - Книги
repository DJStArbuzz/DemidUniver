#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

using namespace std;


int main()
{
    int n, t;
    cin >> n >> t;
   
    vector<long long> list(n);
    for (int i = 0; i < n; i++) {
        cin >> list[i];
    }

    int countBook = 0;
    long long sumTime = 0;
    int countBufBook = 0;
    /*
    vector<long long> Pref(n + 1);
    for (int i = 1; i <= n; i++) {
        Pref[i] = Pref[i - 1] + list[i - 1];
    }

    for (int l = 0; l < n; l++) {
        for (int r = l; r < n; r++) {
            sumTime = Pref[r] - Pref[l - 1];

            if (sumTime <= t) {
                countBook = max(countBook, r - l + 1);
            }
        }
    }
    */

    int l = 0;
    for (int r = 0; r < n; r++) {
        sumTime += list[r];
        while (sumTime > t) {
            sumTime -= list[l];
            l++;
        }

        countBook = max(countBook, r - l + 1);
    }

    cout << countBook;

}
