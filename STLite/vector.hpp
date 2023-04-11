#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include <iostream>
#include "exceptions.hpp"

namespace sjtu {

template < typename T >
class vector {
  private:
    std::allocator<T> alloc;
    T* arr;
    int siz, max_length;
    const int SIZE=8;

    void double_space () { 
        T* tmp=alloc.allocate(max_length<<1);
        for ( int i=0 ; i<siz ; i++ ) 
            alloc.construct(tmp+i,arr[i]);
        for ( int i=0 ; i<siz ; i++ ) 
            alloc.destroy(arr+i);
        alloc.deallocate(arr,max_length);
        arr=tmp;
        max_length<<=1;
        return;
    }
    void reduce_space () {
        if ( max_length<=SIZE ) return ;
        T* tmp=alloc.allocate(max_length>>1);
        for ( int i=0 ; i<siz ; i++ ) 
            alloc.construct(tmp+i,arr[i]);
        for ( int i=0 ; i<siz ; i++ ) 
            alloc.destroy(arr+i);
        alloc.deallocate(arr,max_length);
        arr=tmp;
        max_length>>=1;
        return;
    }
  public:
    class const_iterator;
    class iterator {
      private:
        int pos;
        vector* ptr;
      public:
        iterator () { pos=0; ptr=nullptr; }
        iterator ( int pos_ , vector* ptr_ ):pos(pos_),ptr(ptr_) {}
        iterator ( iterator& obj ) {
            if ( &obj==this ) return;
            pos=obj.pos;
            ptr=obj.ptr;
        }

        int point_position () const { return pos; }
        vector* point_vector () const { return ptr; }

        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        iterator operator+ ( const int& n ) const {
            iterator it(pos+n,ptr);
            return it;
        }
        iterator operator- ( const int& n ) const {
            iterator it(pos-n,ptr);
            return it;
        }

        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw invaild_iterator.
        int operator- ( const iterator& rhs ) const {
            if ( ptr!=rhs.point_vector() ) 
                throw invalid_iterator();
            return pos-rhs.pos;
        }

        iterator& operator+= ( const int& n ) {
            *this=*this+n;
            return *this;
        }
        iterator& operator-= ( const int& n ) {
            *this=*this-n;
            return *this;
        }

        /**
         * TODO iter++
         */
        iterator operator++ ( int ) {
            iterator it=*this;
            (*this)+=1;
            return it;
        }
        /**
         * TODO ++iter
         */
        iterator& operator++ () {
            (*this)+=1;
            return *this;
        }
        /**
         * TODO iter--
         */
        iterator operator-- ( int ) {
            iterator it=*this;
            (*this)-=1;
            return it;
        }
        /**
         * TODO --iter
         */
        iterator& operator--() {
            (*this)-=1;
            return *this;
        }

        /**
         * TODO *it
         */
        T& operator* () const {
            return ptr->at(pos);
        }

        /**
         * a operator to check whether two iterators are same (pointing to the same memory address).
         */
        bool operator== ( const iterator& rhs ) const {
            return pos==rhs.point_position() && ptr==rhs.point_vector();
        }
        bool operator== ( const const_iterator& rhs ) const {
            return pos==rhs.point_position() && ptr==rhs.point_vector();
        }
        /**
         * some other operator for iterator.
         */
        bool operator!= ( const iterator& rhs ) const {
            return pos!=rhs.point_position() || ptr!=rhs.point_vector();
        }
        bool operator!= ( const const_iterator& rhs ) const {
            return pos!=rhs.point_position() || ptr!=rhs.point_vector();
        }
    };
    class const_iterator {
      private:
        int pos;
        const vector* ptr;
      public:
        const_iterator () { pos=0; ptr=nullptr; }
        const_iterator ( int pos_ , const vector* ptr_ ):pos(pos_),ptr(ptr_) {}
        const_iterator ( const_iterator& obj ) {
            if ( &obj==this ) return;
            pos=obj.pos;
            ptr=obj.ptr;
        }

