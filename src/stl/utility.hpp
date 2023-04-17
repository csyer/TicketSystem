#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

// #include <utility>

#include <cstring>
#include <iostream>

namespace cay {

template < const int MAX_SIZE >
class string {
    char s[MAX_SIZE+1];
  public:
    string () = default;
    string ( const string& obj ) = default;
    string ( const char* rhs ) {
        int len=std::min(MAX_SIZE, (int)std::strlen(rhs));
        memcpy(s, rhs, len);
        s[len]=0;
    }

    template < const int _MAX_SIZE >
    string ( const string<_MAX_SIZE>& obj ) {
        int len=std::min(MAX_SIZE, _MAX_SIZE);
        memcpy(s, obj.s, len);
        s[len]=0;
    }

    void print () { 
        std::cout << s <<std::endl;
    }

    ~string () = default;
};

template < class T1, class T2 >
class pair {
  public:
	T1 first;
	T2 second;
	constexpr pair () : first(), second() {}
	pair ( const pair& obj ) = default;
	pair ( pair&& obj ) = default;

	pair ( const T1& x, const T2& y ): first(x), second(y) {}
	template < class U1, class U2 >
	pair ( U1&& x, U2&& y ): first(x), second(y) {}

	template < class U1, class U2 >
	pair ( const pair<U1, U2>& obj ): first(obj.first), second(obj.second) {}
	template < class U1, class U2 >
	pair ( pair<U1, U2>&& obj ): first(obj.first), second(obj.second) {}

    ~pair () = default;

    pair<T1, T2>& operator= ( const pair<T1, T2>& obj ) {
        if ( this==&obj ) return *this;
        first=obj.first;
        second=obj.second;
        return *this;
    }
    bool operator< ( const pair<T1, T2>& obj ) const {
        if ( first==obj.first ) return second<obj.second;
        return first<obj.first;
    }

};

}

#endif