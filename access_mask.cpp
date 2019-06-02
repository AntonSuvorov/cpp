#include <iostream>
#include <boost/operators.hpp>

#include <iostream>

template<typename AccessMask, unsigned Offset>
struct AccessMaskElement 
    : boost::addable<AccessMaskElement<AccessMask, Offset>
    , boost::subtractable<AccessMaskElement<AccessMask, Offset>
    > >
{    
    struct Part1
    {
        enum 
        {
            Deny = 1 << Offset - 1,
            Ask = 2 << Offset - 1,
            Allow = 3 << Offset - 1,
            Mask = Deny | Ask | Allow
        };
    };
    struct Part2
    {
        enum 
        {
            Log = 4 << Offset -1,
            Mask = Log
        };
    };
    
    enum 
    {
        ElementMask
    };
        
    
    AccessMaskElement& operator+=(const AccessMaskElement& right)
    {        
        static_cast<AccessMask&>(*this).m_mask |= static_cast<AccessMask&>(right).m_mask;        
        return *this;
    }
    
    AccessMaskElement& operator-=(const AccessMaskElement& right)
    {
        static_cast<AccessMask&>(*this).m_mask &= ~static_cast<AccessMask&>(right).m_mask;         
        return *this;
    }
};

struct AccessMask 
    : AccessMaskElement<AccessMask, 0>
    , AccessMaskElement<AccessMask, 1>
    , AccessMaskElement<AccessMask, 2>
    , AccessMaskElement<AccessMask, 3>
    , boost::addable<AccessMask
    , boost::subtractable<AccessMask
    > >
{
    using PrivilegeMask = AccessMaskElement<AccessMask, 0>;
    using WriteMask = AccessMaskElement<AccessMask, 0>;
    using ReadMask = AccessMaskElement<AccessMask, 1>;
    using CreateMask = AccessMaskElement<AccessMask, 2>;
    using DeleteMask = AccessMaskElement<AccessMask, 3>;
    
    uint32_t m_mask = 0;
    
    AccessMask& operator+=(const AccessMask& right)
    {        
        m_mask |= right.m_mask;        
        return *this;
    }
    
    AccessMask& operator-=(const AccessMask& right)
    {
        m_mask &= ~right.m_mask;         
        return *this;
    }    
};



int main()
{
    AccessMask inherite, allow, ask, deny, log;
//    allow.m_mask = AccessMask::
    return 0;
}
