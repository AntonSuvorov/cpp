#include <iostream>
#include <bitset>
#include <boost/operators.hpp>


template<typename AccessMask, unsigned Offset>
struct AccessMaskElement 
    : boost::addable<AccessMaskElement<AccessMask, Offset>
    , boost::subtractable<AccessMaskElement<AccessMask, Offset>
    , boost::equality_comparable<AccessMaskElement<AccessMask, Offset>
    > > >
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
        ElementMask = Access::Mask | Addition::Mask,
    };
        
    operator AccessMask() const noexcept
    {
        return AccessMask(ElementMask & static_cast<const AccessMask&>(*this).m_mask);
    }
        
    AccessMaskElement& operator=(const AccessMaskElement& right) noexcept
    {        
        static_cast<AccessMask&>(*this).m_mask = ElementMask & static_cast<const AccessMask&>(right).m_mask;        
        return *this;
    }
    
    AccessMaskElement& operator+=(const AccessMaskElement& right) noexcept
    {        
        static_cast<AccessMask&>(*this).m_mask |= ElementMask & static_cast<const AccessMask&>(right).m_mask;        
        return *this;
    }
    
    AccessMaskElement& operator-=(const AccessMaskElement& right) noexcept
    {
        static_cast<AccessMask&>(*this).m_mask &= ~(ElementMask & static_cast<const AccessMask&>(right).m_mask);         
        return *this;
    }
    
    bool operator==(const AccessMaskElement& right) noexcept
    {
        return (ElementMask & static_cast<AccessMask&>(*this).m_mask) == (ElementMask & static_cast<const AccessMask&>(right).m_mask);         
    }
};

struct AccessMask 
    : AccessMaskElement<AccessMask, 0>
    , AccessMaskElement<AccessMask, 4>
    , AccessMaskElement<AccessMask, 8>
    , AccessMaskElement<AccessMask, 12>
    , boost::addable<AccessMask
    , boost::subtractable<AccessMask
    , boost::equality_comparable<AccessMask
    > > >
{
    using PrivilegeMask = AccessMaskElement<AccessMask, 0>;
    using WriteMask = PrivilegeMask;
    using ReadMask = AccessMaskElement<AccessMask, 4>;
    using CreateMask = AccessMaskElement<AccessMask, 8>;
    using DeleteMask = AccessMaskElement<AccessMask, 12>;
    
    uint16_t m_mask = 0;
    
    AccessMask() = default;
    
    explicit AccessMask(uint16_t mask) noexcept
        : m_mask(mask)
    {}
    
    AccessMask& operator+=(const AccessMask& right) noexcept
    {        
        m_mask |= right.m_mask;        
        return *this;
    }
    
    AccessMask& operator-=(const AccessMask& right) noexcept
    {
        m_mask &= ~right.m_mask;         
        return *this;
    }    
    
    bool operator==(const AccessMask& right) noexcept
    {
        return m_mask == right.m_mask;         
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
    std::cout << std::bitset<16>(tmp.m_mask) << "\n";
    
    tmp -= allow;
    std::cout << std::bitset<16>(tmp.m_mask) << "\n";
    
    static_cast<AccessMask::PrivilegeMask&>(tmp) += static_cast<const AccessMask::PrivilegeMask&>(ask);
    std::cout << std::bitset<16>(tmp.m_mask) << "\n";
    
    static_cast<AccessMask::PrivilegeMask&>(tmp) -= ask;
    std::cout << std::bitset<16>(tmp.m_mask) << "\n";
    
    tmp += static_cast<const AccessMask::PrivilegeMask&>(ask);
    std::cout << std::bitset<16>(tmp.m_mask) << "\n";
    
    allow == ask;
    return 0;
}
