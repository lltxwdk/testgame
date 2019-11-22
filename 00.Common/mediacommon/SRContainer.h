#ifndef __SR_CONTAINER_H__
#define __SR_CONTAINER_H__
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "TraceLog.h"

template<uint32_t ARRAY_SIZE, class ITEM_TYPE>
class ArrayQueue
{
    friend class ArrayQueue_UTest;
    typedef struct _tagArrayItem
    {
        ITEM_TYPE* user_data_;
        uint32_t next_index_;
        _tagArrayItem()
        {
            user_data_ = NULL;
            next_index_ = ARRAY_SIZE;
        }
    } ArrayItem;
public:
    explicit ArrayQueue()
    {
        min_used_index_ = ARRAY_SIZE;
        max_used_index_ = 0;
    }

    bool GetAvailableIndex(uint32_t& index)
    {
        for (uint32_t i = 0; i < ARRAY_SIZE; i++)
        {
            if (NULL != array_[i].user_data_)
            {
                //assert(array_[i].next_index_ != 0);
                continue;
            }
            index = i;
            return true;
        }
        return false;
    }
    void Clear()
    {
        min_used_index_ = ARRAY_SIZE;
        max_used_index_ = 0;
        for (uint32_t i = 0; i < ARRAY_SIZE; i++)
        {
            ResetIndex(i);
        }
    }
    void Insert(uint32_t index, ITEM_TYPE* user_data)
    {
        if (InsertInl(index, user_data))
        {
            if (max_used_index_ < index)
            {
                max_used_index_ = index;
            }
        }
    }

    void Remove(uint32_t index)
    {
        uint32_t out_preindex = ARRAY_SIZE;
        if (RemoveInl(index, out_preindex))
        {
            assert(index <= max_used_index_);
            assert(out_preindex <= ARRAY_SIZE);
            if (max_used_index_ == index)
            {
                if (ARRAY_SIZE == out_preindex)
                {
                    assert(ARRAY_SIZE == min_used_index_);
                    max_used_index_ = 0;
                }
                else
                {
                    max_used_index_ = out_preindex;
                }
            }
        }
    }
#if 0
    ITEM_TYPE* GetBeginItem(uint32_t& index)
    {
        if (ARRAY_SIZE == min_used_index_)
        {
            return NULL;
        }
        assert(NULL != array_[min_used_index_].user_data_); 
        index = min_used_index_;
		//YANCH: ���ܴ�������? ���統ǰֻ��һ��Ԫ��, ִ�е��˴�ʱ, ��һ���߳�ɾ����Ψһ��Ԫ��, ����min_used_index_��ΪARRAY_SIZE, ��˾�ᵼ���ڴ����Խ��(array_[min_used_index_].user_data_ΪҰָ��)? 
		//       YANCH_TODO: array_��������һ��(ArrayItem array_[ARRAY_SIZE+1];), ���һ����Ϊ����λ, ����ʹ��, ����ֹ�������ڴ�Խ��(array_[min_used_index_].user_data_ΪNULL). ����Ķ���ҲӦ��ȥ��.
        return array_[min_used_index_].user_data_; 
    }
#else //YANCH: YANCH_TODO: ����2: ʹ����ʱ����, ��ֹ mix_used_index_���޸�, ����֤.
	ITEM_TYPE* GetBeginItem(uint32_t& index)
	{
		uint32_t index_tmp = min_used_index_;
		if (ARRAY_SIZE == index_tmp) //��������ʹ��min_used_index_, ��ֹ�������̱߳��޸�(��ΪARRAY_SIZE)
		{
			return NULL;
		}
		//assert(NULL != array_[index_tmp].user_data_); //���������߳�ͬʱ���� Remove ɾ����һ��Ԫ��(����index����min_used_index_), user_data_ ���ܱ��ÿ�.
		index = index_tmp;
		return array_[index_tmp].user_data_;
	}
#endif

