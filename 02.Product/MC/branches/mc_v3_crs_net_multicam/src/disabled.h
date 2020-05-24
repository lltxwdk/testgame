#if !defined(DISABLED_H)
#define DISABLED_H
#pragma pack(push, 8)

class CDisabled
{
protected:
    
    CDisabled()
    {
    }
    
    virtual ~CDisabled()
    {
    }
    
private:
    
    CDisabled(const CDisabled&);
    CDisabled& operator=(const CDisabled&);
};

#pragma pack(pop)
#endif //// DISABLED_H
