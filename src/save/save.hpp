#ifndef SAVE_HPP
#define SAVE_HPP

#include <fstream>
#include <iostream>
#include <string>

#include "exceptions.hpp"
#include "hash_map.hpp"
#include "utility.hpp"

namespace cay {

const std::ios_base::openmode default_mode=std::ios::in|std::ios::out|std::ios::binary;

template < class T >
class LRU {
  private:
    static const int M=std::max(100ul, 4000/sizeof(T)), N=M+5;
    hash_map<int> mp; // pos -> list pos
    int nxt[N], pre[N], idx[N], head, tail, siz;
    T dat[N];

    void erase ( int pos ) {
        if ( pos==tail ) tail=pre[pos];
        if ( pre[pos] ) nxt[pre[pos]]=nxt[pos];
        if ( nxt[pos] ) pre[nxt[pos]]=pre[pos];
        nxt[pos]=pre[pos]=0;
    }
    void forward ( int pos ) {
        if ( !head ) return (void)(head=pos);
        if ( pos==head ) return ;
        erase(pos);
        nxt[pos]=head;
        pre[head]=pos;
        head=pos;
    }
    void pop ( int pos, const T& data ) {
        sav.seekp(sizeof(T)*(pos-1));
        sav.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    void touch ( std::fstream& file, const std::string& file_name, 
                std::ios_base::openmode mode=default_mode ) {
        std::ifstream checker(file_name);
        if ( !checker.is_open() ) 
            std::ofstream create(file_name);
        file.open(file_name, mode);
    }  

  public: 
    LRU () { reset(); }
    LRU ( const std::string& file_name, std::ios_base::openmode mode=default_mode ) {
        reset();
        open(file_name, mode);
    }
    ~LRU () {
        for ( int i=head ; i ; i=nxt[i] ) {
            pop(idx[i], dat[i]);
            // std::cerr <<"delete "<< i <<'\n';
        }
        sav.close();
    }

  protected:
    std::fstream sav;

    void open ( const std::string& file_name, std::ios_base::openmode mode=default_mode ) {
        touch(sav, file_name, mode);
    }
    void reset () {
        memset(nxt, 0, sizeof(nxt));
        memset(pre, 0, sizeof(pre));
        memset(dat, 0, sizeof(dat));
        memset(idx, 0, sizeof(idx));
        head=siz=0, tail=1;
    }

    pair<T, bool> find ( int x ) {
        auto pr=mp.find(x);
        if ( !pr.second ) return {T(), 0};
        forward(pr.first);
        return {dat[pr.first], 1};
    }
    void insert ( int x, const T& data ) {
        int pos;
        if ( siz==M ) {
            pop(idx[tail], dat[tail]);
            mp.erase(idx[tail]);
            mp.insert(x, pos=tail);
        }
        else mp.insert(x, pos=++siz);
        dat[pos]=data, idx[pos]=x;
        forward(pos);
    }
    void replace ( int x, const T& data ) {
        auto pr=mp.find(x);
        if ( !pr.second ) insert(x, data);
        else {
            dat[pr.first]=data;
            forward(pr.first);
        }
    }
};

template < class T > 
class save : public LRU<T> {
  private:
    std::string name;
    int siz;
    std::ios_base::openmode mode;

    int get_address () { return ++siz; }

  public:
    save () {}
    void open ( const std::string& file_name, std::ios_base::openmode file_mode=default_mode ) {
        mode=file_mode;
        LRU<T>::open(file_name, mode);
        LRU<T>::sav.seekg(0, std::ios::end);
        siz=LRU<T>::sav.tellg()/sizeof(T);
    }
    save ( const std::string& file_name, std::ios_base::openmode file_mode=default_mode ) {
        open(file_name, file_mode);
    }

    void close () {}
    ~save () {}

    T read ( int pos ) {
        if ( pos<1 || pos>siz ) throw read_eof();
        T res;
        auto pr=LRU<T>::find(pos);
        if ( !pr.second ) {
            LRU<T>::sav.seekg(sizeof(T)*(pos-1));
            LRU<T>::sav.read(reinterpret_cast<char*>(&res), sizeof(T));
            LRU<T>::insert(pos, res);
        }
        else res=pr.first;
        return res;
    }
    void write ( int pos, const T& data ) {
        if ( !pos ) return ;
        LRU<T>::replace(pos, data);
    }

    bool empty() { return siz==0; }

    void clear ( int pos ) {}
    int insert ( const T& data ) {
        int pos=get_address();
        write(pos,data);
        return pos;
    }

    void clear () {
        LRU<T>::sav.close();
        std::fstream clr_sav(name, std::ios::out|std::ios::binary|std::ios::trunc);
        clr_sav.seekp(0, std::ios::beg);
        clr_sav.close();
        LRU<T>::open(name, mode);
        siz=0;
    }
};

template < class T >
class database {
  public:
    database () {}
    void open ( const std::string& file_path, const std::string& file_name ) {
        name=file_path+file_name;
        std::filesystem::create_directories(file_path);
        std::ifstream checker(name);
        if ( !checker.is_open() ) {
            std::ofstream create(name);
            create.close();
        }
        sav.open(name, default_mode);

        sav.seekg(0, std::ios::end);
        siz=sav.tellg()/sizeof(T);
    }
    database ( const std::string& file_name ) { open(file_name); }
    void close () { sav.close(); }
    ~database () { close(); }

    T read ( int pos ) {
        T ret;
        sav.seekg(sizeof(T)*(pos-1));
        sav.read(reinterpret_cast<char*>(&ret), sizeof(T));
        return ret;
    }
    void write ( int pos, const T& data ) {
        sav.seekp(sizeof(T)*(pos-1));
        sav.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    // template < class U >
    // void get_header ( U& x ) {
    //     sav.seekg(0, std::ios::beg);
    //     sav.read(reinterpret_cast<char*>(&x), offset);
    // }
    // template < class U >
    // void put_header ( const U& x ) {
    //     sav.seekg(0, std::ios::beg);
    //     sav.write(reinterpret_cast<const char*>(&x), offset);
    // }

    int push_back ( const T& data ) {
        write(++siz, data);
        return siz;
    }
    int size () { return siz; }

    void clear () {
        sav.close();
        std::fstream clr_sav(name, std::ios::out|std::ios::binary|std::ios::trunc);
        clr_sav.seekp(0, std::ios::beg);
        clr_sav.close();
        sav.open(name, default_mode);
        siz=0;
    }
  private:
    std::fstream sav;
    std::string name;
    int siz;
};

}

#endif 