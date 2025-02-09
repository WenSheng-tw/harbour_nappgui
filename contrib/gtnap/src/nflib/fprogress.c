/* Form progressbar */

#include "fprogress.h"
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FProgress *fprogress_create(void)
{
    return dbind_create(FProgress);
}
