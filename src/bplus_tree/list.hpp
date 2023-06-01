#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "save.hpp"

namespace cay {

template < class T >
class list {
    friend class console;
  private:
    static const int M=1e5+10;
    int nxt[M], pre[M], head, tail, cnt;
    int stk[M], top;
    T dat[M];
    database<T> sav;

    int newNode ( const T& data ) { 
        int nod=top?stk[top--]:++cnt;
        dat[nod]=data;
        return nod;
    }
    void clear ( int x ) { 
        nxt[x]=pre[x]=0;
        stk[++top]=x;
    }
  public:
    
    list () {
        memset(nxt, 0, sizeof(nxt));
        memset(pre, 0, sizeof(pre));
        top=cnt=0;
    }
    void open ( const std::string path, const std::string name ) { 
        sav.open(path, name); 
        cnt=sav.size();
        for ( int i=1 ; i<=cnt ; i++ ) {
            dat[i]=sav.read(i);
            nxt[i]=i+1;
            pre[i]=i-1;
        } 
        if ( sav.size()>=1 ) head=1, tail=cnt, nxt[cnt]=0;
        else head=tail=0;
        sav.clear();
    }
    list ( const std::string path, const std::string name ) { open(path, name); }
    void close () { 
        for ( int i=head ; i ; i=nxt[i] ) 
            sav.push_back(dat[i]);
        sav.close(); 
        head=tail=0;
    }
    ~list () { close(); }

    void push_back ( const T& data ) {
        if ( !head ) head=tail=newNode(data);
        else {
            int nod=newNode(data);
            nxt[tail]=nod;
            pre[nod]=tail;
            tail=nod;
        }
        return ;
    }
        
    void erase ( int x ) {
        if ( head==tail ) head=tail=0;
        else {
            if ( pre[x] ) nxt[pre[x]]=nxt[x];
            if ( nxt[x] ) pre[nxt[x]]=pre[x];
            if ( x==head ) head=nxt[x];
            if ( x==tail ) tail=pre[x];
        }
        clear(x);
        return ;
    }

};

}

#endif