#ifndef BUFFER_H
#define BUFFER_H
#include <list>
#include <stdio.h>
#include <string.h>
template<class T, class LockT>
class Buffer
{
public:
    Buffer() {}
    virtual ~Buffer() {}
public:
    void Push ( T* data ) {
        lock_.lock();
        buf_list_.push_back ( data );
        lock_.unlock();
    }
    void PushFront ( T* data ) {
        lock_.lock();
        buf_list_.push_front ( data );
        lock_.unlock();
    }
    T* pop() {
        T* obj_ptr = NULL;
        lock_.lock();
        if ( !buf_list_.empty() ) {
            obj_ptr = * ( buf_list_.begin() );
            buf_list_.pop_front();
        }
        lock_.unlock();
        return obj_ptr;
    }
    bool isEmpty() {
        bool is_empty = false;
        lock_.lock();
        is_empty = buf_list_.empty();
        lock_.unlock();
        return is_empty;
    }
private:
    std::list<T*> buf_list_;
    LockT lock_;
private:
    Buffer<T, LockT>& operator= ( const Buffer<T, LockT>& lhs );
    Buffer<T, LockT> ( const Buffer<T, LockT>& lhs );
};

template<class T, class LockT>
class MemPool
{
public:
    typedef T*  BufPtr;

    explicit MemPool ( int buf_len=3 ) 
    {
        bufptr_list_ = new BufPtr[buf_len];
        memset ( bufptr_list_, 0, sizeof ( BufPtr ) *buf_len );
        buf_len_ = buf_len;
        cur_obj_cnt_ = 0;
        new_cnt_statis_ = 0;
        delete_cnt_statis_ = 0;
    }
    ~MemPool() 
    {
        unsigned int index = 0;
        for ( ; index < buf_len_; index++ ) 
        {
            if ( NULL != bufptr_list_[index] )
            {
                delete bufptr_list_[index];
            }
        }
        delete[] bufptr_list_;
    }
public:
    void Release ( T*& data ) {
        if (NULL == data)
        {
            return;
        }
        lock_.lock();
        if(cur_obj_cnt_ >= buf_len_)
        {
            delete data;
            data = NULL;
            delete_cnt_statis_++;
        }
        else
        {
            bufptr_list_[cur_obj_cnt_] = data;
            cur_obj_cnt_ += 1;
        }
        lock_.unlock();
    }
    template<class TSubClass>
	T* GetMemery(TSubClass* objtmp = NULL) {

        T* obj_ptr = NULL;
        lock_.lock();        
        if (0 == cur_obj_cnt_)
        {
            obj_ptr = new TSubClass;
            new_cnt_statis_++;
        }
        else
        {
            cur_obj_cnt_ -= 1;
            obj_ptr = bufptr_list_[cur_obj_cnt_];
            bufptr_list_[cur_obj_cnt_] = NULL;
        }
        lock_.unlock();
        return obj_ptr;
    }

    T* GetMemery() 
    {
        T* obj_ptr = NULL;
        lock_.lock();		 
        if (0 == cur_obj_cnt_)
        {
        	obj_ptr = new T;
        	new_cnt_statis_++;
        }
        else
        {
        	cur_obj_cnt_ -= 1;
        	obj_ptr = bufptr_list_[cur_obj_cnt_];
        	bufptr_list_[cur_obj_cnt_] = NULL;
        }
        lock_.unlock();
        return obj_ptr;
	}
    
    void GetStatics(unsigned int& new_cnt_statis, unsigned int& delete_cnt_statis, bool reset = true)
    {
        lock_.lock();
        new_cnt_statis = new_cnt_statis_;
        delete_cnt_statis = delete_cnt_statis_;
        if (reset)
        {
            new_cnt_statis_ = 0;
            delete_cnt_statis_ = 0;
        }
        lock_.unlock();
    }
private:
    LockT lock_;
    BufPtr* bufptr_list_;
    unsigned int buf_len_;
    unsigned int cur_obj_cnt_;
    unsigned int new_cnt_statis_;
    unsigned int delete_cnt_statis_;
private:
    MemPool<T, LockT>& operator= ( const MemPool<T, LockT>& lhs );
    MemPool<T, LockT> ( const MemPool<T, LockT>& lhs );
};

#endif // BUFFER_H




