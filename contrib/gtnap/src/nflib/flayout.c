/* Form layout */

#include "flayout.h"
#include "nflib.h"
#include <gui/button.h>
#include <gui/cell.h>
#include <gui/label.h>
#include <gui/layout.h>
#include <gui/layouth.h>
#include <gui/edit.h>
#include <gui/textview.h>
#include <gui/imageview.h>
#include <gui/slider.h>
#include <gui/progress.h>
#include <draw2d/image.h>
#include <geom2d/s2d.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <core/heap.h>
#include <core/stream.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

static uint16_t i_VERSION = 3;
static uint16_t i_STM_VERSION = 0;

/*---------------------------------------------------------------------------*/

static void i_remove_column(FColumn *column)
{
    dbind_remove(column, FColumn);
}

/*---------------------------------------------------------------------------*/

static void i_remove_row(FRow *row)
{
    dbind_remove(row, FRow);
}

/*---------------------------------------------------------------------------*/

static void i_remove_cell(FCell *cell)
{
    dbind_remove(cell, FCell);
}

/*---------------------------------------------------------------------------*/

FLayout *flayout_create(const uint32_t ncols, const uint32_t nrows)
{
    FLayout *layout = dbind_create(FLayout);
    cassert(ncols > 0);
    cassert(nrows > 0);

    /* Add columns to layout */
    {
        uint32_t i = 0;
        for (i = 0; i < ncols; ++i)
        {
            FColumn *col = arrst_new(layout->cols, FColumn);
            dbind_init(col, FColumn);
        }
    }

    /* Add rows to layout */
    {
        uint32_t i = 0;
        for (i = 0; i < nrows; ++i)
        {
            FRow *row = arrst_new(layout->rows, FRow);
            dbind_init(row, FRow);
        }
    }

    /* Add cells to layout */
    {
        uint32_t n = ncols * nrows, i = 0;
        for (i = 0; i < n; ++i)
        {
            FCell *cell = arrst_new(layout->cells, FCell);
            dbind_init(cell, FCell);
        }
    }

    return layout;
}

/*---------------------------------------------------------------------------*/

static void i_read_col(Stream *stm, FColumn *col)
{
    cassert_no_null(col);
    col->forced_width = stm_read_r32(stm);
    col->margin_right = stm_read_r32(stm);
}

/*---------------------------------------------------------------------------*/

static void i_read_row(Stream *stm, FRow *row)
{
    cassert_no_null(row);
    row->forced_height = stm_read_r32(stm);
    row->margin_bottom = stm_read_r32(stm);
}

/*---------------------------------------------------------------------------*/

static FLabel *i_read_label(Stream *stm)
{
    FLabel *label = heap_new0(FLabel);
    label->text = str_read(stm);
    if (i_STM_VERSION >= 3)
    {
        label->multiline = stm_read_bool(stm);
        label->min_width = stm_read_r32(stm);
        label->align = stm_read_enum(stm, halign_t);
    }
    else
    {
        label->multiline = FALSE;
        label->min_width = 0;
        label->align = ekHALIGN_LEFT;
    }
    return label;
}

/*---------------------------------------------------------------------------*/

static FButton *i_read_button(Stream *stm)
{
    FButton *button = heap_new0(FButton);
    button->text = str_read(stm);
    if (i_STM_VERSION >= 1)
        button->min_width = stm_read_r32(stm);
    else
        button->min_width = 0;
    return button;
}

/*---------------------------------------------------------------------------*/

static FCheck *i_read_check(Stream *stm)
{
    FCheck *check = heap_new0(FCheck);
    check->text = str_read(stm);
    return check;
}

/*---------------------------------------------------------------------------*/

static FEdit *i_read_edit(Stream *stm)
{
    FEdit *edit = heap_new0(FEdit);
    edit->passmode = stm_read_bool(stm);
    edit->autosel = stm_read_bool(stm);
    edit->text_align = stm_read_enum(stm, halign_t);

    if (i_STM_VERSION >= 2)
        edit->min_width = stm_read_r32(stm);
    else
        edit->min_width = 100;

    return edit;
}

