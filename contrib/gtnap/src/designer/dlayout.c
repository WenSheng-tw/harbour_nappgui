/* Design layout (editable parameters) */

#include "dlayout.h"
#include <gui/button.h>
#include <gui/label.h>
#include <gui/labelh.h>
#include <gui/layout.h>
#include <gui/layouth.h>
#include <gui/cell.h>
#include <gui/drawctrl.inl>
#include <geom2d/r2d.h>
#include <geom2d/v2d.h>
#include <draw2d/color.h>
#include <draw2d/draw.h>
#include <draw2d/drawg.h>
#include <draw2d/image.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/bmem.h>
#include <sewer/cassert.h>

static real32_t i_EMPTY_CELL_WIDTH = 40;
static real32_t i_EMPTY_CELL_HEIGHT = 20;

/*---------------------------------------------------------------------------*/

static void i_init_empty_column(DColumn *col)
{
    cassert_no_null(col);
    col->margin_right = 0;
}

/*---------------------------------------------------------------------------*/

static void i_init_empty_row(DRow *row)
{
    cassert_no_null(row);
    row->margin_bottom = 0;
}

/*---------------------------------------------------------------------------*/

static void i_remove_column(DColumn *column)
{
    dbind_remove(column, DColumn);
}

/*---------------------------------------------------------------------------*/

static void i_remove_row(DRow *row)
{
    dbind_remove(row, DRow);
}

/*---------------------------------------------------------------------------*/

static void i_remove_cell(DCell *cell)
{
    dbind_remove(cell, DCell);
}

/*---------------------------------------------------------------------------*/

static void i_init_empty_cell(DCell *cell)
{
    cassert_no_null(cell);
    bmem_zero(cell, DCell);
    cell->type = ekCELL_TYPE_EMPTY;
    cell->halign = ekHALIGN_LEFT;
    cell->valign = ekVALIGN_TOP;
}

/*---------------------------------------------------------------------------*/

DLayout *dlayout_create(const uint32_t ncols, const uint32_t nrows)
{
    DLayout *layout = dbind_create(DLayout);
    cassert(ncols > 0);
    cassert(nrows > 0);

    /* Add columns to layout */
    {
        uint32_t i = 0;
        for (i = 0; i < ncols; ++i)
        {
            DColumn *col = arrst_new(layout->cols, DColumn);
            i_init_empty_column(col);
        }
    }

    /* Add rows to layout */
    {
        uint32_t i = 0;
        for (i = 0; i < nrows; ++i)
        {
            DRow *row = arrst_new(layout->rows, DRow);
            i_init_empty_row(row);
        }
    }

    /* Add cells to layout */
    {
        uint32_t n = ncols * nrows, i = 0;
        for (i = 0; i < n; ++i)
        {
            DCell *cell = arrst_new(layout->cells, DCell);
            i_init_empty_cell(cell);
        }
    }

    return layout;
}

/*---------------------------------------------------------------------------*/

void dlayout_destroy(DLayout **layout)
{
    dbind_destroy(layout, DLayout);
}

/*---------------------------------------------------------------------------*/

void dlayout_name(DLayout *layout, const char_t *name)
{
    cassert_no_null(layout);
    str_upd(&layout->name, name);
}

/*---------------------------------------------------------------------------*/

void dlayout_margin_left(DLayout *layout, const real32_t margin)
{
    cassert_no_null(layout);
    layout->margin_left = margin;
}

/*---------------------------------------------------------------------------*/

void dlayout_margin_top(DLayout *layout, const real32_t margin)
{
    cassert_no_null(layout);
    layout->margin_top = margin;
}

/*---------------------------------------------------------------------------*/

void dlayout_margin_right(DLayout *layout, const real32_t margin)
{
    cassert_no_null(layout);
    layout->margin_right = margin;
}

/*---------------------------------------------------------------------------*/

void dlayout_margin_bottom(DLayout *layout, const real32_t margin)
{
    cassert_no_null(layout);
    layout->margin_bottom = margin;
}

/*---------------------------------------------------------------------------*/

void dlayout_margin_col(DLayout *layout, const uint32_t col, const real32_t margin)
{
    DColumn *dcol = NULL;
    cassert_no_null(layout);
    cassert(col < arrst_size(layout->cols, DColumn) - 1);
    dcol= arrst_get(layout->cols, col, DColumn);
    dcol->margin_right = margin;
}

