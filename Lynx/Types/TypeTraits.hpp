#include "Lynx/Types/Integers/Integers.hpp"




namespace lnx{
    template<int32 tNum, int32 tIdx, bool tGet, class tLast, class tType, class... tTypes> struct get_type_at_t :
    public get_type_at_t<tNum, tIdx, tNum - (sizeof...(tTypes) + 1) == tIdx, tType, tTypes...>{};

    template<int32 tNum, int32 tIdx, class tLast, class tType, class... tTypes> struct get_type_at_t<tNum, tIdx, true, tLast, tType, tTypes...> {
        using type = tLast;
    };

    template<int32 tIdx, class tType, class... tTypes> struct get_type_at_dispatch_t :
    public get_type_at_t<sizeof...(tTypes) + 1/*tType*/ + 1/*void*/, tIdx, tIdx == 0, tType, tType, tTypes..., void> {};


    template<uint32 tIdx, class... tTypes> struct get_type_at {
        static_assert(tIdx < sizeof...(tTypes), "Index out of range");
        using type = get_type_at_dispatch_t<tIdx, tTypes...>::type;
    };
}