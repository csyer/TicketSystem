#ifndef SAVE_HPP
#define SAVE_HPP

#include <fstream>
#include <iostream>
#include <string>

namespace cay {

class file_exception {
  protected:
    const std::string variant="";
    std::string detail="";
  public:
    file_exception () {}
    file_exception ( const file_exception& obj ):
        variant(obj.variant), detail(obj.detail) {}
    
    virtual std::string what () {
        return variant+" "+detail;
    }
};

class read_eof : public file_exception {};

template < class T > 
class save {
  private:
    std::fstream file;
  public:
    save () {}

    void open ( const std::string& file_name, 
                std::ios_base::openmode mode=std::ios::in|
                                             std::ios::out|
                                             std::ios::binary ) {
        std::ifstream checker("data/"+file_name+".dat");
        if ( !checker.is_open() ) 
            std::ofstream create("data/"+file_name+".dat");
        file.open("data/"+file_name+".dat", mode);
    }
    save ( const std::string& file_name, 
           std::ios_base::openmode mode=std::ios::in|
                                        std::ios::out|
                                        std::ios::binary ) {
        this->open_file(file_name, mode);
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
    }

    void close
};

}

#endif 