/*---------------------------------------------------------------------------*/

void dlayout_margin_row(DLayout *layout, const uint32_t row, const real32_t margin)
{
    DRow *drow = NULL;
    cassert_no_null(layout);
    cassert(row < arrst_size(layout->rows, DRow) - 1);
    drow = arrst_get(layout->rows, row, DRow);
    drow->margin_bottom = margin;
}

/*---------------------------------------------------------------------------*/

void dlayout_insert_col(DLayout *layout, const uint32_t col)
{
    uint32_t ncols = 0, nrows = 0, i = 0;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, DColumn);
    nrows = arrst_size(layout->rows, DRow);

    /* Insert and init (empty) the new cells */
    for (i = 0; i < nrows; ++i)
    {
        uint32_t inspos = ((ncols + 1) * i) + col;
        DCell *cell = arrst_insert_n(layout->cells, inspos, 1, DCell);
        i_init_empty_cell(cell);
    }

    /* Add a new column */
    {
        DColumn *ncol = arrst_insert_n(layout->cols, col, 1, DColumn);
        i_init_empty_column(ncol);
    }
}

/*---------------------------------------------------------------------------*/

void dlayout_remove_col(DLayout *layout, const uint32_t col)
{
    uint32_t ncols = 0, nrows = 0, i = 0;
    cassert_no_null(layout);
    cassert(col < arrst_size(layout->cols, DColumn));
    ncols = arrst_size(layout->cols, DColumn);
    nrows = arrst_size(layout->rows, DRow);

    /* Destroy the column cells */
    for (i = 0; i < nrows; ++i)
    {
        uint32_t delrow = nrows - i - 1;
        uint32_t delpos = (ncols * delrow) + col;
        arrst_delete(layout->cells, delpos, i_remove_cell, DCell);
    }

    /* Destroy the column */
    arrst_delete(layout->cols, col, i_remove_column, DColumn);
}

/*---------------------------------------------------------------------------*/

void dlayout_insert_row(DLayout *layout, const uint32_t row)
{
    uint32_t ncols = 0, nrows = 0, i = 0;
    uint32_t inspos = 0;
    DCell *cells = NULL;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, DColumn);
    nrows = arrst_size(layout->rows, DRow);
    /* Cells insert position */
    inspos = row * ncols;
    /* Cells array is in row-major order. All row cells are together in memory */
    cells = arrst_insert_n(layout->cells, inspos, ncols, DCell);

    /* Initialize the new cells (empty) */
    for (i = 0; i < ncols; ++i)
    {
        i_init_empty_cell(cells + i);
    }

    /* Add a new row */
    {
        DRow *nrow = arrst_insert_n(layout->rows, row, 1, DRow);
        i_init_empty_row(nrow);
    }
}

/*---------------------------------------------------------------------------*/

void dlayout_remove_row(DLayout *layout, const uint32_t row)
{
    uint32_t i, ncols = 0, nrows = 0;
    cassert_no_null(layout);
    cassert(row < arrst_size(layout->rows, DRow));
    ncols = arrst_size(layout->cols, DColumn);
    nrows = arrst_size(layout->rows, DRow);

    /* Destroy the row cells */
    for (i = 0; i < ncols; ++i)
    {
        uint32_t delcol = ncols - i - 1;
        uint32_t delpos = (ncols * row) + delcol;
        arrst_delete(layout->cells, delpos, i_remove_cell, DCell);
    }

    /* Destroy the row */
    arrst_delete(layout->rows, row, i_remove_row, DRow);
}

/*---------------------------------------------------------------------------*/

static ___INLINE DCell *i_cell(DLayout *layout, const uint32_t col, const uint32_t row)
{
    uint32_t ncols = UINT32_MAX;
    uint32_t pos = UINT32_MAX;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, DColumn);
    pos = row * ncols + col;
    return arrst_get(layout->cells, pos, DCell);
}

/*---------------------------------------------------------------------------*/

void dlayout_remove_cell(DLayout *layout, const uint32_t col, const uint32_t row)
{
    String *name = NULL;
    DCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    name = str_c(tc(cell->name));
    i_remove_cell(cell);
    i_init_empty_cell(cell);
    str_upd(&cell->name, tc(name));
    str_destroy(&name);
}

/*---------------------------------------------------------------------------*/

