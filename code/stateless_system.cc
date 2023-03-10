#include<signal.h>
#include <iostream>
using namespace std;
int main(int argc, char **argv){
    string s;
    cin>>s;
    if(s[5]=='+'){
        if(s[6]==']'){
            if(s[7]==':'){
                raise(SIGSEGV);
            }
        }
    }
    return 0;
}