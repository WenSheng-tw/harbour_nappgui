/* Form imageview */

#include "fimage.h"
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FImage *fimage_create(void)
{
    return dbind_create(FImage);
}
