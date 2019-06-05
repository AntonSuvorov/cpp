#include <iostream>
#include <bitset>
#include <tuple>
#include <boost/operators.hpp>


template<typename AccessMask, unsigned Offset>
struct AccessMaskElement 
    : boost::addable<AccessMaskElement<AccessMask, Offset>
    , boost::subtractable<AccessMaskElement<AccessMask, Offset>
    , boost::equality_comparable<AccessMaskElement<AccessMask, Offset>
    , boost::less_than_comparable<AccessMaskElement<AccessMask, Offset>
    , boost::dividable<AccessMaskElement<AccessMask, Offset>
    , boost::modable<AccessMaskElement<AccessMask, Offset>
    > > > > > >
{    
    struct Access
    {
        enum 
        {
            Deny = 1 << Offset,
            Ask = 1 << (Offset + 1),
            Log  = 1 << (Offset + 2),
            Allow = 1 << (Offset + 3),
        };
    };
    
    enum 
    {
        ElementMask = Access::Deny | Access::Ask | Access::Log | Access::Allow,
    };
        

    constexpr inline operator AccessMask() const noexcept
    {
        return AccessMask(GetMask());
    }
    
    
    inline AccessMaskElement& operator=(const AccessMaskElement& right) noexcept
    {        
        reset() + right;        
        return *this;
    }
    
    inline AccessMaskElement& operator+=(const AccessMaskElement& right) noexcept
    {        
        static_cast<AccessMask&>(*this).m_mask |= right.GetMask();        
        return *this;
    }
    
    inline AccessMaskElement& operator-=(const AccessMaskElement& right) noexcept
    {
        static_cast<AccessMask&>(*this).m_mask &= ~right.GetMask();         
        return *this;
    }
    
    inline AccessMaskElement& operator /=(const AccessMaskElement& right) noexcept
    {
        static_cast<AccessMask&>(*this).m_mask = ~(GetMask() ^ right.GetMask());         
        return *this;
    }
    
    inline AccessMaskElement& operator %=(const AccessMaskElement& right) noexcept
    {
        static_cast<AccessMask&>(*this).m_mask ^= right.GetMask();         
        return *this;
    }
    
    inline bool operator==(const AccessMaskElement& right) const noexcept
    {
        return GetMask() == right.GetMask();         
    }
    
    inline bool operator<(const AccessMaskElement& right) const noexcept
    {
        return GetMask() > right.GetMask();         
    }    
    
    AccessMaskElement& reset() noexcept
    {
         static_cast<AccessMask&>(*this).m_mask &= ~ElementMask;
         return *this;
    }
    
private:
    constexpr inline auto GetMask() const noexcept
    {
        return ElementMask & static_cast<const AccessMask&>(*this).m_mask;
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
    , boost::less_than_comparable<AccessMask
    , boost::dividable<AccessMask
    , boost::modable<AccessMask
    > > > > > >
{
    using PrivilegeMask = AccessMaskElement<AccessMask, 0>;
    using WriteMask = PrivilegeMask;
    using ReadMask = AccessMaskElement<AccessMask, 4>;
    using CreateMask = AccessMaskElement<AccessMask, 8>;
    using DeleteMask = AccessMaskElement<AccessMask, 12>;
    
    uint16_t m_mask = 0;
    
    AccessMask() = default;
    
    constexpr explicit AccessMask(uint16_t mask) noexcept
        : m_mask(mask)
    {}
    
    inline AccessMask& operator+=(const AccessMask& right) noexcept
    {        
        m_mask |= right.m_mask;        
        return *this;
    }
    
    inline AccessMask& operator-=(const AccessMask& right) noexcept
    {
        m_mask &= ~right.m_mask;         
        return *this;
    }    
    
    inline AccessMask& operator /=(const AccessMask& right) noexcept
    {
        m_mask = ~(m_mask ^ right.m_mask);         
        return *this;
    }
    
    inline AccessMask& operator %=(const AccessMask& right) noexcept
    {
        m_mask ^= right.m_mask;         
        return *this;
    }
    
    constexpr inline bool operator==(const AccessMask& right) const noexcept
    {
        return m_mask == right.m_mask;         
    }
    
    inline bool operator<(const AccessMask& right) const noexcept
    {
        return static_cast<const WriteMask&>(*this) < static_cast<const WriteMask&>(right) 
        &&  static_cast<const ReadMask&>(*this) < static_cast<const ReadMask&>(right)
        &&  static_cast<const CreateMask&>(*this) < static_cast<const CreateMask&>(right)
        &&  static_cast<const DeleteMask&>(*this) < static_cast<const DeleteMask&>(right);
    } 
};



int main()
{
    AccessMask inherite, allow, ask, deny, log;
    allow.m_mask = AccessMask::PrivilegeMask::Access::Allow;
    ask.m_mask = AccessMask::PrivilegeMask::Access::Ask;
    deny.m_mask = AccessMask::PrivilegeMask::Access::Deny;
    log.m_mask = AccessMask::PrivilegeMask::Access::Log;
 
    static_assert(AccessMask() == AccessMask(), "==");
    static_assert(AccessMask(AccessMask::PrivilegeMask::Access::Allow) != AccessMask(AccessMask::PrivilegeMask::Access::Ask), "Allow != Ask");
    
    
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
