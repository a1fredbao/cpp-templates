/**   int128 输出流自定义
 *    2023-03-20: https://codeforces.com/contest/1806/submission/198413531
 **/
using i128 = __int128;

std::ostream &operator<<(std::ostream &os, i128 n) {
    std::string s;
    while (n) {
        s += '0' + n % 10;
        n /= 10;
    }
    std::reverse(s.begin(), s.end());
    return os << s;
}

std::istream &operator>>(std::istream &is, i128 &n) {
    std::string s;
    is >> s;
    for (auto &c : s) {
        n = n * 10 + c - '0';
    }
    return is;
}

i128 toi128(const std::string &s) {
    i128 n = 0;
    for (auto c : s) {
        n = n * 10 + (c - '0');
    }
    return n;
}

i128 sqrti128(i128 n) {
    i128 lo = 0, hi = 1E16;
    while (lo < hi) {
        i128 x = (lo + hi + 1) / 2;
        if (x * x <= n) {
            lo = x;
        } else {
            hi = x - 1;
        }
    }
    return lo;
}