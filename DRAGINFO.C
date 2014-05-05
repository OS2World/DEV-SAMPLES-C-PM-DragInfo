/*****************************************************************************/
/*                                                                           */
/*  PROGRAM NAME:  DRAGINFO                                                  */
/*                                                                           */
/*  PURPOSE:       This program provides information for testing and         */
/*                 debugging drag-and-drop applications.  When an object is  */
/*                 dragged from this program's window, the window shows the  */
/*                 messages received from the target.                        */
/*                 Additionally, the Draginfo and Dragitem-structures that   */
/*                 are used to specify the requested operation can be        */
/*                 configured on screen.It is also possible to select the    */
/*                 desired Drag-API (DrgDrag or DrgDragFiles).               */
/*                 This program only simulates the dragging; no explicit     */
/*                 rendering is performed (yet).                             */
/*                                                                           */
/*                 This program was created using code from the DROPINFO     */
/*                 sample program that comes with the Redbook Vol.4 sample   */
/*                 code. Both programs complement eachother.                 */
/*                                                                           */
/*                 As far as I'm concerned you may use this code in whatever */
/*                 way you choose, provided I'm in no way responsible for    */
/*                 the outcome. Good luck.                                   */
/*                                                                           */
/*                 Christian Sell, Germany CIS 100021,3151                   */
/*****************************************************************************/

#include "draginfo.h"                            /* Application header file  */


