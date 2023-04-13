#ifndef SAVE_HPP
#define SAVE_HPP

#include <cstring>
#include <fstream>
#include <iostream>

namespace cay {

template < class T > 
class save {
  private:
    std::fstream file;
  public:
    save () {
        std::filesystem::create_directories("data");
    }
    save ( const char* file_name, std::ios_base::openmode mode ) {
        std::filesystem::create_directories("data");
        file.open(file_name, std::ios::app);
        file.close();
        file.open(file_name, mode);
    }

    ~save () {}

    void open_file ( const char* file_name, std::ios_base::openmode mode ) {
        file.open(file_name, std::ios::app);
        file.close();
        file.open(file_name, mode);
    }

    void read ( T& val, int pos ) {
        file.read(reinterpret_cast<char*>(&val), sizeof(T));
    }

    void write ( int pos, const T& data ) {
        file.seekp(sizeof(T)*(pos-1));
        file.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

};

}

#endif 