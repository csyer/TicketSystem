#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include "utility.hpp"
#include "exceptions.hpp"
#include <iostream>

namespace cayso {

template < typename T >
struct my_type_traits {
    using iterator_assignable=typename T::iterator_assignable;
};

struct my_true_type {};
struct my_false_type {};

template<
	class Key,
	class T,
	class Compare=std::less<Key>
> 
class map {
  public:
	typedef pair<const Key,T> value_type;
  private:
    #define ls c[0]
    #define rs c[1]
    #define RED 0
    #define BLACK 1

    struct node {
        value_type val;
        node* c[2];
        node* fth; // father
        int col; // color: 0 red; 1 black
        node ( const value_type& v ):
            val(v), fth(nullptr), col(RED) {
            ls=rs=nullptr;
        }
        node ( const node& obj ):
            val(obj.val), fth(obj.fth), col(obj.col) {
            ls=obj.ls;
            rs=obj.rs;
        }
    };
    node* rt;
    node* ptr_end;
    int siz;

    std::allocator<node> alloc;

    void dfsCopy ( node*& x, const node* y ) {
        if ( y==nullptr ) return;
        x=alloc.allocate(1);
        alloc.construct(x,*y);
        dfsCopy(x->ls,y->ls);
        dfsCopy(x->rs,y->rs);
        if ( x->ls!=nullptr ) x->ls->fth=x;
        if ( x->rs!=nullptr ) x->rs->fth=x;
        return;
    }
    void dfsDestroy ( node* x ) {
        if ( x==nullptr ) return;
        dfsDestroy(x->ls);
        dfsDestroy(x->rs);
        alloc.destroy(x);
        alloc.deallocate(x,1);
        return;
    }

    int isRoot ( const node* x ) const {
        return x->fth==nullptr;
    }
    int son ( const node* x ) const {
        return x->fth->rs==x;
    }
    void rotate ( node* x ) {
        if ( x==nullptr || isRoot(x) ) return;

        node* y=x->fth;
        node* z=y->fth;
        int s=son(x);

        if ( isRoot(y) ) rt=x;

        if ( z!=nullptr ) z->c[son(y)]=x; 
        x->fth=z;

        y->c[s]=x->c[s^1];
        if ( x->c[s^1]!=nullptr ) x->c[s^1]->fth=y;

        x->c[s^1]=y;
        y->fth=x;

    }

