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
    srand(unsigned(time(0)));

    ofstream fin("test.in");
    ofstream fout("test.ans");

    int n=10000;

    fin << n <<'\n'; 
    while ( n-- ) {
        int op=rand()%4;
        int d=rand()%10;
        char c='a'+d;
        int x=rand()%100;
        if ( op<=2 ) {
            if ( s[d].find(x)==s[d].end() ) {
                s[d].insert(x);
                fin <<"i "<< c <<" "<< x <<"\n";
            }
            else {
                auto it=s[d].find(x);
                s[d].erase(it);
                fin <<"d "<< c <<" "<< x <<"\n";
            }
        }
        if ( op==3 ) {
            fin <<"f "<< c <<"\n";
            if ( s[d].empty() ) fout <<"null\n";
            else {
                for ( auto it=s[d].begin() ; it!=s[d].end() ; ++it )
                    fout << *it <<' ';
                fout <<'\n';
            }
        }
    }
    return 0;
}