void dlayout_add_layout(DLayout *layout, DLayout *sublayout, const uint32_t col, const uint32_t row)
{
    DCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(sublayout);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_LAYOUT;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_JUSTIFY;
    cell->content.layout = sublayout;
}

/*---------------------------------------------------------------------------*/

void dlayout_add_label(DLayout *layout, DLabel *label, const uint32_t col, const uint32_t row)
{
    DCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(label);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_LABEL;
    cell->halign = ekHALIGN_LEFT;
    cell->valign = ekVALIGN_CENTER;
    cell->content.label = label;
}

/*---------------------------------------------------------------------------*/

void dlayout_add_button(DLayout *layout, DButton *button, const uint32_t col, const uint32_t row)
{
    DCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(button);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_BUTTON;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_CENTER;
    cell->content.button = button;
}

/*---------------------------------------------------------------------------*/

/*
static color_t i_color(void)
{
    static uint32_t index = 0;
    switch(index++ % 3) {
    case 0:
        return kCOLOR_RED;
    case 1:
        return kCOLOR_BLUE;
    case 2:
        return kCOLOR_GREEN;
    cassert_default();
    }

    return kCOLOR_DEFAULT;
}*/

/*---------------------------------------------------------------------------*/

DColumn *dlayout_column(DLayout *layout, const uint32_t col)
{
    cassert_no_null(layout);
    return arrst_get(layout->cols, col, DColumn);
}

/*---------------------------------------------------------------------------*/

DRow *dlayout_row(DLayout *layout, const uint32_t row)
{
    cassert_no_null(layout);
    return arrst_get(layout->rows, row, DRow);
}

/*---------------------------------------------------------------------------*/

bool_t dlayout_empty_cell(const DSelect *sel)
{
    cassert_no_null(sel);
    if (sel->layout != NULL)
    {
        if (sel->elem == ekLAYELEM_CELL)
        {
            const DCell *cell = i_cell(sel->layout, sel->col, sel->row);
            cassert_no_null(cell);
            if (cell->type == ekCELL_TYPE_EMPTY)
                return TRUE;
        }
    }
        
    return FALSE;
}

/*---------------------------------------------------------------------------*/

DCell *dlayout_cell(DLayout *layout, const uint32_t col, const uint32_t row)
{
    return i_cell(layout, col, row);
}

/*---------------------------------------------------------------------------*/

const DCell *dlayout_ccell(const DLayout *layout, const uint32_t col, const uint32_t row)
{
    return i_cell(cast(layout, DLayout), col, row);
}

/*---------------------------------------------------------------------------*/

DCell *dlayout_cell_sel(const DSelect *sel)
{
    cassert_no_null(sel);
    if (sel->layout != NULL)
    {
        if (sel->elem == ekLAYELEM_CELL)
            return i_cell(sel->layout, sel->col, sel->row);
    }
        
    return NULL;
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

Layout *dlayout_gui_layout(const DLayout *layout)
{
    uint32_t ncols = 0, nrows = 0;
    Layout *glayout = NULL;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, DColumn);
    nrows = arrst_size(layout->rows, DRow);
    glayout = layout_create(ncols, nrows);

    /* Layout border margins */
    layout_margin4(glayout, layout->margin_top, layout->margin_right, layout->margin_bottom, layout->margin_left);

    /* Inter-column margins */
    {
        uint32_t i = 0;
        for (i = 0; i < ncols - 1; ++i)
        {
            const DColumn *col = arrst_get_const(layout->cols, i, DColumn);
            layout_hmargin(glayout, i, col->margin_right);
        }
    }

    /* Inter-row margins */
    {
        uint32_t i = 0;
        for (i = 0; i < nrows - 1; ++i)
        {
            const DRow *row = arrst_get_const(layout->rows, i, DRow);
            layout_vmargin(glayout, i, row->margin_bottom);
        }
    }

    /* Cells */
    {
        uint32_t i, j;
        const DCell *cells = arrst_all_const(layout->cells, DCell);
        for (j = 0; j < nrows; ++j)
        {
            for(i = 0; i < ncols; ++i)
            {
                Cell *gcell = layout_cell(glayout, i, j);
                align_t halign = i_halign(cells->halign);
                align_t valign = i_valign(cells->valign);
                layout_halign(glayout, i, j, halign);
                layout_valign(glayout, i, j, valign);
                switch(cells->type) {
                case ekCELL_TYPE_EMPTY:
                    cell_force_size(gcell, i_EMPTY_CELL_WIDTH, i_EMPTY_CELL_HEIGHT);
                    break;

                case ekCELL_TYPE_LABEL:
                {
                    DLabel *dlabel = cells->content.label;
                    Label *glabel = label_create();
                    label_text(glabel, tc(dlabel->text));
                    layout_label(glayout, glabel, i, j);
                    break;
                }

                case ekCELL_TYPE_BUTTON:
                {
                    DButton *dbutton = cells->content.button;
                    Button *gbutton = button_push();
                    button_text(gbutton, tc(dbutton->text));
                    layout_button(glayout, gbutton, i, j);
                    break;
                }

                case ekCELL_TYPE_LAYOUT:
                {
                    Layout *gsublayout = dlayout_gui_layout(cells->content.layout);
                    layout_layout(glayout, gsublayout, i, j);
                    break;
                }

                }

                cells += 1;
            }
        }
    }

    /*layout_bgcolor(glayout, i_color());*/
    return glayout;
}