    node* findBegin () const {
        if ( rt==nullptr ) return ptr_end;
        node* x=rt;
        while ( x->ls!=nullptr )
            x=x->ls;
        return x;
    }    
    node* findEnd () const {
        if ( rt==nullptr ) return ptr_end;
        node* x=rt;
        while ( x->rs!=nullptr )
            x=x->rs;
        return x;
    }
    const node* findSucc ( const node* x ) const {
        if ( x->rs==nullptr ) {
            while ( !isRoot(x) && son(x) ) 
                x=x->fth;
            if ( isRoot(x) ) return ptr_end;
            else return x->fth;
        }
        x=x->rs;
        while ( x->ls!=nullptr )
            x=x->ls;
        return x;
    }
    node* findSucc ( node* x ) const {
        if ( x->rs==nullptr ) {
            while ( !isRoot(x) && son(x) ) 
                x=x->fth;
            if ( isRoot(x) ) return ptr_end;
            else return x->fth;
        }
        x=x->rs;
        while ( x->ls!=nullptr )
            x=x->ls;
        return x;
    }
    const node* findPrev ( const node* x ) const {
        if ( x==ptr_end ) return findEnd();
        if ( x->ls==nullptr ) {
            while ( !isRoot(x) && !son(x) )
                x=x->fth;
            if ( isRoot(x) ) return ptr_end;
            else return x->fth;
        }
        x=x->ls;
        while ( x->rs!=nullptr ) 
            x=x->rs;
        return x;
    }
    node* findPrev ( node* x ) const {
        if ( x==ptr_end ) return findEnd();
        if ( x->ls==nullptr ) {
            while ( !isRoot(x) && !son(x) )
                x=x->fth;
            if ( isRoot(x) ) return ptr_end;
            else return x->fth;
        }
        x=x->ls;
        while ( x->rs!=nullptr ) 
            x=x->rs;
        return x;
    }
    node* findNode ( const Key& key ) const {
        node* ptr=rt;
        while ( ptr!=nullptr ) {
            if ( Compare()(key,ptr->val.first) )
                ptr=ptr->ls;
            else if ( Compare()(ptr->val.first,key) ) 
                ptr=ptr->rs;
            else return ptr;
        }
        return ptr_end;
    }
    void fixInsert ( node* x ) {
        while ( !isRoot(x) && x->col==RED && x->fth->col==RED ) { 
            node* p=x->fth;
            node* g=p->fth;
            node* u=g->c[son(p)^1];
            if ( u==nullptr || u->col==1 ) {
                if ( son(x)!=son(p) )
                    rotate(p=x);
                rotate(p);
                p->col=BLACK; g->col=RED;
                break;
            }
            else {
                p->col=u->col=BLACK;
                g->col=RED;
                x=g;
            }
        }
        if ( isRoot(x) ) x->col=BLACK;
    }
    void fixErase ( node* x ) {
        while ( !isRoot(x) && x->col==BLACK ) {
            int ch=son(x);
            node* f=x->fth;
            node* b=f->c[ch^1];
            if ( b->col==RED ) {
                rotate(b);
                std::swap(b->col,f->col);
            }
            else {
                int lc=(b->c[ch]==nullptr)||(b->c[ch]->col==BLACK),
                    rc=(b->c[ch^1]==nullptr)||(b->c[ch^1]->col==BLACK);
                if ( lc==BLACK && rc==BLACK ) {
                    b->col=RED;
                    if ( x->fth->col==RED ) {
                        x->fth->col=BLACK;
                        break;
                    }
                    x=x->fth;
                }
                else {
                    if ( rc==BLACK ) {
                        std::swap(b->col,b->c[ch]->col);
                        rotate(b->c[ch]);
                        b=f->c[ch^1];
                    }
                    std::swap(b->col,f->col);
                    rotate(b);
                    b->c[ch^1]->col=BLACK;
                    break;
                }
            }
        }
    }
    node* insertNode ( const value_type& v ) {
        node* ptr=rt;
        if ( rt==nullptr ) {
            rt=alloc.allocate(1);
            alloc.construct(rt,v);
            rt->col=BLACK;
            ptr=rt;
        }
        else {
            while ( ptr!=nullptr ) {
                int l=Compare()(v.first,ptr->val.first),
                    r=Compare()(ptr->val.first,v.first);
                if ( !l && !r ) {
                    fixInsert(ptr);
                    return ptr;
                }
                if ( ptr->c[r]==nullptr ) {
                    ptr->c[r]=new node(v);
                    ptr->c[r]->fth=ptr;
                }
                ptr=ptr->c[r];
            }
        }
        return ptr;
    }
    void eraseNode ( node* x ) {
        if ( x->ls!=nullptr && x->rs!=nullptr ) {
            node* y=findSucc(x);
            if ( !isRoot(x) ) x->fth->c[son(x)]=y;
            else rt=y;
            std::swap(x->col,y->col);
            if ( y!=x->rs ) {
                y->fth->ls=x;
                std::swap(x->fth,y->fth);
                x->ls->fth=y;
                std::swap(x->ls,y->ls);
                x->rs->fth=y;
                if ( y->rs!=nullptr ) y->rs->fth=x;
                std::swap(x->rs,y->rs);
            }
            else {
                y->fth=x->fth;
                x->fth=y;
                x->ls->fth=y;
                y->ls=x->ls;
                x->ls=nullptr;
                if ( y->rs!=nullptr ) y->rs->fth=x;
                x->rs=y->rs;
                y->rs=x;
            }
        }

        /*-------------------- fixErase --------------------*/

        node* y=x->c[x->ls==nullptr];
        if ( isRoot(x) ) {
            if ( y!=nullptr ) {
                rt=y;
                y->fth=nullptr;
                y->col=BLACK;
            }
            else rt=nullptr;
            alloc.destroy(x);
            alloc.deallocate(x,1);
        }
        else {
            if ( y==nullptr || y->col==BLACK )
                fixErase(x);
            else y->col=BLACK;
            if ( y!=nullptr ) y->fth=x->fth;
            x->fth->c[son(x)]=y;
            alloc.destroy(x);
            alloc.deallocate(x,1);
        }
    }

