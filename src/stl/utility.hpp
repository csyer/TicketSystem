#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <cstring>
#include <iostream>

namespace cay {

template < const int MAX_SIZE >
class string {
    char s[MAX_SIZE+1];
  public:
    string () { memset(s, 0, sizeof(s)); }
    string ( const string<MAX_SIZE>& obj ) {
        strncpy(s, obj.s, MAX_SIZE);
        s[MAX_SIZE]=0;
    }
    string ( const char* rhs ) {
        memset(s, 0, sizeof(s));
        int len=std::min((int)std::strlen(rhs), MAX_SIZE);
        strncpy(s, rhs, len); s[len]=0;
    }

    string& operator= ( const string<MAX_SIZE>& obj ) {
        if ( this==&obj ) return *this;
        memset(s, 0, sizeof(s));
        strncpy(s, obj.s, MAX_SIZE);
        s[MAX_SIZE]=0;
        return *this;
    }
    string& operator= ( const char* rhs ) {
        memset(s, 0, sizeof(s));
        int len=std::min((int)std::strlen(rhs), MAX_SIZE);
        strncpy(s, rhs, len); s[len]=0;
        return *this;
    }

    void print () const { 
        std::cerr << s <<' ';
    }

    bool operator== ( const string& obj ) const {
        return !strcmp(s, obj.s);
    }
    bool operator!= ( const string& obj ) const {
        return strcmp(s, obj.s);
    }
    bool operator< ( const string& obj ) const {
        for ( int i=0 ; i<MAX_SIZE ; i++ ) 
            if ( s[i]!=obj.s[i] ) return s[i]<obj.s[i];
        return 0;
    }

    const std::string str () const { return s; }

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
    bool operator== ( const pair<T1, T2>& obj ) const {
        return first==obj.first && second==obj.second;
    }

};

bool is_digit ( char c ) { return c>='0' && c<='9'; }
int to_int ( const std::string& str ) {
    int ret=0, len=str.length();
    for ( int i=0 ; is_digit(str[i]) && i<len ; i++ ) ret=ret*10+str[i]-'0';
    return ret;
}
std::string to_string ( int num ) {
    std::string ret="";
    int flg=0;
    if ( num<0 ) flg=1, num=-num;
    for ( ; num ; num/=10 ) ret=char(num%10+'0')+ret;
    if ( flg ) return "-"+ret;
    return ret;
}
std::string to_string ( const char* s ) { return s; }

}

#endif