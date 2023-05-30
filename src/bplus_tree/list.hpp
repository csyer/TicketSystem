#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "save.hpp"

namespace cay {

template < class T >
class list {
  private:
    struct node {
        T data;
        int pre, nxt;
    };
    database<node, sizeof(int)> arr;
    int head;
  public:
    class iterator { 
      private:
        int pos; 
      public:
        iterator ():pos(0) {}
        iterator ( const iterator& obj ) { pos=obj.pos; }

        iterator& operator++ () { 
            pos=arr.read(pos).nxt; 
            return this;
        }

        const T operator* () { return arr.read(pos).data; }
    };

    list () {}
    void open ( const std::string& path ) {
        arr.open("data/"+path+"list.dat");
        arr.get_header(head);
    }
    list ( const std::string& name ) { open(name); }
    void close () { arr.put_header(head); }
    ~list () { close(); }

    void push_back ( const T& x ) { 

};

}

#endif