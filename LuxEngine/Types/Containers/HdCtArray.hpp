#define LUX_H_HDCTARRAY
#include "LuxEngine/Types/Integers/Integers.hpp"








namespace lux{
    struct dummy{};

    namespace __pvt{
        //Get_t iteration action
        enum __action : uint32{
            CONT = (uint32)-1,
            DESC = 0,
            GETV = 1
        };



        //TODO RECURSIVELY ADD PARAMETERS AND CALL FUNCTION AT THE LAST ONE

        //TODO ci sono diversi tipi di superfici
        //TODO questi tipi vengono calcolati automaticamente e salvati nei dati dell'oggetto
        //TODO così in runtime non serve calcolarli. devono essere ricalcolati in background se l'oggetto viene modificato
        //TODO ogni superficie ha dei punti caratteristici da cui dipendono le sue proprietà fisiche (non grafica)
        //TODO i punti vengono calcolati dopo aver trovato il tipo e sono probabilment indici a dei punti normali
        //TODO nn
        //TODO
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
            inline void ctGet(const type& _val, const types&... vals){ val = _val; this->lux::__pvt::seq<index - 1, types...>::ctGet(vals...); }
            inline void* rtGet(const uint32 _index){ return (getOriginalSize() - 1 - index == _index) ? (void*)&val : this->lux::__pvt::seq<index - 1, types...>::rtGet(_index); }
            template<class funcType, class retType, class ...packTypes> inline void exec(funcType func, retType* __ret, packTypes&... packVals){
                this->lux::__pvt::seq<index - 1, types...>::template exec<funcType, retType, packTypes..., type>(func, __ret, packVals..., val);
            }
            template<class objType, class funcType, class retType, class ...packTypes> inline void exec(objType& object, funcType func, retType* __ret, packTypes&... packVals){
                this->lux::__pvt::seq<index - 1, types...>::template exec<objType, funcType, retType, packTypes...>(object, func, __ret, packVals...);
                // lux::__pvt::execObj_t<objType, funcType, retType, packTypes..., type>::exec(object, func, __ret, packVals..., val);
            }
        };




        template<class funcType, class retType, class ...packTypes> struct exec_t{
            static void exec(funcType func, retType* __ret, packTypes&... packVals){ *__ret = func(packVals...); }
        };
        template<class funcType, class ...packTypes> struct exec_t<funcType, void, packTypes...> {
            static void exec(funcType func, void* ret, packTypes&... packVals){ func(packVals...); }
        };
        template<class objType, class funcType, class retType, class ...packTypes> struct execObj_t{
            static void exec(objType& object, funcType func, retType* __ret, packTypes&... packVals){ *__ret = (object.*func)(packVals...); }
        };
        template<class objType, class funcType, class ...packTypes> struct execObj_t<objType, funcType, void, packTypes...> {
            static void exec(objType& object, funcType func, void* ret, packTypes&... packVals){ (object.*func)(packVals...); }
        };
        //Stop at index 0 (seq specialization)
        template<class type> struct seq<0, type> :
        public get_t<CONT, 0, type>,
        public get_t<DESC, 0, type>,
        public get_t<GETV, 0, type>{
            type val;
            inline virtual seq<0, type>* getArr() final override { return this; }
            void ctGet(const type& _val){ val = _val; }
            inline void* rtGet(const uint32 _index){ return (void*)&val; }
            template<class funcType, class retType, class ...packTypes> inline void exec(funcType func, retType* __ret, packTypes&... packVals){
                lux::__pvt::execObj_t<funcType, retType, packTypes..., type>::exec(func, __ret, packVals..., val);
            }
            template<class objType, class funcType, class retType, class ...packTypes> inline void exec(objType& object, funcType func, retType* __ret, packTypes&... packVals){
                lux::__pvt::execObj_t<objType, funcType, retType, packTypes..., type>::exec(object, func, __ret, packVals..., val);
            }
        };
    }




    #define seqIndex (sizeof...(types) - 1)
    //"Heterogeneous Data Compile Time Array" (lux::__pvt::seq uint32erface)
    //An array that can contain elements of different types
    //Size and types must be known at compile time
    //e.g.  lux::HdCtArray<uint32, float32, bool> arr = { 1, 5.10f, true };
    template<class ...types> struct HdCtArray : __pvt::seq<seqIndex, types...>{
        HdCtArray(){}
        HdCtArray(const types&... vals){
            this->lux::__pvt::seq<seqIndex, types...>::ctGet(vals...);
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
        inline void* operator[](const uint32 index) { return this->lux::__pvt::seq<seqIndex, types...>::rtGet(index); }
        inline uint32 count() const { return sizeof...(types); }

        //Calls a function using the array elements as arguments
        //pFunc --| The function to call
        //pReturn | The variable where to store the return value
        //pReturn can be omitted to ignore the return value or call void functions
        template<class funcType, class retType> inline void exec(funcType pFunc, retType& pReturn){
            this->lux::__pvt::seq<seqIndex, types...>::template exec<funcType, retType>(pFunc, &pReturn);
        }
        template<class funcType>                inline void exec(funcType pFunc                  ){
            this->lux::__pvt::seq<seqIndex, types...>::template exec<funcType, void   >(pFunc, nullptr );
        }
        //Calls a function of a class instance using the array elements as arguments
        //pObject | The class instance containing the function
        //pFunc --| The function to call
        //pReturn | The variable where to store the return value
        //pReturn can be omitted to ignore the return value or call void functions
        template<class objType, class funcType, class retType> inline void exec(objType& pObject, funcType pFunc, retType& pReturn){
            this->lux::__pvt::seq<seqIndex, types...>::template exec<objType, funcType, retType>(pObject, pFunc, &pReturn);
        }
        template<class objType, class funcType               > inline void exec(objType& pObject, funcType pFunc                  ){
            this->lux::__pvt::seq<seqIndex, types...>::template exec<objType, funcType, void   >(pObject, pFunc, nullptr );
        }
    };
    #undef seqIndex
}
