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
    // const static int M=BLOCK_SIZE/sizeof(Key);
    const static int M=3;
    const static int LOW=(M+1)/2, HIGH=M;

    struct node {
        int parent, left, right, siz, is_leaf;
        int child[M+2];

        Key keys[M+1];
        node ( int _parent=0, int _left=0, int _right=0, int _siz=0, int _is_leaf=1 ):
            parent(_parent), left(_left), right(_right), siz(_siz), is_leaf(_is_leaf) {
            for ( int i=0 ; i<M+1 ; i++ ) child[i]=0;
            for ( int i=0 ; i<M ; i++ ) keys[i]=Key();
        }
        node ( const node& obj ): 
            parent(obj.parent), left(obj.left), right(obj.right), siz(obj.siz), is_leaf(obj.is_leaf) {
            for ( int i=0 ; i<M+1 ; i++ ) child[i]=obj.child[i];
            for ( int i=0 ; i<M ; i++ ) keys[i]=obj.keys[i];
        }
        node ( const node&& obj ): 
            parent(obj.parent), left(obj.left), right(obj.right), siz(obj.siz), is_leaf(obj.is_leaf) {
            for ( int i=0 ; i<M+1 ; i++ ) child[i]=obj.child[i];
            for ( int i=0 ; i<M ; i++ ) keys[i]=obj.keys[i];
        }
        ~node () = default;

        node& operator= ( const node& obj ) {
            if ( this==&obj ) return *this;
            parent=obj.parent;
            left=obj.left;
            right=obj.right;
            siz=obj.siz;
            is_leaf=obj.is_leaf;
            for ( int i=0 ; i<M+1 ; i++ ) child[i]=obj.child[i];
            for ( int i=0 ; i<M ; i++ ) keys[i]=obj.keys[i];
            return *this;
        }

        int search ( const Key& key ) const {
            // here can be binary search
            // find the min id s.t. key<keys[id]
            int id;
            for ( id=0 ; id<siz ; id++ ) 
                if ( Comp()(key, keys[id]) ) break;
            return id;
        }
        void insert ( const int& pos, const Key& key, const int& add ) {
            for ( int i=siz++ ; i>pos ; i-- ) {
                keys[i]=keys[i-1],
                child[i+1]=child[i];
            }
            child[pos+1]=child[pos];
            keys[pos]=key;
            child[pos]=add;
        }
        void push ( const int& pos, const Key& key, const int& add ) {
            insert(pos, key, add);
            std::swap(child[pos], child[pos+1]);
        }
        void erase ( const int& pos ) {
            for ( int i=pos ; i<siz-1 ; i++ ) {
                keys[i]=keys[i+1],
                child[i]=child[i+1];
            }
            child[siz]=child[siz+1];
            child[siz+1]=0;
            keys[siz]=Key();
            --siz;
        }

        bool empty () { return siz==0; }

        void printKeys () {
            std::cerr <<"  keys: ";
            for ( int i=0 ; i<siz ; i++ ) 
                std::cerr << keys[i] <<' ';
            putchar('\n');
        }
    };
    save<node> f_tree;
    save<T> f_data;
    save<int> f_temp;
    int root, siz;

    using iterator=pair<node, int>;

    pair<node, Key> split ( int addr, node& nod, const int is_leaf ) {
        int delta=is_leaf^1, new_siz=nod.siz-LOW-delta;
        node new_nod(nod.parent, addr, nod.right, new_siz, is_leaf);
        Key key=nod.keys[LOW];

        for ( int i=0 ; i<new_siz ; i++ ) {
            new_nod.keys[i]=nod.keys[LOW+i+delta],
            new_nod.child[i]=nod.child[LOW+i+delta];
            nod.keys[LOW+i+delta]=Key(),
            nod.child[LOW+i+delta]=0;
        }

        if ( !is_leaf ) {
            new_nod.child[new_nod.siz]=nod.child[M+1];
            nod.child[M+1]=0;
        }
        nod.siz=LOW;
        return pair<node, Key>(new_nod, key);
    }
    void reset_parent ( const node& nod, const int addr ) {
        for ( int i=0 ; i<nod.siz ; i++ ) {
            node son(f_tree.read(nod.child[i]));
            son.parent=addr;
            f_tree.write(nod.child[i], son);
        }
    }

    void fix_insert ( int addr, node& nod, int is_leaf=0 ) {
        // std::cerr <<"fix insert "<< addr <<std::endl;

        if ( nod.siz<=HIGH ) {
            f_tree.write(addr,nod);
            return ;
        }

        node pa;
        if ( root!=addr ) pa=f_tree.read(nod.parent);
        else {
            pa.is_leaf=0;
            pa.child[0]=addr;
            root=nod.parent=f_tree.insert(pa);
        }

        // puts("check origin");
        // nod.printKeys();

        pair<node, Key> pr=split(addr, nod, is_leaf);

        node new_nod=pr.first;
        int new_addr=nod.right=f_tree.insert(new_nod);

        if ( !is_leaf ) reset_parent(new_nod, new_addr);

        // puts("check node");
        // nod.printKeys();
        // puts("check new node");
        // new_nod.printKeys();

        pa.push(pa.search(pr.second), pr.second, new_addr);

        if ( new_nod.right ) {
            node right_nod=f_tree.read(new_nod.right);
            right_nod.left=new_addr;
            f_tree.write(new_nod.right, right_nod);
        }

        f_tree.write(addr, nod);
        f_tree.write(new_addr, new_nod);

        fix_insert(nod.parent, pa);
        return ;
    }
    void fix_erase ( int addr, node& x ) {}

    pair<node,int> find_leaf ( const Key& key ) {
        int ptr=root;
        node nod=f_tree.read(ptr);
        for ( ; !nod.is_leaf ; ) {
            ptr=nod.child[nod.search(key)];
            nod=f_tree.read(ptr);
        }
        return {nod, ptr};
    }
    pair<iterator,int> upper_bound ( const Key& key ) {
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

    void print ( int x, int p=0, int mode=0 ) {
        std::cerr <<"print "<< x <<std::endl;
        node nod=f_tree.read(x);
        if ( mode==1 && nod.parent!=p ) puts("    -- ERROR! --    ");
        if ( nod.is_leaf ) std::cerr <<"leaf "<< x <<" with "<< nod.siz <<" keys:\n";
        else std::cerr <<"node "<< x <<" with "<< nod.siz <<" keys:\n";
        nod.printKeys();

        if ( mode==1 ) {
            if ( nod.is_leaf ) return (void)(puts("RETURN"));
            std::cerr <<"  childs: ";
            for ( int i=0 ; i<=nod.siz ; i++ ) 
                std::cerr << nod.child[i] <<' ';
            putchar('\n');
            for ( int i=0 ; i<=nod.siz ; i++ ) 
                print(nod.child[i], x);
        }
        else {
            if ( nod.right ) std::cerr <<"  next: "<< nod.right <<std::endl;
            if ( !nod.left && !nod.is_leaf ) std::cerr <<"  head son: "<< nod.child[0] <<std::endl;
            if ( nod.right ) print(nod.right);
            if ( !nod.left && !nod.is_leaf ) {
                print(nod.child[0]);
            }
        }
    }

  public:
    bplus_tree () {
        f_tree.open("tree"),
        f_data.open("data");
        f_temp.open("temp");

        if ( f_temp.empty() ) root=0;
        else root=f_temp.read(1);

        // std::cerr <<"check root "<< root <<std::endl;
    }
    ~bplus_tree () {
        f_temp.write(1,root);
        f_tree.close(),
        f_data.close(),
        f_temp.close();
    }

    void debug () {
        puts("------ DEBUG ------");
        print(root);
        return ;
    }

    void insert ( const Key& key, const T& data ) {
        int data_addr=f_data.insert(data);
        if ( !root ) {
            node nod;
            nod.insert(0, key, data_addr);
            root=f_tree.insert(nod);
            return ;
        }
        
        auto pr=upper_bound(key);
        node nod(pr.first.first);
        int id=pr.first.second;

        nod.insert(id, key, data_addr);
        fix_insert(pr.second, nod, 1);
        return ;
    }
    void erase ( const Key& key ) {
        if ( !root ) throw erase_fail();

        auto pr=find(key);
        if ( !pr.second ) return ; // not exist

        node nod(pr.first.first);
        int id=pr.first.second;
        nod.erase(id);

        fix_erase(pr.second, nod, 1);

        return ;
    }

    void clear () {
        f_tree.clear();
        f_data.clear();
        f_temp.clear();
    }

};

}

#endif