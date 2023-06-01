#ifndef SJTU_SYSTEM_HPP
#define SJTU_SYSTEM_HPP

#include <cstring>

namespace cay {

const int FAIL=-1;
const int SUCCESS=0;

class system {
  public:
    system () {}
  protected:
    char* get ( char* key[], char* arg[], int len, const char* src ) {
        for ( int i=0 ; i<len ; i++ )
            if ( !strcmp(key[i], src) ) return arg[i];
        return nullptr;
    }
};

bool default_int_cmp_less ( const int& x, const int& y ) { return x>y; }
bool default_int_cmp_greater ( const int& x, const int& y ) { return x<y; }

}

#endif