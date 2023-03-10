# 1. AFL++ basic tutorial (tested on macOS Ventura 13.0)

1. Download [AFL++](https://github.com/AFLplusplus/AFLplusplus) from the official GitHub repository
2. Build the tool:
     ```
   > cd $afl_repository
   > sudo ./afl-system-config
   > make
   ```
   ![alt text](./images/1.png)
3. Choose a C or C++ program to fuzz ([here](./code/stateless_system.cc) a basic C++ example)
In this example, the system crashes with a string that is the character *+]:* in positions 5 6 and 7.
4. Create a seed file containing the character **a**:
    ```
    > mkdir input_stateless
    > echo "a" > input_stateless/seed.txt
    ```
5. Instrument the code:
    ```
    > ./afl-clang++ ./code/stateless_system.cc -o stateless_system
    ``` 
    ![alt text](./images/5.png)
6. Run the fuzzer:
    ``` 
    > ./afl-fuzz -D -i input_stateless -o output ./code/stateless_system
    ``` 
    ![alt text](./images/6.png)

7. Wait for a bug!
    ![alt text](./images/7.png)

8. Let's take a look at the inputs obtained
    ```
    > cat output/default/crashes/
    ```
    ![alt text](./images/8.png)

# 2. Fuzzing a stateful system using AFL++

A stateful system is a system that keeps some internal state variable for its reasoning. Unlike the stateless ones, these particular systems follow some state model. If you want to have more detail, check out my paper on [stateful fuzzing](https://www.cristiandaniele.com/files/Papers/Fuzzers%20for%20stateful%20systems%20Survey%20and%20Research%20Directions.pdf).
When we want to fuzz a stateful system we have to send not only one message but multiple messages without resetting the SUT.
AFL++ offer a mode, the **PERSISTENT MODE**, that allows to not reset the SUT (Software Under Test). 
This mode is perfect when we have to deal with stateful systems.

### 2.1 Persistent mode on macOS

It's not trivial to run the persistent mode on the mac. You can take a look at this [blog](https://reverse.put.as/2017/07/10/compiling-afl-osx-llvm-mode/) or just try the following commands:

```
 > brew install llvm
 > LLVM_CONFIG=$llvm_path/llvm-config       
 > xcode-select --instal
``` 

### 2.1.1 Fuzzing a stateful system without persistent mode

**[stateful_system.cpp:](./code/stateful_system.cpp)**

    #include <iostream>
    #include <signal.h>
    using namespace std;

    int main(){
        char x;
        int state=0;
        cin >>x;
        while(true){
            switch (state)
            {
                case 0:
                    if(x=='login'){
                        state=1;
                    }
                    break;

                case 1:
                    if(x=='password'){
                        state=2;
                    }
                    break;
                case 2:
                    if(x=='password?'){
                        raise(SIGSEGV);
                    }
                    break;
                default:
                    break;
            }
            if(x=="exit"){
                return 0;
            }
        }
    }

If we run AFL++, the fuzzers cannot handle the loop cycle and it signals a timeout:

![alt text](./images/9.png)

If we get rid of the cycle, the fuzzer runs but shuts down the SUT after every iteration and never reaches the bugged state (state 2) even if we provide the bugged seed file in input:
![alt text](./images/10.png)

### 2.1.2 Fuzzing a stateful system WITH persistent mode

We need the **PERSISTENT MODE** to properly fuzz the SUT. 
We only need to add an artificial loop within the code with the macro *AFL_LOOP*.
**[stateful_system_persistent.cpp:](./code/stateful_system_persistent.cpp)**
    
    #include <iostream>
    #include <signal.h>
    using namespace std;

    int main(){
    char x;
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
                        raise(SIGSEGV);
                    }
                    break;
                default:
                    break;
            }
        }
    }

Now, the SUT will accept multiple messages keeping the same state variables. If we use the [seed file](code/input_stateful/seeds.txt):

    login
    password
    passworda

now AFL++ spots the bug immediately:

![alt text](./images/11.png)

The problem is that the tool treats the seed file like a single message "*login\npassword\npassworda\n*". For this reason, it's not able to discern the different messages and mutate them individually. 

### 2.2.3 AFL++ grammar aware mutator

The only solution seems to write an ad-hoc grammar-based mutator able to handle the traces...
