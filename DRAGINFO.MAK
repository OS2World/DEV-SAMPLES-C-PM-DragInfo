# IBM Developer's Workframe/2 Make File Creation run at 14:24:06 on 02/14/93

# Make File Creation run in directory:
#   D:\QUELLEN\C2\DRAGTEST;

.SUFFIXES:

.SUFFIXES: .c .rc

ALL: DRAGINFO.EXE \
     DRAGINFO.RES

draginfo.exe:  \
  DRAGDLG.OBJ \
  DRAGINFO.OBJ \
  DRAGINFO.RES \
  draginfo.DEF \
  DRAGINFO.MAK
   @REM @<<DRAGINFO.@0
     /CO /PM:PM +
     DRAGDLG.OBJ +
     DRAGINFO.OBJ
     draginfo.exe
     
     
     draginfo.DEF;
<<
   LINK386.EXE @DRAGINFO.@0
   RC DRAGINFO.RES draginfo.exe

{.}.rc.res:
   RC -r .\$*.RC

{.}.c.obj:
   ICC.EXE /Kbocgapexr /Ti /W2 /C .\$*.c

!include DRAGINFO.DEP
