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
            Allow = 1 << Offset,
            Log = 1 << (Offset + 1),
            Ask  = 1 << (Offset + 2),
            Deny = 1 << (Offset + 3),
        };
    };
    
    enum 
    {
        ElementMask = Access::Deny | Access::Ask | Access::Log | Access::Allow,
    };
        

    inline operator AccessMask() const noexcept
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
        return GetMask() < right.GetMask();         
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
        m_mask &= (m_mask ^ right.m_mask);         
        return *this;
    }
    
    inline AccessMask& operator %=(const AccessMask& right) noexcept
    {
        m_mask ^= right.m_mask;         
        return *this;
    }
    
    inline bool operator==(const AccessMask& right) const noexcept
    {
        return m_mask == right.m_mask;         
    }
    
    inline bool operator<(const AccessMask& right) const noexcept
    {
        return m_mask != right.m_mask
        &&  static_cast<const WriteMask&>(*this) <= static_cast<const WriteMask&>(right) 
        &&  static_cast<const ReadMask&>(*this) <= static_cast<const ReadMask&>(right)
        &&  static_cast<const CreateMask&>(*this) <= static_cast<const CreateMask&>(right)
        &&  static_cast<const DeleteMask&>(*this) <= static_cast<const DeleteMask&>(right);
    } 
};

#define TEST(check) std::cout << (check) << " - " #check << "\n"


int main()
{
    AccessMask inherite, allow, ask, deny, log;
    allow.m_mask = AccessMask::PrivilegeMask::Access::Allow;
    ask.m_mask = AccessMask::PrivilegeMask::Access::Ask;
    deny.m_mask = AccessMask::PrivilegeMask::Access::Deny;
    log.m_mask = AccessMask::PrivilegeMask::Access::Log;
 
    TEST(inherite == inherite);
    TEST(allow != ask);
    TEST(ask == ask);
    TEST(allow + ask == AccessMask(AccessMask::PrivilegeMask::Access::Allow | AccessMask::PrivilegeMask::Access::Ask));
    TEST(allow + log + ask + deny == AccessMask(AccessMask::PrivilegeMask::ElementMask));
    TEST(AccessMask(AccessMask::PrivilegeMask::ElementMask) - ask == AccessMask(AccessMask::PrivilegeMask::Access::Allow | AccessMask::PrivilegeMask::Access::Log | AccessMask::PrivilegeMask::Access::Deny));
    TEST(inherite < allow);
    TEST(allow < log);
    TEST(log < ask);
    TEST(ask < deny);
    TEST(ask < ask + log);
    TEST(allow == allow / allow);
    TEST(inherite == ask / allow);
    TEST(inherite == allow / ask);
    TEST(allow == (allow + log) / allow);
    TEST(inherite == allow % allow);
    TEST(deny + log == ask % allow);
    TEST(deny + log == allow % ask);

    std::cout << std::bitset<16>(((ask + allow) / allow).m_mask) << "\n";

    return 0;
}
