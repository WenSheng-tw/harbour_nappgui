/* Form listbox */

#include "flistbox.h"
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FListBox *flistbox_create(void)
{
    return dbind_create(FListBox);
}
