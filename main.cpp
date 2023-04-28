#include <cstring>
#include <iostream>

#include "bplus_tree.hpp"

int main () {
    using pr=cay::pair<cay::string<65>, int>;

    cay::bplus_tree<pr, int> T;

    try {

    int n;
    scanf("%d",&n);
    while ( n-- ) {
        char op[10];
        char* _index=new char[65];
        int value;
        scanf("%s %s", op, _index);

        cay::string<65> index(reinterpret_cast<const char*>(_index));

        if ( op[0]=='i' ) {
            scanf("%d",&value);
            T.insert(pr(index, value), 0);
        }
        if ( op[0]=='d' ) {
            scanf("%d",&value);
            T.erase(pr(index, value));
        }
        if ( op[0]=='f' ) {
            cay::vector<pr> ret=T.find(pr(index, -1), pr(index, 2147483647));
            if ( ret.empty() ) printf("null");
            for ( auto it=ret.begin() ; it!=ret.end() ; ++it ) 
                printf("%d ",(*it).second);
            putchar('\n');
        }

        if ( op[0]=='g' ) T.debug(1);

        delete[] _index;
    }

    }
    catch ( ... ) {
        std::cerr <<"ERROR!\n";
    }

    // T.clear();

    return 0;
}