/*---------------------------------------------------------------------------*/

void dlayout_synchro_visual(DLayout *layout, const Layout *glayout, const V2Df origin)
{
    DColumn *col = NULL;
    DRow *row = NULL;
    DCell *cell = NULL;
    uint32_t ncols, nrows, i, j;
    real32_t total_width = 0, total_height = 0;
    real32_t inner_width = 0, inner_height = 0;
    real32_t x, y;

    cassert_no_null(layout);
    col = arrst_all(layout->cols, DColumn);
    row = arrst_all(layout->rows, DRow);
    cell = arrst_all(layout->cells, DCell);
    ncols = arrst_size(layout->cols, DColumn); 
    nrows = arrst_size(layout->rows, DRow); 

    /* Compute the columns width and total layout widths */
    total_width = 0;
    inner_width = 0;
    for (i = 0; i < ncols; ++i)
    {
        col[i].width = layout_get_hsize(glayout, i);
        inner_width += col[i].width;
        if (i < ncols - 1)
            inner_width += col[i].margin_right;
        else
            total_width = inner_width + layout->margin_left + layout->margin_right;
    }

    /* Compute the rows height and total layout heights */
    total_height = 0;
    inner_height = 0;
    for (j = 0; j < nrows; ++j)
    {
        row[j].height = layout_get_vsize(glayout, j);
        inner_height += row[j].height;
        if (j < nrows - 1)
            inner_height += row[j].margin_bottom;
        else
            total_height = inner_height + layout->margin_top + layout->margin_bottom;
    }

    /* Global layout rectangle */
    layout->rect = r2df(origin.x, origin.y, total_width, total_height);

    /* Compute the vertical rectangles */
    layout->rect_left = r2df(origin.x, origin.y, layout->margin_left, total_height);
    x = origin.x + layout->margin_left;
    for (i = 0; i < ncols; ++i)
    {
        x += col[i].width;
        col[i].margin_rect = r2df(x, origin.y, col[i].margin_right, total_height);
        x += col[i].margin_right;
    }
    layout->rect_right = r2df(x, origin.y, layout->margin_right, total_height);

    /* Compute the horizontal rectangles */
    layout->rect_top = r2df(origin.x, origin.y, total_width, layout->margin_top);
    y = origin.y + layout->margin_top;
    for (j = 0; j < nrows; ++j)
    {
        y += row[j].height;
        row[j].margin_rect = r2df(origin.x, y, total_width, row[j].margin_bottom);
        y += row[j].margin_bottom;
    }
    layout->rect_bottom = r2df(origin.x, y, total_width, layout->margin_bottom);

    /* Compute the cells rectangles */
    {
        DCell *dcell = cell;
        y = origin.y + layout->margin_top;

        for (j = 0; j < nrows; ++j)
        {
            x = origin.x + layout->margin_left;
            for(i = 0; i < ncols; ++i)
            {
                Cell *gcell = layout_cell(cast(glayout, Layout), i, j);
                real32_t hsize = cell_get_hsize(gcell);
                real32_t vsize = cell_get_vsize(gcell);
                align_t halign = cell_get_halign(gcell);
                align_t valign = cell_get_valign(gcell);
                real32_t cellx = 0;
                real32_t celly = 0;
                cassert_no_null(gcell);
                cassert(hsize <= col[i].width);
                cassert(vsize <= row[j].height);

                switch(halign) {
                case ekLEFT:
                    cassert(dcell->halign == ekHALIGN_LEFT);
                    cellx = x;
                    break;
                case ekCENTER:
                    cassert(dcell->halign == ekHALIGN_CENTER);
                    cellx = x + (col[i].width - hsize) / 2;
                    break;
                case ekRIGHT:
                    cassert(dcell->halign == ekHALIGN_RIGHT);
                    cellx = x + (col[i].width - hsize);
                    break;
                case ekJUSTIFY:
                    cassert(dcell->halign == ekHALIGN_JUSTIFY);
                    cellx = x;
                    break;
                cassert_default();
                }

                switch(valign) {
                case ekTOP:
                    cassert(dcell->valign == ekVALIGN_TOP);
                    celly = y;
                    break;
                case ekCENTER:
                    cassert(dcell->valign == ekVALIGN_CENTER);
                    celly = y + (row[j].height - vsize) / 2;
                    break;
                case ekBOTTOM:
                    cassert(dcell->valign == ekVALIGN_BOTTOM);
                    celly = y + (row[j].height - vsize);
                    break;
                case ekJUSTIFY:
                    cassert(dcell->valign == ekVALIGN_JUSTIFY);
                    celly = y;
                    break;
                cassert_default();
                }

                switch(dcell->type) {
                case ekCELL_TYPE_EMPTY:
                case ekCELL_TYPE_LABEL:
                case ekCELL_TYPE_BUTTON:
                    break;

                case ekCELL_TYPE_LAYOUT:
                {
                    DLayout *sublayout = dcell->content.layout;
                    Layout *subglayout = layout_get_layout(cast(glayout, Layout), i, j);
                    dlayout_synchro_visual(sublayout, subglayout, v2df(cellx, celly));
                    break;
                }

                cassert_default();
                }

                dcell->rect = r2df(x, y, col[i].width, row[j].height);
                dcell->content_rect = r2df(cellx, celly, hsize, vsize);
                dcell += 1;
                x += col[i].width;
                x += col[i].margin_right;
            }

            y += row[j].height;
            y += row[j].margin_bottom;
        }
    }
}

