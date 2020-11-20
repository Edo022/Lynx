#define LUX_H_HDCTARRAY
#include "LuxEngine/Types/Integers/Integers.hpp"








namespace lux{
    namespace __pvt{
        //Get_t iteration action
        enum __action : int32{
            CONT = -1,
            DESC = 0,
            GETV = 1
        };




        //common virtual functions, Seq declaration, unspecialized get_t definition
        #define genGetVFunc inline virtual seq<index, type, types...>* getArr() = 0;
        template<uint32 index, class type, class ...types> struct seq;
        template <__action act, uint32 index, class type, class... types> struct get_t{ };


        //Iterate through elements (get_t specializations)
        template <uint32 index, class type, class... types> struct get_t<CONT, index, type, types...>{
            template <uint32 getIndex> inline auto &getFunc() { return getArr()->lux::__pvt::get_t<(__action)(getIndex == index), index, type, types...>::template getFunc<getIndex>(); }
            genGetVFunc;
        };
        template <uint32 index, class type, class... types> struct get_t<DESC, index, type, types...>{
            template <uint32 getIndex> inline auto &getFunc() { return getArr()->lux::__pvt::seq<index - 1, types...>::template get_t<CONT, index - 1, types...>::template getFunc<getIndex>(); }
            genGetVFunc;
        };
        template <uint32 index, class type, class... types> struct get_t<GETV, index, type, types...>{
            template <uint32 getIndex> inline type &getFunc() { return getArr()->val; }
            genGetVFunc;
        };
        #undef genGetVFunc




        //seq iterator
        template<uint32 index, class type, class ...types> struct seq :
        public get_t<CONT, index, type, types...>,
        public get_t<DESC, index, type, types...>,
        public get_t<GETV, index, type, types...>,
        public seq<index - 1, types...>{
            type val;
            virtual uint64 getOriginalSize() = 0;
            inline virtual seq<index, type, types...>* getArr() final override { return this; }
            inline void unwrCtor(const type& _val, const types&... vals){ val = _val; this->lux::__pvt::seq<index - 1, types...>::unwrCtor(vals...); }
            inline void* getPtr(const uint32 _index){ return (getOriginalSize() - 1 - index == _index) ? (void*)&val : this->lux::__pvt::seq<index - 1, types...>::getPtr(_index); }
        };


        //Stop at index 0 (seq specialization)
        template<class type> struct seq<0, type> :
        public get_t<CONT, 0, type>,
        public get_t<DESC, 0, type>,
        public get_t<GETV, 0, type>{
            type val;
            void unwrCtor(const type& _val){ val = _val; }
            inline void* getPtr(const uint32 _index){ return (void*)&val; }
        };
    }




    #define seqIndex (sizeof...(types) - 1)
    //"Heterogeneous Data Compile Time Array" (lux::__pvt::seq interface)
    //An array that can contain elements of different types
    //Size and types must be known at compile time
    //e.g.  lux::HdCtArray<int32, float32, bool> arr = { 1, 5.10f, true };
    template<class ...types> struct HdCtArray : __pvt::seq<seqIndex, types...>{
        HdCtArray(){}
        HdCtArray(const types&... vals){
            this->lux::__pvt::seq<seqIndex, types...>::unwrCtor(vals...);
        }
        virtual uint64 getOriginalSize(){ return count(); }

        //Returns a reference to an element
        //This can only be used with compile time known indices
        //Use operator[] to retrieve values in runtime. e.g.
        //arr.get<2>() = false;
        //bool isRuntime = arr.get<2>();
        template<uint32 getIndex> inline auto& get(){
            return this->lux::__pvt::seq<seqIndex, types...>::template get_t<lux::__pvt::CONT, seqIndex, types...>::template getFunc<seqIndex - getIndex>();
        }

        //Returns the address of an element as a void pointer
        inline void* operator[](const uint32 index) { return this->lux::__pvt::seq<seqIndex, types...>::getPtr(index); }
        inline uint32 count() const { return sizeof...(types); }
    };
    #undef seqIndex
}
