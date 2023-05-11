#ifndef SAVE_HPP
#define SAVE_HPP

#include <fstream>
#include <iostream>
#include <string>

#include "exceptions.hpp"
#include "hash_map.hpp"

namespace cay {

template < class T >
class cache {
  private:
    static const int N=1e4+10;
    hash_map<int> mp; 
    int nxt[N], pre[N], head;
    T dat[N];

    void forward ( int pos ) {
        nxt[pre[pos]]=nxt[pos];
        pre[nxt[pos]]=pre[pos];
        pre[pos]=0;
        nxt[pos]=head;
        pre[head]=pos;
        head=pos;
    }
  public:
    cache () { 
        memset(nxt, 0, sizeof(nxt));
        memset(pre, 0, sizeof(pre));
        memset(dat, 0, sizeof(dat));
        head=0;
    }

    T access ( int x ) {
        auto pr=mp.find(x);
        forward(pr.first);
        return 
    }
    void access ( int x, const T& data ) {
        forward(pos);
    }
};

template < class T > 
class save {
  private:
    cache<T> ca;
    std::fstream sav;
    std::string name, path;
    int siz;
    
    void touch ( std::fstream& file, const std::string& file_name, const std::string& file_path, 
                std::ios_base::openmode mode=std::ios::in|
                                             std::ios::out|
                                             std::ios::binary ) {
        std::ifstream checker(path+file_name+".dat");
        if ( !checker.is_open() ) 
            std::ofstream create(path+file_name+".dat");
        file.open(path+file_name+".dat", mode);
    }
  public:
    save () {}

    void open ( const std::string& file_name, const std::string& file_path="data/", 
                std::ios_base::openmode mode=std::ios::in|
                                             std::ios::out|
                                             std::ios::binary ) {
        name=file_name, path=file_path;
        touch(sav, file_name, file_path, mode);
        sav.seekg(0,std::ios::end);
        siz=sav.tellg()/sizeof(T);
    }
    save ( const std::string& file_name, const std::string& file_path="data/", 
           std::ios_base::openmode mode=std::ios::in|
                                        std::ios::out|
                                        std::ios::binary ) {
        open(file_name, file_path, mode);
    }

    void close () {
        sav.close();
    }
    ~save () {
        close();
    }

    T read ( int pos ) {
        if ( pos<1 || pos>siz ) throw read_eof();
        auto pr=ca.access(pos);
        if ( pr.second ) return pr.first;
        T res;
        sav.seekg(sizeof(T)*(pos-1));
        sav.read(reinterpret_cast<char*>(&res), sizeof(T));
        return res;
    }
    void write ( int pos, const T& data ) {
        if ( !pos ) return ;
        if ( ca.find(pos) ) ca.access(pos, data);
        sav.seekp(sizeof(T)*(pos-1));
        sav.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    bool empty() {
        return siz==0;
    }

    void clear ( int pos ) {}
    int get_address () {
        return ++siz;
    }
    int insert ( const T& data ) {
        int pos=get_address();
        write(pos,data);
        return pos;
    }

    void clear () {
        close();
        std::fstream clr_sav(path+name+".dat", std::ios::out|std::ios::binary|std::ios::trunc);
        clr_sav.seekp(0, std::ios::beg);
        clr_sav.close();
    }

};

}

#endif 