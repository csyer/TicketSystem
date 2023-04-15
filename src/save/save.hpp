#ifndef SAVE_HPP
#define SAVE_HPP

#include <fstream>
#include <iostream>
#include <string>

#include "exceptions.hpp"

namespace cay {

template < class T > 
class save {
  private:
    std::fstream file, rec;
    int top, siz;
  public:
    save () {}

    void open ( const std::string& file_name, 
                std::ios_base::openmode mode=std::ios::in|
                                             std::ios::out|
                                             std::ios::binary ) {
        std::ifstream checker1("data/"+file_name+".dat");
        std::ifstream checker2("data/"+file_name+"_rec.dat");
        if ( !checker1.is_open() ) 
            std::ofstream create("data/"+file_name+".dat");
        if ( !checker2.is_open() ) 
            std::ofstream create("data/"+file_name+"_rec.dat");
        file.open("data/"+file_name+".dat", mode);
        rec.open("data/"+file_name+"_rec.dat", mode);
    }
    save ( const std::string& file_name, 
           std::ios_base::openmode mode=std::ios::in|
                                        std::ios::out|
                                        std::ios::binary ) {
        this->open(file_name, mode);
    }

    void close () {
        file.close();
    }
    ~save () {
        close();
    }

    T read ( const int pos ) {
        file.seekg(0,std::ios::end);
        if ( (int)file.tellg()<sizeof(T)*pos ) throw read_eof();
        T res=T();
        file.seekg(sizeof(T)*(pos-1));
        file.read(reinterpret_cast<char*>(&res), sizeof(T));
        return res;
    }
    void write ( int pos, const T& data ) {
        file.seekp(sizeof(T)*(pos-1));
        file.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    void clear ( int pos ) {
        T tmp=T();
        file.seekp(sizeof(T)*(pos-1));
        file.write(reinterpret_cast<char*>(&tmp), sizeof(T));

        rec.seekp(0,std::ios::end);
        rec.write(reinterpret_cast<char*>(&pos), sizeof(int));
    }
    int insert ( const T& data ) {
        int pos;
        if ( top ) {
            rec.seekg(sizeof(int)*(top--));
            rec.read(reinterpret_cast<char*>(&pos), sizeof(int));
        }
        else pos=++siz;

        this->write(pos,data);
        return pos;
    }

};

}

#endif 