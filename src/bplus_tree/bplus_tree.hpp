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
    // const static int M=4;
    const static int LOW=M/2, SIZ=LOW+1, HIGH=M;

    struct node {
        int parent, left, right, siz, is_leaf;
        int child[M+2];

        Key keys[M+1];
        node ( int _parent=0, int _left=0, int _right=0, int _siz=0, int _is_leaf=1 ):
            parent(_parent), left(_left), right(_right), siz(_siz), is_leaf(_is_leaf) {
            memset(child, 0, sizeof(child));
            memset(keys, 0, sizeof(keys));
        }
        node ( const node& obj ): 
            parent(obj.parent), left(obj.left), right(obj.right), siz(obj.siz), is_leaf(obj.is_leaf) {
            memcpy(child, obj.child, sizeof(child));
            memcpy(keys, obj.keys, sizeof(keys));
        }
        node ( const node&& obj ): 
            parent(obj.parent), left(obj.left), right(obj.right), siz(obj.siz), is_leaf(obj.is_leaf) {
            memcpy(child, obj.child, sizeof(child));
            memcpy(keys, obj.keys, sizeof(keys));
        }
        ~node () = default;

        node& operator= ( const node& obj ) {
            if ( this==&obj ) return *this;
            parent=obj.parent;
            left=obj.left;
            right=obj.right;
            siz=obj.siz;
            is_leaf=obj.is_leaf;
            memcpy(child, obj.child, sizeof(child));
            memcpy(keys, obj.keys, sizeof(keys));
            return *this;
        }

        int search ( const Key& key ) const {
            if ( siz==0 ) return 0;
            if ( !Comp()(key, keys[siz-1]) ) return siz;
            int l=0, r=siz-1, mid;
            for ( mid=(l+r)>>1 ; l<r ; mid=(l+r)>>1 ) 
                if ( Comp()(key, keys[mid]) ) r=mid;
                else l=mid+1;
            return l;
        }
        void insert ( const int pos, const Key& key, const int add ) {
            memmove(keys+pos+1, keys+pos, sizeof(Key)*(siz-pos));
            memmove(child+pos+1, child+pos, sizeof(int)*(siz-pos+1));
            keys[pos]=key;
            child[pos]=add;
            ++siz;
        }
        void push ( const int pos, const Key& key, const int add ) {
            insert(pos, key, add);
            std::swap(child[pos], child[pos+1]);
        }
        void erase ( const int& pos ) {
            memmove(keys+pos, keys+pos+1, sizeof(Key)*(siz-pos));
            memmove(child+pos, child+pos+1, sizeof(int)*(siz-pos+1));
            --siz;
        }
        void pop ( const int pos ) {
            std::swap(child[pos], child[pos+1]);
            erase(pos);
        }

        void merge ( const node& nod ) {
            memcpy(keys+siz, nod.keys, sizeof(Key)*nod.siz);
            memcpy(child+siz, nod.child, sizeof(int)*(nod.siz+1));
            siz+=nod.siz;
            right=nod.right;
        }

        void push_front ( const pair<Key, int>& pr ) {
            insert(0, pr.first, pr.second);
        }
        pair<Key, int> pop_front () {
            Key _key=keys[0];
            int _child=child[0];
            erase(0);
            return pair<Key, int>(_key, _child);
        }
        void push_back ( const pair<Key, int>& pr ) {
            keys[siz++]=pr.first;
            child[siz]=pr.second;
        }
        pair<Key, int> pop_back () {
            Key _key=keys[siz-1];
            int _child=child[siz];
            keys[siz-1]=Key();
            child[siz--]=0;
            return pair<Key, int>(_key, _child);
        }

        bool empty () { return siz==0; }

        void printKeys () {
            std::cerr <<"  keys: ";
            for ( int i=0 ; i<siz ; i++ ) {
                std::cerr <<"(";
                keys[i].first.print();
                std::cerr << keys[i].second <<") ";
            }
            std::cerr <<'\n';
        }
    };
    save<node> f_tree;
    save<T> f_data;
    int root, siz;

    using iterator=pair<node, int>;

    pair<node, Key> split ( int addr, node& nod, const int is_leaf ) {
        int delta=is_leaf^1, new_siz=nod.siz-SIZ-delta;
        node new_nod(nod.parent, addr, nod.right, new_siz, is_leaf);
        Key key=nod.keys[SIZ];

        memcpy(new_nod.keys, nod.keys+SIZ+delta, sizeof(Key)*new_siz);
        memcpy(new_nod.child, nod.child+SIZ+delta, sizeof(int)*new_siz);
        memset(nod.keys+SIZ+delta, 0, sizeof(Key)*new_siz);
        memset(nod.child+SIZ+delta, 0, sizeof(int)*new_siz);

        if ( !is_leaf ) {
            new_nod.child[new_nod.siz]=nod.child[M+1];
            nod.child[M+1]=0;
        }
        nod.siz=SIZ;
        return pair<node, Key>(new_nod, key);
    }

    void reset_parent ( const node& nod, const int addr ) {
        for ( int i=0 ; i<=nod.siz ; i++ ) {
            node son(f_tree.read(nod.child[i]));
            son.parent=addr;
            f_tree.write(nod.child[i], son);
        }
    }

    void fix_insert ( int addr, node& nod, int is_leaf=0 ) {
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

        pair<node, Key> pr=split(addr, nod, is_leaf);
        node new_nod=pr.first;
        int new_addr=nod.right=f_tree.insert(new_nod);

        if ( !is_leaf ) reset_parent(new_nod, new_addr);

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
    void fix_erase ( int addr, node& nod, int is_leaf=0 ) {
        if ( addr==root ) {
            if ( nod.siz>=1 ) 
                f_tree.write(addr, nod);
            else {
                if ( nod.child[0] ) reset_parent(nod, 0);
                root=nod.child[0];
                f_tree.clear(addr);
            }
            return ;
        }
        if ( nod.siz>=LOW ) {
            f_tree.write(addr, nod);
            return ;
        }
        
        node left_nod, right_nod;
        if ( nod.left ) left_nod=f_tree.read(nod.left);
        else left_nod.parent=-1;
        if ( nod.right ) right_nod=f_tree.read(nod.right);
        else right_nod.parent=-1;

        node pa=f_tree.read(nod.parent);
        if ( left_nod.parent==nod.parent && left_nod.siz-1>=LOW ) {
            int id=pa.search(left_nod.keys[0]);
            if ( is_leaf ) {
                nod.insert(0, left_nod.keys[left_nod.siz-1], left_nod.child[left_nod.siz-1]);
                left_nod.erase(left_nod.siz-1);
                pa.keys[id]=nod.keys[0];
            }
            else {
                nod.push_front(pair<Key, int>(pa.keys[id], left_nod.child[left_nod.siz]));

                node son(f_tree.read(left_nod.child[left_nod.siz]));
                son.parent=addr;
                f_tree.write(left_nod.child[left_nod.siz], son);

                pa.keys[id]=left_nod.pop_back().first;
            }
            f_tree.write(nod.parent, pa);
            f_tree.write(addr, nod);
            f_tree.write(nod.left, left_nod);
            return ;
        }
        if ( right_nod.parent==nod.parent && right_nod.siz-1>=LOW ) {
            int id=pa.search(right_nod.keys[0])-1;
            if ( is_leaf ) {
                nod.insert(nod.siz, right_nod.keys[0], right_nod.child[0]);
                right_nod.erase(0);
                pa.keys[id]=right_nod.keys[0];
            }
            else {
                nod.push_back(pair<Key, int>(pa.keys[id], right_nod.child[0]));

                node son(f_tree.read(right_nod.child[0]));
                son.parent=addr;
                f_tree.write(right_nod.child[0], son);

                pa.keys[id]=right_nod.pop_front().first;
            }
            f_tree.write(nod.parent, pa);
            f_tree.write(addr, nod);
            f_tree.write(nod.right, right_nod);
            return ;
        }

        if ( left_nod.parent==nod.parent && left_nod.siz+nod.siz<=HIGH ) {
            int id=pa.search(left_nod.keys[0]);
            f_tree.clear(addr);

            if ( !is_leaf ) {
                left_nod.push_back(pair<Key, int>(pa.keys[id], 0));
                reset_parent(nod, nod.left);
            }
            left_nod.merge(nod);

            f_tree.write(nod.left, left_nod);
            if ( nod.right ) {
                right_nod.left=nod.left;
                f_tree.write(nod.right, right_nod);
            }
            pa.pop(id);
        }
        else if ( right_nod.parent==nod.parent && right_nod.siz+nod.siz<=HIGH ) {
            int id=pa.search(right_nod.keys[0])-1;
            f_tree.clear(nod.right);

            if ( !is_leaf ) {
                nod.push_back(pair<Key, int>(pa.keys[id], 0));
                reset_parent(right_nod, addr);
            }

            nod.merge(right_nod);
            f_tree.write(addr, nod);

            if ( nod.right ) {
                node new_right=f_tree.read(nod.right);
                new_right.left=addr;
                f_tree.write(nod.right, new_right);
            }

            pa.pop(id);
        }
        fix_erase(nod.parent, pa);
    }

    pair<node, int> find_leaf ( const Key& key ) {
        if ( !root ) return pair<node, int>(node(), 0);
        int ptr=root;
        node nod=f_tree.read(ptr);
        for ( ; !nod.is_leaf ; ) {
            ptr=nod.child[nod.search(key)];
            nod=f_tree.read(ptr);
        }
        return {nod, ptr};
    }
    pair<iterator, int> upper_bound ( const Key& key ) {
        if ( !root ) return {iterator(node(), -1), 0};
        auto pr=find_leaf(key);
        node nod=pr.first;
        int id=nod.search(key);
        return {iterator(nod, id), pr.second};
    }
    pair<iterator, int> find ( const Key& key ) {
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
        if ( mode==1 && nod.parent!=p ) {
            std::cerr <<"    -- ERROR! --    \n";
            std::cerr <<"real parent: "<< p <<", but parent: "<< nod.parent <<std::endl;
        }
        if ( nod.is_leaf ) std::cerr <<"leaf "<< x <<" with "<< nod.siz <<" keys:\n";
        else std::cerr <<"node "<< x <<" with "<< nod.siz <<" keys:\n";
        nod.printKeys();

        if ( mode==1 ) {
            if ( nod.is_leaf ) return (void)(puts("RETURN"));
            std::cerr <<"  childs: ";
            for ( int i=0 ; i<=nod.siz ; i++ ) 
                std::cerr << nod.child[i] <<' ';
            std::cerr <<'\n';
            for ( int i=0 ; i<=nod.siz ; i++ ) 
                print(nod.child[i], x, mode);
        }
        else {
            if ( nod.right ) std::cerr <<"  next: "<< nod.right <<std::endl;
            if ( !nod.left && !nod.is_leaf ) std::cerr <<"  head son: "<< nod.child[0] <<std::endl;
            if ( nod.right ) print(nod.right);
            if ( !nod.left && !nod.is_leaf ) {
                print(nod.child[0], mode);
            }
        }
    }

  public:
    bplus_tree () {
        f_tree.open("data/tree.dat"),
        f_data.open("data/data.dat");
        
        std::ifstream checker("data/temp.dat");
        if ( !checker.is_open() ) {
            std::ofstream create("data/temp.dat");
            root=0;
            create.close();
        }
        else {
            checker.seekg(0, std::ios::beg);
            checker.read(reinterpret_cast<char*>(&root), sizeof(int));
        }
        checker.close();
    }
    ~bplus_tree () {
        std::ofstream reset("data/temp.dat");
        reset.seekp(0, std::ios::beg);
        reset.write(reinterpret_cast<const char*>(&root), sizeof(int));
        reset.close();
        f_tree.close(),
        f_data.close();
    }

    void debug ( int mode ) {
        std::cerr <<"------ DEBUG ------\n";
        if ( root ) print(root, 0, mode);
        else std::cerr <<"TREE IS EMPTY\n";;
        std::cerr <<"------ QUIT  ------\n";
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
        if ( !root ) return ;

        auto pr=find(key);
        if ( !pr.second ) return ; // not exist

        node nod(pr.first.first);
        int id=pr.first.second;
        nod.erase(id);

        fix_erase(pr.second, nod, 1);

        return ;
    }

    vector<Key> find ( const Key& beg, const Key& end ) {
        vector<Key> ret;

        auto pr=upper_bound(beg);
        node nod=pr.first.first;
        int id=pr.first.second;

        if ( !root || ( id==nod.siz && nod.right==0 ) ) return ret;
        if ( id==nod.siz ) {
            nod=f_tree.read(nod.right);
            id=0;
        }

        while ( !Comp()(end, nod.keys[id]) ) {
            ret.push_back(nod.keys[id]);
            ++id;
            if ( id==nod.siz ) {
                if ( !nod.right ) break;
                nod=f_tree.read(nod.right);
                id=0;
            }
        }

        return ret;
    }

    void clear () {
        f_tree.clear();
        f_data.clear();
    }

};

}

#endif