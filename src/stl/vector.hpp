#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include <iostream>
#include "exceptions.hpp"

namespace cay {

template < typename T >
class vector {
  private:
    std::allocator<T> alloc;
    T* arr;
    int siz, max_length;
    const int SIZE=16;

    void double_space () { 
        T* tmp=alloc.allocate(max_length<<1);
        for ( int i=0 ; i<siz ; i++ ) alloc.construct(tmp+i,arr[i]);
        for ( int i=0 ; i<siz ; i++ ) alloc.destroy(arr+i);
        alloc.deallocate(arr,max_length);
        arr=tmp;
        max_length<<=1;
        return;
    }
    void reduce_space () {
        if ( max_length<=SIZE ) return ;
        T* tmp=alloc.allocate(max_length>>1);
        for ( int i=0 ; i<siz ; i++ ) alloc.construct(tmp+i,arr[i]);
        for ( int i=0 ; i<siz ; i++ ) alloc.destroy(arr+i);
        alloc.deallocate(arr,max_length);
        arr=tmp;
        max_length>>=1;
        return;
    }

    template < bool is_const >
    class base_iterator {
		using reference=typename std::conditional<is_const, const T&, T&>::type;
		using pointer=typename std::conditional<is_const, const T*, T*>::type;
      private:
        pointer pos;
        const void* ptr;
      public:
        friend class vector;
        base_iterator ( pointer _pos, const void* _ptr ): pos(_pos), ptr(_ptr) {}

        bool operator== ( const base_iterator& obj ) const { return pos==obj.pos; }
        bool operator!= ( const base_iterator& obj ) const { return pos!=obj.pos; }

        base_iterator operator+ ( const int& n ) const { return {pos+n, ptr}; }
        base_iterator operator- ( const int& n ) const { return {pos-n, ptr}; }
        int operator- ( const base_iterator& rhs ) const {
            if ( ptr!=rhs.ptr ) throw invalid_iterator();
            return pos-rhs.pos;
        }

        base_iterator& operator+= ( const int& n ) {
            pos+=n;
            return *this;
        }
        base_iterator& operator-= ( const int& n ) {
            pos-=n;
            return *this;
        }

        base_iterator operator++ ( int ) {
            iterator it=*this;
            ++pos;
            return it;
        }
        base_iterator& operator++ () {
            ++pos;
            return *this;
        }
        base_iterator operator-- ( int ) {
            iterator it=*this;
            --pos;
            return it;
        }
        base_iterator& operator--() {
            --pos;
            return *this;
        }
                
        reference operator* () const { return *pos; }
    };

  public:
    using iterator=base_iterator<false>;
    using const_iterator=base_iterator<true>;

    vector () { 
        siz=0; max_length=SIZE;
        arr=alloc.allocate(max_length);
    }
    vector ( const vector& obj ) {
        if ( &obj==this ) return;
        siz=obj.size(); max_length=siz<<1;
        arr=alloc.allocate(max_length);
        for ( int i=0 ; i<siz ; i++ ) 
            alloc.construct(arr+i,obj[i]);
        return;
    }
    
    ~vector() { 
        for ( int i=0 ; i<siz ; i++ ) alloc.destroy(arr+i);
        alloc.deallocate(arr,max_length);
    }
    
    vector& operator= ( const vector& obj ) {
        if ( &obj==this ) return *this;
        for ( int i=0 ; i<siz ; i++ ) alloc.destroy(arr+i);
        alloc.deallocate(arr,max_length);
        siz=obj.size(); 
        max_length=siz<<1;
        arr=alloc.allocate(max_length);
        for ( int i=0 ; i<siz ; i++ ) 
            alloc.construct(arr+i,obj[i]);
        return *this;
    }

    T& at ( const size_t &pos ) {
        if ( pos>=siz ) throw index_out_of_bound();
        return arr[pos];
    }
    const T& at ( const size_t &pos ) const {
        if ( pos>=siz ) throw index_out_of_bound();
        return arr[pos];
    }

    T& operator[] ( const size_t &pos ) { return at(pos); }
    const T& operator[] ( const size_t &pos ) const { return at(pos); }

    const T& front () const {
        if ( siz==0 ) throw container_is_empty();
        return arr[0];
    }
    const T& back () const {
        if ( siz==0 ) throw container_is_empty();
        return arr[siz-1];
    }

    iterator begin () { return {arr, this}; }
    const_iterator cbegin () const { return {arr, this}; }
    iterator end () { return {arr+siz, this}; }
    const_iterator cend() const { return {arr+siz, this}; }

    bool empty() const { return siz==0; }
    size_t size () const { return siz; }

    void clear() {
        for ( int i=0 ; i<siz ; i++ ) alloc.destroy(arr+i);
        alloc.deallocate(arr,max_length);
        siz=0; max_length=SIZE;
        arr=alloc.allocate(max_length);
    }

    iterator insert ( iterator pos, const T& value ) {
        alloc.construct(arr+siz,arr[siz-1]);
        std::cerr << (int)(pos.pos-arr) <<std::endl;
        for ( T* ptr=arr+siz-1 ; ptr!=pos.pos ; --ptr ) *ptr=*(ptr-1);
        *pos=value;
        ++siz;
        if ( siz==max_length ) double_space();
        return pos;
    }
    iterator insert ( const size_t& ind, const T& value ) {
        if ( ind>siz ) throw index_out_of_bound();
        alloc.construct(arr+siz, arr[siz-1]);
        for ( int i=siz-1 ; i>ind ; i-- ) arr[i]=arr[i-1];
        arr[ind]=value;
        ++siz;
        if ( siz==max_length ) double_space();
        return {arr+ind, this};
    }
    
    iterator erase ( iterator pos ) {
        for ( T* ptr=pos.pos ; ptr!=arr+siz-1 ; ptr++ ) *ptr=*(ptr+1);
        alloc.destroy(arr+(--siz));
        if ( siz*4<=max_length ) reduce_space();
        return {arr+siz, this}; 
    }
    iterator erase ( const size_t &ind ) {
        if ( ind>=siz ) throw index_out_of_bound();
        for ( int i=ind ; i<siz-1 ; i++ ) 
            arr[i]=arr[i+1];
        alloc.destroy(arr+(--siz));
        if ( siz*4<=max_length ) reduce_space();
        return {arr+siz, this}; 
    }
    
    void push_back ( const T& value ) {
        alloc.construct(arr+(siz++),value);
        if ( siz==max_length ) double_space();
    }
    void pop_back() {
        if ( siz==0 ) throw container_is_empty();
        alloc.destroy(arr+(--siz));
        if ( siz*4<=max_length ) reduce_space();
    }

    // ------------------- cool function -------------------

    void sort ( bool (*cmp)(const T&, const T&) ) {
        int n=siz;
        T* tmp=new T[n];
        for ( int stp=2 ; stp<=(n<<1) ; stp<<=1 ) {
            for ( int l=0 ; l<n ; l+=stp ) {
                int mid=l+(stp>>1)-1, r=std::min(n-1, l+stp-1);
                for ( int i=0, p=l, q=mid+1 ; i<r-l+1 ; i++ ) {
                    if ( p<=mid && q<=r ) tmp[i]=cmp(arr[p], arr[q])?arr[p++]:arr[q++];
                    else tmp[i]=p<=mid?arr[p++]:arr[q++];
                }
                for ( int i=0 ; i<r-l+1 ; i++ ) arr[l+i]=tmp[i];
            }
        }
        delete[] tmp;
    }
};

}

#endif