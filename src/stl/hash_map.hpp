#ifndef SJTU_HASH_MAP_HPP
#define SJTU_HASH_MAP_HPP

template < class T >
class hash_map {
  private:
    static const int M=1e5+10, N=1e5;
    int nxt[M], key[M], lst[M], cnt;
    T dat[M];
    void add ( int x, int u, const T& v ) {
        ++cnt, nxt[cnt]=lst[x], lst[x]=cnt, key[cnt]=u, dat[cnt]=v; 
    }

    int hash ( int x ) { return x%N; }
  public:
    hash_map (): {
        cnt=0;
        memset(nxt, 0, sizeof(nxt));
        memset(key, 0, sizeof(key));
        memset(dat, 0, sizeof(dat));
        memset(lst, 0, sizeof(lst));
    }

    void insert ( int x, const T& y ) { add(hash(x), x, y); }
    void erase ( int x ) {
        int hs=hash(x);
        for ( int pre=0, i=lst[hs] ; i ; pre=i, i=nxt[i] ) 
            if ( key[i]==x ) {
                if ( i==lst[hs] ) lst[hs]=nxt[i];
                else nxt[pre]=nxt[i];
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

#endif