/*---------------------------------------------------------------------------*/

static FText *i_read_text(Stream *stm)
{
    FText *text = heap_new0(FText);
    text->read_only = stm_read_bool(stm);
    text->min_width = stm_read_r32(stm);
    text->min_height = stm_read_r32(stm);
    return text;
}

/*---------------------------------------------------------------------------*/

static FImage *i_read_image(Stream *stm)
{
    FImage *image = heap_new0(FImage);
    image->path = str_read(stm);
    image->scale = stm_read_enum(stm, scale_t);
    image->min_width = stm_read_r32(stm);
    image->min_height = stm_read_r32(stm);
    return image;
}

/*---------------------------------------------------------------------------*/

static FSlider *i_read_slider(Stream *stm)
{
    FSlider *slider = heap_new0(FSlider);
    slider->min_width = stm_read_r32(stm);
    return slider;
}

/*---------------------------------------------------------------------------*/

static FProgress *i_read_progress(Stream *stm)
{
    FProgress *progress = heap_new0(FProgress);
    progress->min_width = stm_read_r32(stm);
    return progress;
}

/*---------------------------------------------------------------------------*/

static void i_read_cell(Stream *stm, FCell *cell)
{
    cassert_no_null(cell);
    cell->name = str_read(stm);
    cell->type = stm_read_enum(stm, celltype_t);
    cell->halign = stm_read_enum(stm, halign_t);
    cell->valign = stm_read_enum(stm, valign_t);
    switch (cell->type)
    {
    case ekCELL_TYPE_EMPTY:
        break;
    case ekCELL_TYPE_LABEL:
        cell->widget.label = i_read_label(stm);
        break;
    case ekCELL_TYPE_BUTTON:
        cell->widget.button = i_read_button(stm);
        break;
    case ekCELL_TYPE_CHECK:
        cell->widget.check = i_read_check(stm);
        break;
    case ekCELL_TYPE_EDIT:
        cell->widget.edit = i_read_edit(stm);
        break;
    case ekCELL_TYPE_TEXT:
        cell->widget.text = i_read_text(stm);
        break;
    case ekCELL_TYPE_IMAGE:
        cell->widget.image = i_read_image(stm);
        break;
    case ekCELL_TYPE_SLIDER:
        cell->widget.slider = i_read_slider(stm);
        break;
    case ekCELL_TYPE_PROGRESS:
        cell->widget.progress = i_read_progress(stm);
        break;
	case ekCELL_TYPE_LAYOUT:
        cell->widget.layout = flayout_read(stm);
        break;
        cassert_default();
    }
}

/*---------------------------------------------------------------------------*/

FLayout *flayout_read(Stream *stm)
{
    i_STM_VERSION = stm_read_u16(stm);
    if (i_STM_VERSION <= i_VERSION)
    {
        FLayout *layout = heap_new0(FLayout);
        layout->name = str_read(stm);
        layout->margin_left = stm_read_r32(stm);
        layout->margin_top = stm_read_r32(stm);
        layout->margin_right = stm_read_r32(stm);
        layout->margin_bottom = stm_read_r32(stm);
        layout->cols = arrst_read(stm, i_read_col, FColumn);
        layout->rows = arrst_read(stm, i_read_row, FRow);
        layout->cells = arrst_read(stm, i_read_cell, FCell);
        return layout;
    }
    else
    {
        stm_corrupt(stm);
        return NULL;
    }
}

/*---------------------------------------------------------------------------*/

void flayout_destroy(FLayout **layout)
{
    cassert_no_null(layout);
    str_destroy(&(*layout)->name);
    arrst_destroy(&(*layout)->cols, i_remove_column, FColumn);
    arrst_destroy(&(*layout)->rows, i_remove_row, FRow);
    arrst_destroy(&(*layout)->cells, i_remove_cell, FCell);
    heap_delete(layout, FLayout);
    /* TODO: Change by */
    /* dbind_destroy(layout, FLayout); */
}

/*---------------------------------------------------------------------------*/

