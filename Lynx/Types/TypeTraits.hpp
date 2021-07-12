#include "Lynx/Types/Integers/Integers.hpp"




namespace lnx{
    //Recursive function that finds the type index
    template<int32 tNum, int32 tIdx, bool tGet, class tLast, class tType, class... tTypes> struct __get_type_at :
    public __get_type_at<tNum, tIdx, tNum - (sizeof...(tTypes) + 1) == tIdx, tType, tTypes...>{};

    //Specialization of __get_type_at that saves the type
    template<int32 tNum, int32 tIdx, class tLast, class tType, class... tTypes> struct __get_type_at<tNum, tIdx, true, tLast, tType, tTypes...> {
        using type = tLast;
    };

    //__get_type_at helper
    template<int32 tIdx, class tType, class... tTypes> struct get_type_at_dispatch_t :
    public __get_type_at<sizeof...(tTypes) + 1/*tType*/ + 1/*void*/, tIdx, tIdx == 0, tType, tType, tTypes..., void> {};


    /**
     * @brief Provides the type of the tIdx-th type in a template parameter pack \n
     * Complexity: O(1)
     * @tparam tIdx The index of the type
     * @tparam tTypes The expanded template parameter pack
     */
    template<uint32 tIdx, class... tTypes> struct get_type_at {
        static_assert(tIdx < sizeof...(tTypes), "Index out of range");
        using type = get_type_at_dispatch_t<tIdx, tTypes...>::type;
    };
    template<uint32 tIdx, class... tTypes> using get_type_at_t = get_type_at<tIdx, tTypes...>;

}