/*****************************************************************************/
/* Window procedure prototypes                                               */
/*****************************************************************************/
MRESULT EXPENTRY MyWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
MRESULT EXPENTRY wpSubList( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
void PutMsg( USHORT unSrc, ULONG msg, MPARAM mp1, MPARAM mp2);
HWND DoDrgDrag( HWND hwndSrc );
void DoDragFiles( HWND hwnd );

/*****************************************************************************/
/* Global data                                                               */
/*****************************************************************************/
HAB  hAB;                       /* Anchor block handle      */
PFNWP pwpList;                  /* Listbox winproc address  */
HWND  hwndTarget;
HWND  hListBox;                 /* Listbox window handle    */
HPOINTER  hptrDrag;                 /* Drag Pointer handle */

/*****************************************************************************/
/* Application main routine                                                  */
/*****************************************************************************/
INT main (VOID)
{
  HMQ  hMsgQ;                                    /* Message queue handle     */
  QMSG qMsg;                                     /* Message structure        */
  HWND hFrame;                                   /* Frame window handle      */

  ULONG flCreate = FCF_STANDARD;     /* Frame creation flags     */

  ULONG rc;                                      /* Return code              */

  hAB = WinInitialize(0);                        /* Register application     */

  hMsgQ = WinCreateMsgQueue(hAB, 0);             /* Create message queue     */

  rc = WinRegisterClass(hAB,                     /* Register window class    */
                        (PSZ)"MyWindow",         /* Class name               */
                        (PFNWP)MyWindowProc,     /* Window procedure address */
                        CS_SIZEREDRAW,           /* Class style              */
                        sizeof(PVOID));          /* Window words             */

  hFrame = WinCreateStdWindow(HWND_DESKTOP,      /* Desktop is parent        */
                              0,                 /* Standard window style    */
                              &flCreate,         /* Frame control flags      */
                              "MyWindow",        /* Window class name        */
                              "DragInfo",        /* Window title text        */
                              0,                 /* No special class style   */
                              (HMODULE)0L,       /* Resources in EXE file    */
                              ID_WINDOW,         /* Frame window identifier  */
                              NULL);             /* No pres params           */

  while (WinGetMsg(hAB, &qMsg, 0L, 0, 0))        /* Process messages until   */
        WinDispatchMsg(hAB, &qMsg);              /* WM_QUIT received         */

  WinDestroyWindow(hFrame);                      /* Destroy window           */
  WinDestroyMsgQueue(hMsgQ);                     /* Destroy message queue    */
  WinTerminate(hAB);                             /* Deregister application   */
  
  return (rc);
}

/*****************************************************************************/
/* Main window procedure                                                     */
/*****************************************************************************/
MRESULT EXPENTRY MyWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  SWP   swp;
  ULONG rc;
  HWND  hFrame, hwndMenu;

  switch (msg)
  {
  case WM_CREATE:
       hListBox = WinCreateWindow(hwnd,
                                  WC_LISTBOX,
                                  NULL,
                                  WS_VISIBLE     |
                                  LS_NOADJUSTPOS |
                                  LS_HORZSCROLL,
                                  0, 0, 0, 0,
                                  hwnd,
                                  HWND_TOP,
                                  ID_LISTBOX,
                                  0,
                                  0);

       pwpList = WinSubclassWindow(hListBox,
                    wpSubList);

       hFrame = WinQueryWindow(hwnd,
                               QW_PARENT);
       rc = WinSetWindowPos(hFrame,
                            HWND_TOP,
                            50, 250,
                            300, 200,
                            SWP_MOVE     |
                            SWP_SIZE     |
                            SWP_ACTIVATE |
                            SWP_SHOW);
       hptrDrag = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ID_DRAGPTR);
                            
       break;

  case WM_SIZE:
       WinQueryWindowPos(hwnd, &swp);
       hListBox=WinWindowFromID(hwnd,
                                ID_LISTBOX);
       WinSetWindowPos(hListBox,
                       HWND_TOP,
                       swp.x, swp.y,
                       swp.cx, swp.cy,
                       SWP_SIZE  |
                       SWP_SHOW);
       break;

  /* just record drag messages sent to client window 
  */
  case DM_DROP:
  case DM_DRAGOVER:
  case DM_DRAGLEAVE:
  case DM_DROPHELP:
  case DM_ENDCONVERSATION:
  case DM_PRINT:
  case DM_RENDER:
  case DM_RENDERCOMPLETE:
  case DM_RENDERPREPARE:
  case DM_DRAGFILECOMPLETE:
  case DM_EMPHASIZETARGET:
  case DM_DRAGERROR:
  case DM_FILERENDERED:
  case DM_RENDERFILE:
  case DM_DRAGOVERNOTIFY:
  case DM_PRINTOBJECT:
  case DM_DISCARDOBJECT:
    PutMsg( 0, msg, mp1, mp2 );
    return (MRESULT)FALSE;
    break;

  case WM_MENUSELECT:
    switch( SHORT1FROMMP(mp1) )
    {
    case ID_OPTIONS:
      WinCheckMenuItem(HWNDFROMMP(mp2), IDM_DRGDRAG, bDrgDrag);
      WinCheckMenuItem(HWNDFROMMP(mp2), IDM_DRGDRAGFILES, !bDrgDrag);
      return (MRESULT)FALSE;

    default:
      return(WinDefWindowProc(hwnd, msg, mp1, mp2));
    }
  case WM_COMMAND:
    if( SHORT1FROMMP(mp2) == CMDSRC_MENU )
    {
      hwndMenu = WinWindowFromID(hFrame, FID_MENU);
      switch( SHORT1FROMMP(mp1) )
      {
      case IDM_DRGDRAG:
        bDrgDrag = TRUE;
        WinCheckMenuItem(hwndMenu, IDM_DRGDRAG, bDrgDrag);
        WinCheckMenuItem(hwndMenu, IDM_DRGDRAGFILES, !bDrgDrag);
        return (MRESULT)FALSE;
  
      case IDM_DRGDRAGFILES:
        bDrgDrag = FALSE;
        WinCheckMenuItem(hwndMenu, IDM_DRGDRAG, bDrgDrag);
        WinCheckMenuItem(hwndMenu, IDM_DRGDRAGFILES, !bDrgDrag);
        return (MRESULT)FALSE;
  
      case IDM_CONFIGDLG:
        if( bDrgDrag == TRUE )
          WinDlgBox(HWND_DESKTOP, hwnd, wpConfDrgDrag, NULLHANDLE,
                    IDD_CONFIG1, NULL);
        else
          WinDlgBox(HWND_DESKTOP, hwnd, wpConfDrgDragFiles, NULLHANDLE,
                    IDD_CONFIG2, NULL);
        return (MRESULT)TRUE;
    
      case IDM_CLEARLIST:
        WinSendMsg( hListBox, LM_DELETEALL, (MPARAM)0, (MPARAM)0 );
        return((MRESULT)TRUE);
    
      case IDM_DOSOMETHING:
        if( WinMessageBox(HWND_DESKTOP, hwnd, "OK, so do something else!",
                       "Message", 0, MB_OKCANCEL ) == MBID_OK )
          WinPostMsg( hwnd, WM_QUIT, (MPARAM)0, (MPARAM)0 );
        return((MRESULT)TRUE);
      }
    }
    break;

  default:
       return(WinDefWindowProc(hwnd,
                               msg,
                               mp1,
                               mp2));
  }
  return((MRESULT)FALSE);
}

