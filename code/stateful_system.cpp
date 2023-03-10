#include <iostream>
#include <signal.h>
#include <string.h>
using namespace std;

int main(){
    string x;
    int state=0;
    

    // while(true){
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
                    raise(SIGSEGV);
                }
                break;
            default:
                break;
        }
        if(x=="exit"){
            return 0;
        }
        return 0;
    // }
}