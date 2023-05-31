#include "console.hpp"

cay::console con;

int main () {
    std::ios::sync_with_stdio(false);
    
    while ( con.solve() );

    // for ( int i=1 ; i<=1000 ; i++ ) 
    //     con.solve();
}