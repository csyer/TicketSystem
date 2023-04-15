#ifndef BPLUS_TREE_HPP
#define BPLUS_TREE_HPP

#include <fstream>
#include <iostream>

#include "exceptions.hpp"
#include "save.hpp"
#include "utility.hpp"
#include "vector.hpp"

namespace cay {

template < class Key, class T, class Comp=std::less<Key> >
class bplus_tree {
  private:
    const static int BLOCK_SIZE=4000; //4kb
    const static int M=BLOCK_SIZE/sizeof(Key);
    const static int LOW=(M+1)/2, HIGH=M;

    struct node {
        int parent, left, right, siz, addr, is_node;
        int child[M];

        Key keys[M-1];
        node () = default;
        ~node () = default;

        int search ( const Key& key ) const {
            // here can be binary search
            // find the min id s.t. key<keys[id]
            int id;
            for ( id=0 ; id<siz ; id++ ) 
                if ( Comp()(key, keys[id]) ) break;
            return id;
        }
        void insert ( const int& pos, const Key& key ) {
            for ( int i=++siz ; i>pos ; i-- ) 
                keys[i]=keys[i-1];
            keys[pos]=key;
        }
    };
    save<node> f_tree;
    save<T> f_data;
    stack<int> f_tree_rec, f_data_rec;
    int root, siz, top;

    void fix_insert ( node x ) {}
    void fix_erase ( node x ) {}

    int new_node () {
        if ( top ) return f_tree_rec.read(top--);
        else return ++siz;
    }
    void delete_node ( int addr ) {
        f_tree.clear(addr);
        f_tree_rec.write(++top, addr);
    }

    node find_leaf ( const Key& key ) const {
        int ptr=root;
        node nod=f_tree.read(ptr);
        for ( ; nod.is_node ; ) {
            ptr=nod.child[nod.search(key)];
            nod=f_tree.read(ptr);
        }
        return nod;
    }

  public:

    // iterator and const_iterator refer to Wankupi/map
    // base_iterator: operator== and operator!=
    // common_iterator: operator++ and operator--
    class iterator {
        node nod;
        int id;
      public:

        iterator () = default;
        iterator ( const node& _nod, const int& _id ): nod(_nod), id(_id) {}
        iterator ( const iterator& obj ): nod(obj.nod), id(obj.id) {}

        bool is_begin () const { return nod.left==0 && id==0; }
        bool is_end () const { return id==-1; }
        bool is_head () const { return id==0; }
        node get_node () const { return nod; }
        int get_id () const { return id; }

        iterator& operator-- () {
            if ( !id ) id=(nod=f_tree.read(nod.left)).siz-1;
            else --id;
            return *this;
        }

        ~iterator () = default;
    };

    bplus_tree () {
        f_tree.open("tree"),
        f_data.open("data"),
        f_tree_rec.open("tree_recycle"),
        f_data_rec.open("data_recycle");

        siz=root=0;
    }
    ~bplus_tree () {
        f_tree.close(),
        f_data.close(),
        f_tree_rec.close(),
        f_data_rec.close();
    }

    iterator upper_bound ( const Key& key ) const {
        if ( !root ) return {node(),-1};
        node nod=find_leaf(key);
        return iterator(nod, nod.search(key));
    }
    iterator find ( const Key& key ) const {
        iterator it=upper_bound(key);
        if ( it.is_end() ) return it;
        node nod=it.get_node();
        int id=it.get_id();
        if ( !id || Comp()(nod.keys[id-1], key) )
            return {node(),-1};
        return --it;
    }

    void insert ( const Key& key, const T& data ) {}
    void erase ( const Key& key ) {}

};

}

#endif