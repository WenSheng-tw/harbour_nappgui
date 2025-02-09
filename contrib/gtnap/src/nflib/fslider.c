/* Form slider */

#include "fslider.h"
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FSlider *fslider_create(void)
{
    return dbind_create(FSlider);
}
