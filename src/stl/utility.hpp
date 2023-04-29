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
    string () {}
    string ( const string& rhs ) {
        for ( int i=0 ; i<=MAX_SIZE ; i++ )
            s[i]=rhs.s[i];
    }
    string ( const char* rhs ) {
        int len=std::min(MAX_SIZE, (int)std::strlen(rhs));
        memcpy(s, rhs, len);
        memset(s+len, 0, MAX_SIZE-len);
    }

    // template < const int _MAX_SIZE >
    // string ( const string<_MAX_SIZE>& obj ) {
    //     int len=std::min(MAX_SIZE, _MAX_SIZE);
    //     memcpy(s, obj.s, len);
    //     memset(s+len, 0, MAX_SIZE-len);
    // }

    string& operator= ( const string& obj ) {
        if ( this==&obj ) return *this;
        memcpy(s, obj.s, MAX_SIZE);
        return *this;
    }

    void print () const { 
        std::cout << s <<' ';
    }

    bool operator== ( const string& obj ) const {
        return strcmp(s, obj.s)==0;
    }
    bool operator< ( const string& obj ) const {
        for ( int i=0 ; i<MAX_SIZE ; i++ ) 
            if ( s[i]!=obj.s[i] ) return s[i]<obj.s[i];
        return 0;
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