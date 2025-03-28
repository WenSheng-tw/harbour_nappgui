/* Design form */

#include "dform.h"
#include "designer.h"
#include "dlayout.h"
#include "dialogs.h"
#include "inspect.h"
#include "propedit.h"
#include <nflib/flabel.h>
#include <nflib/flayout.h>
#include <gui/guicontrol.h>
#include <gui/button.h>
#include <gui/edit.h>
#include <gui/popup.h>
#include <gui/label.h>
#include <gui/listbox.h>
#include <gui/imageview.h>
#include <gui/textview.h>
#include <gui/layout.h>
#include <gui/layouth.h>
#include <gui/panel.h>
#include <gui/panel.inl>
#include <gui/slider.h>
#include <gui/progress.h>
#include <gui/window.h>
#include <draw2d/image.h>
#include <geom2d/v2d.h>
#include <geom2d/s2d.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <core/heap.h>
#include <core/stream.h>
#include <core/strings.h>
#include <sewer/bstd.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>

struct _dform_t
{
    Designer *app;
    DLayout *dlayout;
    FLayout *flayout;
    Layout *glayout;
    Window *window;
    DSelect hover;
    DSelect sel;
    ArrSt(DSelect) *temp_path;
    ArrSt(DSelect) *sel_path;
    uint32_t layout_id;
    uint32_t cell_id;
    bool_t need_save;
};

/*---------------------------------------------------------------------------*/

static real32_t i_EMPTY_CELL_WIDTH = 40;
static real32_t i_EMPTY_CELL_HEIGHT = 20;

/*---------------------------------------------------------------------------*/

