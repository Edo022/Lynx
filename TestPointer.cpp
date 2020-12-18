#include "LuxEngine/Types/Containers/RtArray.hpp"
using namespace lux;







void io(){
    RtArray<int> p0(100*sizeof(int));
    for(int i = 0; i < 100; i++) p0[i] = i;
    p0.resize(105);
    p0.resize(110);
    p0.resize(109);
    printf("\n");
    for(int i = 0; i < p0.count(); ++i) printf("%d ", p0[i]);
}


RtArray<int> ret(){
    RtArray<int> p0(10);
    for(int i = 0; i < 10; i++) p0[i] = i;
    return p0;
}


int main(){
    io();
    printf("\n");
    RtArray<int> ret_ = ret();
    for(int i = 0; i < ret_.count(); ++i) printf("%d ", ret_[i]);
}