static void i_write_col(Stream *stm, const FColumn *col)
{
    cassert_no_null(col);
    stm_write_r32(stm, col->forced_width);
    stm_write_r32(stm, col->margin_right);
}

/*---------------------------------------------------------------------------*/

static void i_write_row(Stream *stm, const FRow *row)
{
    cassert_no_null(row);
    stm_write_r32(stm, row->forced_height);
    stm_write_r32(stm, row->margin_bottom);
}

/*---------------------------------------------------------------------------*/

static void i_write_label(Stream *stm, const FLabel *label)
{
    cassert_no_null(label);
    str_write(stm, label->text);
    stm_write_bool(stm, label->multiline);
    stm_write_r32(stm, label->min_width);
    stm_write_enum(stm, label->align, halign_t);
}

/*---------------------------------------------------------------------------*/

static void i_write_buttom(Stream *stm, const FButton *button)
{
    cassert_no_null(button);
    str_write(stm, button->text);
    stm_write_r32(stm, button->min_width);
}

/*---------------------------------------------------------------------------*/

static void i_write_check(Stream *stm, const FCheck *check)
{
    cassert_no_null(check);
    str_write(stm, check->text);
}

/*---------------------------------------------------------------------------*/

static void i_write_edit(Stream *stm, const FEdit *edit)
{
    cassert_no_null(edit);
    stm_write_bool(stm, edit->passmode);
    stm_write_bool(stm, edit->autosel);
    stm_write_enum(stm, edit->text_align, halign_t);
    stm_write_r32(stm, edit->min_width);
}

/*---------------------------------------------------------------------------*/

static void i_write_text(Stream *stm, const FText *text)
{
    cassert_no_null(text);
    stm_write_bool(stm, text->read_only);
    stm_write_r32(stm, text->min_width);
    stm_write_r32(stm, text->min_height);
}

/*---------------------------------------------------------------------------*/

static void i_write_image(Stream *stm, const FImage *image)
{
    cassert_no_null(image);
    str_write(stm, image->path);
    stm_write_enum(stm, image->scale, scale_t);
    stm_write_r32(stm, image->min_width);
    stm_write_r32(stm, image->min_height);
}

/*---------------------------------------------------------------------------*/

static void i_write_slider(Stream *stm, const FSlider *slider)
{
    cassert_no_null(slider);
    stm_write_r32(stm, slider->min_width);
}

/*---------------------------------------------------------------------------*/

static void i_write_progress(Stream *stm, const FProgress *progress)
{
    cassert_no_null(progress);
    stm_write_r32(stm, progress->min_width);
}

/*---------------------------------------------------------------------------*/

static void i_write_cell(Stream *stm, const FCell *cell)
{
    cassert_no_null(cell);
    str_write(stm, cell->name);
    stm_write_enum(stm, cell->type, celltype_t);
    stm_write_enum(stm, cell->halign, halign_t);
    stm_write_enum(stm, cell->valign, valign_t);
    switch (cell->type)
    {
    case ekCELL_TYPE_EMPTY:
        break;
    case ekCELL_TYPE_LABEL:
        i_write_label(stm, cell->widget.label);
        break;
    case ekCELL_TYPE_BUTTON:
        i_write_buttom(stm, cell->widget.button);
        break;
    case ekCELL_TYPE_CHECK:
        i_write_check(stm, cell->widget.check);
        break;
    case ekCELL_TYPE_EDIT:
        i_write_edit(stm, cell->widget.edit);
        break;
    case ekCELL_TYPE_TEXT:
        i_write_text(stm, cell->widget.text);
        break;
    case ekCELL_TYPE_IMAGE:
		i_write_image(stm, cell->widget.image);
        break;
    case ekCELL_TYPE_SLIDER:
		i_write_slider(stm, cell->widget.slider);
        break;
    case ekCELL_TYPE_PROGRESS:
		i_write_progress(stm, cell->widget.progress);
        break;
    case ekCELL_TYPE_LAYOUT:
        flayout_write(stm, cell->widget.layout);
        break;
        cassert_default();
    }
}

