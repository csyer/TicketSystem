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

        // touch(rec, file_name+"_rec", file_path, mode);

        // rec.seekg(0,std::ios::beg);
        // for ( int i=0 ; !rec.eof() ; i++ ) {
        //     int val;
        //     rec.read(reinterpret_cast<char*>(&val), sizeof(int));
        //     if ( !rec.eof() ) bin.push_back(val);
        // }
        // rec.close();

        // std::ofstream reset(path+file_name+"_rec.dat", std::ios::out|std::ios::trunc);
        // reset.close();
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
        // touch(rec, name+"_rec", path);
        // rec.seekp(0, std::ios::beg);
        // for ( int i=0 ; i<(int)bin.size() ; i++ ) 
        //     rec.write(reinterpret_cast<char*>(&bin[i]), sizeof(int));
        // bin.clear();
        // rec.close();
    }

    T read ( int pos ) {
        // std::cerr <<"  read in "<< pos <<std::endl;
        // sav.seekg(0, std::ios::end);
        // std::cerr <<"    read check end "<< sav.tellg() <<' '<< sizeof(T) <<std::endl;
        if ( pos<1 || pos>siz ) throw read_eof();
        T res;
        sav.seekg(sizeof(T)*(pos-1));
        sav.read(reinterpret_cast<char*>(&res), sizeof(T));
        return res;
    }
    void write ( int pos, const T& data ) {
        // std::cerr <<"  write in "<< pos <<std::endl;
        if ( !pos ) return ;
        // sav.seekg(0, std::ios::end);
        // std::cerr <<"    write check end "<< sav.tellg() <<' '<< sizeof(T) <<std::endl;
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

        // bin.push_back(pos);
    }
    int get_address () {
        // int pos;
        // if ( !bin.empty() ) pos=bin.back(), bin.pop_back();
        // else pos=++siz;
        // return pos;
        return ++siz;
    }
    int insert ( const T& data ) {
        int pos=get_address();
        write(pos,data);
        return pos;
    }

    void clear () {
        close();
        // puts(("clear data/"+name+".dat").c_str());
        std::fstream clr_sav(path+name+".dat", std::ios::out|std::ios::binary|std::ios::trunc);
        std::fstream clr_rec(path+name+"_rec.dat", std::ios::out|std::ios::binary|std::ios::trunc);
        clr_sav.seekp(0, std::ios::beg);
        clr_rec.seekp(0, std::ios::beg);
        clr_sav.close();
        clr_rec.close();
    }

};

}

#endif 