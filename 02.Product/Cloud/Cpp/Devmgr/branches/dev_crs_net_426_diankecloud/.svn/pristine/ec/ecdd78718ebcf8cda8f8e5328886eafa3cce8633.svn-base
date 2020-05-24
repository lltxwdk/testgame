#ifndef ACCICONTROL_ST
#define ACCICONTROL_ST

#ifndef ACCICOMMON_ST
#include <acciCommon.h>
#endif

#ifndef STEXCEPTION
#define STEXCEPTION
#include <exception>
#endif

namespace shentong {
namespace acci {
/*---------------------------------------------------------------------------
                     PUBLIC TYPES AND CONSTANTS
  ---------------------------------------------------------------------------*/

//return codes for user callbacks
enum
{
  ACCI_SUCCESS = ACI_SUCCESS,
  FO_RETRY = ACI_FO_RETRY
};


class Connection
{
   public :

      // specifies the type of proxy to be created,
      // used for future enhancements
      enum ProxyType
      {
		  PROXY_DEFAULT
      };

      enum FailOverType
      {
        FO_NONE = ACI_FO_NONE,
        FO_SESSION = ACI_FO_SESSION,
        FO_SELECT = ACI_FO_SELECT
      }; 

      enum FailOverEventType
      {
        FO_BEGIN = ACI_FO_BEGIN,
        FO_END = ACI_FO_END,
        FO_ABORT = ACI_FO_ABORT,
        FO_REAUTH = ACI_FO_REAUTH,
        FO_ERROR = ACI_FO_ERROR
      };

      enum Purity
      {
        DEFAULT = ACI_ATTR_PURITY_DEFAULT,
        NEW = ACI_ATTR_PURITY_NEW,
        SELF = ACI_ATTR_PURITY_SELF
      };

      virtual ~Connection() { }
      virtual Statement* createStatement(
        const ACCI_STD_NAMESPACE::string  &sql = "") 
         =0; 
      virtual void terminateStatement(Statement *statement) =0;
      virtual void commit()  =0; 
      virtual void rollback()  =0;
      virtual void changePassword(const ACCI_STD_NAMESPACE::string &user,
        const ACCI_STD_NAMESPACE::string &oldPassword,
        const ACCI_STD_NAMESPACE::string &newPassword) 
         =0;
	  virtual void setStmtCacheSize(unsigned int cacheSize) = 0;
};

class   Environment     
{
 public:
                    // class constants

  enum Mode
  {
    DEFAULT = ACI_DEFAULT,
    OBJECT = ACI_OBJECT,
    SHARED = ACI_SHARED,
    NO_USERCALLBACKS = ACI_NO_UCB,
    THREADED_MUTEXED = ACI_THREADED,
    THREADED_UNMUTEXED = ACI_THREADED | ACI_NO_MUTEX,
    EVENTS = ACI_EVENTS,
    USE_LDAP = ACI_USE_LDAP
  };

  virtual ~Environment(){}

                    // public methods

  static Environment * createEnvironment(
                              Mode mode = DEFAULT,
                              void *ctxp = 0,
                              void *(*malocfp)(void *ctxp, size_t size) = 0,
                              void *(*ralocfp)(void *ctxp, void *memptr,
                                               size_t newsize) = 0,
                              void (*mfreefp)(void *ctxp, void *memptr) = 0);

  static void terminateEnvironment(Environment *env);

  static void getClientVersion( int &majorVersion, int &minorVersion, 
	  int &updateNum, int &patchNum, 
	  int &portUpdateNum );

  virtual Connection * createConnection(
    const ACCI_STD_NAMESPACE::string &userName,
    const ACCI_STD_NAMESPACE::string &password, 
    const ACCI_STD_NAMESPACE::string &connectString = "") = 0;

  virtual void terminateConnection(Connection *connection) = 0;


  //new interfaces

  private:

};

   
class SQLException : public ACCI_STD_NAMESPACE::exception
{
 public:

  virtual int getErrorCode() const;
  
  virtual ACCI_STD_NAMESPACE::string getMessage() const;

  const char *what() const throw();

  virtual void setErrorCtx(void *ctx);
  
  SQLException();

  SQLException(const SQLException &e);

  void operator=(const SQLException &other);

  virtual ~SQLException() throw();

//   virtual int getXAErrorCode(const ACCI_STD_NAMESPACE::string& dbname) const;
//   
//   virtual UString getUStringMessage() const;
// 
//   virtual ACCI_STD_NAMESPACE::string getNLSMessage(Environment *env) const;
//   
//   virtual UString getNLSUStringMessage(Environment *env) const;

 protected:

  Ptr<SQLExceptionImpl> ptr_;
  SQLException(SQLExceptionImpl *ptr);

  friend SQLException SQLExceptionCreate(int errorCode);
  friend SQLException SQLExceptionCreate(void *handle,
  int handleType);
  friend class BatchSQLException;
};

class Statement 
{
 public:
                                // class constants

	virtual ~Statement() {}

	enum Status                                   
	{
	UNPREPARED,
	PREPARED,
	RESULT_SET_AVAILABLE,
	UPDATE_COUNT_AVAILABLE,
	NEEDS_STREAM_DATA,
	STREAM_DATA_AVAILABLE
	};
  
                                // common methods
  
	virtual void setSQL(const ACCI_STD_NAMESPACE::string &sql) = 0; 

	virtual ACCI_STD_NAMESPACE::string getSQL() const = 0;

	virtual Status execute(const ACCI_STD_NAMESPACE::string &sql = "") = 0;
   
	virtual ResultSet * getResultSet() = 0;
   
	virtual unsigned int getUpdateCount() const = 0; 
  
	virtual ResultSet * executeQuery(
    const ACCI_STD_NAMESPACE::string &sql = "") = 0;
  
	virtual unsigned int executeUpdate(
     const ACCI_STD_NAMESPACE::string &sql = "") = 0; 
   
	virtual Status status() const = 0;  

	virtual void closeResultSet(ResultSet *resultSet) = 0;
   
	virtual void setPrefetchRowCount(unsigned int rowCount) = 0; 
   
	virtual void setPrefetchMemorySize(unsigned int bytes) = 0;
   
	virtual void setAutoCommit(bool autoCommit) = 0;
   
	virtual bool getAutoCommit() const = 0;
   
	virtual ACIStmt * getOCIStatement() const = 0;


                    // methods for prepared statements with IN
                    // parameters  

//   virtual void setMaxParamSize(unsigned int paramIndex,unsigned int maxSize)=0;
//   
//   virtual unsigned int getMaxParamSize(unsigned int paramIndex) const = 0;
// 
	virtual void setNull(unsigned int paramIndex, Type type) = 0;
  
	virtual void setInt(unsigned int paramIndex, int x) = 0; 
   
//   virtual void setUInt(unsigned int paramIndex, unsigned int x) = 0; 
   
	virtual void setFloat(unsigned int paramIndex, float x) = 0; 
 
	virtual void setDouble(unsigned int paramIndex, double x) = 0; 
 
	virtual void setNumber(unsigned int paramIndex, const Number &x) = 0; 
   
	virtual void setString(unsigned int paramIndex, 
     const ACCI_STD_NAMESPACE::string &x) = 0; 
 
	virtual void setBytes(unsigned int paramIndex, const Bytes &x) = 0; 
 
	virtual void setDate(unsigned int paramIndex, const Date &x) = 0; 
   
	virtual void setTimestamp(unsigned int paramIndex, const Timestamp &x) = 0; 
 
    virtual void setBlob(unsigned int paramIndex, const Blob &x) = 0;
// 
//   virtual void setClob(unsigned int paramIndex, const Clob &x) = 0;
// 
//   virtual void setBfile(unsigned int paramIndex, const Bfile &x) = 0;
// 
	virtual void setIntervalYM(unsigned int paramIndex, const IntervalYM &x) = 0;
  