/*---------------------------------------------------------------------------*/

Layout *dlayout_search_layout(const DLayout *layout, Layout *glayout, const DLayout *required)
{
    cassert_no_null(layout);
    if (layout != required)
    {
        DCell *cell = arrst_all(layout->cells, DCell);
        uint32_t ncols = arrst_size(layout->cols, DColumn); 
        uint32_t nrows = arrst_size(layout->rows, DRow); 
        uint32_t i, j;

        for (j = 0; j < nrows; ++j)
        {
            for(i = 0; i < ncols; ++i)
            {
                if (cell->type == ekCELL_TYPE_LAYOUT)
                {
                    DLayout *sublayout = cell->content.layout;
                    Layout *subglayout = layout_get_layout(cast(glayout, Layout), i, j);
                    Layout *fglayout = dlayout_search_layout(sublayout, subglayout, required);
                    if (fglayout != NULL)
                        return fglayout;
                }

                cell += 1;
            }
        }

        return NULL;
    }
    else
    {
        cassert(dlayout_ncols(layout) == layout_ncols(glayout));
        cassert(dlayout_nrows(layout) == layout_nrows(glayout));
        return glayout;
    }
}

/*---------------------------------------------------------------------------*/

uint32_t dlayout_ncols(const DLayout *layout)
{
    cassert_no_null(layout);
    return arrst_size(layout->cols, DColumn); 
}

/*---------------------------------------------------------------------------*/

uint32_t dlayout_nrows(const DLayout *layout)
{
    cassert_no_null(layout);
    return arrst_size(layout->rows, DRow); 
}

