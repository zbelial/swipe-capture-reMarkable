#include <iostream>
#include <linux/input.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <sys/time.h>
#include <cstring>
#include <sstream>

using namespace std;

// Longest time considered a "swipe"
long int holdLength = 700000L;
long int minLength = 150000L;
#define EV_PRESSED 1
#define EV_RELEASED 0
#define EV_REPEAT 2
#define device "/dev/input/event2"


struct TouchRecord {
    bool pressed = false;
    struct timeval pressTime;
    string name = "Touch";
    int initX = -1;
    int initY = -1;
    int x = -1;
    int y = -1;
    bool needX = true;
    bool needY = true;
    TouchRecord (string name) : name(name) {}
    TouchRecord(){}
};



int main()
{

    TouchRecord swipe;

    // Open the touch device.
    ifstream eventsfile;
    eventsfile.open("/dev/input/event1", ios::in);
    if(eventsfile.is_open()) {
        // cout << "File open for reading." << endl;
    }
    else {
        // cout << "File couldn't be opened." << endl;
        return 0;
    }


    // Open the buttons device.
    int fd = 0;
    // Get the size of an input event in the right format!
    input_event ie;
    streamsize sie = static_cast<streamsize>(sizeof(struct input_event));
    input_event empty{0};

    while(eventsfile.read((char*)&ie, sie)) {
        switch(ie.code) {
            case ABS_MT_POSITION_X :
                // Reported horizontal distance from right
                if(swipe.needX) {
                    swipe.initX = ie.value;
                    swipe.needX = false;
                }
                else swipe.x = ie.value;

                break;

            case ABS_MT_POSITION_Y:
                // Reported vertical distance from bottom
                if(swipe.needY) {
                    swipe.initY = ie.value;
                    swipe.needY = false;
                }
                else swipe.y = ie.value;

                break;

            case 57:
            {
                // release
                struct timeval ctime;
                gettimeofday(&ctime,NULL);

                // Calculate length of hold
                long usecs = ((ctime.tv_sec   -  swipe.pressTime.tv_sec  )*holdLength
                              +ctime.tv_usec) -  swipe.pressTime.tv_usec;

                // cout<<"usecs "<<usecs<<" Init X "<<swipe.initX<<" After X "<<swipe.x<<endl;
                if(usecs >= minLength && usecs < holdLength && swipe.initX < 150 && swipe.x - swipe.initX >= 100) {
                    // cout << "RIGHT" << endl;
                    if( (fd = open(device, O_RDWR)) > 0 )
                    {
                        struct input_event event;
                        // Press a key (stuff the keyboard with a keypress)
                        event.type = 1;
                        event.value = EV_PRESSED;
                        event.code = 106;
                        write(fd, &event, sie);
                        write(fd, &empty, sie);

                        usleep(20000);

                        // Release the key
                        event.value = EV_RELEASED;
                        event.code = 106;
                        write(fd, &event, sie);
                    }
                }

                if(usecs >= minLength && usecs < holdLength && swipe.initX >= 617 && swipe.initX - swipe.x >= 100) {
                    // cout << "LEFT" << endl;
                    if( fd > 0 )
                    {
                        struct input_event event;
                        // Press a key (stuff the keyboard with a keypress)
                        event.type = 1;
                        event.value = EV_PRESSED;
                        event.code = 105;
                        write(fd, &event, sie);
                        write(fd, &empty, sie);

                        usleep(20000);

                        // Release the key
                        event.value = EV_RELEASED;
                        event.code = 105;
                        write(fd, &event, sie);
                    }
                }

                // Unset the current swipe data.
                swipe.x = swipe.y = swipe.initX = swipe.initY = -1;
                swipe.needX = swipe.needY = true;
                gettimeofday(&swipe.pressTime,NULL);

                break;
            }
            case 0:
            default: break;
        }

    }

    eventsfile.close();
    close(fd);

    return 0;
}
