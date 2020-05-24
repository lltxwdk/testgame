
#if !defined(____FUNCTOR_COMMAND_H____)
#define ____FUNCTOR_COMMAND_H____
#pragma pack(push, 8)

#include "disabled.h"
#include <cassert>

/////////////////////////////////////////////////////////////////////////////
////
#ifdef LINUX
#define __stdcall
#endif

class IFunctorCommand
{
public:
    
    virtual void __stdcall Destroy() = 0;
    
    virtual void __stdcall Execute() = 0;
    
    virtual unsigned long __stdcall GetUserCode() const = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

template<class RECEIVER, class ACTION>
class CFunctorCommand_1 : public IFunctorCommand, public CDisabled
{
public:
    
    static CFunctorCommand_1* CreateInstance(
        RECEIVER&     receiver,
        ACTION        action,
        const void*   arg1     = NULL,
        unsigned long userCode = 0
        )
    {
        assert(action != NULL);
        if (action == NULL)
        {
            return (NULL);
        }
        
        CFunctorCommand_1* const command =
            new CFunctorCommand_1(receiver, action, arg1, userCode);
        
        return (command);
    }
    
protected:
    
    CFunctorCommand_1(
        RECEIVER&     receiver,
        ACTION        action,
        const void*   arg1,
        unsigned long userCode
        ) : m_receiver(receiver), m_userCode(userCode)
    {
        m_action = action;
        m_arg1   = arg1;
    }
    
    virtual ~CFunctorCommand_1()
    {
    }
    
    virtual void __stdcall Destroy()
    {
        delete this;
    }
    
    virtual void __stdcall Execute()
    {
        (m_receiver.*m_action)((void*)m_arg1);
    }
    
    virtual unsigned long __stdcall GetUserCode() const
    {
        return (m_userCode);
    }
    
private:
    
    RECEIVER&           m_receiver;
    ACTION              m_action;
    const void*         m_arg1;
    const unsigned long m_userCode;
};

/////////////////////////////////////////////////////////////////////////////
////

template<class RECEIVER, class ACTION>
class CFunctorCommand_2 : public IFunctorCommand, public CDisabled
{
public:
    
    static CFunctorCommand_2* CreateInstance(
        RECEIVER&     receiver,
        ACTION        action,
        const void*   arg1     = NULL,
        const void*   arg2     = NULL,
        unsigned long userCode = 0
        )
    {
        assert(action != NULL);
        if (action == NULL)
        {
            return (NULL);
        }
        
        CFunctorCommand_2* const command =
            new CFunctorCommand_2(receiver, action, arg1, arg2, userCode);
        
        return (command);
    }
    
protected:
    
    CFunctorCommand_2(
        RECEIVER&     receiver,
        ACTION        action,
        const void*   arg1,
        const void*   arg2,
        unsigned long userCode
        ) : m_receiver(receiver), m_userCode(userCode)
    {
        m_action = action;
        m_arg1   = arg1;
        m_arg2   = arg2;
    }
    
    virtual ~CFunctorCommand_2()
    {
    }
    
    virtual void __stdcall Destroy()
    {
        delete this;
    }
    
    virtual void __stdcall Execute()
    {
        (m_receiver.*m_action)((void*)m_arg1, (void*)m_arg2);
    }
    
    virtual unsigned long __stdcall GetUserCode() const
    {
        return (m_userCode);
    }
    
private:
    
    RECEIVER&           m_receiver;
    ACTION              m_action;
    const void*         m_arg1;
    const void*         m_arg2;
    const unsigned long m_userCode;
};

/////////////////////////////////////////////////////////////////////////////
////

template<class RECEIVER, class ACTION>
class CFunctorCommand_3 : public IFunctorCommand, public CDisabled
{
public:
    
    static CFunctorCommand_3* CreateInstance(
        RECEIVER&     receiver,
        ACTION        action,
        const void*   arg1     = NULL,
        const void*   arg2     = NULL,
        const void*   arg3     = NULL,
        unsigned long userCode = 0
        )
    {
        assert(action != NULL);
        if (action == NULL)
        {
            return (NULL);
        }
        
        CFunctorCommand_3* const command =
            new CFunctorCommand_3(receiver, action, arg1, arg2, arg3, userCode);
        
        return (command);
    }
    
protected:
    
    CFunctorCommand_3(
        RECEIVER&     receiver,
        ACTION        action,
        const void*   arg1,
        const void*   arg2,
        const void*   arg3,
        unsigned long userCode
        ) : m_receiver(receiver), m_userCode(userCode)
    {
        m_action = action;
        m_arg1   = arg1;
        m_arg2   = arg2;
        m_arg3   = arg3;
    }
    
    virtual ~CFunctorCommand_3()
    {
    }
    
    virtual void __stdcall Destroy()
    {
        delete this;
    }
    
    virtual void __stdcall Execute()
    {
        (m_receiver.*m_action)((void*)m_arg1, (void*)m_arg2, (void*)m_arg3);
    }
    
    virtual unsigned long __stdcall GetUserCode() const
    {
        return (m_userCode);
    }
    
private:
    
    RECEIVER&           m_receiver;
    ACTION              m_action;
    const void*         m_arg1;
    const void*         m_arg2;
    const void*         m_arg3;
    const unsigned long m_userCode;
};

/////////////////////////////////////////////////////////////////////////////
////

template<class ACTION>
class CFunctorCommand_c1 : public IFunctorCommand, public CDisabled
{
public:
    