        int point_position ()  const { return pos; }
        const vector* point_vector () const { return ptr; }

        /**
         * return a new iterator which pointer n-next elements
         * as well as operator-
         */
        const_iterator operator+ ( const int& n ) const {
            const_iterator it(pos+n,ptr);
            return it;
        }
        const_iterator operator- ( const int& n ) const {
            const_iterator it(pos-n,ptr);
            return it;
        }

        // return the distance between two iterators,
        // if these two iterators point to different vectors, throw invaild_iterator.
        int operator- ( const const_iterator& rhs ) const {
            if ( ptr!=rhs.point_vector() ) 
                throw invalid_iterator();
            return pos-rhs.pos;
        }

        const_iterator& operator+= ( const int& n ) {
            *this=*this+n;
            return *this;
        }
        const_iterator& operator-= ( const int& n ) {
            *this=*this-n;
            return *this;
        }

        /**
         * TODO iter++
         */
        const_iterator operator++ ( int ) {
            const_iterator it=*this;
            (*this)+=1;
            return it;
        }
        /**
         * TODO ++iter
         */
        const_iterator& operator++ () {
            (*this)+=1;
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator-- ( int ) {
            const_iterator it=*this;
            (*this)-=1;
            return it;
        }
        /**
         * TODO --iter
         */
        const_iterator& operator--() {
            (*this)-=1;
            return *this;
        }

        /**
         * TODO *it
         */
        const T operator* () const {
            return ptr->at(pos);
        }

        /**
         * a operator to check whether two iterators are same (pointing to the same memory address).
         */
        bool operator== ( const iterator& rhs ) const {
            return pos==rhs.point_position() && ptr==rhs.point_vector();
        }
        bool operator== ( const const_iterator& rhs ) const {
            return pos==rhs.point_position() && ptr==rhs.point_vector();
        }
        /**
         * some other operator for iterator.
         */
        bool operator!= ( const iterator& rhs ) const {
            return pos!=rhs.point_position() || ptr!=rhs.point_vector();
        }
        bool operator!= ( const const_iterator& rhs ) const {
            return pos!=rhs.point_position() || ptr!=rhs.point_vector();
        }
    };
    
    vector () { 
        siz=0; max_length=SIZE;
        arr=alloc.allocate(max_length);
    }
    vector ( const vector& obj ) {
        if ( &obj==this ) return;
        siz=obj.size(); max_length=2*siz;
        arr=alloc.allocate(max_length);
        for ( int i=0 ; i<siz ; i++ ) 
            alloc.construct(arr+i,obj[i]);
        return;
    }
    
    ~vector() { 
        for ( int i=0 ; i<siz ; i++ ) 
            alloc.destroy(arr+i);
        alloc.deallocate(arr,max_length);
    }
    
    vector& operator= ( const vector& obj ) {
        if ( &obj==this ) return *this;
        for ( int i=0 ; i<siz ; i++ ) 
            alloc.destroy(arr+i);
        alloc.deallocate(arr,max_length);
        siz=obj.size(); max_length=siz*2;
        arr=alloc.allocate(max_length);
        for ( int i=0 ; i<siz ; i++ ) 
            alloc.construct(arr+i,obj[i]);
        return *this;
    }

    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     */
    T& at ( const size_t &pos ) {
        if ( pos<0 || pos>=siz ) 
            throw index_out_of_bound();
        return arr[pos];
    }
    const T& at ( const size_t &pos ) const {
        if ( pos<0 || pos>=siz ) 
            throw index_out_of_bound();
        return arr[pos];
    }

    /**
     * assigns specified element with bounds checking
     * throw index_out_of_bound if pos is not in [0, size)
     * !!! Pay attentions
     *   In STL this operator does not check the boundary but I want you to do.
     */
    T& operator[] ( const size_t &pos ) {
        if ( pos<0 || pos>=siz ) 
            throw index_out_of_bound();
        return arr[pos];
    }
    const T& operator[] ( const size_t &pos ) const {
        if ( pos<0 || pos>=siz ) 
            throw index_out_of_bound();
        return arr[pos];
    }

