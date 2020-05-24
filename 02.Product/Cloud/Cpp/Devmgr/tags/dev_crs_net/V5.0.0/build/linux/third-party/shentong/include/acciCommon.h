#ifndef ACCICOMMON_ST
#define ACCICOMMON_ST

#include <aci.h>

#ifndef ORASTRINGSTL
#define ORASTRINGSTL
#include <string>
#endif

#ifndef ORAVECTORSTL
#include <vector>
#define ORAVECTORSTL
#endif

#ifndef ORALISTSTL
#include <list>
#define ORALISTSTL
#endif

#define ACCI_STD_NAMESPACE std
#define ACCI_HAVE_STD_NAMESPACE 1

// version definitions
#define ACCI_MAJOR_VERSION ACI_MAJOR_VERSION
#define ACCI_MINOR_VERSION ACI_MINOR_VERSION

namespace shentong {
namespace acci {

//UString is the class for UTF16 characterset
//check for version = 3.2 or 3.3
#if (__GNUC__ == 3 && (__GNUC_MINOR__ == 2 || __GNUC_MINOR__ == 3))  
  //char_traits<T> specialization for utext for gcc 3.2.3
  struct utext_char_traits
  {
  typedef utext          char_type;
  typedef unsigned int   int_type;

  typedef OCCI_STD_NAMESPACE::streampos      pos_type;
  typedef OCCI_STD_NAMESPACE::streamoff      off_type;
  typedef OCCI_STD_NAMESPACE::mbstate_t      state_type;

  //assign one char to another
  static void assign(char_type& c1, const char_type& c2)
    { 
        c1 = c2; 
    }

  //are 2 chars equal ?
  static bool eq(const char_type& c1, const char_type& c2) 
    {
       return c1 == c2; 
    }

  //is char c1 less then c2 ?
  static bool lt(const char_type& c1, const char_type& c2) 
    { 
       return c1 < c2; 
    }

  //compare 2 strings of char
  static int compare(const char_type* s1, const char_type* s2, size_t n) 
    {
      for (size_t i = 0; i < n; ++i)
        if (!eq(s1[i], s2[i]))
          return s1[i] < s2[i] ? -1 : 1;
      return 0;
    }

  //length of a char string
  static size_t length(const char_type* s) 
    {
      const char_type nullchar = char_type(0);
      size_t i = 0;
   
        while (!eq(*s++, nullchar)) i++; 
        return i;
    }

  //find a character in the char string
  static const char_type* find(const char_type* s, 
                      size_t n, const char_type& c)
    {
       for ( ; n > 0 ; ++s, --n)
         if (eq(*s, c))
           return s;
       return 0;
    }

  //move n chars from s2 to s1
  static char_type* move(char_type* s1, const char_type* s2, size_t n) 
    {
       memmove(s1, s2, n * sizeof(char_type));
       return s1;
    }
 
  //copy n chars from s2 to s1   
  static char_type* copy(char_type* s1, const char_type* s2, size_t n) 
    {
       memcpy(s1, s2, n * sizeof(char_type));
       return s1;
    } 

  //fill char c into s
  static char_type* assign(char_type* s, size_t n, char_type c) 
    {
      for (size_t i = 0; i < n; ++i)
        assign(s[i], c);
      return s;
    }

  //is the int representation eof ?
  static int_type not_eof(const int_type& c) 
    {
       if (c == eof())
         return 0;
       else
         return c;
    }

  //cast a int type to char
  static char_type to_char_type(const int_type& c)
    { 
       return static_cast<char_type>(c);
    }

  //cast char to int type
  static int_type to_int_type(const char_type& c)
    {  
      return static_cast<int_type>(c);
    }

  //eq operator when the chars are represented as ints
  static bool eq_int_type(const int_type& c1, const int_type& c2) 
    {
      return c1 == c2;
    }

