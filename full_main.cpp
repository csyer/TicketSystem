#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

namespace cay {

class exception {
  protected:
	const std::string variant="";
	std::string detail="";
  public:
	exception () {}
	exception ( const exception &ec ): variant(ec.variant), detail(ec.detail) {}
	virtual std::string what () {
		return variant+" "+detail;
	}
};

class index_out_of_bound : public exception {
	/* __________________________ */
};

class runtime_error : public exception {
	/* __________________________ */
};

class invalid_iterator : public exception {
	/* __________________________ */
};

class container_is_empty : public exception {
	/* __________________________ */
};

class read_eof : public exception {
	/* __________________________ */
};

class erase_fail : public exception {
	/* __________________________ */
};

template < const int MAX_SIZE >
class string {
    char s[MAX_SIZE+1];
  public:
    string () {}
    string ( const string& rhs ) {
        for ( int i=0 ; i<=MAX_SIZE ; i++ )
            s[i]=rhs.s[i];
    }
    string ( const char* rhs ) {
        int len=std::min(MAX_SIZE, (int)std::strlen(rhs));
        memcpy(s, rhs, len);
        memset(s+len, 0, MAX_SIZE-len);
    }

    // template < const int _MAX_SIZE >
    // string ( const string<_MAX_SIZE>& obj ) {
    //     int len=std::min(MAX_SIZE, _MAX_SIZE);
    //     memcpy(s, obj.s, len);
    //     memset(s+len, 0, MAX_SIZE-len);
    // }

    string& operator= ( const string& obj ) {
        if ( this==&obj ) return *this;
        memcpy(s, obj.s, MAX_SIZE);
        return *this;
    }

    void print () const { 
        std::cout << s <<' ';
    }

    bool operator== ( const string& obj ) const {
        return strcmp(s, obj.s)==0;
    }
    bool operator< ( const string& obj ) const {
        for ( int i=0 ; i<MAX_SIZE ; i++ ) 
            if ( s[i]!=obj.s[i] ) return s[i]<obj.s[i];
        return 0;
    }

    ~string () = default;
};

template < class T1, class T2 >
class pair {
  public:
	T1 first;
	T2 second;
	constexpr pair () : first(), second() {}
	pair ( const pair& obj ) = default;
	pair ( pair&& obj ) = default;

	pair ( const T1& x, const T2& y ): first(x), second(y) {}
	template < class U1, class U2 >
	pair ( U1&& x, U2&& y ): first(x), second(y) {}

	template < class U1, class U2 >
	pair ( const pair<U1, U2>& obj ): first(obj.first), second(obj.second) {}
	template < class U1, class U2 >
	pair ( pair<U1, U2>&& obj ): first(obj.first), second(obj.second) {}

    ~pair () = default;