	virtual void setIntervalDS(unsigned int paramIndex, const IntervalDS &x) = 0;
//  
//   virtual void setRowid(unsigned int paramIndex, const Bytes &x) = 0; 
//   
//   virtual void setRef(unsigned int paramIndex, const RefAny &x) = 0; 
// 
//   virtual void setObject(unsigned int paramIndex, PObject * x) = 0; 
//   
	virtual void setDataBuffer(unsigned int paramIndex, void *buffer, 
                              Type type,
                              sb4 size, ub2 *length, sb2 *ind = NULL,
                              ub2 *rc = NULL) = 0; 
 
//   virtual void setDataBufferArray(unsigned int paramIndex, void *buffer, 
//                                   Type type,
//                                   ub4 arraySize, ub4 *arrayLength,
//                                   sb4 elementSize,
//                                   ub2 *elementLength, sb2 *ind = NULL,
//                                   ub2 *rc = NULL) = 0;
// 
//   virtual void setCharSet(unsigned int paramIndex, 
//   const ACCI_STD_NAMESPACE::string & charSet) = 0; 
//   
//   virtual ACCI_STD_NAMESPACE::string getCharSet(unsigned int paramIndex) 
//   const = 0; 
//   
//   virtual void setDatabaseNCHARParam(
//     unsigned int paramIndex, bool isNCHAR) = 0;
//                                          
//   virtual bool getDatabaseNCHARParam(unsigned int paramIndex) const = 0;       
// 
//   virtual void closeStream(Stream *stream) =0;
// 
//   virtual Stream * getStream(unsigned int paramIndex) = 0; 
//   
//   virtual unsigned int getCurrentStreamParam() const = 0; 
//   
//   virtual unsigned int getCurrentStreamIteration() const = 0;
// 
//   virtual void setBinaryStreamMode(unsigned int colIndex, 
//     unsigned int size) =0;
// 
//   virtual void setCharacterStreamMode(unsigned int colIndex, 
//     unsigned int size) =0;
//   
//   virtual void setMaxIterations(unsigned int maxIterations) = 0;
//   
//   virtual unsigned int getMaxIterations() const = 0; 
// 
//   virtual void addIteration() = 0; 
//   
//   virtual unsigned int getCurrentIteration() const = 0; 
// 
	virtual Status executeArrayUpdate(unsigned int arrayLength) = 0;
//   
// 
//                     // methods for Callable Statements
// 
//   virtual void registerOutParam(unsigned int paramIndex, Type type, 
//   unsigned int maxSize=0, const ACCI_STD_NAMESPACE::string &sqltype="") = 0;
//   
	virtual bool isNull(unsigned int paramIndex) const = 0;
 
	virtual bool isTruncated(unsigned int paramIndex) const =0;
 
   
	virtual void setErrorOnNull(unsigned int paramIndex, 
     bool causeException) = 0;
                                
	virtual void setErrorOnTruncate(unsigned int paramIndex,
   bool causeException)  = 0;
 
//   virtual int preTruncationLength(unsigned int paramIndex) const
//    =0;
 
 
	virtual int getInt(unsigned int paramIndex)  = 0; 
   
	virtual unsigned int getUInt(unsigned int paramIndex)  = 0; 
   
	virtual float getFloat(unsigned int paramIndex)  = 0; 
 
	virtual double getDouble(unsigned int paramIndex)  = 0; 
   
	virtual Number getNumber(unsigned int paramIndex)  = 0; 
   
	virtual ACCI_STD_NAMESPACE::string getString(unsigned int paramIndex)  = 0; 
   
	virtual Bytes getBytes(unsigned int paramIndex)  = 0; 
 
	virtual Date getDate(unsigned int paramIndex)  = 0; 
   
	virtual Timestamp getTimestamp(unsigned int paramIndex)  = 0; 
 
//   virtual Bytes getRowid(unsigned int paramIndex)  = 0; 
//   
//   virtual PObject * getObject(unsigned int paramIndex)  = 0; 
   
    virtual Blob getBlob(unsigned int paramIndex)  = 0; 
//   
//   virtual Clob getClob(unsigned int paramIndex)  = 0; 
//   
//   virtual Bfile getBfile(unsigned int paramIndex)  = 0; 
// 
	virtual IntervalYM getIntervalYM(unsigned int paramIndex)  = 0; 
  
	virtual IntervalDS getIntervalDS(unsigned int paramIndex)  = 0; 
// 
//   virtual RefAny getRef(unsigned int paramIndex)  = 0;  
// 
	virtual ResultSet * getCursor(unsigned int paramIndex)   = 0;
 
	virtual Connection* getConnection() const =0; 

  //new interfaces

	virtual void setBFloat(unsigned int paramIndex, const BFloat &fval) = 0;

	virtual void setBDouble(unsigned int paramIndex, const BDouble &dval) = 0;

	virtual BFloat getBFloat(unsigned int paramIndex)  = 0;

	virtual BDouble getBDouble(unsigned int paramIndex)  = 0;

	virtual void setBatchErrorMode( bool batchErrorMode ) =0;

	virtual bool getBatchErrorMode( ) const =0;

};



class ResultSet 
{
 public:
                                // class constants
  
  enum Status                                   
  {
    END_OF_FETCH = 0,
    DATA_AVAILABLE,
    STREAM_DATA_AVAILABLE
  };
  virtual ~ResultSet(){}
 
                                // public methods

  virtual Status next(unsigned int numRows = 1) = 0;

   virtual Status status() const = 0;            
 
   virtual unsigned int getNumArrayRows()  const = 0;
 
   virtual void cancel() = 0;
    
   virtual void setMaxColumnSize(unsigned int colIndex, unsigned int max) = 0;
 
   virtual unsigned int getMaxColumnSize(unsigned int colIndex) const = 0;
 
   virtual bool isNull(unsigned int colIndex) const = 0;
 
