/***********************************************************
 *   文件名: sr_pb_codec.h
 *     版权: 武汉随锐亿山 2015-2020
 *     作者: Marvin Zhai
 * 文件说明: protobuf的32/64位Varint编解码,脱离google protobuf库
 * 创建日期: 2016.02.24
 * 修改日期:
 *   修改人:
 *************************************************************/
#ifndef SR_PB_CODEC_H
#define SR_PB_CODEC_H

#include "sr_type.h"

namespace SRMC{


enum WireType {
  WIRETYPE_VARINT			= 0,//uint32,uint64,bool
  WIRETYPE_FIXED64			= 1,
  WIRETYPE_LENGTH_DELIMITED = 2,//string
  WIRETYPE_START_GROUP		= 3,
  WIRETYPE_END_GROUP		= 4,
  WIRETYPE_FIXED32			= 5,
};

//string 类型 tag_no  field_number 最大不要超过 9
//所有的tag 最大不要超过9,小于等于9的直接赋值就可以
#define SR_string_tag(FIELD_NUMBER) (((FIELD_NUMBER) << 3) | WIRETYPE_LENGTH_DELIMITED)
//(tag = FIELD_NUMBER)
#define SR_uint_tag(FIELD_NUMBER) (((FIELD_NUMBER) << 3) | WIRETYPE_VARINT)

//ASSERT !!! tag 最大值 16384
static inline SR_uint8* WriteTagToArray(SR_uint32 value, SR_uint8* target) SUIRUI_ATTRIBUTE_ALWAYS_INLINE;
static inline SR_uint8* WriteTagToArray(SR_uint32 value, SR_uint8* target) 
{
	if (value < (1 << 7)) 
	{
		target[0] = value;
		return target + 1;
	}
	else //if (value < (1 << 14))
	{
		target[0] = static_cast<SR_uint8>(value | 0x80);
		target[1] = static_cast<SR_uint8>(value >> 7);
		return target + 2;
	}
}

static inline SR_uint8* WriteVarint64ToArray(SR_uint64 value, SR_uint8* target) SUIRUI_ATTRIBUTE_ALWAYS_INLINE;
static inline SR_uint8* WriteVarint64ToArray(SR_uint64 value, SR_uint8* target) 
{
	// Splitting into 32-bit pieces gives better performance on 32-bit processors.
	SR_uint32 part0 = static_cast<SR_uint32>(value);
	SR_uint32 part1 = static_cast<SR_uint32>(value >> 28);
	SR_uint32 part2 = static_cast<SR_uint32>(value >> 56);

	SR_int32 size;

	// Here we can't really optimize for small numbers, since the value is
	// split into three parts.  Cheking for numbers < 128, for instance,
	// would require three comparisons, since you'd have to make sure part1
	// and part2 are zero.  However, if the caller is using 64-bit integers,
	// it is likely that they expect the numbers to often be very large, so
	// we probably don't want to optimize for small numbers anyway.  Thus,
	// we end up with a hardcoded binary search tree...
	if (part2 == 0) {
	if (part1 == 0) {
	  if (part0 < (1 << 14)) {
	    if (part0 < (1 << 7)) {
	      size = 1; goto size1;
	    } else {
	      size = 2; goto size2;
	    }
	  } else {
	    if (part0 < (1 << 21)) {
	      size = 3; goto size3;
	    } else {
	      size = 4; goto size4;
	    }
	  }
	} else {
	  if (part1 < (1 << 14)) {
	    if (part1 < (1 << 7)) {
	      size = 5; goto size5;
	    } else {
	      size = 6; goto size6;
	    }
	  } else {
	    if (part1 < (1 << 21)) {
	      size = 7; goto size7;
	    } else {
	      size = 8; goto size8;
	    }
	  }
	}
	} else {
	if (part2 < (1 << 7)) {
	  size = 9; goto size9;
	} else {
	  size = 10; goto size10;
	}
	}

	size10: target[9] = static_cast<SR_uint8>((part2 >>  7) | 0x80);
	size9 : target[8] = static_cast<SR_uint8>((part2      ) | 0x80);
	size8 : target[7] = static_cast<SR_uint8>((part1 >> 21) | 0x80);
	size7 : target[6] = static_cast<SR_uint8>((part1 >> 14) | 0x80);
	size6 : target[5] = static_cast<SR_uint8>((part1 >>  7) | 0x80);
	size5 : target[4] = static_cast<SR_uint8>((part1      ) | 0x80);
	size4 : target[3] = static_cast<SR_uint8>((part0 >> 21) | 0x80);
	size3 : target[2] = static_cast<SR_uint8>((part0 >> 14) | 0x80);
	size2 : target[1] = static_cast<SR_uint8>((part0 >>  7) | 0x80);
	size1 : target[0] = static_cast<SR_uint8>((part0      ) | 0x80);

	target[size-1] &= 0x7F;
	return target + size;
}

// 128(0x80)  16384(0x4000)
//bool 类型也是 WriteVarint32ToArray
static inline SR_uint8* WriteVarint32ToArray(SR_uint32 value,SR_uint8* target) SUIRUI_ATTRIBUTE_ALWAYS_INLINE;
static inline SR_uint8* WriteVarint32ToArray(SR_uint32 value,SR_uint8* target)
{
	if (value < 0x80) 
	{
		*target = value;
		return target + 1;
	} 
	else 
	{
		target[0] = static_cast<SR_uint8>(value | 0x80);
		target[1] = static_cast<SR_uint8>((value >>  7) | 0x80);
	    if (value >= (1 << 14)) 
		{
			target[2] = static_cast<SR_uint8>((value >> 14) | 0x80);
			if (value >= (1 << 21)) 
			{
				target[3] = static_cast<SR_uint8>((value >> 21) | 0x80);
				if (value >= (1 << 28)) {
				  target[4] = static_cast<SR_uint8>(value >> 28);
				  return target + 5;
				} else {
				  target[3] &= 0x7F;
				  return target + 4;
				}
			} 
			else 
			{
				target[2] &= 0x7F;
				return target + 3;
			}
	    }
		else 
		{
			target[1] &= 0x7F;//TODO:不需要
			return target + 2;
	    }
  	}
}

static inline SR_uint8* WriteStringToArray(SR_int32 field_number,const std::string& value,SR_uint8* target) SUIRUI_ATTRIBUTE_ALWAYS_INLINE;
static inline SR_uint8* WriteStringToArray(SR_int32 field_number,const std::string& value,SR_uint8* target) 
{
	// String is for UTF-8 text only
	target = WriteTagToArray(SR_string_tag(field_number),target);
	memcpy(target,value.data(),static_cast<int>(value.size()));
	return target + static_cast<int>(value.size());
}


static inline const SR_uint8* ReadVarint32FromArrayFast(const SR_uint8* ptr, SR_uint32* value) SUIRUI_ATTRIBUTE_ALWAYS_INLINE;
static inline const SR_uint8* ReadVarint32FromArrayFast(const SR_uint8* ptr, SR_uint32* value) 
{
  // Fast path:  We have enough bytes left in the buffer to guarantee that
  // this read won't cross the end, so we can skip the checks.
  SR_uint32 b;
  SR_uint32 result;

  b = *(ptr++); result  = b      ; if (!(b & 0x80)) goto done;
  result -= 0x80;
  b = *(ptr++); result += b <<  7; if (!(b & 0x80)) goto done;
  result -= 0x80 << 7;
  b = *(ptr++); result += b << 14; if (!(b & 0x80)) goto done;
  result -= 0x80 << 14;
  b = *(ptr++); result += b << 21; if (!(b & 0x80)) goto done;
  result -= 0x80 << 21;
  b = *(ptr++); result += b << 28; if (!(b & 0x80)) goto done;
 
  return (SR_uint8*)0;

 done:
  *value = result;
  return ptr;
}

static inline const SR_uint8* ReadVarint64FromArrayFast(const SR_uint8* ptr, SR_uint64* value) SUIRUI_ATTRIBUTE_ALWAYS_INLINE;
static inline const SR_uint8* ReadVarint64FromArrayFast(const SR_uint8* ptr, SR_uint64* value) 
{
	SR_uint32 b;
	// Splitting into 32-bit pieces gives better performance on 32-bit processors.
	SR_uint32 part0 = 0, part1 = 0, part2 = 0;

	b = *(ptr++); part0  = b	  ; if (!(b & 0x80)) goto done;
	part0 -= 0x80;
	b = *(ptr++); part0 += b <<  7; if (!(b & 0x80)) goto done;
	part0 -= 0x80 << 7;
	b = *(ptr++); part0 += b << 14; if (!(b & 0x80)) goto done;
	part0 -= 0x80 << 14;
	b = *(ptr++); part0 += b << 21; if (!(b & 0x80)) goto done;
	part0 -= 0x80 << 21;
	b = *(ptr++); part1  = b	  ; if (!(b & 0x80)) goto done;
	part1 -= 0x80;
	b = *(ptr++); part1 += b <<  7; if (!(b & 0x80)) goto done;
	part1 -= 0x80 << 7;
	b = *(ptr++); part1 += b << 14; if (!(b & 0x80)) goto done;
	part1 -= 0x80 << 14;
	b = *(ptr++); part1 += b << 21; if (!(b & 0x80)) goto done;
	part1 -= 0x80 << 21;
	b = *(ptr++); part2  = b	  ; if (!(b & 0x80)) goto done;
	part2 -= 0x80;
	b = *(ptr++); part2 += b <<  7; if (!(b & 0x80)) goto done;

	return (SR_uint8*)0;

   done:
	*value = (static_cast<SR_uint64>(part0) 	 ) |
			 (static_cast<SR_uint64>(part1) << 28) |
			 (static_cast<SR_uint64>(part2) << 56);
	return ptr;
}


static inline SR_bool ReadVarint64FromArraySlow(const SR_uint8* ptr,SR_int32 ptr_len,SR_uint64* value) SUIRUI_ATTRIBUTE_ALWAYS_INLINE;
static inline SR_bool ReadVarint64FromArraySlow(const SR_uint8* ptr,SR_int32 ptr_len,SR_uint64* value) 
{
    SR_uint32 b;
    // Splitting into 32-bit pieces gives better performance on 32-bit processors.
    SR_uint32 part0 = 0, part1 = 0, part2 = 0;

    if(ptr_len < 1) goto ptr_error; b = *(ptr++); part0  = b      ; if (!(b & 0x80)) goto done;
    part0 -= 0x80;
    if(ptr_len < 2) goto ptr_error; b = *(ptr++); part0 += b <<  7; if (!(b & 0x80)) goto done;
    part0 -= 0x80 << 7;
    if(ptr_len < 3) goto ptr_error; b = *(ptr++); part0 += b << 14; if (!(b & 0x80)) goto done;
    part0 -= 0x80 << 14;
    if(ptr_len < 4) goto ptr_error; b = *(ptr++); part0 += b << 21; if (!(b & 0x80)) goto done;
    part0 -= 0x80 << 21;
    if(ptr_len < 5) goto ptr_error; b = *(ptr++); part1  = b      ; if (!(b & 0x80)) goto done;
    part1 -= 0x80;
    if(ptr_len < 6) goto ptr_error; b = *(ptr++); part1 += b <<  7; if (!(b & 0x80)) goto done;
    part1 -= 0x80 << 7;
    if(ptr_len < 7) goto ptr_error; b = *(ptr++); part1 += b << 14; if (!(b & 0x80)) goto done;
    part1 -= 0x80 << 14;
    if(ptr_len < 8) goto ptr_error; b = *(ptr++); part1 += b << 21; if (!(b & 0x80)) goto done;
    part1 -= 0x80 << 21;
    if(ptr_len < 9) goto ptr_error; b = *(ptr++); part2  = b      ; if (!(b & 0x80)) goto done;
    part2 -= 0x80;
    if(ptr_len < 10)goto ptr_error; b = *(ptr++); part2 += b <<  7; if (!(b & 0x80)) goto done;
    // "part2 -= 0x80 << 7" is irrelevant because (0x80 << 7) << 56 is 0.

    // We have overrun the maximum size of a varint (10 bytes).  The data must be corrupt.
   ptr_error: 
    return false;

   done:
    *value = (static_cast<SR_uint64>(part0)      ) |
             (static_cast<SR_uint64>(part1) << 28) |
             (static_cast<SR_uint64>(part2) << 56);
    return true;
}

} // namespace SRMC
#endif // SR_PB_CODEC_H