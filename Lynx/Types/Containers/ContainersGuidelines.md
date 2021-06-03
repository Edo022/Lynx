
# General member function structure

```cpp
/**
 * @brief Function description
 *     Details                                                  //Optional
 * Complexity:
 *     Best:  O(<complexity>) [specify when this applies]       //[...]: always applies if not specified
 *     Best:  O(<complexity>) [specify when this applies]       //Repeat if there are special cases. Explain in [...]
 *     Worst: O(<complexity>) [specify when this applies]
 *     where <variable> = <value> and <variable2> = <value2>    //Specify what variables are, if there is more than 1
 * @param <parameterName> Parameter description                 //Always comment parameters. Use one @param for each of them
 *     Details                                                  //Optional
 * @return Return value description                             //Always comment non void return values
 *     Details                                                  //Optional
 */
template<targs> ... type functionName(args)) ... {
    static_assert(/*check template validity*/);                 //Static assert templates, if used
    dbg::/*check self validity*/
    dbg::/*check arguments validity*/;

    ...
}
```
e.g. (`template<class tType> class Array{...};`)
```cpp
/**
 * @brief Finds the first element with pValue value
 * Complexity:
 *     Best:  O(1)
 *     Worst: O(n)
 * @param pValue The value to search for
 * @return The index of the first element that has pValue value
 *     Returns -1 if the value is not found
 */
 int find(const tType* pValue){
     dbg::checkParam(pValue != nullptr, "pValue", "pValue cannot be null");
     for(int i = 0; i < this->count(); ++i){
        if(this->elms[i] == *pValue) return i;
     }
     return -1;
 }
 
/**
 * @brief Finds the first element of pArray with pValue and concatenates its index to this object
 * Complexity:
 *     Best:  O(1)
 *     Best:  O(n)   [memory reallocation required]
 *     Worst: O(m)
 *     Worst: O(n+m) [memory reallocation required]
 *     where n = this->count() and m = pArray.count();
 * @param pArray The array where to find pValue
 * @param pValue The value to search for
 * @return true if the value is found, false otherwise
 */
 template<class tType> bool catFirst(const Array<tType>& pArray, const tType& pValue){
    int index = pArray.find(pValue);
    if(index == -1) return false;
    else{
        this->cat(index); //Complexity of cat is Best: O(1), Best: O(n) [memory reallocation required]
        return true;
    }
 }
 
 /**
  * @brief Assignes the pValue value to the element at index vIndex
  *     Elements are copy assigned
  * Complexity: O(1)
  * @param vIndex The index of the element to set
  * @param pValue The new value of the element
  */
  void set(const int vIndex, const tType& pValue){
      dbg::checkParam(vIndex >= 0, "vIndex", "Index must be positive";
      dbg::checkParam(vIndex < this->Count(), "vIndex", "Index out of range";
      this->data[vIndex] = pValue;
  }
```

Do not construct or destroy trivial types  
Specify which elements are constructed or destroyed  

If the function needs to cast templated parameters, specify what casts are used on them  
"Best" and "Worst" are omitted if the complexity doesn't change

# Getters and setters

```cpp
/**
 * @brief Getter description
 * Complexity: O(1)
 * @return <returnedMember>
 */
alwaysInline ... type getMember() ... const {
    dbg::/*check self validity*/;
    return member;
}


/**
 * @brief Setter description
 * Complexity: O(1)
 * @return ...                                  //Error code or member forwarding
 */
alwaysInline ... type setMember(type& vValue) ... {
    dbg::/*check self validity*/;
    dbg::/*check argument validity*/;
    member = vValue;
}
```

Getters and setters should only get and set variables, without additional operations  
If they need to do more, write another function that specifically does that and explain how it works

# General

Prefer debug checks over error codes
Prefer auto over complete templates, if you don't need the name of the type

# Naming conventions

| Structure                                                  | Example                                                                                         |
|------------------------------------------------------------|-------------------------------------------------------------------------------------------------|
|`FolderName`                                                | **`Lynx`**<br>`Lynx/`**`Core`**                                                                 |
|`FileName.extension`                                        | `Lynx/`**`Lynx.hpp`**                                                                           |
|`MacroName`<br>`ClassOrStruct`<br>`TypeAlias`<br>`EnumName` | `#define `**`FunctionLikeMacro`**`(a, b) (a) + (b)`<br>`#define `**`BadMacro`**` 5`<br>`class `**`HcArray`**`{...};`<br>`class `**`String`**`{...};`<br>`typedef HcArray `**`L`**`;`<br>`enum `**`BufferType`**`{ eStorage, eUniform };`<br>`enum `**`CellClass`**`{ eA, eB, eC };`|
|`eEnumValue`                                                | `enum BufferType{`**`eStorage`**`, `**`eUniform`**`}`;<br>`enum CellClass{`**`eA`**`, `**`eB`**`, `**`eC`**`};`|
|`commonstruct`<br>`commonalias`<br>`namespacename`          | `struct `**`vec2`**`{...};`<br>`class `**`vec4`**`{...};`<br>`typedef double `**`f64`**`;`<br>`typedef vec2<f64> `**`f64v2`**`;`<br>`namespace `**`std`**`{...}`<br>`namespace `**`lnx`**`{...}`|
|`variableName`                                              | `int `**`tmp`**` = 1, `**`tmp2`**`;`<br>`float `**`floatVariable`**` = 0.5f;`|
|`vByValueParameter`<br>`pByAddressParameter`<br>`pByReferenceParameter` | `void func(int `**`vIndex`**`);`<br>`void func2(double* `**`vDoubleAddr`**`);`<br>`void func(RaArray<int>& `**`pHugeLynxArray`**`);`<br>`void func2(double* `**`pHugeArray`**`);`|
|`tTemplateType`<br>`kTemplateValue`                         | `template<class `**`tType`**`, int kValue> ...`<br>`template<class tType, int `**`kValue`**`> ...`|
|`type_trait_name`                                           | **`is_integral`**|
|`LNX_CNF_CONFIGURATION_MACRO`                               | `#define `**`LNX_CNF_GLOBAL_THREAD_POOL`**|
|`__stuffYouDontWantToPupupInUserAutocomplete`                | `template<class tType> struct `**`__is_integral_t`**`{...};`
