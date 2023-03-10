#include <iostream>
#include <signal.h>
#include <string>
using namespace std;

int main(){
string x;
int state=0;
     while (__AFL_LOOP(1000)){
        cin >>x;
        switch (state)
        {
            case 0:
                    if(x=="login"){
                        state=1;
                    }
                    break;

            case 1:
                    if(x=="password"){
                        state=2;
                    }
                    break;
            case 2:
                    if(x=="password?"){
                        cout<<">Bug!";
                        raise(SIGSEGV);
                    }
                    break;
            default:
                    break;
        }
     }
}