    ITEM_TYPE* GetNextItem(uint32_t& index)
    {
        assert(index < ARRAY_SIZE);
        if (index >= ARRAY_SIZE)
        {
            return NULL;
        }

        uint32_t next_index = array_[index].next_index_;
        ITEM_TYPE* next_data = NULL;
        assert(0 < next_index);
        if (next_index < ARRAY_SIZE)
        {
            next_data = array_[next_index].user_data_;
            if (next_data)
            {
                index = next_index;
                return next_data;
            }
        }
        else
        {
            assert(ARRAY_SIZE == next_index);
        }

        if (index >= max_used_index_)
        {
            return NULL;
        }

        //handle exception case
        LogNOTICE("handle exception case by concurrent access, index:%d, max:%d", \
            index, max_used_index_);
        for (next_index = index + 1; next_index <= max_used_index_; next_index++)
        {
            next_data = array_[next_index].user_data_;
            if (next_data)
            {
                index = next_index;
                return next_data;
            }
        }
        return NULL;
    }

    ITEM_TYPE* GetItem(uint32_t index)
    {
        if (index >= ARRAY_SIZE)
        {
            assert(false);
            return NULL;
        }
        return array_[index].user_data_;
    }
private:
    bool InsertInl(uint32_t index, ITEM_TYPE* user_data)
    {
        assert(user_data);
        assert(index < ARRAY_SIZE);
        if (NULL == user_data || index >= ARRAY_SIZE)
        {
            return false;
        }

        if (array_[index].user_data_)
        {
            assert(array_[index].user_data_ == user_data);
            return false;
        }

        if (ARRAY_SIZE == min_used_index_)
        {
            array_[index].user_data_ = user_data;
            min_used_index_ = index;
            return true;
        }

        if (index < min_used_index_)
        {
            assert(array_[min_used_index_].user_data_ != NULL);
            array_[index].user_data_ = user_data;
            array_[index].next_index_ = min_used_index_;
            min_used_index_ = index;
            return true;
        }

        assert(index > min_used_index_);
        uint32_t pre_index = index - 1;
        for (; pre_index > min_used_index_; pre_index--)
        {
            if (NULL != array_[pre_index].user_data_)
            {
                break;
            }
            assert(ARRAY_SIZE == array_[pre_index].next_index_);
        }

        assert(NULL != array_[pre_index].user_data_);
        assert(ARRAY_SIZE == array_[pre_index].next_index_ || array_[pre_index].next_index_ > index);
        array_[index].user_data_ = user_data;
        array_[index].next_index_ = array_[pre_index].next_index_;
        array_[pre_index].next_index_ = index;
        return true;
    }

    bool RemoveInl(uint32_t index, uint32_t& out_preindex)
    {
        out_preindex = ARRAY_SIZE;
        if (ARRAY_SIZE == min_used_index_)
        {
            //assert(false);
            return false;
        }
        if (!(index < ARRAY_SIZE && index >= min_used_index_))
        {
            if (index >= ARRAY_SIZE)
            {
                assert(false);
            }
            return false;
        }

        if (index == min_used_index_)
        {
            min_used_index_ = array_[index].next_index_;
            assert(array_[index].user_data_ != NULL);
            ResetIndex(index);
            return true;
        }

        bool is_find = false;
        uint32_t pre_index = min_used_index_;
        for (; pre_index < index;)
        {
            if (index == array_[pre_index].next_index_)
            {
                is_find = true;
                break;
            }

            pre_index = array_[pre_index].next_index_;
            if (ARRAY_SIZE == pre_index)
            {
                break;
            }
        }

        if (is_find)
        {
            array_[pre_index].next_index_ = array_[index].next_index_;
            ResetIndex(index);
            out_preindex = pre_index;
            return true;
        }

        //assert(false);
        return false;
    }

    void ResetIndex(uint32_t index)
    {
        array_[index].user_data_ = NULL;
        array_[index].next_index_ = ARRAY_SIZE;
    }
private:
    ArrayItem array_[ARRAY_SIZE];
    volatile  uint32_t min_used_index_;
    volatile  uint32_t max_used_index_;
private:
    ArrayQueue(const ArrayQueue&);
};


#define BEGIN_TRAVERSE_ARRAY(ITEM_TYPE, ITEMPTR, ARRAY) \
    {\
        uint32_t cur_index = 0;\
        ITEM_TYPE* ITEMPTR = ARRAY.GetBeginItem(cur_index);\
        if (NULL != ITEMPTR)\
        {\
            do\
            {

#define END_TRAVERSE_ARRAY(ITEMPTR, ARRAY) \
                ITEMPTR = ARRAY.GetNextItem(cur_index); \
            } while (ITEMPTR);\
         }\
    }

#endif

