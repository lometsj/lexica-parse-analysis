#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char **argv){
    ifstream f;
    f.open(argv[1], ios::in);
    while(f){
        string t; 
        f>>t;
        cout<<t<<endl;
    }
    return 0;
}