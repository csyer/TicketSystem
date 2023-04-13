#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <iostream>
#include "exceptions.hpp"

namespace cay {

/**
 * a container like std::priority_queue which is a heap internal.
 */
template < typename T, class Compare=std::less<T> >
class priority_queue {
  private:
    struct node {
        node* ls;
        node* rs;
        T val;
        int h;
        node ( const T& val_ ):
            ls(nullptr), rs(nullptr), val(val_), h(0) {}
        node ( const node& obj ):
            ls(obj.ls), rs(obj.rs), val(obj.val), h(obj.h) {}
    };
    node* root;
    int siz;

    void copy ( node*& x, const node* y ) {
        if ( y==nullptr ) return;
        x=new node(y->val);
        copy(x->ls,y->ls);
        copy(x->rs,y->rs);
        return;
    }
    void clear ( node* &x ) {
        if ( x==nullptr ) return;
        clear(x->ls);
        clear(x->rs);
        delete x;
        return;
    }
    node* merge ( node*& a, node*& b ) {
        if ( a==nullptr ) return b;
        if ( b==nullptr ) return a;
        if ( Compare()(a->val,b->val) ) std::swap(a,b);
        a->rs=merge(a->rs,b);
        int lh, rh;
        if ( a->ls==nullptr ) lh=-1;
        else lh=a->ls->h;
        if ( a->rs==nullptr ) rh=-1;
        else rh=a->rs->h;
        if ( lh<rh ) {
            std::swap(a->ls,a->rs);
            std::swap(lh,rh);
        }
        a->h=rh+1;
        return a;
    }

  public:
	/**
	 * TODO constructors
	 */
	priority_queue () {
        root=nullptr;
        siz=0;
    }
	priority_queue ( const priority_queue& obj ) {
        if ( &obj==this ) return;
        siz=obj.size();
        copy(root,obj.get_root());
    }
	/**
	 * TODO deconstructor
	 */
	~priority_queue () {
        siz=0;
        if ( root!=nullptr ) 
            clear(root);
    }
	/**
	 * TODO Assignment operator
	 */
	priority_queue& operator= ( const priority_queue& obj ) {
        if ( &obj==this ) return *this;
        siz=obj.size();
        if ( root!=nullptr ) clear(root);
        copy(root,obj.get_root());
        return *this;
    }

    const node* get_root () const { return root; }
    node*& get_root () { return root; }
    void clear () { 
        siz=0; root=nullptr;
    }

	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T& top () const {
        if ( root==nullptr ) 
            throw container_is_empty();
        return root->val;
	}
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
	void push ( const T& e ) {
        node* nod=new node(e);
        try {
            root=merge(root,nod);
            ++siz;
        }
        catch ( ... ) {
            delete nod;
        }
    }
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop () {
        if ( root==nullptr ) 
            throw container_is_empty();
        node* tmp=root;
        root=merge(root->ls,root->rs);
        delete tmp;
        --siz;
	}
	/**
	 * return the number of the elements.
	 */
	size_t size () const {
        return siz;
	}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty () const {
        return siz==0;
	}
	/**
	 * merge two priority_queues with at least O(logn) complexity.
	 * clear the other priority_queue.
	 */
	void merge ( priority_queue& obj ) {
        root=merge(root,obj.get_root());
        siz+=obj.size();
        obj.clear();
	}
};

}

#endif