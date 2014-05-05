/***************************************************************************/
/* Dialog module for DRAGINFO                                              */
/***************************************************************************/
#include "draginfo.h"

 
static char *apcReturn[][2] = {
   { "DRR_SOURCE",  (char *)DRR_SOURCE },
   { "DRR_TARGET",  (char *)DRR_TARGET },
   { "DRR_ABORT",   (char *)DRR_ABORT },
   { NULL, NULL }
};

static char *apcControl[][2] = {
   { "DC_OPEN",              (char *)DC_OPEN },
   { "DC_REF",               (char *)DC_REF },
   { "DC_GROUP",             (char *)DC_GROUP },
   { "DC_CONTAINER",         (char *)DC_CONTAINER },
   { "DC_PREPARE",           (char *)DC_PREPARE },
   { "DC_REMOVEABLEMEDIA",   (char *)DC_REMOVEABLEMEDIA },
   { NULL, NULL }
};
static char *apcOperations[][2] = {
  {"DO_DEFAULT", (char *)DO_DEFAULT },
  {"DO_COPY",    (char *)DO_COPY },
  {"DO_LINK",    (char *)DO_LINK },
  {"DO_MOVE",    (char *)DO_MOVE },
  {"DO_CREATE",  (char *)DO_CREATE },
  {"DO_UNKNOWN", (char *)DO_UNKNOWN },
  { NULL, NULL }
};
static char *apcTypes[][2] = {
  {"DRT_TEXT",               "Plain Text"},
  {"DRT_UNKNOWN",            "Unknown"},
  {"DRT_TEXT(obj)",          "DRT_TEXT" },
  {"DRT_ASM",                "Assembler Code"},
  {"DRT_BASIC",              "BASIC Code"},
  {"DRT_BINDATA",            "Binary Data"},
  {"DRT_BITMAP",             "Bitmap"},
  {"DRT_C",                  "C Code"},
  {"DRT_COBOL",              "COBOL Code"},
  {"DRT_DLL",                "Dynamic Link Library"},
  {"DRT_DOSCMD",             "DOS Command File"},
  {"DRT_EXE",                "Executable"},
  {"DRT_FORTRAN",            "FORTRAN Code"},
  {"DRT_ICON",               "Icon"},
  {"DRT_LIB",                "Library"},
  {"DRT_METAFILE",           "Metafile"},
  {"DRT_OS2CMD",             "OS/2 Command File"},
  {"DRT_PASCAL",             "Pascal Code"},
  {"DRT_RESOURCE",           "Resource File"},
  { NULL, NULL }
};
static char *apcMechanisms[] = {
  "DRM_OS2FILE",
  "DRM_PRINT",
  "DRM_DDE",
  "DRM_OBJECT",
  "DRM_DISCARD",
  NULL
};
static char *apcFormats[] = {
  "DRF_TEXT",
  "DRF_UNKNOWN",
  "DRF_OBJECT",
  "DRF_BITMAP",
  "DRF_DIB",
  "DRF_DIF",
  "DRF_DSPBITMAP",
  "DRF_METAFILE",
  "DRF_OEMTEXT",
  "DRF_QWNERDISPLAY",
  "DRF_PTRPICT",
  "DRF_RTF",
  "DRF_SYLK",
  "DRF_TIFF",
  NULL
};

/*************************/
/* exportierte Variablen */
/*************************/
BOOL   bDrgDrag       = TRUE;
BOOL   bSourceRender  = TRUE;
USHORT usDrgControl   = DC_OPEN;
USHORT usDrgOperation = DO_DEFAULT;
USHORT usDrgReturn    = DRR_SOURCE;
PSZ    szDrgType      = DRT_TEXT;
CHAR   szDrgRMF[100]  = "<DRM_OS2FILE, DRF_TEXT>";
CHAR   szPath[CCHMAXPATHCOMP] = "D:\\TMP\\";
CHAR   szFile[256] = "TEST.TXT";
CHAR   szFilePath[CCHMAXPATHCOMP] = "D:\\TMP\\TEST.TXT";

static USHORT unRe=0;
static USHORT unOp=0;
static USHORT unTy=0;
static USHORT unMe=0;
static USHORT unFo=0;


USHORT InitConfigDlg( HWND hwnd );
void SetInfoParams( HWND hwnd );


