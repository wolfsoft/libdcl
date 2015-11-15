# This wrapper for hello_world program lets program working without having DCL
# installed. Instead of specifying directory where default graphical frontends
# of the library are placed (by standard shared library methods, see
# man 8 ld.so), you can configure DCL_GUI_FRONTEND environment variable,
# initialized with path to the frontend library (if your system supports
# several ones, for example).

#!/bin/sh
LD_LIBRARY_PATH=../../../src/dcl-gtk/.libs ./hello_world $1
