#include "main.h"
#include <iostream>
using namespace std;

int addi(int a, int b){
    return a + b;
}

int sub(int a, int b){
    return a - b;
}

int mul(int a,int b){
    return a*b;
}

int divi(int a,int b){
    if(b!=0) 
    return a/b;
    else 
    cout <<"被除数不能为零"<< endl;
}