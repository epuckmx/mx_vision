## mx vision library for e-puck

Just make sure to NOT define MX_DEV when compiling the e-puck source code. This macro is already defined in mx_vision.h so you must comment this line before compiling for the e-puck.  The macro MX_DEV is used for desktop development.

If you want to run the tests in a desktop just run

    $ gcc -o test main.c mx_vision.c
    $ ./test