  public:

	class const_iterator;
	class iterator {
      public:
        using iterator_assignable=my_true_type;
	  private:
        node* ptr;
        const map* bel;
	  public:
		using difference_type = std::ptrdiff_t;
		using value_type = map::value_type;
		using pointer = value_type*;
		using reference = value_type&;
		using iterator_category = std::output_iterator_tag;
		iterator ():ptr(nullptr), bel(nullptr) {}
        iterator ( node* ptr_, const map* bel_ ):
            ptr(ptr_), bel(bel_) {}
		iterator ( const iterator& obj ):
            ptr(obj.ptr), bel(obj.bel) {}

        node* point_pos () const { return ptr; }
        node*& point_pos () { return ptr; }
        const map* belong_to () const { return bel; }

        iterator& operator= ( const iterator& obj ) {
            ptr=obj.point_pos();
            bel=obj.belong_to();
            return *this;
        }

		iterator operator++ ( int ) {
            iterator it=*this;
            if ( it==bel->cend() ) 
                throw invalid_iterator();
            node* tmp=bel->findSucc(ptr);
            ptr=tmp;
            return it;
        }
		iterator& operator++ () {
            (*this)++;
            return *this;
        }
		iterator operator-- ( int ) {
            iterator it=*this;
            if ( it==bel->cbegin() ) 
                throw invalid_iterator();
            node* tmp=bel->findPrev(ptr);
            ptr=tmp;
            return it;
        }
		iterator& operator-- () {
            (*this)--;
            return *this;
        }
        
		value_type& operator* () const {
            return ptr->val;
        }
		bool operator== ( const iterator& rhs ) const {
            return ptr==rhs.point_pos();
        }
		bool operator== ( const const_iterator& rhs ) const {
            return ptr==rhs.point_pos();
        }
		bool operator!= ( const iterator& rhs ) const {
            return !(*this==rhs);
        }
		bool operator!= ( const const_iterator& rhs ) const {
            return !(*this==rhs);
        }

		value_type* operator-> () const noexcept {
            return &ptr->val;
        }
	};
	class const_iterator {
      public:
        using iterator_assignable=my_false_type;
	  private:
        const node* ptr;
        const map* bel;
	  public:
		using difference_type = std::ptrdiff_t;
		using value_type = map::value_type;
		using pointer = value_type*;
		using reference = value_type&;
		using iterator_category = std::output_iterator_tag;
		const_iterator ():ptr(nullptr), bel(nullptr) {}
        const_iterator ( const node* ptr_, const map* bel_ ):
            ptr(ptr_), bel(bel_) {}
		const_iterator ( const iterator& obj ):
            ptr(obj.point_pos()), bel(obj.belong_to()) {}
		const_iterator ( const const_iterator& obj ):
            ptr(obj.ptr), bel(obj.bel) {}

        const node* point_pos () const { return ptr; }
        const map* belong_to () const { return bel; }

        const_iterator& operator= ( const const_iterator& obj ) {
            ptr=obj.point_pos();
            bel=obj.belong_to();
            return *this;
        }        
        const_iterator& operator= ( const iterator& obj ) {
            ptr=obj.point_pos();
            bel=obj.belong_to();
            return *this;
        }

