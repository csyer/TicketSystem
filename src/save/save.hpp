#ifndef SAVE_HPP
#define SAVE_HPP

#include <fstream>
#include <iostream>
#include <string>

#include "exceptions.hpp"
#include "vector.hpp"

namespace cay {

template < class T > 
class save {
  private:
    std::fstream sav, rec;
    vector<int> bin;
    std::string name;
    int siz;
    
    void touch ( std::fstream& file, const std::string& file_name, 
                std::ios_base::openmode mode=std::ios::in|
                                             std::ios::out|
                                             std::ios::binary ) {
        std::ifstream checker("data/"+file_name+".dat");
        if ( !checker.is_open() ) 
            std::ofstream create("data/"+file_name+".dat");
        file.open("data/"+file_name+".dat", mode);
    }
  public:
    save () {}

    void open ( const std::string& file_name, 
                std::ios_base::openmode mode=std::ios::in|
                                             std::ios::out|
                                             std::ios::binary ) {

        name=file_name;
        touch(sav, file_name);
        sav.seekg(0,std::ios::end);
        siz=sav.tellg()/sizeof(T);

        touch(rec, file_name+"_rec");

        rec.seekg(0,std::ios::beg);
        for ( int i=0 ; !rec.eof() ; i++ ) {
            int val;
            rec.read(reinterpret_cast<char*>(&val), sizeof(int));
            if ( !rec.eof() ) bin.push_back(val);
        }
        rec.close();

        std::ofstream reset("data/"+file_name+"_rec.dat", std::ios::out|std::ios::trunc);
        reset.close();
    }
    save ( const std::string& file_name, 
           std::ios_base::openmode mode=std::ios::in|
                                        std::ios::out|
                                        std::ios::binary ) {
        open(file_name, mode);
    }

    void close () {
        sav.close();
    }
    ~save () {
        close();
        touch(rec, name+"_rec");
        rec.seekp(0, std::ios::beg);
        for ( int i=0 ; i<(int)bin.size() ; i++ ) 
            rec.write(reinterpret_cast<char*>(&bin[i]), sizeof(int));
        bin.clear();
        rec.close();
    }

    T read ( int pos ) {
        sav.seekg(0, std::ios::end);
        if ( (int)(sav.tellg())<(int)sizeof(T)*pos ) throw read_eof();
        T res;
        sav.seekg(sizeof(T)*(pos-1));
        sav.read(reinterpret_cast<char*>(&res), sizeof(T));
        return res;
    }
    void write ( int pos, const T& data ) {
        sav.seekp(sizeof(T)*(pos-1));
        sav.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    bool empty() {
        sav.seekg(0,std::ios::end);
        return sav.tellg()==0;
    }
    void clear ( int pos ) {
        T tmp=T();
        sav.seekp(sizeof(T)*(pos-1));
        sav.write(reinterpret_cast<char*>(&tmp), sizeof(T));

        bin.push_back(pos);
    }
    int insert ( const T& data ) {
        int pos;
        if ( !bin.empty() ) pos=bin.back(), bin.pop_back();
        else pos=++siz;

        this->write(pos,data);
        return pos;
    }

};

}

#endif 