  //eof character
  static int_type eof() 
    {
      return static_cast<int_type>(-1);
    }

   
  }; //end char_traits<unsigned short>

typedef ACCI_STD_NAMESPACE::basic_string<utext, utext_char_traits> UString;
#else
//for non gcc 3.2.3 platforms
typedef ACCI_STD_NAMESPACE::basic_string<utext> UString;
#endif /* if gcc 3.2.3 */

class Environment;
class EnvironmentImpl;
class Connection;
class ConnectionImpl;
class ConnectionPool;
class ConnectionPoolImpl;
class StatelessConnectionPool;
class StatelessConnectionPoolImpl;
class Statement;
class StatementImpl;
class ResultSet;
class ResultSetImpl;
class SQLException;
class SQLExceptionImpl;
class BatchSQLException;
class BatchSQLExceptionImpl;
class Stream;
class PObject;
class Number;
class Bytes;
class BytesImpl;
class Date;
class Timestamp;

class MetaData;
class MetaDataImpl;
template <class T> class Ref;
class RefImpl;
class RefAny;
class Blob;
class Bfile;
class Clob;
class LobStreamImpl;
class AnyData;
class AnyDataImpl;
class Map;
class IntervalDS;
class IntervalYM;

namespace aq {
class Message;
class MessageImpl;
class Agent;
class AgentImpl;
class Producer;
class ProducerImpl;
class Consumer;
class ConsumerImpl;
class Listener;
class Subscription;
class SubscriptionImpl;
class NotifyResult;
}

typedef struct BFloat BFloat;
typedef struct BDouble BDouble;

/*---------------------------------------------------------------------------
                           ENUMERATORS
  ---------------------------------------------------------------------------*/
enum Type
{
 ACCI_SQLT_CHR=SQLT_CHR,
 ACCI_SQLT_NUM=SQLT_NUM,
 ACCIINT = SQLT_INT,
 ACCIFLOAT = SQLT_FLT,
 ACCIBFLOAT = SQLT_BFLOAT,
 ACCIBDOUBLE = SQLT_BDOUBLE,
 ACCIIBFLOAT = SQLT_IBFLOAT,
 ACCIIBDOUBLE = SQLT_IBDOUBLE,
 ACCI_SQLT_STR=SQLT_STR,
 ACCI_SQLT_VNU=SQLT_VNU,
 ACCI_SQLT_PDN=SQLT_PDN,
 ACCI_SQLT_LNG=SQLT_LNG,
 ACCI_SQLT_VCS=SQLT_VCS,
 ACCI_SQLT_NON=SQLT_NON,
 ACCI_SQLT_RID=SQLT_RID,
 ACCI_SQLT_DAT=SQLT_DAT,
 ACCI_SQLT_VBI=SQLT_VBI,
 ACCI_SQLT_BIN=SQLT_BIN,
 ACCI_SQLT_LBI=SQLT_LBI,
 ACCIUNSIGNED_INT = SQLT_UIN,
 ACCI_SQLT_SLS=SQLT_SLS,
 ACCI_SQLT_LVC=SQLT_LVC,
 ACCI_SQLT_LVB=SQLT_LVB,
 ACCI_SQLT_AFC=SQLT_AFC,
 ACCI_SQLT_AVC=SQLT_AVC,
 ACCI_SQLT_CUR=SQLT_CUR,
 ACCI_SQLT_RDD=SQLT_RDD,
 ACCI_SQLT_LAB=SQLT_LAB,
 ACCI_SQLT_OSL=SQLT_OSL,
 ACCI_SQLT_NTY=SQLT_NTY,
 ACCI_SQLT_REF=SQLT_REF,
 ACCI_SQLT_CLOB=SQLT_CLOB,
 ACCI_SQLT_BLOB=SQLT_BLOB,
 ACCI_SQLT_BFILEE=SQLT_BFILEE,
 ACCI_SQLT_CFILEE=SQLT_CFILEE,
 ACCI_SQLT_RSET=SQLT_RSET,
 ACCI_SQLT_NCO=SQLT_NCO,
 ACCI_SQLT_VST=SQLT_VST,
 ACCI_SQLT_ODT=SQLT_ODT,
 ACCI_SQLT_DATE=SQLT_DATE,
 ACCI_SQLT_TIME=SQLT_TIME,
 ACCI_SQLT_TIME_TZ=SQLT_TIME_TZ,
 ACCI_SQLT_TIMESTAMP=SQLT_TIMESTAMP,
 ACCI_SQLT_TIMESTAMP_TZ=SQLT_TIMESTAMP_TZ,
 ACCI_SQLT_INTERVAL_YM=SQLT_INTERVAL_YM,
 ACCI_SQLT_INTERVAL_DS=SQLT_INTERVAL_DS,
 ACCI_SQLT_TIMESTAMP_LTZ=SQLT_TIMESTAMP_LTZ,
 ACCI_SQLT_FILE=SQLT_FILE,
 ACCI_SQLT_CFILE=SQLT_CFILE,
 ACCI_SQLT_BFILE=SQLT_BFILE,
 
 ACCICHAR = 32 *1024,
 ACCIDOUBLE,
 ACCIBOOL,
 ACCIANYDATA ,
 ACCINUMBER,
 ACCIBLOB,
 ACCIBFILE,
 ACCIBYTES,
 ACCICLOB ,
 ACCIVECTOR,
 ACCIMETADATA,
 ACCIPOBJECT,
 ACCIREF ,
 ACCIREFANY,
 ACCISTRING  ,
 ACCISTREAM  ,
 ACCIDATE  ,
 ACCIINTERVALDS  ,
 ACCIINTERVALYM  ,
 ACCITIMESTAMP,
 ACCIROWID,
 ACCICURSOR

};

enum {ACCI_MAX_PREFETCH_DEPTH = UB4MAXVAL};

enum CharSetForm
{
  ACCI_SQLCS_IMPLICIT = SQLCS_IMPLICIT // use local db char set
 ,ACCI_SQLCS_NCHAR = SQLCS_NCHAR // use local db nchar set
 ,ACCI_SQLCS_EXPLICIT = SQLCS_EXPLICIT // char set explicitly specified
 ,ACCI_SQLCS_FLEXIBLE = SQLCS_FLEXIBLE // pl/sql flexible parameter
};

enum LobOpenMode
{ ACCI_LOB_READONLY = ACI_LOB_READONLY
,ACCI_LOB_READWRITE = ACI_LOB_READWRITE
};

template <class T> 
class ConstPtr
{

public:

	ConstPtr( const T* ptr = 0 );
	ConstPtr( const ConstPtr<T>& mp );
	~ConstPtr();
	const T * operator->() const;
	const T* rawPtr() const;

#ifdef MEMBER_TEMPLATE
	template<class OtherType> operator ConstPtr<OtherType>();
#endif

protected:

	void operator=( const ConstPtr<T>& mp );
	const T* rawPtr_;

};

template <class T>
class Ptr : public ConstPtr<T> {

public:

	Ptr( T* ptr = 0 );
	Ptr( const Ptr<T>& mp );
	void operator=( const Ptr<T>& mp );
	const T * operator->() const;
	T * operator->();
	T* rawPtr() ;
	const T* rawPtr() const;

#ifdef MEMBER_TEMPLATE
	template<class OtherType>
	operator Ptr<OtherType>();
#endif

};

} /* end of namespace acci */
} /* end of namespace shentong */

#endif                                              /* ACCICOMMON_ST */
