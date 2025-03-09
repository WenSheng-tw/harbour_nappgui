/* Property editor */

#include "propedit.h"
#include "designer.h"
#include "dlayout.h"
#include "dform.h"
#include "dialogs.h"
#include "res_designer.h"
#include <nflib/flayout.h>
#include <gui/gui.h>
#include <gui/cell.h>
#include <gui/comwin.h>
#include <gui/button.h>
#include <gui/edit.h>
#include <gui/label.h>
#include <gui/layout.h>
#include <gui/listbox.h>
#include <gui/panel.h>
#include <gui/popup.h>
#include <gui/view.h>
#include <gui/updown.h>
#include <draw2d/image.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <core/event.h>
#include <core/heap.h>
#include <core/strings.h>
#include <sewer/bstd.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>

typedef struct _propdata_t PropData;

struct _propdata_t
{
    DSelect sel;
    Designer *app;
    DForm *form;
    Panel *cell_panel;
    Layout *layout_layout;
    Layout *column_layout;
    Layout *row_layout;
    Layout *cell_layout;
    Layout *label_layout;
    Layout *button_layout;
    Layout *check_layout;
    Layout *edit_layout;
    Layout *text_layout;
    Layout *image_layout;
    Layout *slider_layout;
    Layout *progress_layout;
    Layout *popup_layout;
    ListBox *popup_list;
    Cell *column_margin_cell;
    Cell *row_margin_cell;
    Label *layout_geom_label;
    Label *cell_geom_label;
    PopUp *column_popup;
    PopUp *row_popup;
    Button *load_button;
};

/*---------------------------------------------------------------------------*/

static const real32_t i_GRID_HMARGIN = 5;
static const real32_t i_HEADER_VMARGIN = 3;

/*---------------------------------------------------------------------------*/

