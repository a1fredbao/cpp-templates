#ifndef AFDS_HASHMAP
#define AFDS_HASHMAP

#include "splitmix.hpp"
#include <utility>
#include <vector>

template <class K, class V>
struct HashMap { // HashMap for integer keys.
    using u32 = unsigned int;
    struct Node {
        K key;
        V val;
        u32 ne;
    };
    Splitmix<K> hash;
    u32 lim, mask, tot;
    std::vector<u32> fi;
    std::vector<Node> e;
    inline void init(u32 sz) {
        u32 lim = 1;
        for (sz++; lim < sz; lim <<= 1);
        fi.assign(lim, 0), tot = 0;
        mask = lim - 1, e.resize(sz);
    }
    inline void inc(K x, const V &delta) {
        u32 u = hash(x) & mask;
        for (u32 i = fi[u]; i; i = e[i].ne) {
            if (e[i].key == x) {
                e[i].val += delta;
                return;
            }
        }
        e[++tot] = {x, V() + delta, fi[u]}, fi[u] = tot;
    }
    inline void set(K x, V v) {
        u32 u = hash(x) & mask;
        for (u32 i = fi[u]; i; i = e[i].ne) {
            if (e[i].key == x) {
                e[i].val = v;
                return;
            }
        }
        e[++tot] = {x, v, fi[u]}, fi[u] = tot;
    }
    // this method does not create new element, returns V() by default.
    inline V get(K x) {
        u32 u = hash(x) & mask;
        for (u32 i = fi[u]; i; i = e[i].ne) {
            if (e[i].key == x) return e[i].val;
        }
        return V();
    }
    // this method creates new element.
    inline V &operator[](K x) {
        u32 u = hash(x) & mask;
        for (u32 i = fi[u]; i; i = e[i].ne) {
            if (e[i].key == x) return e[i].val;
        }
        e[++tot] = {x, V(), fi[u]}, fi[u] = tot;
        return e[tot].val;
    }
};

#endif