///*---------------------------------------------------------------------------*/
//
//real32_t dlayout_get_margin_top(const DLayout *layout)
//{
//    cassert_no_null(layout);
//    return layout->margin_top;
//}
//
///*---------------------------------------------------------------------------*/
//
//real32_t dlayout_get_margin_bottom(const DLayout *layout)
//{
//    DRow *row = NULL;
//    cassert_no_null(layout);
//    row = arrst_last(layout->rows, DRow);
//    return row->margin_bottom;
//}
//
///*---------------------------------------------------------------------------*/
//
//real32_t dlayout_get_margin_left(const DLayout *layout)
//{
//    cassert_no_null(layout);
//    return layout->margin_left;
//}
//
///*---------------------------------------------------------------------------*/
//
//real32_t dlayout_get_margin_right(const DLayout *layout)
//{
//    DColumn *col = NULL;
//    cassert_no_null(layout);
//    col = arrst_last(layout->cols, DColumn);
//    return col->margin_right;
//}
//
///*---------------------------------------------------------------------------*/
//
//real32_t dlayout_get_margin_col(const DLayout *layout, const uint32_t col)
//{
//    DColumn *dcol = NULL;
//    cassert_no_null(layout);
//    cassert(col < arrst_size(layout->cols, DColumn) - 1);
//    dcol= arrst_get(layout->cols, col, DColumn);
//    return dcol->margin_right;
//}
//
///*---------------------------------------------------------------------------*/
//
//real32_t dlayout_get_margin_row(const DLayout *layout, const uint32_t row)
//{
//    DRow *drow = NULL;
//    cassert_no_null(layout);
//    cassert(row < arrst_size(layout->rows, DRow) - 1);
//    drow = arrst_get(layout->rows, row, DRow);
//    return drow->margin_bottom;
//}

/*---------------------------------------------------------------------------*/

void dlayout_elem_at_pos(const DLayout *layout, const real32_t x, const real32_t y, ArrSt(DSelect) *selpath)
{
    cassert_no_null(layout); 
    if (r2d_containsf(&layout->rect, x, y) == TRUE)
    {
        DSelect *sel = arrst_new(selpath, DSelect);

        if (r2d_containsf(&layout->rect_left, x, y) == TRUE)
        {
            sel->layout = cast(layout, DLayout);
            sel->elem = ekLAYELEM_MARGIN_LEFT;
            sel->col = UINT32_MAX;
            sel->row = UINT32_MAX;
            return;
        }

        if (r2d_containsf(&layout->rect_top, x, y) == TRUE)
        {
            sel->layout = cast(layout, DLayout);
            sel->elem = ekLAYELEM_MARGIN_TOP;
            sel->col = UINT32_MAX;
            sel->row = UINT32_MAX;
            return;
        }

        if (r2d_containsf(&layout->rect_right, x, y) == TRUE)
        {
            sel->layout = cast(layout, DLayout);
            sel->elem = ekLAYELEM_MARGIN_RIGHT;
            sel->col = UINT32_MAX;
            sel->row = UINT32_MAX;
            return;
        }

        if (r2d_containsf(&layout->rect_bottom, x, y) == TRUE)
        {
            sel->layout = cast(layout, DLayout);
            sel->elem = ekLAYELEM_MARGIN_BOTTOM;
            sel->col = UINT32_MAX;
            sel->row = UINT32_MAX;
            return;
        }

        arrst_foreach_const(col, layout->cols, DColumn)
            if (col_i < col_total - 1)
            {
                if (r2d_containsf(&col->margin_rect, x, y) == TRUE)
                {
                    sel->layout = cast(layout, DLayout);
                    sel->elem = ekLAYELEM_MARGIN_COLUMN;
                    sel->col = col_i;
                    sel->row = UINT32_MAX;
                    return;
                }
            }
        arrst_end()

        arrst_foreach_const(row, layout->rows, DRow)
            if (row_i < row_total - 1)
            {
                if (r2d_containsf(&row->margin_rect, x, y) == TRUE)
                {
                    sel->layout = cast(layout, DLayout);
                    sel->elem = ekLAYELEM_MARGIN_ROW;
                    sel->col = UINT32_MAX;
                    sel->row = row_i;
                    return;
                }
            }
        arrst_end()

        arrst_foreach_const(cell, layout->cells, DCell)
            if (r2d_containsf(&cell->rect, x, y) == TRUE)
            {
                uint32_t ncols = arrst_size(layout->cols, DColumn);
                sel->layout = cast(layout, DLayout);
                sel->elem = ekLAYELEM_CELL;
                sel->col = cell_i % ncols;
                sel->row = cell_i / ncols;

                switch(cell->type) {
                case ekCELL_TYPE_EMPTY:
                case ekCELL_TYPE_LABEL:
                case ekCELL_TYPE_BUTTON:
                    break;
           
                case ekCELL_TYPE_LAYOUT:
                    dlayout_elem_at_pos(cell->content.layout, x, y, selpath);
                    break;
                }

                return;
            }
        arrst_end()
    }
}