/*******************************************/
/*Dialogprozedur fÅr Konfigurations-Dialog */
/*******************************************/
MRESULT EXPENTRY 
wpConfDrgDragFiles( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch( msg )
  {
  case WM_INITDLG:
    WinSendDlgItemMsg( hwnd, RB_SOURCETRUE, BM_SETCHECK, 
                       (MPARAM)bSourceRender, 0L );
    WinSendDlgItemMsg( hwnd, RB_SOURCEFALSE, BM_SETCHECK,
                       (MPARAM)(!bSourceRender), 0L );
    WinSetDlgItemText( hwnd, EF_FILEPATH, szFilePath );
    break;
  case WM_COMMAND:                 
    switch( SHORT1FROMMP( mp1 ) )
    {
    case DID_OK:
      bSourceRender = (BOOL)WinSendDlgItemMsg( hwnd, RB_SOURCETRUE,
                                         BM_QUERYCHECK, 0L, 0L );
      WinQueryDlgItemText( hwnd, EF_FILEPATH, 100L, szFilePath );
    case DID_CANCEL:
      WinDismissDlg( hwnd, TRUE );  /* Remove the dialog box    */
      break;
    }
  default:
    return WinDefDlgProc( hwnd, msg, mp1, mp2 );
  }
  return (MRESULT) FALSE;
}

/*******************************************/
/*Dialogprozedur fÅr Konfigurations-Dialog */
/*******************************************/
MRESULT EXPENTRY 
wpConfDrgDrag( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch ( msg )
  {
  case WM_INITDLG:
     InitConfigDlg(hwnd);
     break;

  case WM_COMMAND:                 /* Posted by pushbutton or key  */
    switch( SHORT1FROMMP( mp1 ) )     
    {
      case DID_OK:
        SetInfoParams( hwnd );
      case DID_CANCEL:
        WinDismissDlg( hwnd, TRUE );  /* Remove the dialog box    */
        return (MRESULT) FALSE;
      default:
        break;
    }
    break;
  default:
    return WinDefDlgProc( hwnd, msg, mp1, mp2 );
  }
  return (MRESULT) FALSE;
}
 

/****************************************/
/* Initialisieren der Dialog-Cotrols    */
/****************************************/
USHORT InitConfigDlg( HWND hwnd )
{
  HWND    hControl;
  USHORT  unCnt;

  /* set entryfields */
  WinSetDlgItemText( hwnd, EF_CONTAINER, szPath );
  WinSetDlgItemText( hwnd, EF_FILE, szFile );

  /* set combobox 'Operation' */
  hControl = WinWindowFromID(hwnd, CO_OPERATION);
  for( unCnt=0; apcOperations[unCnt][0]; unCnt++ )
  {
    WinSendMsg( hControl, LM_INSERTITEM, (MPARAM)LIT_END,
                            MPFROMP(apcOperations[unCnt][0]) );
  }
  WinSendMsg( hControl, LM_SELECTITEM, (MPARAM)unOp, (MPARAM)0 );
  WinSetDlgItemText( hwnd, CO_OPERATION, apcOperations[unOp][0] );

  /* set combobox 'Return' */
  hControl = WinWindowFromID(hwnd, CO_RETURN);
  for( unCnt=0; apcReturn[unCnt][0]; unCnt++ )
  {
    WinSendMsg( hControl, LM_INSERTITEM, (MPARAM)LIT_END,
                            MPFROMP(apcReturn[unCnt][0]) );
  }
  WinSendMsg( hControl, LM_SELECTITEM, (MPARAM)unRe, (MPARAM)0 );
  WinSetDlgItemText( hwnd, CO_RETURN, apcReturn[unRe][0] );

  /* set combobox 'Type' */
  hControl = WinWindowFromID(hwnd, CO_TYPE );
  for( unCnt=0; apcTypes[unCnt][0]; unCnt++ )
  {
    WinSendMsg( hControl, LM_INSERTITEM, (MPARAM)LIT_END,
                            MPFROMP(apcTypes[unCnt][0]) );
  }
  WinSendMsg( hControl, LM_SELECTITEM, (MPARAM)unTy, (MPARAM)0 );
  WinSetDlgItemText( hwnd, CO_TYPE, apcTypes[unTy][0] );

  /* set listbox 'Control' */
  hControl = WinWindowFromID(hwnd, CO_FCONTROL );
  for( unCnt=0; apcControl[unCnt][0]; unCnt++ )
  {
    WinSendMsg( hControl, LM_INSERTITEM, (MPARAM)LIT_END,
                            MPFROMP(apcControl[unCnt][0]) );
  }
  for( unCnt=0; apcControl[unCnt][0]; unCnt++ )
    if( (USHORT)(apcControl[unCnt][1]) & usDrgControl )
      WinSendMsg( hControl, LM_SELECTITEM, (MPARAM)unCnt, (MPARAM)TRUE );

  /* set combobox 'Mechanism' */
  hControl = WinWindowFromID(hwnd, CO_MECH );
  for( unCnt=0; apcMechanisms[unCnt]; unCnt++ )
  {
    WinSendMsg( hControl, LM_INSERTITEM, (MPARAM)LIT_END,
                            MPFROMP(apcMechanisms[unCnt]) );
  }
  WinSendMsg( hControl, LM_SELECTITEM, (MPARAM)unMe, (MPARAM)0 );
  WinSetDlgItemText( hwnd, CO_MECH, apcMechanisms[unMe]);

  /* set combobox 'Format' */
  hControl = WinWindowFromID(hwnd, CO_FORMAT );
  for( unCnt=0; apcFormats[unCnt]; unCnt++ )
  {
    WinSendMsg( hControl, LM_INSERTITEM, (MPARAM)LIT_END,
                            MPFROMP(apcFormats[unCnt]) );
  }
  WinSendMsg( hControl, LM_SELECTITEM, (MPARAM)unFo, (MPARAM)0 );
  WinSetDlgItemText( hwnd, CO_FORMAT, apcFormats[unFo] );

  return 0;
}