/*---------------------------------------------------------------------------*/

void flayout_write(Stream *stm, const FLayout *layout)
{
    cassert_no_null(layout);
    stm_write_u16(stm, i_VERSION);
    str_write(stm, layout->name);
    stm_write_r32(stm, layout->margin_left);
    stm_write_r32(stm, layout->margin_top);
    stm_write_r32(stm, layout->margin_right);
    stm_write_r32(stm, layout->margin_bottom);
    arrst_write(stm, layout->cols, i_write_col, FColumn);
    arrst_write(stm, layout->rows, i_write_row, FRow);
    arrst_write(stm, layout->cells, i_write_cell, FCell);
}

/*---------------------------------------------------------------------------*/

void flayout_margin_left(FLayout *layout, const real32_t margin)
{
    cassert_no_null(layout);
    layout->margin_left = margin;
}

/*---------------------------------------------------------------------------*/

void flayout_margin_top(FLayout *layout, const real32_t margin)
{
    cassert_no_null(layout);
    layout->margin_top = margin;
}

/*---------------------------------------------------------------------------*/

void flayout_margin_right(FLayout *layout, const real32_t margin)
{
    cassert_no_null(layout);
    layout->margin_right = margin;
}

/*---------------------------------------------------------------------------*/

void flayout_margin_bottom(FLayout *layout, const real32_t margin)
{
    cassert_no_null(layout);
    layout->margin_bottom = margin;
}

/*---------------------------------------------------------------------------*/

void flayout_margin_col(FLayout *layout, const uint32_t col, const real32_t margin)
{
    FColumn *fcol = NULL;
    cassert_no_null(layout);
    cassert(col < arrst_size(layout->cols, FColumn) - 1);
    fcol = arrst_get(layout->cols, col, FColumn);
    fcol->margin_right = margin;
}

/*---------------------------------------------------------------------------*/

void flayout_margin_row(FLayout *layout, const uint32_t row, const real32_t margin)
{
    FRow *frow = NULL;
    cassert_no_null(layout);
    cassert(row < arrst_size(layout->rows, FRow) - 1);
    frow = arrst_get(layout->rows, row, FRow);
    frow->margin_bottom = margin;
}

/*---------------------------------------------------------------------------*/

void flayout_insert_col(FLayout *layout, const uint32_t col)
{
    uint32_t ncols = 0, nrows = 0, i = 0;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, FColumn);
    nrows = arrst_size(layout->rows, FRow);

    /* Insert and init (empty) the new cells */
    for (i = 0; i < nrows; ++i)
    {
        uint32_t inspos = ((ncols + 1) * i) + col;
        FCell *cell = arrst_insert_n(layout->cells, inspos, 1, FCell);
        dbind_init(cell, FCell);
    }

    /* Add a new column */
    {
        FColumn *ncol = arrst_insert_n(layout->cols, col, 1, FColumn);
        dbind_init(ncol, FColumn);
    }
}

/*---------------------------------------------------------------------------*/

void flayout_remove_col(FLayout *layout, const uint32_t col)
{
    uint32_t ncols = 0, nrows = 0, i = 0;
    cassert_no_null(layout);
    cassert(col < arrst_size(layout->cols, FColumn));
    ncols = arrst_size(layout->cols, FColumn);
    nrows = arrst_size(layout->rows, FRow);

    /* Destroy the column cells */
    for (i = 0; i < nrows; ++i)
    {
        uint32_t delrow = nrows - i - 1;
        uint32_t delpos = (ncols * delrow) + col;
        arrst_delete(layout->cells, delpos, i_remove_cell, FCell);
    }

    /* Destroy the column */
    arrst_delete(layout->cols, col, i_remove_column, FColumn);
}

/*---------------------------------------------------------------------------*/