/*---------------------------------------------------------------------------*/

static R2Df i_cell_rect(const DLayout *layout, const DSelect *sel)
{
    uint32_t ncols, pos; 
    const DCell *cell = NULL;
    cassert_no_null(layout);
    cassert_no_null(sel);
    cassert(layout == sel->layout);
    cassert(sel->elem == ekLAYELEM_CELL);
    ncols = arrst_size(layout->cols, DColumn);
    pos = sel->row * ncols + sel->col;
    cell = arrst_get_const(layout->cells, pos, DCell);
    return cell->rect;
}

/*---------------------------------------------------------------------------*/

static R2Df i_get_rect(const DLayout *layout, const DSelect *sel)
{
    cassert_no_null(layout);
    cassert_no_null(sel);
    cassert(layout == sel->layout);
    switch(sel->elem) {
    case ekLAYELEM_MARGIN_LEFT:
        return layout->rect_left;

    case ekLAYELEM_MARGIN_TOP:
        return layout->rect_top;

    case ekLAYELEM_MARGIN_RIGHT:
        return layout->rect_right;

    case ekLAYELEM_MARGIN_BOTTOM:
        return layout->rect_bottom;

    case ekLAYELEM_MARGIN_COLUMN:
    {
        const DColumn *col = arrst_get_const(layout->cols, sel->col, DColumn);
        return col->margin_rect;
    }
    
    case ekLAYELEM_MARGIN_ROW:
    {
        const DRow *row = arrst_get_const(layout->rows, sel->row, DRow);
        return row->margin_rect;
    }

    case ekLAYELEM_CELL:
        return i_cell_rect(layout, sel);

    }

    return kR2D_ZEROf;
}

/*---------------------------------------------------------------------------*/

static R2Df i_get_full_rect(const DLayout *layout, const DSelect *sel)
{
    cassert_no_null(layout);
    cassert_no_null(sel);
    cassert(layout == sel->layout);
    switch(sel->elem) {
    case ekLAYELEM_MARGIN_LEFT:
    case ekLAYELEM_MARGIN_TOP:
    case ekLAYELEM_MARGIN_RIGHT:
    case ekLAYELEM_MARGIN_BOTTOM:
    case ekLAYELEM_MARGIN_COLUMN:
    case ekLAYELEM_MARGIN_ROW:
        return layout->rect;

    case ekLAYELEM_CELL:
        return i_cell_rect(layout, sel);

    }

    return kR2D_ZEROf;
}

/*---------------------------------------------------------------------------*/

static bool_t i_is_cell_sel(const DSelect *sel, const DLayout *layout, const uint32_t col, const uint32_t row)
{
    cassert_no_null(sel);
    if (sel->layout != layout)
        return FALSE;
    if (sel->elem != ekLAYELEM_CELL)
        return FALSE;
    if (sel->col != col)
        return FALSE;
    if (sel->row != row)
        return FALSE;
    return TRUE;
}

/*---------------------------------------------------------------------------*/