		const_iterator operator++ ( int ) {
            if ( *this==bel->cend() ) 
                throw invalid_iterator();
            const_iterator it=*this;
            const node* tmp=bel->findSucc(ptr);
            ptr=tmp;
            return it;
        }
		const_iterator& operator++ () {
            (*this)++;
            return *this;
        }
		const_iterator operator-- ( int ) {
            if ( *this==bel->cbegin() ) 
                throw invalid_iterator();
            const_iterator it=*this;
            const node* tmp=bel->findPrev(ptr);
            ptr=tmp;
            return it;
        }
		const_iterator& operator-- () {
            (*this)--;
            return *this;
        }
        
		value_type operator* () const {
            return ptr->val;
        }
		bool operator== ( const iterator& rhs ) const {
            return ptr==rhs.point_pos();
        }
		bool operator== ( const const_iterator& rhs ) const {
            return ptr==rhs.point_pos();
        }
		bool operator!= ( const iterator& rhs ) const {
            return !(*this==rhs);
        }
		bool operator!= ( const const_iterator& rhs ) const {
            return !(*this==rhs);
        }

		const value_type* operator-> () const noexcept {
            return &ptr->val;
        }
	};
    
	map () {
        rt=nullptr;
        ptr_end=alloc.allocate(1);
        siz=0;
    }
	map ( const map& obj ) {
        if ( &obj==this ) return;
        ptr_end=alloc.allocate(1);
        rt=nullptr;
        dfsCopy(rt,obj.getRoot());
        siz=obj.siz;
        return;
    }
	map& operator= ( const map& obj ) {
        if ( &obj==this ) return *this;
        dfsDestroy(rt);
        dfsCopy(rt,obj.getRoot());
        siz=obj.siz;
        if ( ptr_end==nullptr ) 
            ptr_end=alloc.allocate(1);
        return *this;
    }
	~map () {
        dfsDestroy(rt);
        alloc.deallocate(ptr_end,1);
        siz=0;
    }

    const node* getRoot () const { return rt; }
    node*& getRoot () { return rt; }

	T& at ( const Key& key ) {
        node* ptr=findNode(key);
        if ( ptr==ptr_end ) 
            throw index_out_of_bound();
        return ptr->val.second;
    }
	const T& at ( const Key& key ) const {
        node* ptr=findNode(key);
        if ( ptr==ptr_end ) 
            throw index_out_of_bound();
        return ptr->val.second;
    }
	T& operator[] ( const Key& key ) {
        if ( findNode(key)==ptr_end ) {
            insertNode(value_type(key,T()));
            ++siz;
        }
        return at(key);
    }
	const T& operator[] ( const Key& key ) const {
        return at(key);
    }
	iterator begin () {
        return iterator(findBegin(),this);
    }
	const_iterator cbegin () const {
        return const_iterator(findBegin(),this);
    }
	iterator end () {
        return iterator(ptr_end,this);
    }   
	const_iterator cend () const {
        return const_iterator(ptr_end,this);
    }
	bool empty () const {
        return siz==0;
    }
	size_t size () const {
        return (size_t)siz;
    }
	void clear () {
        dfsDestroy(rt);
        rt=nullptr;
        siz=0;
    }
	pair<iterator,bool> insert ( const value_type& val ) {
        node* ptr=findNode(val.first);
        if ( ptr!=ptr_end ) 
            return pair<iterator,bool>(iterator(ptr,this),false);
        ++siz;
        return pair<iterator,bool>(iterator(insertNode(val),this),true);
    }
	void erase ( iterator pos ) {
        if ( pos.belong_to()!=this || pos==this->end() ) 
            throw runtime_error();
        --siz;
        eraseNode(pos.point_pos());
    }
	size_t count ( const Key& key ) const {
        return findNode(key)!=ptr_end;
    }
	iterator find ( const Key& key ) {
        return iterator(findNode(key),this);
    }
	const_iterator find ( const Key& key ) const {
        return const_iterator(findNode(key),this);
    }

};

}

#endif
