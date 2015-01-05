/* Test 1: system() call the windows API
#include <stdlib.h>
void main()
{
    system("\"rundll32.exe user32.dll,LockWorkStation\"");
}*/


/* Test 2: API function LockWorkStation
#include <windows.h>
#include <stdio.h>

//#pragma comment( lib, "user32.lib" )

void main()
{
    // Lock the workstation.

    if( !LockWorkStation() )
        printf ("LockWorkStation failed with %d\n", GetLastError());
}*/



#include <list>
#include <iostream>
#include <numeric>

using namespace std;

int main()
{
	list<int> d;
	d.push_back(1);
	d.push_back(2);
	d.push_back(3);
	d.push_back(4);
	d.push_back(5);

	d.pop_front();
	d.pop_front();
	d.pop_front();
	
	int s = accumulate(d.begin(),d.end(),0);
	cout << s << endl;
	
    return 0;
}