static Layout *i_no_sel_layout(void)
{
    Layout *layout = layout_create(1, 1);
    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_value_updown_layout(void)
{
    Layout *layout = layout_create(2, 1);
    Edit *edit = edit_create();
    UpDown *updown = updown_create();
    layout_edit(layout, edit, 0, 0);
    layout_updown(layout, updown, 1, 0);
    layout_hexpand(layout, 0);
    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_margin_layout(PropData *data)
{
    Layout *layout = layout_create(2, 6);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    Label *label5 = label_create();
    Label *label6 = label_create();
    Label *label7 = label_create();
    Edit *edit = edit_create();
    Layout *val1 = i_value_updown_layout();
    Layout *val2 = i_value_updown_layout();
    Layout *val3 = i_value_updown_layout();
    Layout *val4 = i_value_updown_layout();
    cassert_no_null(data);
    label_text(label1, "Geom");
    label_text(label2, "Name");
    label_text(label3, "Top");
    label_text(label4, "Left");
    label_text(label5, "Bottom");
    label_text(label6, "Right");
    layout_label(layout, label1, 0, 0);
    layout_label(layout, label2, 0, 1);
    layout_label(layout, label3, 0, 2);
    layout_label(layout, label4, 0, 3);
    layout_label(layout, label5, 0, 4);
    layout_label(layout, label6, 0, 5);
    layout_label(layout, label7, 1, 0);
    layout_edit(layout, edit, 1, 1);
    layout_layout(layout, val1, 1, 2);
    layout_layout(layout, val2, 1, 3);
    layout_layout(layout, val3, 1, 4);
    layout_layout(layout, val4, 1, 5);
    layout_halign(layout, 1, 0, ekJUSTIFY);
    layout_hexpand(layout, 1);
    layout_hmargin(layout, 0, i_GRID_HMARGIN);
    data->layout_geom_label = label7;
    cell_dbind(layout_cell(layout, 1, 1), FLayout, String *, name);
    cell_dbind(layout_cell(layout, 1, 2), FLayout, real32_t, margin_top);
    cell_dbind(layout_cell(layout, 1, 3), FLayout, real32_t, margin_left);
    cell_dbind(layout_cell(layout, 1, 4), FLayout, real32_t, margin_bottom);
    cell_dbind(layout_cell(layout, 1, 5), FLayout, real32_t, margin_right);
    return layout;
}

/*---------------------------------------------------------------------------*/

static void i_set_column_obj(PropData *data, const uint32_t col)
{
    FColumn *fcol = NULL;
    uint32_t ncols = 0;
    cassert_no_null(data);
    fcol = flayout_column(data->sel.flayout, col);
    ncols = flayout_ncols(data->sel.flayout);
    layout_dbind_obj(data->column_layout, fcol, FColumn);
    cell_enabled(data->column_margin_cell, col < ncols - 1);
}

/*---------------------------------------------------------------------------*/

static void i_set_row_obj(PropData *data, const uint32_t row)
{
    FRow *frow = NULL;
    uint32_t nrows = 0;
    cassert_no_null(data);
    frow = flayout_row(data->sel.flayout, row);
    nrows = flayout_nrows(data->sel.flayout);
    layout_dbind_obj(data->row_layout, frow, FRow);
    cell_enabled(data->row_margin_cell, row < nrows - 1);
}

/*---------------------------------------------------------------------------*/

static void i_OnColumnSelect(PropData *data, Event *e)
{
    const EvButton *p = event_params(e, EvButton);
    cassert_no_null(data);
    i_set_column_obj(data, p->index);
}

/*---------------------------------------------------------------------------*/

static void i_OnRowSelect(PropData *data, Event *e)
{
    const EvButton *p = event_params(e, EvButton);
    cassert_no_null(data);
    i_set_row_obj(data, p->index);
}

/*---------------------------------------------------------------------------*/

static Layout *i_column_layout(PropData *data)
{
    Layout *layout = layout_create(2, 3);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    PopUp *popup = popup_create();
    Layout *val1 = i_value_updown_layout();
    Layout *val2 = i_value_updown_layout();
    cassert_no_null(data);
    label_text(label1, "Column");
    label_text(label2, "CRight");
    label_text(label3, "FWidth");
    popup_OnSelect(popup, listener(data, i_OnColumnSelect, PropData));
    layout_label(layout, label1, 0, 0);
    layout_label(layout, label2, 0, 1);
    layout_label(layout, label3, 0, 2);
    layout_popup(layout, popup, 1, 0);
    layout_layout(layout, val1, 1, 1);
    layout_layout(layout, val2, 1, 2);
    layout_hexpand(layout, 1);
    layout_hmargin(layout, 0, i_GRID_HMARGIN);
    data->column_popup = popup;
    data->column_margin_cell = layout_cell(layout, 1, 1);
    cell_dbind(layout_cell(layout, 1, 1), FColumn, real32_t, margin_right);
    cell_dbind(layout_cell(layout, 1, 2), FColumn, real32_t, forced_width);
    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_row_layout(PropData *data)
{
    Layout *layout = layout_create(2, 3);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    PopUp *popup = popup_create();
    Layout *val1 = i_value_updown_layout();
    Layout *val2 = i_value_updown_layout();
    cassert_no_null(data);
    label_text(label1, "Row");
    label_text(label2, "RBottom");
    label_text(label3, "RHeight");
    popup_OnSelect(popup, listener(data, i_OnRowSelect, PropData));
    layout_label(layout, label1, 0, 0);
    layout_label(layout, label2, 0, 1);
    layout_label(layout, label3, 0, 2);
    layout_popup(layout, popup, 1, 0);
    layout_layout(layout, val1, 1, 1);
    layout_layout(layout, val2, 1, 2);
    layout_hexpand(layout, 1);
    layout_hmargin(layout, 0, i_GRID_HMARGIN);
    data->row_popup = popup;
    data->row_margin_cell = layout_cell(layout, 1, 1);
    cell_dbind(layout_cell(layout, 1, 1), FRow, real32_t, margin_bottom);
    cell_dbind(layout_cell(layout, 1, 2), FRow, real32_t, forced_height);
    return layout;
}

/*---------------------------------------------------------------------------*/

static void i_OnLayoutNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FLayout, String *, name) == TRUE)
    {
        designer_inspect_update(data->app);
    }
    else if (evbind_modify(e, FLayout, real32_t, margin_left) == TRUE || evbind_modify(e, FLayout, real32_t, margin_top) == TRUE || evbind_modify(e, FLayout, real32_t, margin_right) == TRUE || evbind_modify(e, FLayout, real32_t, margin_bottom) == TRUE)
    {
        cassert(evbind_object(e, FLayout) == data->sel.flayout);
        dform_synchro_layout_margin(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnColumnNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FColumn, real32_t, margin_right) == TRUE)
    {
        FColumn *fcol = evbind_object(e, FColumn);
        uint32_t col = popup_get_selected(data->column_popup);
        dform_synchro_column_margin(data->form, &data->sel, fcol, col);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
    else if (evbind_modify(e, FColumn, real32_t, forced_width) == TRUE)
    {
        FColumn *fcol = evbind_object(e, FColumn);
        uint32_t col = popup_get_selected(data->column_popup);
        dform_synchro_column_width(data->form, &data->sel, fcol, col);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnRowNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FRow, real32_t, margin_bottom) == TRUE)
    {
        FRow *frow = evbind_object(e, FRow);
        uint32_t row = popup_get_selected(data->row_popup);
        dform_synchro_row_margin(data->form, &data->sel, frow, row);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
    else if (evbind_modify(e, FRow, real32_t, forced_height) == TRUE)
    {
        FRow *frow = evbind_object(e, FRow);
        uint32_t row = popup_get_selected(data->row_popup);
        dform_synchro_row_height(data->form, &data->sel, frow, row);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_layout_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 5);
    Layout *layout2 = i_margin_layout(data);
    Layout *layout3 = i_column_layout(data);
    Layout *layout4 = i_row_layout(data);
    Label *label = label_create();
    cassert_no_null(data);
    label_text(label, "Layout properties");
    layout_label(layout1, label, 0, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_layout(layout1, layout3, 0, 2);
    layout_layout(layout1, layout4, 0, 3);
    layout_vmargin(layout1, 0, i_HEADER_VMARGIN);
    layout_vexpand(layout1, 4);
    layout_dbind(layout2, listener(data, i_OnLayoutNotify, PropData), FLayout);
    layout_dbind(layout3, listener(data, i_OnColumnNotify, PropData), FColumn);
    layout_dbind(layout4, listener(data, i_OnRowNotify, PropData), FRow);
    data->layout_layout = layout2;
    data->column_layout = layout3;
    data->row_layout = layout4;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static Layout *i_empty_cell_layout(void)
{
    Layout *layout = layout_create(1, 1);
    Label *label = label_create();
    label_text(label, "Empty cell");
    layout_label(layout, label, 0, 0);
    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_layout_cell_layout(void)
{
    Layout *layout = layout_create(1, 1);
    Label *label = label_create();
    label_text(label, "Layout cell");
    layout_label(layout, label, 0, 0);
    return layout;
}

/*---------------------------------------------------------------------------*/

static void i_OnLabelNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FLabel, String *, text) == TRUE)
    {
        dform_synchro_cell_text(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
    else if (evbind_modify(e, FLabel, bool_t, multiline) == TRUE 
        || evbind_modify(e, FLabel, real32_t, min_width) == TRUE 
        || evbind_modify(e, FLabel, halign_t, align) == TRUE)
    {
        dform_synchro_label(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_label_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 4);
    Layout *layout3 = i_value_updown_layout();
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    Label *label5 = label_create();
    Edit *edit = edit_create();
    Button *check = button_check();
    PopUp *popup = popup_create();
    cassert_no_null(data);
    label_text(label1, "Label properties");
    label_text(label2, "Text");
    label_text(label3, "Multiline");
    label_text(label4, "MWidth");
    label_text(label5, "Align");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_label(layout2, label3, 0, 1);
    layout_label(layout2, label4, 0, 2);
    layout_label(layout2, label5, 0, 3);
    layout_edit(layout2, edit, 1, 0);
    layout_button(layout2, check, 1, 1);
    layout_layout(layout2, layout3, 1, 2);
    layout_popup(layout2, popup, 1, 3);
    layout_layout(layout1, layout2, 0, 1);
    layout_vmargin(layout1, 0, i_HEADER_VMARGIN);
    layout_hmargin(layout2, 0, i_GRID_HMARGIN);
    layout_hexpand(layout2, 1);
    layout_vexpand(layout1, 2);
    cell_dbind(layout_cell(layout2, 1, 0), FLabel, String *, text);
    cell_dbind(layout_cell(layout2, 1, 1), FLabel, bool_t, multiline);
    cell_dbind(layout_cell(layout2, 1, 2), FLabel, real32_t, min_width);
    cell_dbind(layout_cell(layout2, 1, 3), FLabel, halign_t, align);
    layout_dbind(layout1, listener(data, i_OnLabelNotify, PropData), FLabel);
    data->label_layout = layout1;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_OnButtonNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FButton, String *, text) == TRUE)
    {
        dform_synchro_cell_text(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
    else if (evbind_modify(e, FButton, real32_t, min_width) == TRUE)
    {
        dform_synchro_button(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_button_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 2);
    Layout *layout3 = i_value_updown_layout();
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Edit *edit = edit_create();
    cassert_no_null(data);
    label_text(label1, "Button properties");
    label_text(label2, "Text");
    label_text(label3, "MWidth");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_label(layout2, label3, 0, 1);
    layout_edit(layout2, edit, 1, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_layout(layout2, layout3, 1, 1);
    layout_vmargin(layout1, 0, i_HEADER_VMARGIN);
    layout_hmargin(layout2, 0, i_GRID_HMARGIN);
    layout_hexpand(layout2, 1);
    layout_vexpand(layout1, 2);
    cell_dbind(layout_cell(layout2, 1, 0), FButton, String *, text);
    cell_dbind(layout_cell(layout2, 1, 1), FButton, real32_t, min_width);
    layout_dbind(layout1, listener(data, i_OnButtonNotify, PropData), FButton);
    data->button_layout = layout1;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_OnCheckNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FCheck, String *, text) == TRUE)
    {
        dform_synchro_cell_text(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_check_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 1);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Edit *edit = edit_create();
    cassert_no_null(data);
    label_text(label1, "CheckBox properties");
    label_text(label2, "Text");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_edit(layout2, edit, 1, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_vmargin(layout1, 0, i_HEADER_VMARGIN);
    layout_hmargin(layout2, 0, i_GRID_HMARGIN);
    layout_hexpand(layout2, 1);
    layout_vexpand(layout1, 2);
    cell_dbind(layout_cell(layout2, 1, 0), FCheck, String *, text);
    layout_dbind(layout1, listener(data, i_OnCheckNotify, PropData), FCheck);
    data->check_layout = layout1;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_OnEditNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FEdit, bool_t, passmode) == TRUE 
        || evbind_modify(e, FEdit, bool_t, autosel) == TRUE 
        || evbind_modify(e, FEdit, halign_t, text_align) == TRUE
        || evbind_modify(e, FEdit, real32_t, min_width) == TRUE)
    {
        dform_synchro_edit(data->form, &data->sel);

        if (evbind_modify(e, FEdit, real32_t, min_width) == TRUE)
        {
            dform_compose(data->form);
            designer_canvas_update(data->app);
        }
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_edit_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 5);
    Layout *layout2 = layout_create(2, 2);
    Layout *layout3 = i_value_updown_layout();
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Button *button1 = button_check();
    Button *button2 = button_check();
    PopUp *popup = popup_create();
    cassert_no_null(data);
    label_text(label1, "EditBox properties");
    label_text(label2, "Text align");
    label_text(label3, "MWidth");
    button_text(button1, "Passmode");
    button_text(button2, "Autosel");
    layout_label(layout1, label1, 0, 0);
    layout_button(layout1, button1, 0, 1);
    layout_button(layout1, button2, 0, 2);
    layout_label(layout2, label2, 0, 0);
    layout_label(layout2, label3, 0, 1);
    layout_popup(layout2, popup, 1, 0);
    layout_layout(layout1, layout2, 0, 3);
    layout_layout(layout2, layout3, 1, 1);
    layout_vmargin(layout1, 0, i_HEADER_VMARGIN);
    layout_hmargin(layout2, 0, i_GRID_HMARGIN);
    layout_hexpand(layout2, 1);
    layout_vexpand(layout1, 4);
    cell_dbind(layout_cell(layout1, 0, 1), FEdit, bool_t, passmode);
    cell_dbind(layout_cell(layout1, 0, 2), FEdit, bool_t, autosel);
    cell_dbind(layout_cell(layout2, 1, 0), FEdit, halign_t, text_align);
    cell_dbind(layout_cell(layout2, 1, 1), FEdit, real32_t, min_width);
    layout_dbind(layout1, listener(data, i_OnEditNotify, PropData), FEdit);
    data->edit_layout = layout1;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_OnTextNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FText, bool_t, read_only) == TRUE
        || evbind_modify(e, FText, real32_t, min_width) == TRUE
        || evbind_modify(e, FText, real32_t, min_height) == TRUE)
    {
        dform_synchro_textview(data->form, &data->sel);

        if (evbind_modify(e, FText, real32_t, min_width) == TRUE
            || evbind_modify(e, FText, real32_t, min_height) == TRUE)
        {
            dform_compose(data->form);
            designer_canvas_update(data->app);
        }
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_text_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 4);
    Layout *layout2 = layout_create(2, 2);
    Layout *layout3 = i_value_updown_layout();
    Layout *layout4 = i_value_updown_layout();
    Button *button1 = button_check();
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    cassert_no_null(data);
    label_text(label1, "TextView properties");
    label_text(label2, "MWidth");
    label_text(label3, "MHeight");
    button_text(button1, "Read only");
    layout_label(layout1, label1, 0, 0);
    layout_button(layout1, button1, 0, 1);
    layout_label(layout2, label2, 0, 0);
    layout_label(layout2, label3, 0, 1);
    layout_layout(layout2, layout3, 1, 0);
    layout_layout(layout2, layout4, 1, 1);
    layout_layout(layout1, layout2, 0, 2);
    layout_vmargin(layout1, 0, i_HEADER_VMARGIN);
    layout_hmargin(layout2, 0, i_GRID_HMARGIN);
    layout_hexpand(layout2, 1);
    layout_vexpand(layout1, 3);
    cell_dbind(layout_cell(layout1, 0, 1), FText, bool_t, read_only);
    cell_dbind(layout_cell(layout2, 1, 0), FText, real32_t, min_width);
    cell_dbind(layout_cell(layout2, 1, 1), FText, real32_t, min_height);
    layout_dbind(layout1, listener(data, i_OnTextNotify, PropData), FText);
    data->text_layout = layout1;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_OnImageNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FImage, scale_t, scale) == TRUE
        || evbind_modify(e, FImage, real32_t, min_width) == TRUE
        || evbind_modify(e, FImage, real32_t, min_height) == TRUE)
    {
        dform_synchro_imageview(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnLoadImage(PropData *data, Event *e)
{
    const char_t *imgpath = NULL;
    const char_t *folder_path = NULL;
    Window *window = NULL;
    cassert_no_null(data);
    unref(e);
    folder_path = designer_folder_path(data->app);
    window = designer_main_window(data->app);
    imgpath = comwin_open_file(window, NULL, 0, folder_path);
	if (imgpath != NULL)
	{
        Image *image = image_from_file(imgpath, NULL);
        if (image != NULL)
        {
            String *relpath = str_relpath(ekLINUX, folder_path, imgpath); 
            dform_synchro_cell_image(data->form, &data->sel, image, tc(relpath));
            dlayout_set_image(data->sel.dlayout, image, data->sel.col, data->sel.row);
            button_tooltip(data->load_button, tc(relpath));
            dform_compose(data->form);
            designer_canvas_update(data->app);
            str_destroy(&relpath);
            image_destroy(&image);
        }
	}
}

/*---------------------------------------------------------------------------*/

static Layout *i_image_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 4);
    Layout *layout3 = i_value_updown_layout();
    Layout *layout4 = i_value_updown_layout();
    PopUp *popup = popup_create();
    Button *button = button_push();
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    Label *label5 = label_create();
    cassert_no_null(data);
    label_text(label1, "ImageView properties");
    label_text(label2, "MWidth");
    label_text(label3, "MHeight");
    label_text(label4, "Scale");
    label_text(label5, "Image");
    button_text(button, "Load");
    button_OnClick(button, listener(data, i_OnLoadImage, PropData));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_label(layout2, label3, 0, 1);
    layout_label(layout2, label4, 0, 2);
    layout_label(layout2, label5, 0, 3);
    layout_layout(layout2, layout3, 1, 0);
    layout_layout(layout2, layout4, 1, 1);
    layout_popup(layout2, popup, 1, 2);
    layout_button(layout2, button, 1, 3);
    layout_layout(layout1, layout2, 0, 1);
    layout_vmargin(layout1, 0, i_HEADER_VMARGIN);
    layout_hmargin(layout2, 0, i_GRID_HMARGIN);
    layout_hexpand(layout2, 1);
    layout_vexpand(layout1, 2);
    cell_dbind(layout_cell(layout2, 1, 0), FImage, real32_t, min_width);
    cell_dbind(layout_cell(layout2, 1, 1), FImage, real32_t, min_height);
    cell_dbind(layout_cell(layout2, 1, 2), FImage, scale_t, scale);
    layout_dbind(layout1, listener(data, i_OnImageNotify, PropData), FImage);
    data->image_layout = layout1;
    data->load_button = button;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_OnSliderNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FSlider, real32_t, min_width) == TRUE)
    {
        dform_synchro_slider(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_slider_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 1);
    Layout *layout3 = i_value_updown_layout();
    Label *label1 = label_create();
    Label *label2 = label_create();
    cassert_no_null(data);
    label_text(label1, "Slider properties");
    label_text(label2, "MWidth");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_layout(layout2, layout3, 1, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_vmargin(layout1, 0, i_HEADER_VMARGIN);
    layout_hmargin(layout2, 0, i_GRID_HMARGIN);
    layout_hexpand(layout2, 1);
    layout_vexpand(layout1, 2);
    cell_dbind(layout_cell(layout2, 1, 0), FSlider, real32_t, min_width);
    layout_dbind(layout1, listener(data, i_OnSliderNotify, PropData), FSlider);
    data->slider_layout = layout1;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_OnProgressNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FProgress, real32_t, min_width) == TRUE)
    {
        dform_synchro_progress(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_progress_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 1);
    Layout *layout3 = i_value_updown_layout();
    Label *label1 = label_create();
    Label *label2 = label_create();
    cassert_no_null(data);
    label_text(label1, "Progress properties");
    label_text(label2, "MWidth");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_layout(layout2, layout3, 1, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_vmargin(layout1, 0, i_HEADER_VMARGIN);
    layout_hmargin(layout2, 0, i_GRID_HMARGIN);
    layout_hexpand(layout2, 1);
    layout_vexpand(layout1, 2);
    cell_dbind(layout_cell(layout2, 1, 0), FProgress, real32_t, min_width);
    layout_dbind(layout1, listener(data, i_OnProgressNotify, PropData), FProgress);
    data->progress_layout = layout1;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_OnPopUpAdd(PropData *data, Event *e)
{
    Window *window = NULL;
    const char_t *folder_path = NULL;
    FElem *elem = NULL;
    cassert_no_null(data);
    window = designer_main_window(data->app);
    folder_path = designer_folder_path(data->app);
    elem = dialog_new_elem(window, folder_path);
    if (elem != NULL)
    {
        FPopUp *fpopup = layout_dbind_get_obj(data->popup_layout, FPopUp);
        FElem *nelem = arrst_new(fpopup->elems, FElem);
        String *path = NULL;
        Image *image = NULL;
        nelem->text = str_copy(elem->text);
        nelem->iconpath = str_copy(elem->iconpath);
        path = str_cpath("%s/%s", folder_path, tc(elem->iconpath));
        image = image_from_file(tc(path), NULL);
        listbox_add_elem(data->popup_list, tc(nelem->text), image);
        dform_synchro_popup_add(data->form, &data->sel, image);
        dform_compose(data->form);
        designer_canvas_update(data->app);
        str_destroy(&path);
        ptr_destopt(image_destroy, &image, Image);
        dbind_destroy(&elem, FElem);
    }
    unref(e);
}

/*---------------------------------------------------------------------------*/

static Layout *i_popup_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 5);
    Layout *layout2 = layout_create(2, 1);
    Label *label1 = label_create();
    Label *label2 = label_create();
    ListBox *list = listbox_create();
    Button *button1 = button_flat();
    Button *button2 = button_flat();
    cassert_no_null(data);
    label_text(label1, "PopUp properties");
    label_text(label2, "Elements");
    button_image(button1, cast_const(PLUS16_PNG, Image));
    button_image(button2, cast_const(ERROR16_PNG, Image));
    button_OnClick(button1, listener(data, i_OnPopUpAdd, PropData));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_listbox(layout1, list, 0, 2);
    layout_button(layout2, button1, 0, 0);
    layout_button(layout2, button2, 1, 0);
    layout_layout(layout1, layout2, 0, 3);
    layout_halign(layout1, 0, 3, ekLEFT);
    layout_vmargin(layout1, 0, i_HEADER_VMARGIN);
    layout_vexpand(layout1, 4);
    //cell_dbind(layout_cell(layout2, 1, 0), FProgress, real32_t, min_width);
    //layout_dbind(layout1, listener(data, i_OnProgressNotify, PropData), FProgress);
    layout_dbind(layout1, /*listener(data, i_OnProgressNotify, PropData)*/NULL, FPopUp);
    data->popup_layout = layout1;
    data->popup_list = list;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_OnCellNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FCell, String *, name) == TRUE)
    {
        designer_inspect_update(data->app);
        dform_set_need_save(data->form);
    }
    else if (evbind_modify(e, FCell, halign_t, halign) == TRUE)
    {
        FCell *fcell = evbind_object(e, FCell);
        dform_synchro_cell_halign(data->form, &data->sel, fcell, data->sel.col, data->sel.row);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
    else if (evbind_modify(e, FCell, valign_t, valign) == TRUE)
    {
        FCell *fcell = evbind_object(e, FCell);
        dform_synchro_cell_valign(data->form, &data->sel, fcell, data->sel.col, data->sel.row);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_cell_props_layout(PropData *data)
{
    Layout *layout = layout_create(2, 4);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    Label *label5 = label_create();
    Edit *edit = edit_create();
    PopUp *popup1 = popup_create();
    PopUp *popup2 = popup_create();
    cassert_no_null(data);
    label_text(label1, "Coord");
    label_text(label2, "Name");
    label_text(label3, "HAlign");
    label_text(label4, "VAlign");
    layout_label(layout, label1, 0, 0);
    layout_label(layout, label2, 0, 1);
    layout_label(layout, label3, 0, 2);
    layout_label(layout, label4, 0, 3);
    layout_label(layout, label5, 1, 0);
    layout_edit(layout, edit, 1, 1);
    layout_popup(layout, popup1, 1, 2);
    layout_popup(layout, popup2, 1, 3);
    layout_halign(layout, 1, 0, ekJUSTIFY);
    layout_hmargin(layout, 0, i_GRID_HMARGIN);
    layout_hexpand(layout, 1);
    data->cell_geom_label = label5;
    cell_dbind(layout_cell(layout, 1, 1), FCell, String *, name);
    cell_dbind(layout_cell(layout, 1, 2), FCell, halign_t, halign);
    cell_dbind(layout_cell(layout, 1, 3), FCell, valign_t, valign);
    return layout;
}

/*---------------------------------------------------------------------------*/

static Panel *i_cell_content_panel(PropData *data)
{
    Layout *layout1 = i_empty_cell_layout();
    Layout *layout2 = i_layout_cell_layout();
    Layout *layout3 = i_label_layout(data);
    Layout *layout4 = i_button_layout(data);
    Layout *layout5 = i_check_layout(data);
    Layout *layout6 = i_edit_layout(data);
    Layout *layout7 = i_text_layout(data);
    Layout *layout8 = i_image_layout(data);
    Layout *layout9 = i_slider_layout(data);
    Layout *layout10 = i_progress_layout(data);
    Layout *layout11 = i_popup_layout(data);
    Panel *panel = panel_create();
    cassert_no_null(data);
    panel_layout(panel, layout1);
    panel_layout(panel, layout2);
    panel_layout(panel, layout3);
    panel_layout(panel, layout4);
    panel_layout(panel, layout5);
    panel_layout(panel, layout6);
    panel_layout(panel, layout7);
    panel_layout(panel, layout8);
    panel_layout(panel, layout9);
    panel_layout(panel, layout10);
    panel_layout(panel, layout11);
    panel_visible_layout(panel, 0);
    data->cell_panel = panel;
    return panel;
}

/*---------------------------------------------------------------------------*/

static Layout *i_cell_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 4);
    Layout *layout2 = i_cell_props_layout(data);
    Label *label = label_create();
    Panel *panel = i_cell_content_panel(data);
    label_text(label, "Cell properties");
    layout_label(layout1, label, 0, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_panel(layout1, panel, 0, 2);
    layout_vmargin(layout1, 0, i_HEADER_VMARGIN);
    layout_vmargin(layout1, 1, i_HEADER_VMARGIN);
    layout_vexpand(layout1, 3);
    layout_dbind(layout1, listener(data, i_OnCellNotify, PropData), FCell);
    data->cell_layout = layout1;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static PropData *i_data(Designer *app)
{
    PropData *data = heap_new0(PropData);
    data->app = app;
    return data;
}

/*---------------------------------------------------------------------------*/

static void i_destroy_data(PropData **data)
{
    heap_delete(data, PropData);
}

/*---------------------------------------------------------------------------*/

Panel *propedit_create(Designer *app)
{
    PropData *data = i_data(app);
    Layout *layout0 = i_no_sel_layout();
    Layout *layout1 = i_layout_layout(data);
    Layout *layout2 = i_cell_layout(data);
    Panel *panel = panel_create();
    panel_layout(panel, layout0);
    panel_layout(panel, layout1);
    panel_layout(panel, layout2);
    panel_data(panel, &data, i_destroy_data, PropData);
    panel_visible_layout(panel, 0);
    return panel;
}

/*---------------------------------------------------------------------------*/

void propedit_set(Panel *panel, DForm *form, const DSelect *sel)
{
    PropData *data = panel_get_data(panel, PropData);
    cassert_no_null(sel);
    data->form = form;
    data->sel = *sel;
    /* i_no_sel_layout */
    if (sel->flayout == NULL)
    {
        panel_visible_layout(panel, 0);
    }
    /* i_layout_layout */
    else if (sel->elem != ekLAYELEM_CELL)
    {
        char_t text[64];
        uint32_t ncols = flayout_ncols(sel->flayout);
        uint32_t nrows = flayout_nrows(sel->flayout);
        bstd_sprintf(text, sizeof(text), "%d cols x %d rows", ncols, nrows);
        label_text(data->layout_geom_label, text);

        /* Column selector */
        {
            uint32_t i, col = 0;
            popup_clear(data->column_popup);

            for (i = 0; i < ncols; ++i)
            {
                bstd_sprintf(text, sizeof(text), "%d", i);
                popup_add_elem(data->column_popup, text, NULL);
            }

            if (sel->elem == ekLAYELEM_MARGIN_COLUMN)
                col = sel->col;

            popup_selected(data->column_popup, col);
            i_set_column_obj(data, col);
        }

        /* Row selector */
        {
            uint32_t j, row = 0;
            popup_clear(data->row_popup);

            for (j = 0; j < nrows; ++j)
            {
                bstd_sprintf(text, sizeof(text), "%d", j);
                popup_add_elem(data->row_popup, text, NULL);
            }

            if (sel->elem == ekLAYELEM_MARGIN_ROW)
                row = sel->row;

            popup_selected(data->row_popup, row);
            i_set_row_obj(data, row);
        }

        layout_dbind_obj(data->layout_layout, sel->flayout, FLayout);
        panel_visible_layout(panel, 1);
    }
    /* i_cell_layout */
    else
    {
        char_t text[64];
        FCell *cell = dform_sel_fcell(sel);
        bstd_sprintf(text, sizeof(text), "(%d,%d)", sel->col, sel->row);
        label_text(data->cell_geom_label, text);
        panel_visible_layout(panel, 2);
        layout_dbind_obj(data->cell_layout, cell, FCell);
        if (cell->type == ekCELL_TYPE_EMPTY)
        {
            panel_visible_layout(data->cell_panel, 0);
        }
        else if (cell->type == ekCELL_TYPE_LAYOUT)
        {
            panel_visible_layout(data->cell_panel, 1);
        }
        else if (cell->type == ekCELL_TYPE_LABEL)
        {
            layout_dbind_obj(data->label_layout, cell->widget.label, FLabel);
            panel_visible_layout(data->cell_panel, 2);
        }
        else if (cell->type == ekCELL_TYPE_BUTTON)
        {
            layout_dbind_obj(data->button_layout, cell->widget.button, FButton);
            panel_visible_layout(data->cell_panel, 3);
        }
        else if (cell->type == ekCELL_TYPE_CHECK)
        {
            layout_dbind_obj(data->check_layout, cell->widget.check, FCheck);
            panel_visible_layout(data->cell_panel, 4);
        }
        else if (cell->type == ekCELL_TYPE_EDIT)
        {
            layout_dbind_obj(data->edit_layout, cell->widget.edit, FEdit);
            panel_visible_layout(data->cell_panel, 5);
        }
        else if (cell->type == ekCELL_TYPE_TEXT)
        {
            layout_dbind_obj(data->text_layout, cell->widget.text, FText);
            panel_visible_layout(data->cell_panel, 6);
        }
		else if (cell->type == ekCELL_TYPE_IMAGE)
		{
            layout_dbind_obj(data->image_layout, cell->widget.image, FImage);
            panel_visible_layout(data->cell_panel, 7);
            button_tooltip(data->load_button, tc(cell->widget.image->path));
		}
        else if (cell->type == ekCELL_TYPE_SLIDER)
        {
            layout_dbind_obj(data->slider_layout, cell->widget.slider, FSlider);
            panel_visible_layout(data->cell_panel, 8);
        }  
        else if (cell->type == ekCELL_TYPE_PROGRESS)
        {
            layout_dbind_obj(data->progress_layout, cell->widget.progress, FProgress);
            panel_visible_layout(data->cell_panel, 9);
        }
        else if (cell->type == ekCELL_TYPE_POPUP)
        {
            layout_dbind_obj(data->popup_layout, cell->widget.popup, FPopUp);
            panel_visible_layout(data->cell_panel, 10);
        }
        else
        {
            cassert(FALSE);
            panel_visible_layout(data->cell_panel, 0);
        }

        panel_update(data->cell_panel);
    }

    panel_update(panel);
}
