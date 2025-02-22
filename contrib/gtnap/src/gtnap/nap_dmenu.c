/*
    This is part of gtnap
    TODO: More info
*/
/* Dynamic menus support for GTNAP-Forms */

#include "gtnap.h"
#include "gtnap.inl"
#include <gui/menuitem.h>
#include <gui/menu.h>

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_CREATE)
{
    GtNapMenu *menu = hb_gtnap_menu_create();
    hb_retptr(menu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENUITEM_CREATE)
{
    HB_ITEM *text_block = hb_param(1, HB_IT_BLOCK | HB_IT_STRING);
    const char_t *icon_path = hb_parcx(2);
    HB_ITEM *click_block = hb_param(3, HB_IT_BLOCK);
    GtNapMenuItem *item = hb_gtnap_menuitem_create(text_block, icon_path, click_block);
    hb_retptr(item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENUITEM_SEPARATOR)
{
    GtNapMenuItem *item = hb_gtnap_menuitem_separator();
    hb_retptr(item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_DESTROY)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    hb_gtnap_menu_destroy(menu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_ADD_ITEM)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    GtNapMenuItem *item = cast(hb_parptr(2), GtNapMenuItem);
    hb_gtnap_menu_add_item(menu, item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENUITEM_SUBMENU)
{
    GtNapMenuItem *item = cast(hb_parptr(1), GtNapMenuItem);
    GtNapMenu *submenu = cast(hb_parptr(2), GtNapMenu);
    hb_gtnap_menuitem_submenu(item, submenu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_BAR)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    GtNapForm *form = cast(hb_parptr(2), GtNapForm);
    hb_gtnap_menubar(menu, form);
}
