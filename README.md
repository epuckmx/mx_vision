## mx vision library for e-puck

Just make sure to NOT define MX_DEV when compiling the e-puck source code.  The macro MX_DEV is used for desktop development.

If you want to run the tests in a desktop just run

    $ gcc -D MX_DEV -o test main.c mx_vision.c
    $ ./test