static void i_layout_obj_names(DForm *form, FLayout *flayout)
{
    cassert_no_null(form);

    {
        char_t name[64];
        bstd_sprintf(name, sizeof(name), "layout%d", form->layout_id);
        str_upd(&flayout->name, name);
        form->layout_id += 1;
    }

    {
        uint32_t i, ncols = flayout_ncols(flayout);
        uint32_t j, nrows = flayout_nrows(flayout);
        for (j = 0; j < nrows; ++j)
        {
            for (i = 0; i < ncols; ++i)
            {
                char_t name[64];
                FCell *fcell = flayout_cell(flayout, i, j);
                bstd_sprintf(name, sizeof(name), "cell%d", form->cell_id);
                str_upd(&fcell->name, name);
                form->cell_id += 1;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_need_save(DForm *form)
{
    cassert_no_null(form);
    form->need_save = TRUE;
    designer_need_save(form->app);
}

/*---------------------------------------------------------------------------*/

DForm *dform_first_example(void)
{
    DForm *form = heap_new0(DForm);
    FLayout *flayout1 = flayout_create(2, 6);
    FLayout *flayout2 = flayout_create(1, 4);
    FLayout *flayout3 = flayout_create(2, 1);
    FLabel *label1 = flabel_create();
    FLabel *label2 = flabel_create();
    str_upd(&label1->text, "This is a label");
    str_upd(&label2->text, "Other");
    flayout_add_label(flayout1, label1, 0, 0);
    flayout_add_label(flayout1, label2, 0, 1);
    flayout_add_layout(flayout3, flayout1, 0, 0);
    flayout_add_layout(flayout3, flayout2, 1, 0);
    flayout_margin_col(flayout1, 0, 5);
    flayout_margin_row(flayout1, 0, 5);
    flayout_margin_row(flayout1, 1, 5);
    flayout_margin_row(flayout1, 2, 5);
    flayout_margin_row(flayout1, 3, 5);
    flayout_margin_row(flayout1, 4, 5);
    flayout_margin_row(flayout2, 0, 5);
    flayout_margin_row(flayout2, 1, 5);
    flayout_margin_row(flayout2, 2, 5);
    flayout_margin_left(flayout3, 10);
    flayout_margin_top(flayout3, 10);
    flayout_margin_right(flayout3, 10);
    flayout_margin_bottom(flayout3, 10);
    flayout_margin_col(flayout3, 0, 5);
    form->flayout = flayout3;
    form->temp_path = arrst_create(DSelect);
    form->sel_path = arrst_create(DSelect);
    i_layout_obj_names(form, flayout1);
    i_layout_obj_names(form, flayout2);
    i_layout_obj_names(form, flayout3);
    return form;
}

/*---------------------------------------------------------------------------*/

DForm *dform_empty(Designer *app)
{
    DForm *form = heap_new0(DForm);
    form->app = app;
    form->flayout = flayout_create(1, 1);
    form->temp_path = arrst_create(DSelect);
    form->sel_path = arrst_create(DSelect);
    i_layout_obj_names(form, form->flayout);
    i_need_save(form);
    return form;
}

/*---------------------------------------------------------------------------*/

DForm *dform_read(Stream *stm, Designer *app)
{
    FLayout *flayout = flayout_read(stm);
    if (stm_state(stm) == ekSTOK)
    {
        DForm *form = heap_new0(DForm);
        form->app = app;
        form->flayout = flayout;
        form->temp_path = arrst_create(DSelect);
        form->sel_path = arrst_create(DSelect);
        return form;
    }
    else
    {
        dbind_destopt(&flayout, FLayout);
        return NULL;
    }
}

/*---------------------------------------------------------------------------*/

void dform_destroy(DForm **form)
{
    cassert_no_null(form);
    cassert_no_null(*form);
    flayout_destroy(&(*form)->flayout);
    arrst_destroy(&(*form)->temp_path, NULL, DSelect);
    arrst_destroy(&(*form)->sel_path, NULL, DSelect);
    if ((*form)->window != NULL)
    {
        cassert((*form)->glayout != NULL);
        dlayout_destroy(&(*form)->dlayout);
        window_destroy(&(*form)->window);
    }
    else
    {
        cassert((*form)->dlayout == NULL);
        cassert((*form)->glayout == NULL);
    }

    heap_delete(form, DForm);
}

/*---------------------------------------------------------------------------*/

void dform_write(Stream *stm, DForm *form)
{
    cassert_no_null(form);
    flayout_write(stm, form->flayout);
    form->need_save = FALSE;
}

/*---------------------------------------------------------------------------*/

void dform_compose(DForm *form)
{
    cassert_no_null(form);
    if (form->glayout == NULL)
    {
        Panel *panel = panel_create();
        const char_t *resource_path = designer_folder_path(form->app);
        cassert(form->window == NULL);
        cassert(form->dlayout == NULL);
        form->dlayout = dlayout_from_flayout(form->flayout, resource_path);
        form->glayout = flayout_to_gui(form->flayout, resource_path, i_EMPTY_CELL_WIDTH, i_EMPTY_CELL_HEIGHT);
        panel_layout(panel, form->glayout);
        form->window = window_create(ekWINDOW_STD);
        window_panel(form->window, panel);
    }

    window_update(form->window);
    dlayout_synchro_visual(form->dlayout, form->glayout, kV2D_ZEROf);
}

/*---------------------------------------------------------------------------*/

void dform_set(DForm *form, Panel *inspect, Panel *propedit)
{
    cassert_no_null(form);
    inspect_set(inspect, form);
    propedit_set(propedit, form, &form->sel);
}

/*---------------------------------------------------------------------------*/

static bool_t i_sel_equ(const DSelect *sel1, const DSelect *sel2)
{
    cassert_no_null(sel1);
    cassert_no_null(sel2);
    if (sel1->dlayout != sel2->dlayout)
        return FALSE;

    if (sel1->dlayout == NULL && sel2->dlayout == NULL)
        return TRUE;

    if (sel1->elem == sel2->elem
        && sel1->col == sel2->col
        && sel1->row == sel2->row)
        return TRUE;
    else
        return FALSE;
}

/*---------------------------------------------------------------------------*/

static void i_elem_at_mouse(DLayout *dlayout, FLayout *flayout, Layout *glayout, const real32_t mouse_x, const real32_t mouse_y, ArrSt(DSelect) *selpath, DSelect *sel)
{
    cassert_no_null(sel);
    arrst_clear(selpath, NULL, DSelect);
    dlayout_elem_at_pos(dlayout, flayout, glayout, mouse_x, mouse_y, selpath);
    if (arrst_size(selpath, DSelect) > 0)
    {
        *sel = *arrst_last(selpath, DSelect);
    }
    else
    {
        sel->dlayout = NULL;
        sel->flayout = NULL;
        sel->glayout = NULL;
        sel->elem = ENUM_MAX(layelem_t);
        sel->col = UINT32_MAX;
        sel->row = UINT32_MAX;
    }
}

/*---------------------------------------------------------------------------*/

bool_t dform_need_save(const DForm *form)
{
    cassert_no_null(form);
    return form->need_save;
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnMove(DForm *form, const real32_t mouse_x, const real32_t mouse_y)
{
    DSelect hover;
    bool_t equ = TRUE;
    i_elem_at_mouse(form->dlayout, form->flayout, form->glayout, mouse_x, mouse_y, form->temp_path, &hover);
    equ = i_sel_equ(&form->hover, &hover);
    form->hover = hover;
    return !equ;
}

/*---------------------------------------------------------------------------*/

static align_t i_halign(const halign_t halign)
{
    switch(halign) {
    case ekHALIGN_LEFT:
        return ekLEFT;
    case ekHALIGN_CENTER:
        return ekCENTER;
    case ekHALIGN_RIGHT:
        return ekRIGHT;
    case ekHALIGN_JUSTIFY:
        return ekJUSTIFY;
    cassert_default();
    }
    return ekLEFT;
}

/*---------------------------------------------------------------------------*/

static align_t i_valign(const valign_t valign)
{
    switch(valign) {
    case ekVALIGN_TOP:
        return ekTOP;
    case ekVALIGN_CENTER:
        return ekCENTER;
    case ekVALIGN_BOTTOM:
        return ekBOTTOM;
    case ekVALIGN_JUSTIFY:
        return ekJUSTIFY;
    cassert_default();
    }
    return ekTOP;
}

/*---------------------------------------------------------------------------*/

static gui_scale_t i_scale(const scale_t scale)
{
    switch(scale) {
    case ekSCALE_NONE:
        return ekGUI_SCALE_NONE;
    case ekSCALE_AUTO:
        return ekGUI_SCALE_AUTO;
    case ekSCALE_ASPECT:
        return ekGUI_SCALE_ASPECT;
    case ekSCALE_FIT:
        return ekGUI_SCALE_ADJUST;
    cassert_default();
    }
    return ekGUI_SCALE_ASPECT;
}

/*---------------------------------------------------------------------------*/

static void i_sel_remove_cell(const DSelect *sel)
{
    cassert_no_null(sel);
    flayout_remove_cell(sel->flayout, sel->col, sel->row);
    dlayout_remove_cell(sel->dlayout, sel->col, sel->row);
    layout_remove_cell(sel->glayout, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_sel_synchro_cell(const DSelect *sel)
{
    const FCell *fcell = NULL;
    align_t halign = ENUM_MAX(align_t);
    align_t valign = ENUM_MAX(align_t);
    cassert_no_null(sel);
    fcell = flayout_ccell(sel->flayout, sel->col, sel->row);
    halign = i_halign(fcell->halign);
    valign = i_valign(fcell->valign);
    layout_halign(sel->glayout, sel->col, sel->row, halign);
    layout_valign(sel->glayout, sel->col, sel->row, valign);
}

/*---------------------------------------------------------------------------*/

static bool_t i_sel_empty_cell(const DSelect *sel)
{
    cassert_no_null(sel);
    if (sel->flayout != NULL)
    {
        cassert_no_null(sel->glayout);
        cassert_no_null(sel->dlayout);
        if (sel->elem == ekLAYELEM_CELL)
        {
            const FCell *cell = flayout_ccell(sel->flayout, sel->col, sel->row);
            cassert_no_null(cell);
            if (cell->type == ekCELL_TYPE_EMPTY)
                return TRUE;
        }
    }
    else
    {
        cassert(sel->glayout == NULL);
        cassert(sel->dlayout == NULL);
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnClick(DForm *form, Window *window, Panel *inspect, Panel *propedit, const widget_t widget, const real32_t mouse_x, const real32_t mouse_y, const gui_mouse_t mbutton)
{
    cassert_no_null(form);
    if (mbutton == ekGUI_MOUSE_LEFT)
    {
        DSelect sel;
        i_elem_at_mouse(form->dlayout, form->flayout, form->glayout, mouse_x, mouse_y, form->sel_path, &sel);
        inspect_set(inspect, form);
        if (i_sel_empty_cell(&sel) == TRUE)
        {
            cassert_no_null(form->dlayout);
            switch(widget) {
            case ekWIDGET_SELECT:
                break;

            case ekWIDGET_LABEL:
            {
                FLabel *flabel = dialog_new_label(window, &sel);
                if (flabel != NULL)
                {
                    Label *label = label_create();
                    label_text(label, tc(flabel->text));
                    label_multiline(label, flabel->multiline);
                    label_min_width(label, flabel->min_width);
                    label_align(label, i_halign(flabel->align));
                    i_sel_remove_cell(&sel);
                    flayout_add_label(sel.flayout, flabel, sel.col, sel.row);
                    layout_label(sel.glayout, label, sel.col, sel.row);
                    i_sel_synchro_cell(&sel);
                    dform_compose(form);
                    propedit_set(propedit, form, &sel);
                    inspect_set(inspect, form);
                    form->sel = sel;
                    i_need_save(form);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_BUTTON:
            {
                FButton *fbutton = dialog_new_button(window, &sel);
                if (fbutton != NULL)
                {
                    Button *button = button_push();
                    button_text(button, tc(fbutton->text));
                    i_sel_remove_cell(&sel);
                    flayout_add_button(sel.flayout, fbutton, sel.col, sel.row);
                    layout_button(sel.glayout, button, sel.col, sel.row);
                    i_sel_synchro_cell(&sel);
                    dform_compose(form);
                    propedit_set(propedit, form, &sel);
                    inspect_set(inspect, form);
                    form->sel = sel;
                    i_need_save(form);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_CHECKBOX:
            {
                FCheck *fcheck = dialog_new_check(window, &sel);
                if (fcheck != NULL)
                {
                    Button *check = button_check();
                    button_text(check, tc(fcheck->text));
                    i_sel_remove_cell(&sel);
                    flayout_add_check(sel.flayout, fcheck, sel.col, sel.row);
                    layout_button(sel.glayout, check, sel.col, sel.row);
                    i_sel_synchro_cell(&sel);
                    dform_compose(form);
                    propedit_set(propedit, form, &sel);
                    inspect_set(inspect, form);
                    form->sel = sel;
                    i_need_save(form);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_EDITBOX:
            {
                FEdit *fedit = dialog_new_edit(window, &sel);
                if (fedit != NULL)
                {
                    Edit *edit = edit_create();
                    align_t align = i_halign(fedit->text_align);
                    edit_passmode(edit, fedit->passmode);
                    edit_autoselect(edit, fedit->autosel);
                    edit_align(edit, align);
                    i_sel_remove_cell(&sel);
                    flayout_add_edit(sel.flayout, fedit, sel.col, sel.row);
                    layout_edit(sel.glayout, edit, sel.col, sel.row);
                    i_sel_synchro_cell(&sel);
                    dform_compose(form);
                    propedit_set(propedit, form, &sel);
                    inspect_set(inspect, form);
                    form->sel = sel;
                    i_need_save(form);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_TEXTVIEW:
            {
                FText *ftext = dialog_new_text(window, &sel);
                if (ftext != NULL)
                {
                    TextView *text = textview_create();
                    textview_editable(text, !ftext->read_only);
                    textview_size(text, s2df(ftext->min_width, ftext->min_height));
                    i_sel_remove_cell(&sel);
                    flayout_add_text(sel.flayout, ftext, sel.col, sel.row);
                    layout_textview(sel.glayout, text, sel.col, sel.row);
                    i_sel_synchro_cell(&sel);
                    dform_compose(form);
                    propedit_set(propedit, form, &sel);
                    inspect_set(inspect, form);
                    form->sel = sel;
                    i_need_save(form);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

			case ekWIDGET_IMAGEVIEW:
			{
                const char_t *folder_path = designer_folder_path(form->app);
				FImage *fimage = dialog_new_image(window, &sel, folder_path);
                if (fimage != NULL)
                {
                    ImageView *gimage = imageview_create();
                    String *path = str_printf("%s%s", folder_path, tc(fimage->path));
                    Image *image = image_from_file(tc(path), NULL);
                    imageview_scale(gimage, i_scale(fimage->scale));
                    imageview_image(gimage, image);
                    imageview_size(gimage, s2df(fimage->min_width, fimage->min_height));
                    i_sel_remove_cell(&sel);
                    flayout_add_image(sel.flayout, fimage, sel.col, sel.row);
                    layout_imageview(sel.glayout, gimage, sel.col, sel.row);
                    dlayout_set_image(sel.dlayout, image, sel.col, sel.row);
                    i_sel_synchro_cell(&sel);
                    dform_compose(form);
                    propedit_set(propedit, form, &sel);
                    inspect_set(inspect, form);
                    form->sel = sel;
                    i_need_save(form);
                    ptr_destopt(image_destroy, &image, Image);
                    str_destroy(&path);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
			}

            case ekWIDGET_SLIDER:
            {
                FSlider *fslider = dialog_new_slider(window, &sel);
                if (fslider != NULL)
                {
                    Slider *slider = slider_create();
                    slider_min_width(slider, fslider->min_width);
                    slider_value(slider, .5f);
                    i_sel_remove_cell(&sel);
                    flayout_add_slider(sel.flayout, fslider, sel.col, sel.row);
                    layout_slider(sel.glayout, slider, sel.col, sel.row);
                    i_sel_synchro_cell(&sel);
                    dform_compose(form);
                    propedit_set(propedit, form, &sel);
                    inspect_set(inspect, form);
                    form->sel = sel;
                    i_need_save(form);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_PROGRESS:
            {
                FProgress *fprogress = dialog_new_progress(window, &sel);
                if (fprogress != NULL)
                {
                    Progress *progress = progress_create();
                    progress_min_width(progress, fprogress->min_width);
                    progress_value(progress, .5f);
                    i_sel_remove_cell(&sel);
                    flayout_add_progress(sel.flayout, fprogress, sel.col, sel.row);
                    layout_progress(sel.glayout, progress, sel.col, sel.row);
                    i_sel_synchro_cell(&sel);
                    dform_compose(form);
                    propedit_set(propedit, form, &sel);
                    inspect_set(inspect, form);
                    form->sel = sel;
                    i_need_save(form);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_POPUP:
            {
                FPopUp *fpopup = dialog_new_popup(window, &sel);
                if (fpopup != NULL)
                {
                    PopUp *popup = popup_create();
                    cassert(arrst_size(fpopup->elems, FElem) == 0);
                    i_sel_remove_cell(&sel);
                    flayout_add_popup(sel.flayout, fpopup, sel.col, sel.row);
                    layout_popup(sel.glayout, popup, sel.col, sel.row);
                    i_sel_synchro_cell(&sel);
                    dform_compose(form);
                    propedit_set(propedit, form, &sel);
                    inspect_set(inspect, form);
                    form->sel = sel;
                    i_need_save(form);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_LISTBOX:
            {
                FListBox *flistbox = dialog_new_listbox(window, &sel);
                if (flistbox != NULL)
                {
                    ListBox *listbox = listbox_create();
                    cassert(arrst_size(flistbox->elems, FElem) == 0);
                    listbox_size(listbox, s2df(flistbox->min_width, flistbox->min_height));
                    i_sel_remove_cell(&sel);
                    flayout_add_listbox(sel.flayout, flistbox, sel.col, sel.row);
                    layout_listbox(sel.glayout, listbox, sel.col, sel.row);
                    i_sel_synchro_cell(&sel);
                    dform_compose(form);
                    propedit_set(propedit, form, &sel);
                    inspect_set(inspect, form);
                    form->sel = sel;
                    i_need_save(form);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

			case ekWIDGET_GRID_LAYOUT:
            {
                FLayout *fsublayout = dialog_new_layout(window, &sel);
                if (fsublayout != NULL)
                {
                    const char_t *resource_path = designer_folder_path(form->app);
                    DLayout *dsublayout = dlayout_from_flayout(fsublayout, resource_path);
                    Layout *gsublayout = flayout_to_gui(fsublayout, resource_path, i_EMPTY_CELL_WIDTH, i_EMPTY_CELL_HEIGHT);
                    i_layout_obj_names(form, fsublayout);
                    i_sel_remove_cell(&sel);
                    dlayout_add_layout(sel.dlayout, dsublayout, sel.col, sel.row);
                    flayout_add_layout(sel.flayout, fsublayout, sel.col, sel.row);
                    layout_layout(sel.glayout, gsublayout, sel.col, sel.row);
                    i_sel_synchro_cell(&sel);
                    dform_compose(form);
                    propedit_set(propedit, form, &sel);
                    inspect_set(inspect, form);
                    form->sel = sel;
                    i_need_save(form);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            default:
                break;
            }
        }

        /* No new component added, just select */
        {
            bool_t equ = i_sel_equ(&form->sel, &sel);
            propedit_set(propedit, form, &sel);
            form->sel = sel;
            return !equ;
        }
    }
    else
    {
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnExit(DForm *form)
{
    DSelect sel;
    bool_t equ = TRUE;
    sel.dlayout = NULL;
    sel.flayout = NULL;
    sel.glayout = NULL;
    sel.elem = ENUM_MAX(layelem_t);
    sel.col = UINT32_MAX;
    sel.row = UINT32_MAX;
    equ = i_sel_equ(&form->hover, &sel);
    form->hover = sel;
    return !equ;
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnSupr(DForm *form, Panel *inspect, Panel *propedit)
{
    cassert_no_null(form);
    if (form->sel.dlayout != NULL && form->sel.elem == ekLAYELEM_CELL)
    {
        cassert_no_null(form->sel.flayout);
        cassert_no_null(form->sel.glayout);
        if (i_sel_empty_cell(&form->sel) == FALSE)
        {
            /* Remove all inspector path steps after deleted cell */
            {
                uint32_t n = arrst_size(form->sel_path, DSelect);
                while (n > 0)
                {
                    const DSelect *last = arrst_last_const(form->sel_path, DSelect);
                    if (i_sel_equ(&form->sel, last) == TRUE)
                        break;

                    arrst_delete(form->sel_path, n - 1, NULL, DSelect);
                    n -= 1;
                }
            }

            /* Remove the cell itself */
            {
                Cell *cell = layout_cell(form->sel.glayout, form->sel.col, form->sel.row);
                i_sel_remove_cell(&form->sel);
                cell_force_size(cell, i_EMPTY_CELL_WIDTH, i_EMPTY_CELL_HEIGHT);
                i_sel_synchro_cell(&form->sel);
                dform_compose(form);
                propedit_set(propedit, form, &form->sel);
                inspect_set(inspect, form);
            }

            i_need_save(form);
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

static FCell *i_sel_fcell(const DSelect *sel)
{
    cassert_no_null(sel);
    if (sel->flayout != NULL)
    {
        cassert_no_null(sel->glayout);
        cassert_no_null(sel->dlayout);
        if (sel->elem == ekLAYELEM_CELL)
        {
            return flayout_cell(sel->flayout, sel->col, sel->row);
        }
    }
    else
    {
        cassert(sel->glayout == NULL);
        cassert(sel->dlayout == NULL);
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

void dform_synchro_cell_text(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    i_need_save(form);
    if (cell->type == ekCELL_TYPE_LABEL)
    {
        Label *label = layout_get_label(sel->glayout, sel->col, sel->row);
        label_text(label, tc(cell->widget.label->text));
    }
    else if (cell->type == ekCELL_TYPE_BUTTON)
    {
        Button *button = layout_get_button(sel->glayout, sel->col, sel->row);
        button_text(button, tc(cell->widget.button->text));
    }
    else if (cell->type == ekCELL_TYPE_CHECK)
    {
        Button *button = layout_get_button(sel->glayout, sel->col, sel->row);
        button_text(button, tc(cell->widget.check->text));
    }
    else
    {
        cassert(FALSE);
    }
}

/*---------------------------------------------------------------------------*/

void dform_synchro_cell_image(DForm *form, const DSelect *sel, const Image *image, const char_t *imgname)
{
    FCell *cell = i_sel_fcell(sel);
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    i_need_save(form);
    if (cell->type == ekCELL_TYPE_IMAGE)
    {
        ImageView *imgview = layout_get_imageview(sel->glayout, sel->col, sel->row);
        imageview_image(imgview, image);
        str_upd(&cell->widget.image->path, imgname);
    }
    else
    {
        cassert(FALSE);
    }
}

/*---------------------------------------------------------------------------*/

void dform_synchro_button(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Button *button = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_BUTTON);
    i_need_save(form);
    button = layout_get_button(sel->glayout, sel->col, sel->row);
    button_min_width(button, cell->widget.button->min_width);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_label(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Label *label = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_LABEL);
    i_need_save(form);
    label = layout_get_label(sel->glayout, sel->col, sel->row);
    label_multiline(label, cell->widget.label->multiline);
    label_min_width(label, cell->widget.label->min_width);
    label_align(label, i_halign(cell->widget.label->align));
}

/*---------------------------------------------------------------------------*/

void dform_synchro_edit(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Edit *edit = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_EDIT);
    i_need_save(form);
    edit = layout_get_edit(sel->glayout, sel->col, sel->row);
    edit_passmode(edit, cell->widget.edit->passmode);
    edit_autoselect(edit, cell->widget.edit->autosel);
    edit_align(edit, i_halign(cell->widget.edit->text_align));
    edit_min_width(edit, cell->widget.edit->min_width);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_textview(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    TextView *text = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_TEXT);
    i_need_save(form);
    text = layout_get_textview(sel->glayout, sel->col, sel->row);
    textview_editable(text, !cell->widget.text->read_only);
    textview_size(text, s2df(cell->widget.text->min_width, cell->widget.text->min_height));
}

/*---------------------------------------------------------------------------*/

void dform_synchro_imageview(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    ImageView *imgview = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_IMAGE);
    i_need_save(form);
    imgview = layout_get_imageview(sel->glayout, sel->col, sel->row);
    imageview_size(imgview, s2df(cell->widget.image->min_width, cell->widget.image->min_height));
    imageview_scale(imgview, i_scale(cell->widget.image->scale));
}

/*---------------------------------------------------------------------------*/

void dform_synchro_slider(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Slider *slider = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_SLIDER);
    i_need_save(form);
    slider = layout_get_slider(sel->glayout, sel->col, sel->row);
    slider_min_width(slider, cell->widget.slider->min_width);
    slider_value(slider, .5f);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_progress(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Progress *progress = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_PROGRESS);
    i_need_save(form);
    progress = layout_get_progress(sel->glayout, sel->col, sel->row);
    progress_min_width(progress, cell->widget.progress->min_width);
    progress_value(progress, .5f);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_popup_add(DForm *form, const DSelect *sel, const Image *image)
{
    FCell *cell = i_sel_fcell(sel);
    PopUp *popup = NULL;
    const FElem *elem = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_POPUP);
    i_need_save(form);
    popup = layout_get_popup(sel->glayout, sel->col, sel->row);
    elem = arrst_last_const(cell->widget.popup->elems, FElem);
    popup_add_elem(popup, tc(elem->text), image);
    dlayout_add_image(sel->dlayout, image, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_popup_clear(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    PopUp *popup = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_POPUP);
    i_need_save(form);
    popup = layout_get_popup(sel->glayout, sel->col, sel->row);
    popup_clear(popup);
    dlayout_clear_images(sel->dlayout, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_listbox(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    ListBox *listbox = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_LISTBOX);
    i_need_save(form);
    listbox = layout_get_listbox(sel->glayout, sel->col, sel->row);
    listbox_size(listbox, s2df(cell->widget.listbox->min_width, cell->widget.listbox->min_height));
}

/*---------------------------------------------------------------------------*/

void dform_synchro_listbox_add(DForm *form, const DSelect *sel, const Image *image)
{
    FCell *cell = i_sel_fcell(sel);
    ListBox *listbox = NULL;
    const FElem *elem = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_LISTBOX);
    i_need_save(form);
    listbox = layout_get_listbox(sel->glayout, sel->col, sel->row);
    elem = arrst_last_const(cell->widget.listbox->elems, FElem);
    listbox_add_elem(listbox, tc(elem->text), image);
    dlayout_add_image(sel->dlayout, image, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_listbox_del(DForm *form, const DSelect *sel, const uint32_t index)
{
    FCell *cell = i_sel_fcell(sel);
    ListBox *listbox = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_LISTBOX);
    i_need_save(form);
    listbox = layout_get_listbox(sel->glayout, sel->col, sel->row);
    listbox_del_elem(listbox, index);
    dlayout_del_image(sel->dlayout, index, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_listbox_clear(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    ListBox *listbox = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_LISTBOX);
    i_need_save(form);
    listbox = layout_get_listbox(sel->glayout, sel->col, sel->row);
    listbox_clear(listbox);
    dlayout_clear_images(sel->dlayout, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_layout_margin(DForm *form, const DSelect *sel)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    i_need_save(form);
    layout_margin4(sel->glayout, sel->flayout->margin_top, sel->flayout->margin_right, sel->flayout->margin_bottom, sel->flayout->margin_left);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_column_margin(DForm *form, const DSelect *sel, const FColumn *fcol, const uint32_t col)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(fcol);
    cassert(flayout_column(cast(sel->flayout, FLayout), col) == fcol);
    i_need_save(form);
    layout_hmargin(sel->glayout, col, fcol->margin_right);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_column_width(DForm *form, const DSelect *sel, const FColumn *fcol, const uint32_t col)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(fcol);
    cassert(flayout_column(cast(sel->flayout, FLayout), col) == fcol);
    i_need_save(form);
    layout_hsize(sel->glayout, col, fcol->forced_width);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_row_margin(DForm *form, const DSelect *sel, const FRow *frow, const uint32_t row)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(frow);
    cassert(flayout_row(cast(sel->flayout, FLayout), row) == frow);
    i_need_save(form);
    layout_vmargin(sel->glayout, row, frow->margin_bottom);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_row_height(DForm *form, const DSelect *sel, const FRow *frow, const uint32_t row)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(frow);
    cassert(flayout_row(cast(sel->flayout, FLayout), row) == frow);
    i_need_save(form);
    layout_vsize(sel->glayout, row, frow->forced_height);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_cell_halign(DForm *form, const DSelect *sel, const FCell *fcell, const uint32_t col, const uint32_t row)
{
    align_t align = ENUM_MAX(align_t);
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(fcell);
    cassert(flayout_cell(cast(sel->flayout, FLayout), col, row) == fcell);
    i_need_save(form);
    align = i_halign(fcell->halign);
    layout_halign(sel->glayout, col, row, align);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_cell_valign(DForm *form, const DSelect *sel, const FCell *fcell, const uint32_t col, const uint32_t row)
{
    align_t align = ENUM_MAX(align_t);
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(fcell);
    cassert(flayout_cell(cast(sel->flayout, FLayout), col, row) == fcell);
    i_need_save(form);
    align = i_valign(fcell->valign);
    layout_valign(sel->glayout, col, row, align);
}

/*---------------------------------------------------------------------------*/

FCell *dform_sel_fcell(const DSelect *sel)
{
    return i_sel_fcell(sel);
}

/*---------------------------------------------------------------------------*/

void dform_draw(const DForm *form, const widget_t swidget, const Image *add_icon, const Font *default_font, DCtx *ctx)
{
    cassert_no_null(form);
    dlayout_draw(form->dlayout, form->flayout, form->glayout, &form->hover, &form->sel, swidget, add_icon, default_font, ctx);
}

/*---------------------------------------------------------------------------*/

uint32_t dform_selpath_size(const DForm *form)
{
    uint32_t n = 0;
    cassert_no_null(form);
    n = arrst_size(form->sel_path, DSelect);
    if (n > 0)
    {
        const DSelect *last = arrst_last_const(form->sel_path, DSelect);
        cassert(last->dlayout != NULL);
        if (last->elem == ekLAYELEM_CELL)
            return n * 2;
        else
            return (n - 1) * 2 + 1;
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

const char_t *dform_selpath_caption(const DForm *form, const uint32_t col, const uint32_t row)
{
    const DSelect *sel = NULL;
    cassert_no_null(form);
    sel = arrst_get_const(form->sel_path, row / 2, DSelect);
    cassert(col <= 1);
    cassert_no_null(sel);
    cassert_no_null(sel->dlayout);
    cassert_no_null(sel->flayout);

    /* Even rows == layout */
    if (row % 2 == 0)
    {
        if (col == 0)
            return tc(sel->flayout->name);
        else
            return "Layout";
    }
    /* Odd rows == cell */
    else
    {
        const FCell *cell = i_sel_fcell(sel);
        if (col == 0)
        {
            return tc(cell->name);
        }
        else
        {
            cassert(col == 1);
            switch(cell->type)
            {
            case ekCELL_TYPE_EMPTY:
                return "EmptyCell";
            case ekCELL_TYPE_LABEL:
                return "LabelCell";
            case ekCELL_TYPE_BUTTON:
                return "ButtonCell";
            case ekCELL_TYPE_CHECK:
                return "CheckBoxCell";
            case ekCELL_TYPE_EDIT:
                return "EditBoxCell";
            case ekCELL_TYPE_TEXT:
                return "TextViewCell";
            case ekCELL_TYPE_IMAGE:
                return "ImageViewCell";
            case ekCELL_TYPE_SLIDER:
                return "SliderCell";
            case ekCELL_TYPE_PROGRESS:
                return "ProgressCell";
            case ekCELL_TYPE_POPUP:
                return "PopUpCell";
            case ekCELL_TYPE_LISTBOX:
                return "ListBoxCell";
            case ekCELL_TYPE_LAYOUT:
                return "LayoutCell";
            cassert_default();
            }
        }
    }

    return "";
}

/*---------------------------------------------------------------------------*/

void dform_inspect_select(DForm *form, Panel *propedit, const uint32_t row)
{
    const DSelect *sel = NULL;
    cassert_no_null(form);
    sel = arrst_get_const(form->sel_path, row / 2, DSelect);

    /* Even rows == layout */
    if (row % 2 == 0)
    {
        DSelect propsel;
        propsel.dlayout = sel->dlayout;
        propsel.flayout = sel->flayout;
        propsel.glayout = sel->glayout;
        propsel.elem = ekLAYELEM_MARGIN_LEFT;
        propsel.col = UINT32_MAX;
        propsel.row = UINT32_MAX;
        form->sel = propsel;
        propedit_set(propedit, form, &propsel);
    }
    /* Odd rows == cell */
    else
    {
        cassert(sel->elem == ekLAYELEM_CELL);
        form->sel = *sel;
        propedit_set(propedit, form, sel);
    }
}

/*---------------------------------------------------------------------------*/
/* Unify with 'dialogs' code */
static void i_center_window(const Window *parent, Window *window)
{
    V2Df p1 = window_get_origin(parent);
    S2Df s1 = window_get_size(parent);
    S2Df s2 = window_get_size(window);
    V2Df p2;
    p2.x = p1.x + (s1.width - s2.width) / 2;
    p2.y = p1.y + (s1.height - s2.height) / 2;
    window_origin(window, p2);
}

/*---------------------------------------------------------------------------*/

void dform_set_need_save(DForm *form)
{
    i_need_save(form);
}

/*---------------------------------------------------------------------------*/

void dform_simulate(DForm *form, Window *window)
{
    cassert_no_null(form);
    if (form->window != NULL)
    {
        i_center_window(window, form->window);
        window_modal(form->window, window);
    }
}
