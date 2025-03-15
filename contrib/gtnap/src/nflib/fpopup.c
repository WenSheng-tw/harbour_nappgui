/* Form popup */

#include "fpopup.h"
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FPopUp *fpopup_create(void)
{
    return dbind_create(FPopUp);
}