/*****************************************************************************/
/* Listbox subclass window procedure                                         */
/*****************************************************************************/
MRESULT EXPENTRY wpSubList(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  switch (msg)
  {
  /***********************/
  /*Drag-Initialisierung */
  /***********************/
  case WM_BEGINDRAG:
    if( bDrgDrag == TRUE )
    {
      hwndTarget = DoDrgDrag( hwnd );
      return (MRESULT)(hwndTarget ? TRUE : FALSE);
    }
    else
    {
      DoDragFiles( hwnd );
      return (MRESULT)TRUE;
    }
    break;
  /****************************************/
  /* messages to drag source window       */
  /****************************************/
  case DM_DRAGOVERNOTIFY:   /* Objekt ber anderem window */
                            /* mp1: Draginfo, mp2: Target indicators */
    /*PutMsg( 1, msg, mp1, mp2 );*/
    return (MRESULT)FALSE;
    break;
  case DM_RENDER:           /* Render-Anforderung */
    /* mp1: DragTransfer, return: success/error */
    PutMsg( 1, msg, mp1, mp2 );
    return (MRESULT)FALSE;
    break;
  case DM_PRINTOBJECT:      /* Print-Anforderung */
    /* mp1: Draginfo, mp2: PrintDest, returns: action */
    PutMsg( 1, msg, mp1, mp2 );
    return (MRESULT)usDrgReturn;      
    break;
  case DM_DISCARDOBJECT:    /* Discard-Anforderung */
    /* mp1: Draginfo, mp2: reserved, returns: action */
    PutMsg( 1, msg, mp1, mp2 );
    return (MRESULT)usDrgReturn;  
    break;
  case DM_ENDCONVERSATION:  /* Ende-Meldung */
    /* mp1: ItemId, mp2: success/fail */ 
    PutMsg( 1, msg, mp1, mp2 );
    return (MRESULT)FALSE;
    break;
  /*****************************/
  /* messages to target window */
  /*****************************/
  case DM_DRAGOVER:
    /* mp1: Draginfo, mp2: Koordinaten, return: indicator */
    PutMsg( 1, msg, mp1, mp2 );
    return(MPFROM2SHORT(DOR_NEVERDROP, DO_UNKNOWN));
    break;
  case DM_EMPHASIZETARGET:
    /* mp1: Koordinaten, mp2: Flag */
  case DM_DRAGLEAVE:
    /* mp1: Draginfo */
  case DM_DROP:
    /* mp1: Draginfo, return: indicator */
  /**********************************/
  /*Messages fr DrgDragFiles usw.. */
  /**********************************/
  case DM_DRAGERROR:
    /* nur DrgDragFiles.. */
  case DM_FILERENDERED:
    /* mp1: Renderfile, mp2: success/fail */
  case DM_DRAGFILECOMPLETE:
    /* nur DrgDragFiles.. */
    PutMsg( 1, msg, mp1, mp2 );
    return (MRESULT)FALSE;
    break;

  } /* endswitch */

  return((MRESULT)pwpList(hwnd, msg, mp1, mp2));
}


/*************************************************************************/
/* fhis function performs the drag ...                                   */
/*************************************************************************/
HWND DoDrgDrag( HWND hwndSrc )
{
  PDRAGINFO     pDInfo;
  DRAGITEM      DItem;
  DRAGIMAGE     DImage;
  HWND          hDrop=NULLHANDLE;
  APIRET        rc;
 
  pDInfo = DrgAllocDraginfo(1);  /* Allocate DRAGINFO     */
  pDInfo->usOperation = usDrgOperation;

  /* Initialize DRAGITEM   */ 
  DItem.hwndItem            = hwndSrc;         
  DItem.ulItemID            = (ULONG)55;       /* nothing special */
  DItem.hstrType            = DrgAddStrHandle( szDrgType );
  DItem.hstrRMF             = DrgAddStrHandle( szDrgRMF );
  DItem.hstrContainerName   = DrgAddStrHandle( szPath );
  DItem.hstrSourceName      = DrgAddStrHandle( szFile );
  DItem.hstrTargetName      = DrgAddStrHandle( szFile );
  DItem.fsControl           = usDrgControl;
  DItem.cxOffset            = 2;
  DItem.cyOffset            = 2;
  DItem.fsSupportedOps      = DO_COPYABLE|DO_MOVEABLE|DO_LINKABLE;
 
  rc = DrgSetDragitem(pDInfo,    /* Set item in DRAGINFO  */
          &DItem,                /* Pointer to DRAGITEM   */
          sizeof(DItem),         /* Size of DRAGITEM      */
          0);                    /* Index of DRAGITEM     */
 
  DImage.cb = sizeof(DRAGIMAGE); /* Initialize DRAGIMAGE  */
  DImage.cptl = 0;               /* Not a polygon         */
  DImage.hImage = hptrDrag;      /* Icon handle for drag  */
  DImage.fl = DRG_ICON | DRG_TRANSPARENT;
  DImage.cxOffset = 0;           /* No hotspot            */
  DImage.cyOffset = 0;
 
  /*****************************/
  /* now do the drag           */
  /*****************************/
  hDrop = DrgDrag(hwndSrc,          /* Initiate drag         */
             pDInfo,             /* DRAGINFO structure    */
             &DImage,            /* DRAGIMAGE structure  */
             1,                  /* Only one DRAGIMAGE    */
             VK_ENDDRAG,         /* End of drag indicator */
             NULL);              /* Reserved              */
 
  DrgDeleteDraginfoStrHandles(pDInfo);
  DrgFreeDraginfo(pDInfo);       /* Free DRAGINFO struct  */

  return hDrop;
}


