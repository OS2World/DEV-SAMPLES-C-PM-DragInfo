/*********** DROPINFO C Sample Program Include file (.H) *************************/
/*                                                                            */
/*  The DROPINFO include file defines symbolic constants used                    */
/*  in the DROPINFO.C and .RC files.                                             */
/*  DROPINFO local procedure declarations may appear in this file to             */
/*  ensure have been declared before being used.                              */
/*                                                                            */
/******************************************************************************/

#define INCL_WIN
#define INCL_GPI

#define INCL_WINSTDDRAG
#include <os2.h>                /* PM header file           */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "drgconfg.h"     /* dialog header */

#define ID_WINDOW   256
#define ID_DRAGPTR  257
#define ID_EXITPROG 258
#define ID_LISTBOX  259
#define ID_OPTIONS  300

#define IDM_CONFIGDLG    301
#define IDM_CLEARLIST    302
#define IDM_DOSOMETHING  303
#define IDM_DRGDRAG      304
#define IDM_DRGDRAGFILES 305


extern BOOL   bDrgDrag;
extern BOOL   bSourceRender;
extern USHORT usDrgOperation;
extern USHORT usDrgReturn;
extern PSZ    szDrgType;
extern CHAR   szDrgRMF[];
extern CHAR   szPath[];
extern CHAR   szFile[];
extern CHAR   szFilePath[];
extern USHORT usDrgControl;

MRESULT EXPENTRY wpConfDrgDrag( HWND, ULONG, MPARAM, MPARAM );
MRESULT EXPENTRY wpConfDrgDragFiles( HWND, ULONG, MPARAM, MPARAM );
