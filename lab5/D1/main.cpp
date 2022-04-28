#include<iostream>
using namespace std;

extern "C" void func_from_asm();

int main()
{   cout<<"Main function by C++\n";
    cout<<"I will call the function from assembly\n";
    func_from_asm();
    cout<<"Done!\n";
    return 0;
}