/**********************************************************/
/*this function performs the drag via the DrgDragFiles API*/
/**********************************************************/
void DoDragFiles( HWND hwnd )
{
  BOOL     fSuccess;
  PSZ      pFiles[1];
  PSZ      pTargets[1];

  pFiles[0] = szFilePath;
  pTargets[0] = NULL;
 
  if( !DrgDragFiles(hwnd, pFiles, NULL, pTargets, 1,
                   hptrDrag, VK_BUTTON2, bSourceRender, 0L) )
    PutMsg( 2, (ULONG)"DrgDragFiles failed", NULL, NULL );
  else
    PutMsg( 2, (ULONG)"DrgDragFiles OK", NULL, NULL );
}


/*****************************************************************************/
/* Put message info in listbox item                                          */
/*****************************************************************************/
void PutMsg( USHORT unSrc, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  char buf[100];                 /* Message buffer */

  switch( unSrc )
  {
  case 0:
    strcpy( buf, "message to client window: " );
    break;
  case 1:
    strcpy( buf, "message to listbox: " );
    break;
  default:
    strcpy( buf, "general: " );
    strcat( buf, (char *)msg );
  }

  switch( msg )
  {
  case DM_DROP:
    strcat( buf, "DM_DROP" );
    break;
  case DM_DRAGOVER:
    strcat( buf, "DM_DRAGOVER" );
    break;
  case DM_DRAGLEAVE:
    strcat( buf, "DM_DRAGLEAVE" );
    break;
  case DM_DROPHELP:
    strcat( buf, "DM_DROPHELP" );
    break;
  case DM_ENDCONVERSATION:
    strcat( buf, "DM_ENDCONVERSATION" );
    break;
  case DM_PRINT:
    strcat( buf, "DM_PRINT" );
    break;
  case DM_RENDER:
    strcat( buf, "DM_RENDER" );
    break;
  case DM_RENDERCOMPLETE:
    strcat( buf, "DM_RENDERCOMPLETE" );
    break;
  case DM_RENDERPREPARE:
    strcat( buf, "DM_RENDERPREPARE" );
    break;
  case DM_DRAGFILECOMPLETE:
    strcat( buf, "DM_DRAGFILECOMPLETE" );
    break;
  case DM_EMPHASIZETARGET:
    strcat( buf, "DM_EMPHASIZETARGET" );
    break;
  case DM_DRAGERROR:
    strcat( buf, "DM_DRAGERROR" );
    break;
  case DM_FILERENDERED:
    strcat( buf, "DM_FILERENDERED" );
    break;
  case DM_RENDERFILE:
    strcat( buf, "DM_RENDERFILE" );
    break;
  case DM_DRAGOVERNOTIFY:
    strcat( buf, "DM_DRAGOVERNOTIFY" );
    break;
  case DM_PRINTOBJECT:
    strcat( buf, "DM_PRINTOBJECT" );
    break;
  case DM_DISCARDOBJECT:
    strcat( buf, "DM_DISCARDOBJECT" );
    break;
  }
  WinSendMsg( hListBox, LM_INSERTITEM, (MPARAM)LIT_END, MPFROMP(buf));

  return;
}

 