    static CFunctorCommand_c1* CreateInstance(
        ACTION        action,
        const void*   arg1     = NULL,
        unsigned long userCode = 0
        )
    {
        assert(action != NULL);
        if (action == NULL)
        {
            return (NULL);
        }
        
        CFunctorCommand_c1* const command =
            new CFunctorCommand_c1(action, arg1, userCode);
        
        return (command);
    }
    
protected:
    
    CFunctorCommand_c1(
        ACTION        action,
        const void*   arg1,
        unsigned long userCode
        ) : m_userCode(userCode)
    {
        m_action = action;
        m_arg1   = arg1;
    }
    
    virtual ~CFunctorCommand_c1()
    {
    }
    
    virtual void __stdcall Destroy()
    {
        delete this;
    }
    
    virtual void __stdcall Execute()
    {
        (*m_action)((void*)m_arg1);
    }
    
    virtual unsigned long __stdcall GetUserCode() const
    {
        return (m_userCode);
    }
    
private:
    
    ACTION              m_action;
    const void*         m_arg1;
    const unsigned long m_userCode;
};

/////////////////////////////////////////////////////////////////////////////
////

template<class ACTION>
class CFunctorCommand_c2 : public IFunctorCommand, public CDisabled
{
public:
    
    static CFunctorCommand_c2* CreateInstance(
        ACTION        action,
        const void*   arg1     = NULL,
        const void*   arg2     = NULL,
        unsigned long userCode = 0
        )
    {
        assert(action != NULL);
        if (action == NULL)
        {
            return (NULL);
        }
        
        CFunctorCommand_c2* const command =
            new CFunctorCommand_c2(action, arg1, arg2, userCode);
        
        return (command);
    }
    
protected:
    
    CFunctorCommand_c2(
        ACTION        action,
        const void*   arg1,
        const void*   arg2,
        unsigned long userCode
        ) : m_userCode(userCode)
    {
        m_action = action;
        m_arg1   = arg1;
        m_arg2   = arg2;
    }
    
    virtual ~CFunctorCommand_c2()
    {
    }
    
    virtual void __stdcall Destroy()
    {
        delete this;
    }
    
    virtual void __stdcall Execute()
    {
        (*m_action)((void*)m_arg1, (void*)m_arg2);
    }
    
    virtual unsigned long __stdcall GetUserCode() const
    {
        return (m_userCode);
    }
    
private:
    
    ACTION              m_action;
    const void*         m_arg1;
    const void*         m_arg2;
    const unsigned long m_userCode;
};

/////////////////////////////////////////////////////////////////////////////
////

template<class ACTION>
class CFunctorCommand_c3 : public IFunctorCommand, public CDisabled
{
public:
    
    static CFunctorCommand_c3* CreateInstance(
        ACTION        action,
        const void*   arg1     = NULL,
        const void*   arg2     = NULL,
        const void*   arg3     = NULL,
        unsigned long userCode = 0
        )
    {
        assert(action != NULL);
        if (action == NULL)
        {
            return (NULL);
        }
        
        CFunctorCommand_c3* const command =
            new CFunctorCommand_c3(action, arg1, arg2, arg3, userCode);
        
        return (command);
    }
    
protected:
    
    CFunctorCommand_c3(
        ACTION        action,
        const void*   arg1,
        const void*   arg2,
        const void*   arg3,
        unsigned long userCode
        ) : m_userCode(userCode)
    {
        m_action = action;
        m_arg1   = arg1;
        m_arg2   = arg2;
        m_arg3   = arg3;
    }
    
    virtual ~CFunctorCommand_c3()
    {
    }
    
    virtual void __stdcall Destroy()
    {
        delete this;
    }
    
    virtual void __stdcall Execute()
    {
        (*m_action)((void*)m_arg1, (void*)m_arg2, (void*)m_arg3);
    }
    
    virtual unsigned long __stdcall GetUserCode() const
    {
        return (m_userCode);
    }
    
private:
    
    ACTION              m_action;
    const void*         m_arg1;
    const void*         m_arg2;
    const void*         m_arg3;
    const unsigned long m_userCode;
};

/////////////////////////////////////////////////////////////////////////////
////


template<class RECEIVER, class ACTION,class ArgType>
class CFunctorCommand_Arg : public IFunctorCommand, public CDisabled
{
public:

	static CFunctorCommand_Arg* CreateInstance(
		RECEIVER*     receiver,
		ACTION        action,
		ArgType*	  arg)
	{
		assert(action != NULL);
		if (action == NULL)
		{
			return (NULL);
		}

		CFunctorCommand_Arg* const command = new CFunctorCommand_Arg(receiver, action, arg);
		return (command);
	}

protected:

	CFunctorCommand_Arg(
		RECEIVER*     receiver,
		ACTION        action,
		ArgType*   arg) : m_receiver(receiver), m_action(action), m_arg(arg)
	{
	}

	virtual ~CFunctorCommand_Arg()
	{
	}

	virtual void __stdcall Destroy()
	{
		delete m_arg;
		delete this;
	}

	virtual void __stdcall Execute()
	{
		(m_receiver->*m_action)(m_arg);
	}
	virtual unsigned long __stdcall GetUserCode() const{ return 0UL; }
private:

	RECEIVER*           m_receiver;
	ACTION              m_action;
	ArgType*			m_arg;
};


#pragma pack(pop)
#endif //// FUNCTOR_COMMAND_H
