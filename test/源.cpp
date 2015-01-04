#include <WinBase.h>

static void lockWindows()
{
    WinExec("rundll32.exe user32.dll,LockWorkStation",0);
}

void main(){
	lockWindows();
}