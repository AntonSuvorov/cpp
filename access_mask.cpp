#include <iostream>
#include <bitset>
#include <boost/operators.hpp>


template<typename AccessMask, unsigned Offset>
struct AccessMaskElement 
    : boost::addable<AccessMaskElement<AccessMask, Offset>
    , boost::subtractable<AccessMaskElement<AccessMask, Offset>
    > >
{    
    struct Access
    {
        enum 
        {
            Deny = 1 << Offset,
            Ask = 1 << (Offset + 1),
            Allow = 1 << (Offset + 2),
            Mask = Deny | Ask | Allow
        };
    };
    struct Addition
    {
        enum 
        {
            Log = 1 << (Offset + 3),
            Mask = Log
        };
    };
    
    enum 
    {
        ElementMask
    };
        
    
    AccessMaskElement& operator+=(const AccessMaskElement& right)
    {        
        static_cast<AccessMask&>(*this).m_mask |= ElementMask & static_cast<AccessMask&>(right).m_mask;        
        return *this;
    }
    
    AccessMaskElement& operator-=(const AccessMaskElement& right)
    {
        static_cast<AccessMask&>(*this).m_mask &= ElementMask & ~static_cast<AccessMask&>(right).m_mask;         
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
    using WriteMask = PrivilegeMask;
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
    allow.m_mask = AccessMask::PrivilegeMask::Access::Allow;
    ask.m_mask = AccessMask::PrivilegeMask::Access::Ask;
    deny.m_mask = AccessMask::PrivilegeMask::Access::Deny;
    log.m_mask = AccessMask::PrivilegeMask::Addition::Log;
    
    AccessMask tmp(inherite + allow);
    std::cout << std::bitset<32>(tmp.m_mask) << "\n";
    
    tmp -= allow;
    std::cout << std::bitset<32>(tmp.m_mask) << "\n";
    return 0;
}
