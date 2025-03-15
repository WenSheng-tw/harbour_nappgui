/* Designer layout */

#include "designer.hxx"

void dlayout_global_init(void);

DLayout *dlayout_from_flayout(const FLayout *flayout, const char_t *resource_path);

void dlayout_destroy(DLayout **layout);

void dlayout_insert_col(DLayout *layout, const uint32_t col);

void dlayout_remove_col(DLayout *layout, const uint32_t col);

void dlayout_insert_row(DLayout *layout, const uint32_t row);

void dlayout_remove_row(DLayout *layout, const uint32_t row);

void dlayout_remove_cell(DLayout *layout, const uint32_t col, const uint32_t row);

void dlayout_add_layout(DLayout *layout, DLayout *sublayout, const uint32_t col, const uint32_t row);

void dlayout_set_image(DLayout *layout, const Image *image, const uint32_t col, const uint32_t row);

void dlayout_remove_images(DLayout *layout, const uint32_t col, const uint32_t row);

void dlayout_add_image(DLayout *layout, const Image *image, const uint32_t col, const uint32_t row);

void dlayout_synchro_visual(DLayout *dlayout, const Layout *glayout, const V2Df origin);

void dlayout_elem_at_pos(const DLayout *dlayout, const FLayout *flayout, const Layout *glayout, const real32_t x, const real32_t y, ArrSt(DSelect) *selpath);

void dlayout_draw(const DLayout *dlayout, const FLayout *flayout, const Layout *glayout, const DSelect *hover, const DSelect *sel, const widget_t swidget, const Image *add_icon, DCtx *ctx);
