#ifndef AFMT_LINEAR
#define AFMT_LINEAR

#include <algorithm>
#include <array>
#include <cassert>
#include <limits>
#include <vector>

template <class T>
struct Matrix {
	std::vector<std::vector<T>> M;
	explicit Matrix(std::vector<std::vector<T>> _M) : M(_M) {}
	explicit Matrix(int n, int m, T val = T()) : M(n, std::vector<T>(m, val)) {}
	// Generate a diagonal matrix.
	explicit Matrix(std::vector<T> diag)
	    : M(diag.size(), std::vector<T>(diag.size())) {
		for (size_t i = 0; i < diag.size(); i++) {
			M[i][i] = diag[i];
		}
	}
	// Generate a unit matrix.
	explicit Matrix(int n) : M(n, std::vector<T>(n)) {
		for (int i = 0; i < n; i++) M[i][i] = 1;
	}
	inline std::vector<T> &operator[](int n) { return M[n]; }
	inline size_t n(void) { return M.size(); }
	inline size_t m(void) { return M.empty() ? 0 : M[0].size(); }
};

template <class T>
Matrix<T> operator*(Matrix<T> A, Matrix<T> B) {
	assert(A.m() == B.n());
	Matrix<T> ans(A.n(), B.m());
	for (size_t i = 0; i < A.n(); i++) {
		for (size_t k = 0; k < A.m(); k++) {
			const T &Aik = A[i][k];
			for (size_t j = 0; j < B.m(); j++) {
				ans[i][j] += Aik * B[k][j];
			}
		}
	}
	return ans;
}

template <class T>
std::vector<T> operator*(Matrix<T> A, std::vector<T> B) {
	assert(A.m() == B.size());
	std::vector<T> ans(A.n());
	for (size_t i = 0; i < A.n(); i++) {
		for (size_t j = 0; j < A.m(); j++) {
			ans[i] += A[i][j] * B[j];
		}
	}
	return ans;
}

template <class T>
Matrix<T> power(Matrix<T> M, long long index) {
	assert(M.n() == M.m());
	Matrix<T> ans(M.n());
	while (index) {
		if (index & 1) ans = ans * M;
		index >>= 1, M = M * M;
	}
	return ans;
}

template <class T>
struct XORBasis {
	constexpr static T mx = std::numeric_limits<T>::max();
	constexpr static int C = std::numeric_limits<T>::digits;

	int siz = 0;
	std::array<T, C> p{};
	bool has_zero = false;
	// Insert x to the basis.
	// Returns: successfully inserted to which digit.
	inline int insert(T x) {
		if (x == 0) has_zero = true;
		for (int i = C - 1; i >= 0; i--) {
			if (!(x >> i & 1)) continue;
			if (p[i] == 0) {
				p[i] = x, siz++;
				return i;
			} else x ^= p[i];
		}
		has_zero = true;
		return -1;
	}
	inline T max(T ans = 0) {
		for (int i = C - 1; i >= 0; i--) {
			ans = std::max(ans, ans ^ p[i]);
		}
		return ans;
	}
	inline T min(T ans) {
		for (int i = C - 1; i >= 0; i--) {
			ans = std::min(ans, ans ^ p[i]);
		}
		return ans;
	}
	inline int size(void) { return siz; }
	std::vector<T> rebuild(void) const {
		std::array<T, C> basis = p;
		for (int i = C - 1; i >= 0; i--) {
			for (int j = 0; j < i; j++) {
				if (basis[i] >> j & 1) basis[i] ^= basis[j];
			}
		}
		std::vector<T> narr;
		narr.reserve(siz);
		for (int i = 0; i < C; i++) {
			if (basis[i] != 0) narr.push_back(basis[i]);
		}
		assert(narr.size() == (size_t)siz);
		return narr;
	}
	inline T kth(size_t k) { // kth minimum
		T ans = 0;
		assert(k >= 1);
		auto narr = rebuild(); // narr[0] = smallest by MSB; narr[i] corresponds
		                       // to bit i in k's binary (LSB -> narr[0])
		if (has_zero) {
			if (k == 1) return 0;
			else --k;
		}
		assert(k < (1ull << siz));
		// map bit i of k to narr[i] (LSB -> narr[0])
		for (int i = 0; i < siz; i++) {
			if ((k >> i) & 1ULL) ans ^= narr[i];
		}
		return ans;
	}
	inline size_t rank(T x) const {
		size_t ans = 0;
		auto narr = rebuild(); // narr ordered by increasing MSB
		for (int i = (int)narr.size() - 1; i >= 0; --i) {
			T b = narr[i];
			int hb = std::__lg(b);
			if (((x >> hb) & 1U) != 0) {
				x ^= b;
				ans |= (1ULL << i);
			}
		}
		// after reduction, x should be 0 iff representable
		assert(x == 0 && "rank(x): x is not representable by this basis");
		return ans + (has_zero ? 1ULL : 0ULL); // 1-based
	}
};

#endif // !AFMT_LINEAR