    pair<T1, T2>& operator= ( const pair<T1, T2>& obj ) {
        if ( this==&obj ) return *this;
        first=obj.first;
        second=obj.second;
        return *this;
    }
    bool operator< ( const pair<T1, T2>& obj ) const {
        if ( first==obj.first ) return second<obj.second;
        return first<obj.first;
    }
    bool operator== ( const pair<T1, T2>& obj ) const {
        return first==obj.first && second==obj.second;
    }

};

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
        iterator ( int pos_ , vector* ptr_ ):pos(pos_), ptr(ptr_) {}
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
        const_iterator ( int pos_ , const vector* ptr_ ):pos(pos_), ptr(ptr_) {}
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
        if ( (int)(pos)<0 || (int)(pos)>=siz ) 
            throw index_out_of_bound();
        return arr[pos];
    }
    const T& at ( const size_t &pos ) const {
        if ( (int)(pos)<0 || (int)(pos)>=siz ) 
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
        if ( (int)pos<0 || (int)pos>=siz ) 
            throw index_out_of_bound();
        return arr[pos];
    }
    const T& operator[] ( const size_t &pos ) const {
        if ( (int)(pos)<0 || (int)(pos)>=siz ) 
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
        arr=alloc.allocate(max_length);
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
    
template < class T > 
class save {
  private:
    std::fstream sav, rec;
    vector<int> bin;
    std::string name, path;
    int siz;
    
    void touch ( std::fstream& file, const std::string& file_name, const std::string& file_path, 
                std::ios_base::openmode mode=std::ios::in|
                                             std::ios::out|
                                             std::ios::binary ) {
        std::ifstream checker(path+file_name+".dat");
        if ( !checker.is_open() ) 
            std::ofstream create(path+file_name+".dat");
        file.open(path+file_name+".dat", mode);
    }
  public:
    save () {}

    void open ( const std::string& file_name, const std::string& file_path="", 
                std::ios_base::openmode mode=std::ios::in|
                                             std::ios::out|
                                             std::ios::binary ) {
        name=file_name, path=file_path;
        touch(sav, file_name, file_path, mode);
        sav.seekg(0,std::ios::end);
        siz=sav.tellg()/sizeof(T);

        // touch(rec, file_name+"_rec", file_path, mode);

        // rec.seekg(0,std::ios::beg);
        // for ( int i=0 ; !rec.eof() ; i++ ) {
        //     int val;
        //     rec.read(reinterpret_cast<char*>(&val), sizeof(int));
        //     if ( !rec.eof() ) bin.push_back(val);
        // }
        // rec.close();

        // std::ofstream reset(path+file_name+"_rec.dat", std::ios::out|std::ios::trunc);
        // reset.close();
    }
    save ( const std::string& file_name, const std::string& file_path="", 
           std::ios_base::openmode mode=std::ios::in|
                                        std::ios::out|
                                        std::ios::binary ) {
        open(file_name, file_path, mode);
    }

    void close () {
        sav.close();
    }
    ~save () {
        close();
        // touch(rec, name+"_rec", path);
        // rec.seekp(0, std::ios::beg);
        // for ( int i=0 ; i<(int)bin.size() ; i++ ) 
        //     rec.write(reinterpret_cast<char*>(&bin[i]), sizeof(int));
        // bin.clear();
        // rec.close();
    }

    T read ( int pos ) {
        // std::cerr <<"  read in "<< pos <<std::endl;
        // sav.seekg(0, std::ios::end);
        // std::cerr <<"    read check end "<< sav.tellg() <<' '<< sizeof(T) <<std::endl;
        if ( pos<1 || pos>siz ) throw read_eof();
        T res;
        sav.seekg(sizeof(T)*(pos-1));
        sav.read(reinterpret_cast<char*>(&res), sizeof(T));
        return res;
    }
    void write ( int pos, const T& data ) {
        // std::cerr <<"  write in "<< pos <<std::endl;
        if ( !pos ) return ;
        // sav.seekg(0, std::ios::end);
        // std::cerr <<"    write check end "<< sav.tellg() <<' '<< sizeof(T) <<std::endl;
        sav.seekp(sizeof(T)*(pos-1));
        sav.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    bool empty() {
        sav.seekg(0,std::ios::end);
        return sav.tellg()==0;
    }

    void clear ( int pos ) {
        // T tmp=T();
        // sav.seekp(sizeof(T)*(pos-1));
        // sav.write(reinterpret_cast<char*>(&tmp), sizeof(T));

        // bin.push_back(pos);
    }
    int get_address () {
        // int pos;
        // if ( !bin.empty() ) pos=bin.back(), bin.pop_back();
        // else pos=++siz;
        // return pos;
        return ++siz;
    }
    int insert ( const T& data ) {
        int pos=get_address();
        write(pos,data);
        return pos;
    }

    void clear () {
        close();
        // puts(("clear data/"+name+".dat").c_str());
        std::fstream clr_sav(path+name+".dat", std::ios::out|std::ios::binary|std::ios::trunc);
        std::fstream clr_rec(path+name+"_rec.dat", std::ios::out|std::ios::binary|std::ios::trunc);
        clr_sav.seekp(0, std::ios::beg);
        clr_rec.seekp(0, std::ios::beg);
        clr_sav.close();
        clr_rec.close();
    }

};

template < class Key, class T, class Comp=std::less<Key> >
class bplus_tree {
  private:
    const static int BLOCK_SIZE=8000; //4kb
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
            // for ( int i=siz++ ; i>pos ; i-- ) {
            //     keys[i]=keys[i-1],
            //     child[i+1]=child[i];
            // }
            // child[pos+1]=child[pos];

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
            // for ( int i=pos ; i<siz-1 ; i++ ) {
            //     keys[i]=keys[i+1],
            //     child[i]=child[i+1];
            // }
            // child[siz-1]=child[siz];
            memmove(keys+pos, keys+pos+1, sizeof(Key)*(siz-pos));
            memmove(child+pos, child+pos+1, sizeof(int)*(siz-pos+1));
            --siz;
        }
        void pop ( const int pos ) {
            std::swap(child[pos], child[pos+1]);
            erase(pos);
        }

        void merge ( const node& nod ) {
            // here can use memcpy()
            // for ( int i=0 ; i<nod.siz ; i++ ) {
            //     keys[siz+i]=nod.keys[i];
            //     child[siz+i]=nod.child[i];
            // }
            memcpy(keys+siz, nod.keys, sizeof(Key)*nod.siz);
            memcpy(child+siz, nod.child, sizeof(int)*(nod.siz+1));
            siz+=nod.siz;
            // child[siz]=nod.child[nod.siz];
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
    save<int> f_temp;
    int root, siz;

    using iterator=pair<node, int>;

    pair<node, Key> split ( int addr, node& nod, const int is_leaf ) {
        int delta=is_leaf^1, new_siz=nod.siz-SIZ-delta;
        node new_nod(nod.parent, addr, nod.right, new_siz, is_leaf);
        Key key=nod.keys[SIZ];

        // here can use memcpy()
        // for ( int i=0 ; i<new_siz ; i++ ) {
        //     new_nod.keys[i]=nod.keys[SIZ+i+delta],
        //     new_nod.child[i]=nod.child[SIZ+i+delta];
        //     nod.keys[SIZ+i+delta]=Key(),
        //     nod.child[SIZ+i+delta]=0;
        // }

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
        f_tree.open("tree"),
        f_data.open("data");
        f_temp.open("temp");

        if ( f_temp.empty() ) root=0;
        else root=f_temp.read(1);
    }
    ~bplus_tree () {
        f_temp.write(1,root);
        f_tree.close(),
        f_data.close(),
        f_temp.close();
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
        f_temp.clear();
    }

};

}

int main () {
    using pr=cay::pair<cay::string<65>, int>;

    cay::bplus_tree<pr, int> T;

    try {

    int n;
    scanf("%d",&n);
    while ( n-- ) {
        char op[10];
        char* _index=new char[65];
        int value;
        scanf("%s %s", op, _index);

        cay::string<65> index(reinterpret_cast<const char*>(_index));

        if ( op[0]=='i' ) {
            scanf("%d",&value);
            T.insert(pr(index, value), 0);
        }
        if ( op[0]=='d' ) {
            scanf("%d",&value);
            T.erase(pr(index, value));
        }
        if ( op[0]=='f' ) {
            cay::vector<pr> ret=T.find(pr(index, -1), pr(index, 2147483647));
            if ( ret.empty() ) printf("null");
            for ( auto it=ret.begin() ; it!=ret.end() ; ++it ) 
                printf("%d ",(*it).second);
            putchar('\n');
        }

        if ( op[0]=='g' ) T.debug(1);

        delete[] _index;
    }

    }
    catch ( ... ) {
        std::cerr <<"ERROR!\n";
        T.clear();
    }

    // T.clear();

    return 0;
}