/****************************************************/
/*Belegen der Parameter fÅr die Drag-Drop-Operation */
/****************************************************/
void SetInfoParams( HWND hwnd )
{
#define BUFLEN CCHMAXPATHCOMP
  USHORT  unCnt, unIdx;
  CHAR    achBuf[BUFLEN];
  CHAR    szDrgMechanism[30];
  CHAR    szDrgFormat[30];

  if( WinQueryDlgItemText( hwnd, EF_CONTAINER, BUFLEN, achBuf ) )
    strcpy( szPath, achBuf );
  if( WinQueryDlgItemText( hwnd, EF_FILE, BUFLEN, achBuf ) )
    strcpy( szFile, achBuf );

  WinQueryDlgItemText( hwnd, CO_OPERATION, BUFLEN, achBuf );
  for( unOp=0,unCnt=0; apcOperations[unCnt][0]; unCnt++ )
  {
    if( !strcmp( apcOperations[unCnt][0], achBuf ) )
    {
      unOp = unCnt;
      usDrgOperation = (USHORT)apcOperations[unCnt][1];
      break;
    }
  }

  WinQueryDlgItemText( hwnd, CO_RETURN, BUFLEN, achBuf );
  for( unRe=0,unCnt=0; apcReturn[unCnt][0]; unCnt++ )
  {
    if( !strcmp( apcReturn[unCnt][0], achBuf ) )
    {
      unRe = unCnt;
      usDrgReturn = (USHORT)apcReturn[unCnt][1];
      break;
    }
  }

  WinQueryDlgItemText( hwnd, CO_TYPE, BUFLEN, achBuf );
  for( unTy=0, unCnt=0; apcTypes[unCnt][0]; unCnt++ )
  {
    if( !strcmp( apcTypes[unCnt][0], achBuf ) )
    {
      unTy = unCnt;
      szDrgType = apcTypes[unCnt][1];
      break;
    }
  }

  usDrgControl = 0;
  unIdx = (USHORT)WinSendMsg(WinWindowFromID(hwnd,CO_FCONTROL), 
                       LM_QUERYSELECTION, (MPARAM)LIT_FIRST, (MPARAM)0);
  while( unIdx != (USHORT)LIT_NONE )
  {
    usDrgControl |= (USHORT)(apcControl[unIdx][1]);
    unIdx = (USHORT)WinSendMsg(WinWindowFromID(hwnd,CO_FCONTROL),
                       LM_QUERYSELECTION, (MPARAM)unIdx, (MPARAM)0);
  }

  if( WinQueryDlgItemText( hwnd, CO_MECH, BUFLEN, achBuf ) )
  {
    strcpy( szDrgMechanism, achBuf );
    for( unMe=0, unCnt=0; apcMechanisms[unCnt]; unCnt++ )
    {
      if( !strcmp( apcMechanisms[unCnt], achBuf ) )
      {
        unMe = unCnt;
        break;
      }
    }
  }

  if( WinQueryDlgItemText( hwnd, CO_FORMAT, BUFLEN, achBuf ) )
  {
    strcpy( szDrgFormat, achBuf );
    for( unFo=0, unCnt=0; apcFormats[unCnt]; unCnt++ )
    {
      if( !strcmp( apcFormats[unCnt], achBuf ) )
      {
        unFo = unCnt;
        break;
      }
    }
  }
  sprintf( szDrgRMF, "<%s,%s>", szDrgMechanism, szDrgFormat );
}
