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
        int parent, left, right, siz, is_leaf;
        int child[M];

        Key keys[M-1];
        node ( int _parent=0, int _left=0, int _right=0, int _siz=0, int _is_leaf=1 ):
            parent(_parent), left(_left), right(_right), siz(_siz), is_leaf(_is_leaf) {
            for ( int i=0 ; i<M ; i++ ) child[i]=0;
            for ( int i=0 ; i<M-1 ; i++ ) keys[i]=Key();
        }
        node ( const node& obj ): 
            parent(obj.parent), left(obj.left), right(obj.right), siz(obj.siz), is_leaf(obj.is_leaf) {
            for ( int i=0 ; i<M ; i++ ) child[i]=obj.child[i];
            for ( int i=0 ; i<M-1 ; i++ ) keys[i]=obj.keys[i];
        }
        ~node () = default;

        int search ( const Key& key ) const {
            // here can be binary search
            // find the min id s.t. key<keys[id]
            int id;
            for ( id=0 ; id<siz ; id++ ) 
                if ( Comp()(key, keys[id]) ) break;
            return id;
        }
        void insert ( const int& pos, const Key& key, const int add ) {
            for ( int i=++siz ; i>pos ; i-- ) 
                keys[i]=keys[i-1],
                child[i+1]=child[i];
            child[pos+1]=child[pos];
            keys[pos]=key;
            child[pos]=add;
        }
        void erase ( const int& pos ) {
            for ( int i=pos ; i<siz-1 ; i++ ) 
                keys[i]=keys[i+1],
                child[i]=child[i+1];
            child[siz]=child[siz+1];
            child[siz+1]=0;
            keys[siz]=Key();
            --siz;
        }
    };
    save<node> f_tree;
    save<T> f_data;
    int root, siz, data_siz;

    using iterator=pair<node,int>;

    void fix_insert ( int addr, node& x ) {}
    void fix_erase ( int addr, node& x ) {}

    pair<node,int> find_leaf ( const Key& key ) const {
        int ptr=root;
        node nod=f_tree.read(ptr);
        for ( ; !nod.is_leaf ; ) {
            ptr=nod.child[nod.search(key)];
            nod=f_tree.read(ptr);
        }
        return {nod, ptr};
    }
    pair<iterator,int> upper_bound ( const Key& key ) const {
        if ( !root ) return {iterator(node(),-1), 0};
        auto pr=find_leaf(key);
        node nod=pr.first;
        return {iterator(nod, nod.search(key)), pr.second};
    }
    pair<iterator,int> find ( const Key& key ) const {
        auto pr=upper_bound(key);
        iterator it=pr.first;
        if ( it.second==-1 ) return {it, 0};
        node nod=it.first;
        int id=it.second;
        if ( !id || Comp()(nod.keys[id-1], key) )
            return {iterator(node(), -1), 0};
        return {iterator(nod, id-1), pr.second};
    }

  public:
    bplus_tree () {
        f_tree.open("tree"),
        f_data.open("data");

        root=0;
    }
    ~bplus_tree () {
        f_tree.close(),
        f_data.close();
    }

    void insert ( const Key& key, const T& data ) {
        int data_addr=f_data.insert(data);
        if ( !root ) {
            node nod;
            nod.insert(0,key,data_addr);
            root=f_tree.insert(nod);
            return ;
        }
        
        auto pr=upper_bound(key);

        node nod(pr.first.first);
        int id=pr.first.second;
        nod.insert(id,key,data_addr);
        
        fix_insert(pr.second,nod);

        return ;
    }
    void erase ( const Key& key ) {
        if ( !root ) throw erase_fail();

        auto pr=find(key);
        if ( !pr.second ) return ; // not exist

        node nod(pr.first.first);
        int id=pr.first.second;
        nod.erase(id);

        fix_erase(pr.second, nod);

        return ;
    }

};

}

#endif