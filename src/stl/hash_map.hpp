#ifndef SJTU_HASH_MAP_HPP
#define SJTU_HASH_MAP_HPP

#include <cstring>

#include "utility.hpp"

namespace cay {

template < class T >
class hash_map {
  private:
    static const int M=2e3+10, N=2e3;
    int nxt[M], key[M], lst[M], cnt;
    int stk[M], top;
    T dat[M];
    void add ( int x, int u, const T& v ) {
        int nod;
        if ( top ) nod=stk[top--];
        else nod=++cnt;
        nxt[nod]=lst[x], lst[x]=nod, key[cnt]=u, dat[nod]=v; 
    }

    int hash ( int x ) { return x%N; }
  public:
    hash_map () {
        cnt=top=0;
        memset(nxt, 0, sizeof(nxt));
        memset(key, 0, sizeof(key));
        memset(dat, 0, sizeof(dat));
        memset(lst, 0, sizeof(lst));
        memset(stk, 0, sizeof(stk));
    }

    void insert ( int x, const T& y ) { add(hash(x), x, y); }
    void modify ( int x, const T& y ) {
        int hs=hash(x);
        for ( int pre=0, i=lst[hs] ; i ; pre=i, i=nxt[i] ) 
            if ( key[i]==x ) {
                dat[i]=y;
                return ;
            }
        return ;
    }
    void erase ( int x ) {
        int hs=hash(x);
        for ( int pre=0, i=lst[hs] ; i ; pre=i, i=nxt[i] ) 
            if ( key[i]==x ) {
                if ( !pre ) lst[hs]=nxt[i];
                else nxt[pre]=nxt[i];
                stk[++top]=i;
                return ;
            }
        return ;
    }
    pair<T, bool> find ( int x ) {
        for ( int i=lst[hash(x)] ; i ; i=nxt[i] ) 
            if ( key[i]==x ) return pair<T, bool>(dat[i], 1);
        return pair<T, bool>(T(), 0);
    }
};

}

#endif