/***********************************************************
 *   文件名: sr_type.h
 *     版权: 武汉随锐亿山 2015-2020
 *     作者: Marvin Zhai
 * 文件说明: MC项目类型定义和printf的格式输出定义,常用宏定义
 *           只针对g++ && linux
 * 创建日期: 2015.10.14
 * 修改日期:
 *   修改人:
 *************************************************************/

#ifndef SUIRUI_MC_SRTYPE_H
#define SUIRUI_MC_SRTYPE_H
/////////////////////////////////////////类型定义////////////////////////////////////////////

#ifndef SRMC_DEFINE_TYPE
#define SRMC_DEFINE_TYPE

typedef bool						SR_bool;
typedef char						SR_char;
typedef signed char 				SR_int8;
typedef unsigned char 				SR_uint8;
typedef signed short int 			SR_int16;
typedef unsigned short int 			SR_uint16;
typedef signed int 					SR_int32;
typedef unsigned int 				SR_uint32;
#if defined(__x86_64__) || defined(_WIN64) || defined(__aarch64__)
typedef signed long  int 			SR_int64;
typedef unsigned long int 			SR_uint64;
#elif defined(__i386__)|| defined(_WIN32)
typedef signed long long int 		SR_int64;
typedef unsigned long long int	 	SR_uint64;
#else
#error "unknow platform"
#endif
typedef unsigned long int 			SR_ptr;
typedef void 						SR_void;
typedef int							SR_socket;

// printf 格式输出的时候 ,使用方式
// size_t a;
// printf("a="SR_PRI_size_t"\n",a);
#if defined(__x86_64__) || defined(_WIN64)
#  define SR__PRI64_PREFIX	"l"
#  define SR_PRI_size_t "lu"
#else
#  define SR__PRI64_PREFIX	"ll"
#  define SR_PRI_size_t 	"u"
#endif

# define SR_PRId64		SR__PRI64_PREFIX "d"
# define SR_PRIu64		SR__PRI64_PREFIX "u"
# define SR_PRIx64		SR__PRI64_PREFIX "x"
/* UPPERCASE hexadecimal notation.  */
# define SR_PRIX64		SR__PRI64_PREFIX "X"

#endif // #ifndef SRMC_DEFINE_TYPE

#ifndef SR_MACRO_STRING
#define SR_MACRO_STRING(x) #x
#endif	 
	 
#define SUIRUI_DISALLOW_DEFAULT_CONSTRUCTOR(TypeName) TypeName()
#define SUIRUI_DISALLOW_COPY_AND_ASSIGN(TypeName)\
  TypeName(const TypeName&);                      \
  SR_void operator=(const TypeName&)

//关闭编译器告警
#define SUIRUI_UNUSE_VAR(a) ((a) = (a))


#ifndef SUIRUI_PREDICT_TRUE
#ifdef __GNUC__
// Provided at least since GCC 3.0.
#define SUIRUI_PREDICT_TRUE(x)  (__builtin_expect(!!(x), 1))
#define SUIRUI_PREDICT_FALSE(x) (__builtin_expect(!!(x), 0))
#else
#define SUIRUI_PREDICT_TRUE(x)
#define SUIRUI_PREDICT_FALSE(x) 
#endif
#endif

#ifndef SUIRUI_ATTRIBUTE_ALWAYS_INLINE
#if defined(__GNUC__) && (__GNUC__ > 3 ||(__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
// For functions we want to force inline.
// Introduced in gcc 3.1.
#define SUIRUI_ATTRIBUTE_ALWAYS_INLINE __attribute__ ((always_inline))
#else
// Other compilers will have to figure it out for themselves.
#define SUIRUI_ATTRIBUTE_ALWAYS_INLINE
#endif
#endif


#ifndef sr_printf
#include<stdio.h>
#include "TraceLog.h"

#define SR_LOGLEVEL_EMERG    TL_EMERG
#define SR_LOGLEVEL_ALERT    TL_ALERT
#define SR_LOGLEVEL_CRIT     TL_CRIT
#define SR_LOGLEVEL_ERROR    TL_ERR
#define SR_LOGLEVEL_WARNING  TL_WARNING
#define SR_LOGLEVEL_NORMAL   TL_NOTICE
#define SR_LOGLEVEL_INFO     TL_INFO
#define SR_LOGLEVEL_DEBUG    TL_DEBUG

#define sr_printf(level,format,...) \
	TraceLog::GetInstance().TraceLogging_withoutnewline(__FILE__, __LINE__, level, format, ##__VA_ARGS__)

#endif // #ifndef sr_printf


#endif //#define SUIRUI_MC_SRTYPE_H