void flayout_insert_row(FLayout *layout, const uint32_t row)
{
    uint32_t ncols = 0, i = 0;
    uint32_t inspos = 0;
    FCell *cells = NULL;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, FColumn);
    /* Cells insert position */
    inspos = row * ncols;
    /* Cells array is in row-major order. All row cells are together in memory */
    cells = arrst_insert_n(layout->cells, inspos, ncols, FCell);

    /* Initialize the new cells (empty) */
    for (i = 0; i < ncols; ++i)
        dbind_init(cells + i, FCell);

    /* Add a new row */
    {
        FRow *nrow = arrst_insert_n(layout->rows, row, 1, FRow);
        dbind_init(nrow, FRow);
    }
}

/*---------------------------------------------------------------------------*/

void flayout_remove_row(FLayout *layout, const uint32_t row)
{
    uint32_t i, ncols = 0;
    cassert_no_null(layout);
    cassert(row < arrst_size(layout->rows, FRow));
    ncols = arrst_size(layout->cols, FColumn);

    /* Destroy the row cells */
    for (i = 0; i < ncols; ++i)
    {
        uint32_t delcol = ncols - i - 1;
        uint32_t delpos = (ncols * row) + delcol;
        arrst_delete(layout->cells, delpos, i_remove_cell, FCell);
    }

    /* Destroy the row */
    arrst_delete(layout->rows, row, i_remove_row, FRow);
}

/*---------------------------------------------------------------------------*/

static ___INLINE FCell *i_cell(FLayout *layout, const uint32_t col, const uint32_t row)
{
    uint32_t ncols = UINT32_MAX;
    uint32_t pos = UINT32_MAX;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, FColumn);
    pos = row * ncols + col;
    return arrst_get(layout->cells, pos, FCell);
}

/*---------------------------------------------------------------------------*/

void flayout_remove_cell(FLayout *layout, const uint32_t col, const uint32_t row)
{
    String *name = NULL;
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    name = str_c(tc(cell->name));
    dbind_remove(cell, FCell);
    cell->type = ekCELL_TYPE_EMPTY;
    str_upd(&cell->name, tc(name));
    str_destroy(&name);
}

/*---------------------------------------------------------------------------*/

void flayout_add_layout(FLayout *layout, FLayout *sublayout, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(sublayout);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_LAYOUT;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_JUSTIFY;
    cell->widget.layout = sublayout;
}

/*---------------------------------------------------------------------------*/

void flayout_add_label(FLayout *layout, FLabel *label, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(label);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_LABEL;
    cell->halign = ekHALIGN_LEFT;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.label = label;
}

/*---------------------------------------------------------------------------*/

void flayout_add_button(FLayout *layout, FButton *button, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(button);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_BUTTON;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.button = button;
}

/*---------------------------------------------------------------------------*/

void flayout_add_check(FLayout *layout, FCheck *check, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(check);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_CHECK;
    cell->halign = ekHALIGN_LEFT;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.check = check;
}

/*---------------------------------------------------------------------------*/

void flayout_add_edit(FLayout *layout, FEdit *edit, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(edit);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_EDIT;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.edit = edit;
}

/*---------------------------------------------------------------------------*/

void flayout_add_text(FLayout *layout, FText *text, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(text);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_TEXT;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_JUSTIFY;
    cell->widget.text = text;
}

/*---------------------------------------------------------------------------*/

void flayout_add_image(FLayout *layout, FImage *image, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(image);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_IMAGE;
    cell->halign = ekHALIGN_CENTER;
    cell->valign = ekHALIGN_CENTER;
    cell->widget.image = image;
}

/*---------------------------------------------------------------------------*/

void flayout_add_slider(FLayout *layout, FSlider *slider, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(slider);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_SLIDER;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekHALIGN_CENTER;
    cell->widget.slider = slider;
}

/*---------------------------------------------------------------------------*/

void flayout_add_progress(FLayout *layout, FProgress *progress, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(progress);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_PROGRESS;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekHALIGN_CENTER;
    cell->widget.progress = progress;
}

/*---------------------------------------------------------------------------*/

uint32_t flayout_ncols(const FLayout *layout)
{
    cassert_no_null(layout);
    return arrst_size(layout->cols, FColumn);
}

/*---------------------------------------------------------------------------*/

