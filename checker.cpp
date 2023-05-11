#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

#include <set>

using namespace std; 

int p[100010];

multiset<int> s[26];

int main () {
    int n ;
    cin >> n ; 
    while ( n-- ) {
        char op, c;
        int x;
        cin >> op >> c ;
        int d=c-'a';
        if ( op=='i' ) {
            cin >> x ;
            s[d].insert(x);
        }
        if ( op=='d' ) {
            cin >> x ;
            auto it=s[d].find(x);
            if ( it!=s[d].end() ) s[d].erase(it);
        }
        if ( op=='f' ) {
            if ( s[d].empty() ) cout <<"null\n";
            else {
                for ( auto it=s[d].begin() ; it!=s[d].end() ; ++it )
                    cout << *it <<' ';
                cout <<'\n';
            }
        }
    }

    int cnt=0;
    for ( int d=0 ; d<26 ; d++ ) {
        for ( auto it=s[d].begin() ; it!=s[d].end() ; ++it ) 
            cout << ++cnt <<' '<< char(d+'a') <<' '<< *it <<'\n';
    }
    return 0;
}