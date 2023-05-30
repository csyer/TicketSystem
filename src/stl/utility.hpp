#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <cstring>
#include <iostream>

namespace cay {

std::string to_string ( int num, int bit=10 ) {
    std::string ret="";
    int flg=0;
    if ( num<0 ) flg=1, num=-num;
    for ( ; num ; num/=10, --bit ) ret=char(num%10+'0')+ret;
    while ( bit-- ) ret="0"+ret;
    if ( flg ) return "-"+ret;
    return ret;
}
std::string to_string ( const char* s ) { return s; }

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
        if ( rhs==nullptr ) return ; 
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
        if ( rhs==nullptr ) return *this;
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
    bool operator> ( const string& obj ) const {
        return obj<*this;
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

class Clock {
  public:
    Clock ( int _hour=0, int _minute=0 ):hour(_hour), minute(_minute) {}
    Clock ( const char* rhs ) {
        hour=(rhs[0]-'0')*10+(rhs[1]-'0');
        minute=(rhs[3]-'0')*10+(rhs[4]-'0');
    }

    Clock& operator= ( const Clock& obj ) {
        if ( this==&obj ) return *this;
        hour=obj.hour;
        minute=obj.minute;
        return *this;
    }

    Clock& operator+= ( const int delta ) {
        minute+=delta;
        hour+=minute/60;
        minute%=60;
        hour%=24;
        return *this;
    }
    Clock operator+ ( const int delta ) {
        Clock ret=*this;
        ret+=delta;
        return ret;
    }

    int operator- ( const Clock& rhs ) {
        return (rhs.hour*24+rhs.minute)-(hour*24+minute);
    }

    int add ( const int delta ) {
        minute+=delta;
        hour+=minute/60;
        minute%=60;
        int ret=hour/24;
        hour%=24;
        return ret;
    }

    std::string show () const {
        return to_string(hour, 2)+":"+to_string(minute, 2);
    }
  private:
    int hour, minute;
};
class Date {
  public:
    Date ( int _month=0, int _day=0 ):month(_month), day(_day) {}
    Date ( const char* rhs ) {
        month=(rhs[0]-'0')*10+(rhs[1]-'0');
        day=(rhs[3]-'0')*10+(rhs[4]-'0');
    }

    Date& operator= ( const Date& obj ) {
        if ( this==&obj ) return *this;
        month=obj.month;
        day=obj.day;
        return *this;
    }

    Date& operator+= ( const int delta ) {
        day+=delta;
        while ( day>months[month] ) {
            day-=months[month];
            month=month%12+1;
        }
        return *this;
    }
    Date operator+ ( const int delta ) {
        Date ret=*this;
        ret+=delta;
        return ret;
    }
    Date& operator-= ( const int delta ) {
        day-=delta;
        while ( day<1 ) {
            if ( month==1 ) month=12;
            else month--;
            day+=months[month];
        }
        return *this;
    }
    Date operator- ( const int delta ) {
        Date ret=*this;
        ret-=delta;
        return ret;
    }

    int operator- ( const Date& rhs ) {
        return (sumday[rhs.month-1]+rhs.day)-(sumday[month-1]+day);
    }

    bool operator== ( const Date& obj ) const {
        return month==obj.month && day==obj.day;
    }
    bool operator< ( const Date& obj ) const {
        return month<obj.month || ( month==obj.month && day<obj.day );
    }
    bool operator> ( const Date& obj ) const {
        return obj<*this;
    }
    bool operator<= ( const Date& obj ) const {
        return *this<obj || *this==obj;
    }
    bool operator>= ( const Date& obj ) const {
        return obj<=*this;
    }

    std::string show () const {
        return to_string(month, 2)+"-"+to_string(day, 2);
    }
  private:
    const int months[13]={0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const int sumday[13]={0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
    int month, day;
};

class Time {
  public:
    Time () {}
    Time ( Date _d, Clock _c ):d(_d), c(_c) {}

    Time& operator= ( const Time& obj ) {
        if ( this==&obj ) return *this;
        d=obj.d;
        c=obj.c;
        return *this;
    }

    Time& operator+= ( const int delta ) {
        d+=c.add(delta);
        return *this;
    }
    Time operator+ ( const int delta ) {
        Time ret=*this;
        ret+=delta;
        return ret;
    }

    int operator - ( const Time& rhs ) {
        return (d-rhs.d)*24*60+(c-rhs.c);
    }

    Clock get_clock () const { return c; }
    Date get_date () const { return d; }
    std::string show () const { return d.show()+" "+c.show(); }
  private:
    Date d;
    Clock c;
};

}

#endif