uint32_t flayout_nrows(const FLayout *layout)
{
    cassert_no_null(layout);
    return arrst_size(layout->rows, FRow);
}

/*---------------------------------------------------------------------------*/

FColumn *flayout_column(FLayout *layout, const uint32_t col)
{
    cassert_no_null(layout);
    return arrst_get(layout->cols, col, FColumn);
}

/*---------------------------------------------------------------------------*/

FRow *flayout_row(FLayout *layout, const uint32_t row)
{
    cassert_no_null(layout);
    return arrst_get(layout->rows, row, FRow);
}

/*---------------------------------------------------------------------------*/

FCell *flayout_cell(FLayout *layout, const uint32_t col, const uint32_t row)
{
    return i_cell(layout, col, row);
}

/*---------------------------------------------------------------------------*/

const FCell *flayout_ccell(const FLayout *layout, const uint32_t col, const uint32_t row)
{
    return i_cell(cast(layout, FLayout), col, row);
}

/*---------------------------------------------------------------------------*/

static align_t i_halign(const halign_t halign)
{
    switch (halign)
    {
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
    switch (valign)
    {
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

Layout *flayout_to_gui(const FLayout *layout, const char_t *resource_path, const real32_t empty_width, const real32_t empty_height)
{
    uint32_t ncols = 0, nrows = 0;
    Layout *glayout = NULL;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, FColumn);
    nrows = arrst_size(layout->rows, FRow);
    glayout = layout_create(ncols, nrows);

    /* Layout border margins */
    layout_margin4(glayout, layout->margin_top, layout->margin_right, layout->margin_bottom, layout->margin_left);

    /* Column properties */
    arrst_foreach_const(col, layout->cols, FColumn)
        layout_hsize(glayout, col_i, col->forced_width);
        if (col_i < col_total - 1)
        {
            layout_hmargin(glayout, col_i, col->margin_right);
        }
        else
        {
            cassert(col->margin_right == 0);
        }
    arrst_end()

    /* Row properties */
    arrst_foreach_const(row, layout->rows, FRow)
        layout_vsize(glayout, row_i, row->forced_height);
        if (row_i < row_total - 1)
        {
            layout_vmargin(glayout, row_i, row->margin_bottom);
        }
        else
        {
            cassert(row->margin_bottom == 0);
        }
    arrst_end()

    /* Cells */
    {
        uint32_t i, j;
        const FCell *cells = arrst_all_const(layout->cells, FCell);
        for (j = 0; j < nrows; ++j)
        {
            for (i = 0; i < ncols; ++i)
            {
                Cell *gcell = layout_cell(glayout, i, j);
                align_t halign = i_halign(cells->halign);
                align_t valign = i_valign(cells->valign);
                layout_halign(glayout, i, j, halign);
                layout_valign(glayout, i, j, valign);
                switch (cells->type)
                {
                case ekCELL_TYPE_EMPTY:
                    cell_force_size(gcell, empty_width, empty_height);
                    break;

                case ekCELL_TYPE_LABEL:
                {
                    FLabel *flabel = cells->widget.label;
                    Label *glabel = label_create();
                    label_text(glabel, tc(flabel->text));
                    label_multiline(glabel, flabel->multiline);
                    label_min_width(glabel, flabel->min_width);
                    label_align(glabel, i_halign(flabel->align));
                    layout_label(glayout, glabel, i, j);
                    break;
                }

                case ekCELL_TYPE_BUTTON:
                {
                    FButton *fbutton = cells->widget.button;
                    Button *gbutton = button_push();
                    button_text(gbutton, tc(fbutton->text));
                    layout_button(glayout, gbutton, i, j);
                    break;
                }

                case ekCELL_TYPE_CHECK:
                {
                    FCheck *fcheck = cells->widget.check;
                    Button *gcheck = button_check();
                    button_text(gcheck, tc(fcheck->text));
                    layout_button(glayout, gcheck, i, j);
                    break;
                }

                case ekCELL_TYPE_EDIT:
                {
                    FEdit *fedit = cells->widget.edit;
                    Edit *gedit = edit_create();
                    align_t align = i_halign(fedit->text_align);
                    edit_passmode(gedit, fedit->passmode);
                    edit_autoselect(gedit, fedit->autosel);
                    edit_align(gedit, align);
                    layout_edit(glayout, gedit, i, j);
                    break;
                }

                case ekCELL_TYPE_TEXT:
                {
                    FText *ftext = cells->widget.text;
                    TextView *gtext = textview_create();
                    textview_editable(gtext, !ftext->read_only);
                    textview_size(gtext, s2df(ftext->min_width, ftext->min_height));
                    layout_textview(glayout, gtext, i, j);
                    break;
                }

				case ekCELL_TYPE_IMAGE:
                {
                    FImage *fimage = cells->widget.image;
                    ImageView *gimage = imageview_create();
                    String *path = str_cpath("%s/%s", resource_path, tc(fimage->path));
                    Image *image = image_from_file(tc(path), NULL);
					
					if (image != NULL)
					{
                        imageview_image(gimage, image);
                        image_destroy(&image);
					}
					else
					{
                        /* Use a default image */
                        const Image *rimage = nflib_default_image();
                        imageview_image(gimage, rimage);
					}

					str_destroy(&path);
                    imageview_size(gimage, s2df(fimage->min_width, fimage->min_height));
                    layout_imageview(glayout, gimage, i, j);
                    break;
                }

                case ekCELL_TYPE_SLIDER:
                {
                    FSlider *fslider = cells->widget.slider;
                    Slider *gslider = slider_create();
					slider_min_width(gslider, fslider->min_width);
                    slider_value(gslider, .5f);
                    layout_slider(glayout, gslider, i, j);
                    break;
                }

                case ekCELL_TYPE_PROGRESS:
                {
                    FProgress *fprogress = cells->widget.progress;
                    Progress *gprogress = progress_create();
					progress_min_width(gprogress, fprogress->min_width);
                    layout_progress(glayout, gprogress, i, j);
                    break;
                }

                case ekCELL_TYPE_LAYOUT:
                {
                    Layout *gsublayout = flayout_to_gui(cells->widget.layout, resource_path, empty_width, empty_height);
                    layout_layout(glayout, gsublayout, i, j);
                    break;
                }
                }

                cells += 1;
            }
        }
    }

    return glayout;
}

/*---------------------------------------------------------------------------*/

GuiControl *flayout_search_gui_control(const FLayout *layout, Layout *gui_layout, const char_t *cell_name)
{
    const FCell *cells = NULL;
    uint32_t i, j, ncols = 0, nrows = 0;
    cassert_no_null(layout);
    cells = arrst_all_const(layout->cells, FCell);
    ncols = arrst_size(layout->cols, FColumn);
    nrows = arrst_size(layout->rows, FRow);

    for (j = 0; j < nrows; ++j)
    {
        for (i = 0; i < ncols; ++i)
        {
            if (str_equ(cells->name, cell_name) == TRUE)
            {
                switch (cells->type)
                {
                case ekCELL_TYPE_LABEL:
                case ekCELL_TYPE_BUTTON:
                case ekCELL_TYPE_CHECK:
                case ekCELL_TYPE_EDIT:
                case ekCELL_TYPE_TEXT:
                case ekCELL_TYPE_IMAGE:
                case ekCELL_TYPE_SLIDER:
                case ekCELL_TYPE_PROGRESS:
				{
                    Cell *gcell = layout_cell(gui_layout, i, j);
                    return cell_control(gcell);
                }

                case ekCELL_TYPE_LAYOUT:
                case ekCELL_TYPE_EMPTY:
                    break;
                    cassert_default();
                }
            }

            if (cells->type == ekCELL_TYPE_LAYOUT)
            {
                Cell *gcell = layout_cell(gui_layout, i, j);
                Layout *gsub_layout = cell_layout(gcell);
                GuiControl *control = flayout_search_gui_control(cells->widget.layout, gsub_layout, cell_name);
                if (control != NULL)
                    return control;
            }

            cells += 1;
        }
    }

    return NULL;
}