    /**
     * access the first element.
     * throw container_is_empty if size == 0
     */
    const T& front () const {
        if ( siz==0 ) throw container_is_empty();
        return arr[0];
    }

    /**
     * access the last element.
     * throw container_is_empty if size == 0
     */
    const T& back () const {
        if ( siz==0 ) throw container_is_empty();
        return arr[siz-1];
    }

    /**
     * returns an iterator to the beginning.
     */
    iterator begin () {
        return iterator(0,this);
    }
    const_iterator cbegin () const {
        return const_iterator(0,this);
    }

    /**
     * returns an iterator to the end.
     */
    iterator end () { 
        return iterator(siz,this);
    }
    const_iterator cend() const {
        return const_iterator(siz,this);
    }

    /**
     * checks whether the container is empty
     */
    bool empty() const {
        return siz==0; 
    }

    /**
     * returns the number of elements
     */
    size_t size () const {
        return siz;
    }

    /**
     * clears the contents
     */
    void clear() {
        for ( int i=0 ; i<siz ; i++ ) 
            alloc.destroy(arr+i);
        alloc.deallocate(arr,max_length);
        siz=0; max_length=SIZE;
        alloc.allocate(arr,max_length);
    }

    /**
     * inserts value before pos
     * returns an iterator pointing to the inserted value.
     */
    iterator insert ( iterator pos, const T& value ) {
        if ( siz==max_length ) double_space();
        alloc.construct(arr+siz,arr[siz-1]);
        for ( int i=siz-1 ; i>pos.point_position() ; i-- ) 
            arr[i]=arr[i-1];
        arr[pos.point_position()]=value;
        ++siz;
        return pos;
    }
    /**
     * inserts value at index ind.
     * after inserting, this->at(ind) == value
     * returns an iterator pointing to the inserted value.
     * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
     */
    iterator insert ( const size_t& ind, const T& value ) {
        if ( ind>siz ) throw index_out_of_bound();
        if ( siz==max_length ) double_space();
        alloc.construct(arr+siz,arr[siz-1]);
        for ( int i=siz-1 ; i>ind ; i-- ) 
            arr[i]=arr[i-1];
        arr[ind]=value;
        ++siz;
        return iterator(ind,this);
    }
    /**
     * removes the element at pos.
     * return an iterator pointing to the following element.
     * If the iterator pos refers the last element, the end() iterator is returned.
     */
    iterator erase ( iterator pos ) {
        for ( int i=pos.point_position() ; i<siz-1 ; i++ ) 
            arr[i]=arr[i+1];
        --siz;
        alloc.destroy(arr+siz);
        if ( siz*4<=max_length ) reduce_space();
        return iterator(siz,this); 
    }
    /**
     * removes the element with index ind.
     * return an iterator pointing to the following element.
     * throw index_out_of_bound if ind >= size
     */
    iterator erase ( const size_t &ind ) {
        if ( ind>=siz ) throw index_out_of_bound();
        for ( int i=ind ; i<siz-1 ; i++ ) 
            arr[i]=arr[i+1];
        --siz;
        alloc.destroy(arr+siz);
        if ( siz*4<=max_length ) reduce_space();
        return iterator(siz,this); 
    }
    /**
     * adds an element to the end.
     */
    void push_back ( const T& value ) {
        if ( siz==max_length ) double_space();
        alloc.construct(arr+siz,value);
        ++siz;
    }
    /**
     * remove the last element from the end.
     * throw container_is_empty if size() == 0
     */
    void pop_back() {
        if ( siz==0 ) throw container_is_empty();
        --siz;
        alloc.destroy(arr+siz);
        if ( siz*4<=max_length ) reduce_space();
    }
};

}

#endif