#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif 

using namespace std;

int main()
{
    cout << "I count to 5 and close!" << endl;

    int milliseconds = 1000;
    #ifdef _WIN32
        for (int i = 1; i <= 5; i++) {
            printf("%d\n", i);
            Sleep(milliseconds);
        }
    #else
        for (int i = 1; i <= 5; i++) {
            printf("%d\n", i);
            usleep(milliseconds * 1000);
        }
    #endif

    cout << "See you again!" << endl;
}