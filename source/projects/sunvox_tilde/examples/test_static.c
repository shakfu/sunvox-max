/* We are using a static lib. SUNVOX_STATIC_LIB tells the compiler that all
 *  functions should be included at build time.
 */
#define SUNVOX_STATIC_LIB

#include "sunvox.h"

int main()
{
    int ver = sv_init(0, 44100, 2, 0);
    if (ver >= 0) {
        sv_open_slot(0);
        /*
        The SunVox is initialized.
        Slot 0 is open and ready for use.
        Then you can load and play some files in this slot.
        */
        sv_close_slot(0);
        sv_deinit();
    }
    return 0;
}