void dlayout_draw(const DLayout *layout, const Layout *glayout, const DSelect *hover, const DSelect *sel, const widget_t swidget, const Image *add_icon, DCtx *ctx)
{
    uint32_t ncols, nrows, i, j;
    const DCell *cell = NULL;
    cassert_no_null(layout);
    cassert_no_null(hover);

    ncols = arrst_size(layout->cols, DColumn); 
    nrows = arrst_size(layout->rows, DRow); 
    cell = arrst_all_const(layout->cells, DCell);

    draw_line_color(ctx, kCOLOR_BLACK);
    draw_fill_color(ctx, kCOLOR_BLACK);
    draw_r2df(ctx, ekSTROKE, &layout->rect);
    //draw_r2df(ctx, ekFILL, &layout->rect_left);
    //draw_r2df(ctx, ekFILL, &layout->rect_top);

    //arrst_foreach_const(col, layout->cols, DColumn)
    //    draw_r2df(ctx, ekFILL, &col->margin_rect);
    //arrst_end()

    //arrst_foreach_const(row, layout->rows, DRow)
    //    draw_r2df(ctx, ekFILL, &row->margin_rect);
    //arrst_end()

    for (j = 0; j < nrows; ++j)
    {
        for (i = 0; i < ncols; ++i)
        {
            Cell *gcell = layout_cell(cast(glayout, Layout), i, j);
            draw_r2df(ctx, ekSTROKE, &cell->rect);
            switch(cell->type) {
            case ekCELL_TYPE_EMPTY:
                break;
            case ekCELL_TYPE_LABEL:
            {
                color_t color = i_is_cell_sel(hover, layout, i, j) ? kCOLOR_RED : kCOLOR_BLACK;
                color_t bgcolor = color_rgb(225, 225, 0);
                const Label *glabel = cell_label(gcell);
                const Font *gfont = label_get_font(glabel);
                draw_font(ctx, gfont);
                draw_fill_color(ctx, bgcolor);
                draw_text_color(ctx, color);
                draw_rect(ctx, ekFILL, cell->content_rect.pos.x, cell->content_rect.pos.y, cell->content_rect.size.width, cell->content_rect.size.height);
                drawctrl_text(ctx, tc(cell->content.label->text), (int32_t)cell->content_rect.pos.x, (int32_t)cell->content_rect.pos.y, ekCTRL_STATE_NORMAL);
                break;
            }

            case ekCELL_TYPE_BUTTON:
                draw_rect(ctx, ekFILL, cell->content_rect.pos.x, cell->content_rect.pos.y, cell->content_rect.size.width, cell->content_rect.size.height);
                break;

            case ekCELL_TYPE_LAYOUT:
            {
                Layout *chid_glayout = cell_layout(gcell);
                dlayout_draw(cell->content.layout, chid_glayout, hover, sel, swidget, add_icon, ctx);
                break;
            }
            }

            cell += 1;
        }
    }

    /* This layout has the hover element */
    if (hover->layout == layout)
    {
        R2Df rect = i_get_rect(layout, hover);
        draw_line_color(ctx, kCOLOR_RED);
        if (hover->elem != ekLAYELEM_CELL)
        {
            draw_r2df(ctx, ekSTROKE, &rect);
            draw_line_color(ctx, kCOLOR_BLACK);
            //draw_fill_color(ctx, kCOLOR_RED);
            //draw_r2df(ctx, ekFILL, &rect);
        }
        else
        {
            uint32_t pos = hover->row * ncols + hover->col;
            const DCell *dcell = arrst_get_const(layout->cells, pos, DCell);
            draw_r2df(ctx, ekSTROKE, &rect);
            if (dcell->type == ekCELL_TYPE_EMPTY && swidget != ekWIDGET_SELECT)
            {
                uint32_t iw = image_width(add_icon);
                uint32_t ih = image_height(add_icon);
                real32_t x = rect.pos.x + (rect.size.width - iw) / 2;
                real32_t y = rect.pos.y + (rect.size.height - ih) / 2;
                draw_image(ctx, add_icon, x, y);
            }
        }
        draw_line_color(ctx, kCOLOR_BLACK);
    }

    /* This layout has the selected element */
    if (sel->layout == layout)
    {
        R2Df rect = i_get_full_rect(layout, sel);
        real32_t rsize = 5;
        real32_t x1 = rect.pos.x;
        real32_t x2 = rect.pos.x + rect.size.width;
        real32_t y1 = rect.pos.y;
        real32_t y2 = rect.pos.y + rect.size.height;
        draw_fill_color(ctx, color_rgb(0, 40, 85));
        draw_rect(ctx, ekFILL, x1, y1, rsize, rsize);
        draw_rect(ctx, ekFILL, x2 - rsize, y1, rsize, rsize);
        draw_rect(ctx, ekFILL, x1, y2 - rsize, rsize, rsize);
        draw_rect(ctx, ekFILL, x2 - rsize, y2 - rsize, rsize, rsize);
        draw_rect(ctx, ekFILL, (x1 + x2 - rsize) / 2, y1, rsize, rsize);
        draw_rect(ctx, ekFILL, (x1 + x2 - rsize) / 2, y2 - rsize, rsize, rsize);
        draw_rect(ctx, ekFILL, x1, (y1 + y2 - rsize) / 2, rsize, rsize);
        draw_rect(ctx, ekFILL, x2 - rsize, (y1 + y2 - rsize) / 2, rsize, rsize);
    }
}
