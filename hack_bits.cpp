#include <iostream>
#include <bitset>

template <unsigned P>
struct low_bits
{
    template <typename T>
    static constexpr T calc(const T x)
    {
        auto low = low_bits< (P>>1) >::calc(x);
        return low | (low >> P);
    }    
};

template <>
struct low_bits<1>
{
    template <typename T>
    static constexpr T calc(const T x)
    {
        return x | (x >> 1);
    }    
};

template <typename T>
inline constexpr T high_bit(const T x)
{
   static_assert(std::is_unsigned<T>::value);
   return x & ~low_bits< (sizeof(T) << 3) >::calc(x >> 1);
}    

template <typename T>
inline constexpr T low_bit(const T x)
{
    static_assert(std::is_unsigned<T>::value);
    return x & -x;
}   


int main()
{
    for (unsigned i = 0; i < 256; i++)
        std::cout << std::bitset<16>(i) << "\n" << std::bitset<16>(high_bit(i)) << "\n" << std::bitset<16>(low_bit(i)) << "\n\n";
    
    return 0;
}
