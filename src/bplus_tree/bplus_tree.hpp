#ifndef BPLUS_TREE_HPP
#define BPLUS_TREE_HPP

#include <fstream>
#include <iostream>

namespace cay {

void open_file ( std::fstream& file, const char* file_name, std::ios_base::openmode mode ) {
    file.open(file_name,std::ios::app);
    file.close();
    file.open(file_name,mode);
}

template < class Key, class T, class Comp=std::less<Key> >
class bplus_tree {
  private:
    const int BLOCK_SIZE=2000; //2kb
    const static int M=5;
    struct node {
        int parent, left, right;
        int child[M];

        Key key[M-1];
        int addr;
        node ():parent(0), left(0), right(0) {
            for ( int i=0 ; i<M ; i++ ) child[i]=0;
        }
    };
    std::fstream f_tree, f_data, f_recycle;
  public:
    friend struct node;
    bplus_tree () {
        open_file(f_tree,".//bin//tree",std::ios::in|std::ios::out|std::ios::binary),
        open_file(f_data,".//bin//data",std::ios::in|std::ios::out|std::ios::binary),
        open_file(f_recycle,".//bin//recycle",std::ios::in|std::ios::out|std::ios::binary);
    }
    ~bplus_tree () {
        f_tree.close(),
        f_data.close(),
        f_recycle.close();
    }
};

}

#endif