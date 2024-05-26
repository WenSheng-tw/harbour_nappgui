# HBOFFICE User Guide

* [Initialize](#initialize)
* [Errors](#errors)
* [String parameters](#string-parameters)
* [Color parameters](#color-parameters)
* [SpreadSheet](#spreadsheet)
* [TextDocument](#textdocument)

Harbour interface for access to LibreOffice documents. The HBOFFICE-SDK functions have been divided into three families:

* `HBOFFICE_XLS_??` for Spreadsheet documents (LibreOffice Calc).
* `HBOFFICE_DOC_??` for Text documents (LibreOffice Writer).
* `HBOFFICE_??` for generic shared functionality.

## Initialize

* `HBOFFICE_INIT()` must be called first, before any other HBOFFICE function.
* `HBOFFICE_FINISH()` must be called at end, before program exit or when HBOFFICE support not necessary.

## Errors

Every call to any `HBOFFICE_XLS_??` or `HBOFFICE_DOC_??` function will store in cache the last error code. If function succeeds the last error code will be `HBOFFICE_RES_OK`.

```
// Error number
LOCAL N_Err := HBOFFICE_LAST_ERROR()

// Error string
LOCAL C_Err := HBOFFICE_ERROR_STR(N_Err)
```

The numeric error codes are in `hboffice.ch`
```
#define HBOFFICE_RES_OK 1
#define HBOFFICE_RES_NO_ENVAR 2
#define HBOFFICE_RES_PROC_KILL_FAIL 3
#define HBOFFICE_RES_PROC_INIT_FAIL 4
#define HBOFFICE_RES_CONECT_FAIL 5
#define HBOFFICE_RES_COMPONENT_LOADER 6
#define HBOFFICE_RES_CREATE_FILE_ERROR 7
#define HBOFFICE_RES_OPEN_FILE_ERROR 8
#define HBOFFICE_RES_SAVE_FILE_ERROR 9
#define HBOFFICE_RES_CLOSE_DOC_ERROR 10
#define HBOFFICE_RES_ACCESS_DOC_ERROR 11
#define HBOFFICE_RES_ACCESS_CELL_ERROR 12
#define HBOFFICE_RES_EDIT_CELL_ERROR 13
#define HBOFFICE_RES_FORMAT_CELL_ERROR 14
#define HBOFFICE_RES_ACCESS_COLUMN_ERROR 15
#define HBOFFICE_RES_FORMAT_COLUMN_ERROR 16
#define HBOFFICE_RES_ACCESS_ROW_ERROR 17
#define HBOFFICE_RES_FORMAT_ROW_ERROR 18
#define HBOFFICE_RES_TEXT_PROPERTY_ERROR 19
#define HBOFFICE_RES_TEXT_ADD_ERROR 20
#define HBOFFICE_RES_PAGE_PROPERTY_ERROR 21
#define HBOFFICE_RES_PRINTER_CONFIG_ERROR 22
#define HBOFFICE_RES_PRINT_ERROR 23
```

## String parameters

Every string parameter in any `HBOFFICE_XLS_??` or `HBOFFICE_DOC_??` will accept a string directly or a harbour block that returns a string.

```
// Valid
LOCAL O_XLS := HBOFFICE_XLS_OPEN({|| WORK_PATH() + "/../office/empty.ods" })

// Valid
LOCAL O_XLS := HBOFFICE_XLS_OPEN( WORK_PATH() + "/../office/empty.ods" )
```

## Color parameters

Every color parameter is an 32bit integer rgb.

```
LOCAL N_Color := HBOFFICE_RGB(255, 0, 0)

PAR1: 0-255 red component.
PAR2: 0-255 green component.
PAR3: 0-255 blue component.
RET: The color (integer value).
```

## SpreadSheet

### Open Sheet

```
LOCAL O_XLS := HBOFFICE_XLS_OPEN({|| WORK_PATH() + "/../office/empty.ods" })

PAR1: String with the full path for the document.
RET: The document object. Must be closed with HBOFFICE_XLS_CLOSE().
```

### Create an empty Sheet

```
LOCAL O_XLS := HBOFFICE_XLS_CREATE()

RET: The document object. Must be closed with HBOFFICE_XLS_CLOSE().
```

### Save Sheet

```
HBOFFICE_XLS_SAVE(O_XLS, {|| WORK_PATH() + "/../office/edited.ods" })

PAR1: The sheet document.
PAR2: String with the full path for the document copy.
```

> **Important:** It will save a copy of document, but will not close the original one.


### Export Sheet to PDF

```
HBOFFICE_XLS_PDF(O_XLS, {|| WORK_PATH() + "/../office/ods_gen/Exemple_02.pdf" })

PAR1: The sheet document.
PAR2: String with the full path for the PDF file.
```

> **Important:** It will save a copy of document, but will not close the original one.


### Print the Sheet

```
HBOFFICE_XLS_PRINT(O_XLS, {|| WORK_PATH() + "/../office/ods_gen/Print_03.pdf" }, "Microsoft Print to PDF", HBOFFICE_PAPER_ORIENT_PORTRAIT, HBOFFICE_PAPER_FORMAT_A4, 0, 0, 1, .F., "1-")

PAR1: The sheet document.
PAR2: String with the full path for printed file (if prints to file).
PAR3: Printer name. If empty, it will use the default printer.
PAR4: Paper orientation. If 0 it will use the default option.
PAR5: Paper format. If 0 it will use the default option.
PAR6: Paper width, if we use HBOFFICE_PAPER_FORMAT_USER (integer 1/100th mm).
PAR7: Paper height, if we use HBOFFICE_PAPER_FORMAT_USER (integer 1/100th mm).
PAR8: Number of copies.
PAR9: Collapse copies (.T. / .F.).
PAR10: String with the pages to print in dialog format ("1-", "1-4;9", etc)
```

### Close Sheet

```
HBOFFICE_XLS_CLOSE(O_XLS)

PAR1: The sheet document.
```

> **Important:** All sheets open with `HBOFFICE_XLS_OPEN` or created with `HBOFFICE_XLS_CREATE` must be closed.


### Add new pages to Sheet

```
LOCAL N_Page := HBOFFICE_XLS_ADD(O_XLS)

PAR1: The sheet document.
RET: The page index (0-based).
```

> **Important:** `HBOFFICE_XLS_CREATE` creates a default 0-page.


### Set the page name

```
HBOFFICE_XLS_NAME(O_XLS, 0, "DEMOSTRA�AO RECEITA E DESPESA")

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: String with the name.
```

### Protect one page

```
HBOFFICE_XLS_PROTECT(O_XLS, 0, .T., "ASDF01234")

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Protect (.T.) or unprotect (.F.)
PAR4: String with the password.
```

### Freeze first rows and columns

```
HBOFFICE_XLS_FREEZE(O_XLS, 0, 2, 0)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Number of columns to freeze.
PAR4: Number of rows to freeze.
```

### Get Cell Reference for formulas

```
C_Ref := HBOFFICE_XLS_CELL_REF(O_XLS, 0, 0, 28)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
RET: A string with the cell reference "$'RREO-Anexo 01'.A29"
```

### Set Cell Text

```
HBOFFICE_XLS_CELL_TEXT(O_XLS, 0, 0, 0, {|| "Hello World!"})

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: String with the cell text.
```

### Set Cell Value

```
HBOFFICE_XLS_CELL_VALUE(O_XLS, 0, 0, 0, 917038.94)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Numeric value (double).
```

### Set Cell Date

```
HBOFFICE_XLS_CELL_DATE(O_XLS, 0, 0, 0, 31, 10, 2023)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Day.
PAR6: Month.
PAR7: Year.
```

### Set Cell Formula

```
HBOFFICE_XLS_CELL_FORMULA(O_XLS, 0, 10, 8, "B22+B23")

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: String with the formula.
```

### Cell Numeric Format

```
HBOFFICE_XLS_CELL_NUMFORMAT(O_XLS, 0, 0, 0, HBOFFICE_NUMFORMAT_INT)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Numeric constant with the format.

#define HBOFFICE_NUMFORMAT_INT           1
#define HBOFFICE_NUMFORMAT_INT_1000      2
#define HBOFFICE_NUMFORMAT_DEC2          3
#define HBOFFICE_NUMFORMAT_DEC2_1000     4
#define HBOFFICE_NUMFORMAT_PERC_INT      5
#define HBOFFICE_NUMFORMAT_PERC_DEC2     6

#define HBOFFICE_NUMFORMAT_DATE_SYS_DDMMM            7
#define HBOFFICE_NUMFORMAT_DATE_SYS_DDMMYY           8
#define HBOFFICE_NUMFORMAT_DATE_SYS_DDMMYYYY         9
#define HBOFFICE_NUMFORMAT_DATE_SYS_DMMMMYYYY        10
#define HBOFFICE_NUMFORMAT_DATE_SYS_DMMMYY           11
#define HBOFFICE_NUMFORMAT_DATE_SYS_DMMMYYYY         12
#define HBOFFICE_NUMFORMAT_DATE_SYS_MMYY             13
#define HBOFFICE_NUMFORMAT_DATE_SYS_NNDMMMMYYYY      14
#define HBOFFICE_NUMFORMAT_DATE_SYS_NNDMMMYY         15
#define HBOFFICE_NUMFORMAT_DATE_SYS_NNNNDMMMMYYYY    16
```

### Cell Font Family

```
HBOFFICE_XLS_CELL_FONT_FAMILY(O_XLS, 0, 0, 0, "Arial")

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: String with the font family.
```

### Cell Font Size

```
HBOFFICE_XLS_CELL_FONT_SIZE(O_XLS, 0, 0, 0, 9.0)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Font size (height). Double value.
```

### Cell Bold

```
HBOFFICE_XLS_CELL_BOLD(O_XLS, 0, 0, 0, .T.)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Font weight. Bold (.T.) or Normal (.F.).
```

### Cell Italic

```
HBOFFICE_XLS_CELL_ITALIC(O_XLS, 0, 0, 0, .T.)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Font style. Italic (.T.) or Normal (.F.).
```

### Cell Horizontal Alignment

```
HBOFFICE_XLS_CELL_HALIGN(O_XLS, 0, 0, 0, HBOFFICE_HALIGN_RIGHT)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Numeric value with horizontal alignment.

#define HBOFFICE_HALIGN_LEFT             1
#define HBOFFICE_HALIGN_CENTER           2
#define HBOFFICE_HALIGN_RIGHT            3
#define HBOFFICE_HALIGN_JUSTIFY          4
```

### Cell Vertical Alignment

```
HBOFFICE_XLS_CELL_VALIGN(O_XLS, 0, 0, 0, HBOFFICE_VALIGN_CENTER)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Numeric value with vertical alignment.

#define HBOFFICE_VALIGN_TOP              1
#define HBOFFICE_VALIGN_CENTER           2
#define HBOFFICE_VALIGN_BOTTOM           3
```

### Cell Text Wrap

```
HBOFFICE_XLS_CELL_WRAP(O_XLS, 0, 0, 0, .T.)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR: Wrap text (.T. / .F.).
```

### Cell Text Color

```
HBOFFICE_XLS_CELL_COLOR(O_XLS, 0, 0, 0, HBOFFICE_RGB(255, 255, 255))

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Color value returned by HBOFFICE_RGB
```

### Cell Background color

```
HBOFFICE_XLS_CELL_BACKCOLOR(O_XLS, 0, 0, 0, HBOFFICE_RGB(205, 205, 205))

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Color value returned by HBOFFICE_RGB.
```

### Background color for a group of cells

```
HBOFFICE_XLS_CELLS_BACKCOLOR(O_XLS, 0, 0, 0, 10, 8 HBOFFICE_RGB(205, 205, 205))

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column-left index (0-based).
PAR4: Row-top index (0-based).
PAR5: Column-right index >= column-left.
PAR6: Row-bottom index >= row-top.
PAR7: Color value returned by HBOFFICE_RGB.
```

> **Important:** It will clean the inner grid lines.


### Insert a image in cell position

```
HBOFFICE_XLS_CELL_IMAGE(O_XLS, 0, 0, 0, {|| WORK_PATH() + "/../office/ods/cell_image_01.png" })

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Image file path.
```

> **Important:** The image is not part of the cell (it is not an attribute). It is inserted into the document and positioned and sized in the cell frame.

### Cell border

```
HBOFFICE_XLS_CELL_BORDER(O_XLS, 0, 0, 0, HBOFFICE_LINE_STYLE_SOLID, 50, HBOFFICE_RGB(255, 255, 255))

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Row index (0-based).
PAR5: Line style.
PAR6: Line thickness (integer 1/100th mm)
PAR7: Color value returned by HBOFFICE_RGB.

#define HBOFFICE_LINE_STYLE_NONE                     1
#define HBOFFICE_LINE_STYLE_SOLID                    2
#define HBOFFICE_LINE_STYLE_DOTTED                   3
#define HBOFFICE_LINE_STYLE_DASHED                   4
#define HBOFFICE_LINE_STYLE_DOUBLE                   5
#define HBOFFICE_LINE_STYLE_THINTHICK_SMALLGAP       6
#define HBOFFICE_LINE_STYLE_THINTHICK_MEDIUMGAP      7
#define HBOFFICE_LINE_STYLE_THINTHICK_LARGEGAP       8
#define HBOFFICE_LINE_STYLE_THICKTHIN_SMALLGAP       9
#define HBOFFICE_LINE_STYLE_THICKTHIN_MEDIUMGAP      10
#define HBOFFICE_LINE_STYLE_THICKTHIN_LARGEGAP       11
#define HBOFFICE_LINE_STYLE_EMBOSSED                 12
#define HBOFFICE_LINE_STYLE_ENGRAVED                 13
#define HBOFFICE_LINE_STYLE_OUTSET                   14
#define HBOFFICE_LINE_STYLE_INSET                    15
#define HBOFFICE_LINE_STYLE_FINE_DASHED              16
#define HBOFFICE_LINE_STYLE_DOUBLE_THIN              17
#define HBOFFICE_LINE_STYLE_DASH_DOT                 18
#define HBOFFICE_LINE_STYLE_DASH_DOT_DOT             19
```

### Border for a group of cells

```
HBOFFICE_XLS_CELLS_BORDER(O_XLS, 0, 0, 0, 10, 8, HBOFFICE_LINE_STYLE_SOLID, 50, HBOFFICE_RGB(255, 255, 255))

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column-left index (0-based).
PAR4: Row-top index (0-based).
PAR5: Column-right index >= column-left.
PAR6: Row-bottom index >= row-top.
PAR7: Line style.
PAR8: Line thickness (integer 1/100th mm)
PAR9: Color value returned by HBOFFICE_RGB.
```

> **Important:** The inner lines will not be drawn. Only the border of entire group.


### Merge cells

```
HBOFFICE_XLS_CELLS_MERGE(O_XLS, 0, 2, 8, 3, 8)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column-left index (0-based).
PAR4: Row-top index (0-based).
PAR5: Column-right index >= column-left.
PAR6: Row-bottom index >= row-top.
```

### Column Visible

```
HBOFFICE_XLS_COLUMN_VISIBLE(O_XLS, 0, 0, .T.)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Column visible (.T. / .F.).
```

### Column Optimal Width

```
HBOFFICE_XLS_COLUMN_OPTIMAL_WIDTH(O_XLS, 0, 0, .F.)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Column with optimal width (.T. / .F.).
```

### Column Width

```
HBOFFICE_XLS_COLUMN_WIDTH(O_XLS, 0, 0, 6000)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Column index (0-based).
PAR4: Column width (integer 1/100th mm)
```

### Row Visible

```
HBOFFICE_XLS_ROW_VISIBLE(O_XLS, 0, 0, .T.)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Row index (0-based).
PAR4: Row visible (.T. / .F.).
```

### Row Optimal Height

```
HBOFFICE_XLS_ROW_OPTIMAL_HEIGHT(O_XLS, 0, 0, .F.)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Row index (0-based).
PAR4: Row with optimal height (.T. / .F.).
```

### Row Height

```
HBOFFICE_XLS_ROW_HEIGHT(O_XLS, 0, 0, 715)

PAR1: The sheet document.
PAR2: Page index (0-based).
PAR3: Row index (0-based).
PAR4: Row height (integer 1/100th mm)
```

## TextDocument

### Open TextDocument

```
LOCAL O_DOC := HBOFFICE_DOC_OPEN({|| WORK_PATH() + "/../office/empty.ods" })

PAR1: String with the full path for the document.
RET: The document object. Must be closed with HBOFFICE_DOC_CLOSE().
```

### Create an empty TextDocument

```
LOCAL O_DOC := HBOFFICE_DOC_CREATE()

RET: The document object. Must be closed with HBOFFICE_DOC_CLOSE().
```

### Save TextDocument

```
HBOFFICE_DOC_SAVE(O_DOC, {|| WORK_PATH() + "/../office/edited.odt" })

PAR1: The text document.
PAR2: String with the full path for the document copy.
```
> **Important:** It will save a copy of document, but will not close the original one.


### Export TextDocument to PDF

```
HBOFFICE_DOC_PDF(O_DOC, {|| WORK_PATH() + "/../office/ods_gen/ExempleText_02.pdf" })

PAR1: The text document.
PAR2: String with the full path for the PDF file.
```

> **Important:** It will save a copy of document, but will not close the original one.


### Print the TextDocument

```
HBOFFICE_DOC_PRINT(O_DOC, {|| WORK_PATH() + "/../office/ods_gen/PrintText_03.pdf" }, "Microsoft Print to PDF", HBOFFICE_PAPER_ORIENT_PORTRAIT, HBOFFICE_PAPER_FORMAT_A4, 0, 0, 1, .F., "1-")

PAR1: The text document.
PAR2: String with the full path for printed file (if prints to file).
PAR3: Printer name. If empty, it will use the default printer.
PAR4: Paper orientation. If 0 it will use the default option.
PAR5: Paper format. If 0 it will use the default option.
PAR6: Paper width, if we use HBOFFICE_PAPER_FORMAT_USER (integer 1/100th mm).
PAR7: Paper height, if we use HBOFFICE_PAPER_FORMAT_USER (integer 1/100th mm).
PAR8: Number of copies.
PAR9: Collapse copies (.T. / .F.).
PAR10: String with the pages to print in dialog format ("1-", "1-4;9", etc)
```

### Close TextDocument

```
HBOFFICE_DOC_CLOSE(O_DOC)

PAR1: The text document.
```

> **Important:** All documents opened with `HBOFFICE_DOC_OPEN` or created with `HBOFFICE_DOC_CREATE` must be closed.


### Page header show/hide

```
HBOFFICE_DOC_PAGE_HEADER_SHOW(O_DOC, .T.)

PAR1: The text document.
PAR2: Show/hide the page header (.T. / .F.).
```

### Page header margins

```
HBOFFICE_DOC_PAGE_HEADER_MARGINS(O_DOC, 254, 280, 508, 102, .T., .T.)

PAR1: The text document.
PAR2: Header left margin (integer 1/100th mm).
PAR3: Header right margin (integer 1/100th mm).
PAR4: Header-body distance (integer 1/100th mm).
PAR5: Header height (integer 1/100th mm).
PAR6: Dynamic header-body distance (.T. / .F.).
PAR7: Dynamic header height (.T. / .F.).
```

### Page footer show/hide

```
HBOFFICE_DOC_PAGE_FOOTER_SHOW(O_DOC, .T.)

PAR1: The text document.
PAR2: Show/hide the page footer (.T. / .F.).
```

### Page footer margins

```
HBOFFICE_DOC_PAGE_FOOTER_MARGINS(O_DOC, 254, 280, 508, 102, .T., .T.)

PAR1: The text document.
PAR2: Footer left margin (integer 1/100th mm).
PAR3: Footer right margin (integer 1/100th mm).
PAR4: Footer-body distance (integer 1/100th mm).
PAR5: Footer height (integer 1/100th mm).
PAR6: Dynamic footer-body distance (.T. / .F.).
PAR7: Dynamic footer height (.T. / .F.).
```

### Page margins

```
HBOFFICE_DOC_PAGE_MARGINS(O_DOC, 2819, 178, 1270, 533, 0)

PAR1: The text document.
PAR2: Left margin (integer 1/100th mm).
PAR3: Right margin (integer 1/100th mm).
PAR4: Top margin (integer 1/100th mm).
PAR5: Bottom margin (integer 1/100th mm).
PAR6: Gutter margin (integer 1/100th mm).
```

### Select Text Space

```
HBOFFICE_DOC_TEXT_SPACE(O_DOC, HBOFFICE_TEXT_SPACE_FOOTER)

PAR1: The text document.
PAR2: Text space.

#define HBOFFICE_TEXT_SPACE_HEADER                   1
#define HBOFFICE_TEXT_SPACE_FOOTER                   2
#define HBOFFICE_TEXT_SPACE_PAGE                     3
```

A text space redirects all subsequent text commands to a specific space (Page, Header or Footer). Each space is independent and is not affected by properties set in other spaces. By default `HBOFFICE_TEXT_SPACE_PAGE`. Commands related to font properties, paragraphs, and text insertion will only affect the currently active space.

### Text Font Family

```
HBOFFICE_DOC_FONT_FAMILY(O_DOC, "Arial")

PAR1: The text document.
PAR2: String with the font family.
```

> **Important:** This function will affect the next text added to the document.


### Text Font Size

```
HBOFFICE_DOC_FONT_SIZE(O_DOC, 9.0)

PAR1: The text document.
PAR2: Font size (height). Double value.
```

> **Important:** This function will affect the next text added to the document.


### Text Bold

```
HBOFFICE_DOC_BOLD(O_DOC, .T.)

PAR1: The text document.
PAR2: Font weight. Bold (.T.) or Normal (.F.).
```

> **Important:** This function will affect the next text added to the document.


### Text Italic

```
HBOFFICE_DOC_ITALIC(O_DOC, .T.)

PAR1: The text document.
PAR2: Font style. Italic (.T.) or Normal (.F.).
```
> **Important:** This function will affect the next text added to the document.


### Paragraph Alignment

```
HBOFFICE_DOC_PARAGRAPH_HALIGN(O_DOC, HBOFFICE_HALIGN_RIGHT)

PAR1: The text document.
PAR2: Numeric value with horizontal alignment.

#define HBOFFICE_HALIGN_LEFT             1
#define HBOFFICE_HALIGN_CENTER           2
#define HBOFFICE_HALIGN_RIGHT            3
#define HBOFFICE_HALIGN_JUSTIFY          4
```

> **Important:** This function will affect the next text added to the document.


### Paragraph inter-line spacing

```
HBOFFICE_DOC_PARAGRAPH_LSPACING(O_DOC, 330)

PAR1: The text document.
PAR2: Spacing (integer 1/100th mm).
```

> **Important:** This function will affect the next text added to the document.


### Insert text into document

```
HBOFFICE_DOC_INSERT_TEXT(O_DOC, "This is a new text")

PAR1: The text document.
PAR2: String with the text to insert.
```

> **Important:** The text will added at the end of document, using previously established properties.


### Insert dashes

```
HBOFFICE_DOC_INSERT_DASH(O_DOC, 130)

PAR1: The text document.
PAR2: Number of dashes to insert.
```

> **Important:** Will add a string of special character '-', useful to create separator lines.


### Insert image into text

```
HBOFFICE_DOC_INSERT_IMAGE(O_DOC, HBOFFICE_ANCHOR_AS_CHARACTER, 2000, 2000, HBOFFICE_HALIGN_LEFT, HBOFFICE_VALIGN_CENTER, "macapa.png")

PAR1: The text document.
PAR2: Type of anchor.
PAR3: Image width. 0 for original size.
PAR4: Image height. 0 for original size.
PAR5: Numeric value with horizontal alignment.
PAR6: Numeric value with vertical alignment.
PAR7: String with the full path to the image.

#define HBOFFICE_ANCHOR_AT_PARAGRAPH                 1
#define HBOFFICE_ANCHOR_AS_CHARACTER                 2
#define HBOFFICE_ANCHOR_AT_PAGE                      3
#define HBOFFICE_ANCHOR_AT_FRAME                     4
#define HBOFFICE_ANCHOR_AT_CHARACTER                 5

#define HBOFFICE_HALIGN_LEFT             1
#define HBOFFICE_HALIGN_CENTER           2
#define HBOFFICE_HALIGN_RIGHT            3
#define HBOFFICE_HALIGN_JUSTIFY          4

#define HBOFFICE_VALIGN_TOP              1
#define HBOFFICE_VALIGN_CENTER           2
#define HBOFFICE_VALIGN_BOTTOM           3
```

### Insert page number into text

```
HBOFFICE_DOC_INSERT_PAGE_NUMBER(O_DOC)

PAR1: The text document.
```

> Will insert a text field with current attributes that automatically show the page number.


### Insert a line break

```
HBOFFICE_DOC_INSERT_NEW_LINE(O_DOC)

PAR1: The text document.
```

### Insert a page break

```
HBOFFICE_DOC_INSERT_PAGE_BREAK(O_DOC)

PAR1: The text document.
```