   virtual bool isTruncated(unsigned int paramIndex) const
 	  =0;

   virtual void setErrorOnNull(unsigned int colIndex, bool causeException) = 0;
   virtual void setErrorOnTruncate(unsigned int paramIndex,
 	  bool causeException)   =0;

//   virtual int preTruncationLength(unsigned int paramIndex) const
// 	  =0;
 
   virtual int getInt(unsigned int colIndex)   = 0; 
 
   virtual unsigned int getUInt(unsigned int colIndex)   = 0; 
 
   virtual float getFloat(unsigned int colIndex)  = 0; 
 
   virtual double getDouble(unsigned int colIndex)  = 0; 
 
   virtual Number getNumber(unsigned int colIndex)  = 0; 
 
   virtual ACCI_STD_NAMESPACE::string getString(unsigned int colIndex)  = 0; 
 
   virtual Bytes getBytes(unsigned int colIndex)  = 0; 
 
   virtual Date getDate(unsigned int colIndex)  = 0; 
 
   virtual Timestamp getTimestamp(unsigned int colIndex)  = 0; 
 
   virtual Bytes getRowid(unsigned int colIndex)  = 0; 
// 
//   virtual PObject * getObject(unsigned int colIndex)  = 0; 
// 
   virtual Blob getBlob(unsigned int colIndex)  = 0; 
 
   virtual Clob getClob(unsigned int colIndex)  =0; 
// 
//   virtual Bfile getBfile(unsigned int colIndex)  = 0; 
// 
   virtual  IntervalYM getIntervalYM(unsigned int colIndex)  =0;
 
   virtual  IntervalDS getIntervalDS(unsigned int colIndex)  =0;
 
//   virtual RefAny getRef(unsigned int colIndex)  = 0; 
 
   virtual Bytes getRowPosition() const = 0; 
 
   virtual ResultSet * getCursor(unsigned int colIndex)  = 0; 
 
   virtual void setDataBuffer(unsigned int colIndex, void *buffer, Type type,
 	  sb4 size = 0, ub2 *length = NULL,
 	  sb2 *ind = NULL, ub2 *rc = NULL) = 0;
 
   virtual void setCharSet(unsigned int colIndex, 
 	  const ACCI_STD_NAMESPACE::string & charSet) = 0; 
 
   virtual ACCI_STD_NAMESPACE::string getCharSet(unsigned int colIndex) 
 	  const = 0; 
 
//   virtual void setBinaryStreamMode(unsigned int colIndex, unsigned int size)
// 	  = 0;
// 
//   virtual void setCharacterStreamMode(unsigned int colIndex, unsigned int size)
// 	  = 0;
// 
//   virtual void setDatabaseNCHARParam(unsigned int paramIndex, 
// 	  bool isNCHAR) = 0;
// 
//   virtual bool getDatabaseNCHARParam(unsigned int paramIndex) const = 0;       
// 
//   virtual Stream * getStream(unsigned int colIndex)  = 0; 
// 
//   virtual void closeStream(Stream *stream) =0;
// 
//   virtual unsigned int getCurrentStreamColumn() const= 0; 
// 
//   virtual unsigned int getCurrentStreamRow() const= 0;       
// 
//   virtual OCCI_STD_NAMESPACE::vector<MetaData> getColumnListMetaData() const 
// 	  = 0;
 
   virtual Statement* getStatement() const=0;
 
//   //new interfaces

   virtual BFloat getBFloat(unsigned int colIndex)  = 0;
 
   virtual BDouble getBDouble(unsigned int colIndex)  = 0;
// 
//   virtual UString getUString(unsigned int colIndex)  = 0; 
// 
//   virtual void setCharSetUString(unsigned int colIndex, 
// 	  const UString & charSet) = 0; 
// 
//   virtual UString getCharSetUString(unsigned int colIndex) 
// 	  const = 0; 
// 
//   virtual void setPrefetchRowCount(unsigned int rowCount) = 0; 
// 
//   virtual void setPrefetchMemorySize(unsigned int bytes) = 0;

};

class Stream
{
public : 

	enum Status {READY_FOR_READ, READY_FOR_WRITE, INACTIVE};

	virtual ~Stream(){}
	virtual int readBuffer(char *buffer, unsigned int size)
		=0;
	virtual int readLastBuffer(char *buffer, unsigned int size)
		=0;
	virtual void writeBuffer(char *buffer, unsigned int size)
		=0;
	virtual void writeLastBuffer(char *buffer, unsigned int size)
		=0;
	virtual Status status() const  =0;

};

} /* end of namespace acci */
} /* end of namespace shentong */
#endif                                              /* ACCICONTROL_ST */

