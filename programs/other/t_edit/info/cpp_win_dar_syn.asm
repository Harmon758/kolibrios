macro wo txt,lf1,p1,p2,p3{
@@: db txt
rb @b+40-$
dd lf1
db p1
dw p2+0
db p3
}
count_colors_text dd (text-color_wnd_text)/4
count_key_words dd (f1-text)/48
color_cursor dd 0x0080ff
color_wnd_capt dd 0x004000
color_wnd_work dd 0x000000
color_wnd_bord dd 0x00ff00
color_select dd 0xc0c0c0
color_cur_text dd 0xc0c0c0
color_wnd_text:
	dd 0xffffff
	dd 0x00ffff
	dd 0x00a000
	dd 0x00ff00
	dd 0x808080
	dd 0x808000
	dd 0xa0a0ff
	dd 0xff0000
text:
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'�'>,0,0,,7
wo<'!'>,f1.69-f1,0,,1
wo<'!='>,f1.70-f1,0,,1
wo<'"'>,f1.71-f1,4,(92 shl 8)+34,3
wo<'# define'>,0,7,13,2
wo<'# ifndef'>,0,7,13,2
wo<'#define'>,0,7,13,2
wo<'#else'>,0,3,,2
wo<'#endif'>,0,3,,2
wo<'#if'>,0,3,,2
wo<'#ifndef'>,0,7,13,2
wo<'#include'>,f1.79-f1,3,,2
wo<'#pragma'>,0,3,,2
wo<'%'>,0,0,,1
wo<'&'>,f1.82-f1,0,,1
wo<'&&'>,f1.83-f1,0,,1
wo<'&='>,0,0,,1
wo<39>,0,4,(92 shl 8)+39,3
wo<'('>,0,0,,1
wo<')'>,0,0,,1
wo<'*'>,0,0,,1
wo<'*='>,0,0,,1
wo<'+'>,0,0,,1
wo<'+='>,0,0,,1
wo<','>,0,0,,1
wo<'-'>,0,0,,1
wo<'-='>,0,0,,1
wo<'->'>,f1.95-f1,0,,1
wo<'.'>,0,0,,1
wo<'/'>,0,0,,1
wo<'/*'>,f1.98-f1,4,47,4
wo<'//'>,f1.99-f1,4,13,4
wo<'/='>,0,0,,1
wo<'0'>,0,24,,5
wo<'1'>,0,24,,5
wo<'2'>,0,24,,5
wo<'3'>,0,24,,5
wo<'4'>,0,24,,5
wo<'5'>,0,24,,5
wo<'6'>,0,24,,5
wo<'7'>,0,24,,5
wo<'8'>,0,24,,5
wo<'9'>,0,24,,5
wo<':'>,0,0,,1
wo<';'>,0,0,,1
wo<'<'>,f1.113-f1,0,,1
wo<'<='>,f1.114-f1,0,,1
wo<'<conio.h>'>,0,3,,3
wo<'<cstring.h>'>,0,3,,3
wo<'<dir.h>'>,0,3,,3
wo<'<fstream.h>'>,0,3,,3
wo<'<iostream.h>'>,0,3,,3
wo<'<math.h>'>,0,3,,3
wo<'<owl\animctrl.h>'>,0,3,,3
wo<'<owl\appdict.h>'>,0,3,,3
wo<'<owl\applicat.h>'>,f1.123-f1,3,,3
wo<'<owl\bitmapga.h>'>,0,3,,3
wo<'<owl\bitset.h>'>,0,3,,3
wo<'<owl\button.h>'>,f1.126-f1,3,,3
wo<'<owl\buttonga.h>'>,f1.127-f1,3,,3
wo<'<owl\celarray.h>'>,0,3,,3
wo<'<owl\checkbox.h>'>,f1.129-f1,3,,3
wo<'<owl\checklst.h>'>,0,3,,3
wo<'<owl\chooseco.h>'>,f1.131-f1,3,,3
wo<'<owl\choosefo.h>'>,0,3,,3
wo<'<owl\clipboar.h>'>,0,3,,3
wo<'<owl\clipview.h>'>,0,3,,3
wo<'<owl\colmnhdr.h>'>,0,3,,3
wo<'<owl\color.h>'>,0,3,,3
wo<'<owl\combobox.h>'>,0,3,,3
wo<'<owl\commctrl.h>'>,0,3,,3
wo<'<owl\commdial.h>'>,0,3,,3
wo<'<owl\compat.h>'>,0,3,,3
wo<'<owl\contain.h>'>,0,3,,3
wo<'<owl\control.h>'>,0,3,,3
wo<'<owl\controlb.h>'>,f1.143-f1,3,,3
wo<'<owl\controlg.h>'>,0,3,,3
wo<'<owl\dc.h>'>,f1.145-f1,3,,3
wo<'<owl\decframe.h>'>,f1.146-f1,3,,3
wo<'<owl\decmdifr.h>'>,0,3,,3
wo<'<owl\defs.h>'>,0,3,,3
wo<'<owl\dialog.h>'>,0,3,,3
wo<'<owl\dibitmap.h>'>,0,3,,3
wo<'<owl\dispatch.h>'>,0,3,,3
wo<'<owl\docking.h>'>,0,3,,3
wo<'<owl\docmanag.h>'>,0,3,,3
wo<'<owl\doctpl.h>'>,0,3,,3
wo<'<owl\docview.h>'>,0,3,,3
wo<'<owl\draglist.h>'>,0,3,,3
wo<'<owl\edit.h>'>,0,3,,3
wo<'<owl\editfile.h>'>,0,3,,3
wo<'<owl\editsear.h>'>,0,3,,3
wo<'<owl\editview.h>'>,0,3,,3
wo<'<owl\eventhan.h>'>,0,3,,3
wo<'<owl\except.h>'>,0,3,,3
wo<'<owl\filedoc.h>'>,0,3,,3
wo<'<owl\findrepl.h>'>,0,3,,3
wo<'<owl\floatfra.h>'>,0,3,,3
wo<'<owl\framewin.h>'>,f1.166-f1,3,,3
wo<'<owl\gadget.h>'>,0,3,,3
wo<'<owl\gadgetwi.h>'>,0,3,,3
wo<'<owl\gauge.h>'>,0,3,,3
wo<'<owl\gdibase.h>'>,0,3,,3
wo<'<owl\gdiobjec.h>'>,f1.171-f1,3,,3
wo<'<owl\glyphbtn.h>'>,f1.172-f1,3,,3
wo<'<owl\groupbox.h>'>,0,3,,3
wo<'<owl\hlpmanag.h>'>,0,3,,3
wo<'<owl\hotkey.h>'>,0,3,,3
wo<'<owl\imagelst.h>'>,0,3,,3
wo<'<owl\inputdia.h>'>,0,3,,3
wo<'<owl\layoutco.h>'>,0,3,,3
wo<'<owl\layoutwi.h>'>,0,3,,3
wo<'<owl\listbox.h>'>,f1.180-f1,3,,3
wo<'<owl\listview.h>'>,0,3,,3
wo<'<owl\listwind.h>'>,0,3,,3
wo<'<owl\mailer.h>'>,0,3,,3
wo<'<owl\mci.h>'>,0,3,,3
wo<'<owl\mdi.h>'>,0,3,,3
wo<'<owl\mdichild.h>'>,0,3,,3
wo<'<owl\menu.h>'>,0,3,,3
wo<'<owl\menugadg.h>'>,0,3,,3
wo<'<owl\messageb.h>'>,0,3,,3
wo<'<owl\metafile.h>'>,0,3,,3
wo<'<owl\modegad.h>'>,0,3,,3
wo<'<owl\module.h>'>,0,3,,3
wo<'<owl\notetab.h>'>,0,3,,3
wo<'<owl\ocfevent.h>'>,0,3,,3
wo<'<owl\oledialg.h>'>,0,3,,3
wo<'<owl\oledoc.h>'>,0,3,,3
wo<'<owl\olefacto.h>'>,0,3,,3
wo<'<owl\oleframe.h>'>,0,3,,3
wo<'<owl\olemdifr.h>'>,0,3,,3
wo<'<owl\oleview.h>'>,0,3,,3
wo<'<owl\olewindo.h>'>,0,3,,3
wo<'<owl\opensave.h>'>,f1.202-f1,3,,3
wo<'<owl\owlall.h>'>,0,3,,3
wo<'<owl\owlcore.h>'>,0,3,,3
wo<'<owl\owldefs.h>'>,0,3,,3
wo<'<owl\owlpch.h>'>,0,3,,3
wo<'<owl\panespli.h>'>,0,3,,3
wo<'<owl\pch.h>'>,0,3,,3
wo<'<owl\picklist.h>'>,0,3,,3
wo<'<owl\pictwind.h>'>,0,3,,3
wo<'<owl\point.h>'>,0,3,,3
wo<'<owl\preview.h>'>,0,3,,3
wo<'<owl\prevwin.h>'>,0,3,,3
wo<'<owl\printdia.h>'>,0,3,,3
wo<'<owl\printer.h>'>,0,3,,3
wo<'<owl\profile.h>'>,0,3,,3
wo<'<owl\propsht.h>'>,0,3,,3
wo<'<owl\radiobut.h>'>,0,3,,3
wo<'<owl\rcntfile.h>'>,0,3,,3
wo<'<owl\resource.h>'>,0,3,,3
wo<'<owl\richedit.h>'>,0,3,,3
wo<'<owl\richedpr.h>'>,0,3,,3
wo<'<owl\rolldial.h>'>,0,3,,3
wo<'<owl\scrollba.h>'>,0,3,,3
wo<'<owl\scroller.h>'>,0,3,,3
wo<'<owl\serialze.h>'>,0,3,,3
wo<'<owl\shellitm.h>'>,0,3,,3
wo<'<owl\signatur.h>'>,0,3,,3
wo<'<owl\slider.h>'>,0,3,,3
wo<'<owl\splashwi.h>'>,0,3,,3
wo<'<owl\splitter.h>'>,0,3,,3
wo<'<owl\static.h>'>,0,3,,3
wo<'<owl\statusba.h>'>,0,3,,3
wo<'<owl\stgdoc.h>'>,0,3,,3
wo<'<owl\tabctrl.h>'>,0,3,,3
wo<'<owl\textgadg.h>'>,0,3,,3
wo<'<owl\timegadg.h>'>,0,3,,3
wo<'<owl\tinycapt.h>'>,0,3,,3
wo<'<owl\toolbox.h>'>,0,3,,3
wo<'<owl\tooltip.h>'>,0,3,,3
wo<'<owl\treewind.h>'>,0,3,,3
wo<'<owl\treewn16.h>'>,0,3,,3
wo<'<owl\uihandle.h>'>,0,3,,3
wo<'<owl\uihelper.h>'>,0,3,,3
wo<'<owl\updown.h>'>,0,3,,3
wo<'<owl\validate.h>'>,0,3,,3
wo<'<owl\vbxctl.h>'>,0,3,,3
wo<'<owl\version.h>'>,0,3,,3
wo<'<owl\window.h>'>,0,3,,3
wo<'<owl\windowev.h>'>,0,3,,3
wo<'<owl\wing.h>'>,0,3,,3
wo<'<owl\winsock.h>'>,0,3,,3
wo<'<owl\wskaddr.h>'>,0,3,,3
wo<'<owl\wskerr.h>'>,0,3,,3
wo<'<owl\wskhostm.h>'>,0,3,,3
wo<'<owl\wskservm.h>'>,0,3,,3
wo<'<owl\wsksock.h>'>,0,3,,3
wo<'<owl\wsksockd.h>'>,0,3,,3
wo<'<owl\wsksockm.h>'>,0,3,,3
wo<'<stdio.h>'>,0,3,,3
wo<'<string.h>'>,0,3,,3
wo<'<values.h>'>,0,3,,3
wo<'='>,f1.263-f1,0,,1
wo<'=='>,f1.264-f1,0,,1
wo<'>'>,f1.265-f1,0,,1
wo<'>='>,f1.266-f1,0,,1
wo<'ACS_AUTOPLAY'>,0,3,,2
wo<'ACS_CENTER'>,0,3,,2
wo<'ACS_TIMER'>,0,3,,2
wo<'ACS_TRANSPARENT'>,0,3,,2
wo<'ANSI_CHARSET'>,0,3,,2
wo<'ANSI_FIXED_FONT'>,f1.272-f1,3,,2
wo<'ANSI_VAR_FONT'>,f1.273-f1,3,,2
wo<'ARABIC_CHARSET'>,0,3,,2
wo<'AddString'>,0,3,,6
wo<'AdjustWindowRect'>,0,3,,6
wo<'AdjustWindowRectEx'>,0,3,,6
wo<'AngleArc'>,f1.278-f1,3,,6
wo<'Arc'>,f1.279-f1,3,,6
wo<'AssignContextMenu'>,0,3,,6
wo<'AssignMenu'>,f1.281-f1,3,,6
wo<'Attr'>,0,3,,6
wo<'BALTIC_CHARSET'>,0,3,,2
wo<'BF_RECT'>,f1.284-f1,3,,2
wo<'BF_TOP'>,f1.285-f1,3,,2
wo<'BLACKNESS'>,f1.286-f1,3,,2
wo<'BLACK_BRUSH'>,f1.287-f1,3,,2
wo<'BLACK_PEN'>,f1.288-f1,3,,2
wo<'BM_CLICK'>,0,3,,2
wo<'BM_GETCHECK'>,f1.290-f1,3,,2
wo<'BM_GETIMAGE'>,0,3,,2
wo<'BM_GETSTATE'>,f1.292-f1,3,,2
wo<'BM_SETCHECK'>,f1.293-f1,3,,2
wo<'BM_SETIMAGE'>,0,3,,2
wo<'BM_SETSTATE'>,f1.295-f1,3,,2
wo<'BM_SETSTYLE'>,f1.296-f1,3,,2
wo<'BN_CLICKED'>,0,3,,2
wo<'BN_DBLCLK'>,0,3,,2
wo<'BN_DISABLE'>,0,3,,2
wo<'BN_DOUBLECLICKED'>,0,3,,2
wo<'BN_HILITE'>,0,3,,2
wo<'BN_KILLFOCUS'>,0,3,,2
wo<'BN_PAINT'>,0,3,,2
wo<'BN_PUSHED'>,0,3,,2
wo<'BN_SETFOCUS'>,0,3,,2
wo<'BN_UNHILITE'>,0,3,,2
wo<'BN_UNPUSHED'>,0,3,,2
wo<'BST_CHECKED'>,0,3,,2
wo<'BST_FOCUS'>,0,3,,2
wo<'BST_INDETERMINATE'>,0,3,,2
wo<'BST_PUSHED'>,0,3,,2
wo<'BST_UNCHECKED'>,0,3,,2
wo<'BS_3STATE'>,0,3,,2
wo<'BS_AUTO3STATE'>,0,3,,2
wo<'BS_AUTOCHECKBOX'>,0,3,,2
wo<'BS_AUTORADIOBUTTON'>,0,3,,2
wo<'BS_BITMAP'>,0,3,,2
wo<'BS_BOTTOM'>,0,3,,2
wo<'BS_CENTER'>,0,3,,2
wo<'BS_CHECKBOX'>,0,3,,2
wo<'BS_DEFPUSHBUTTON'>,0,3,,2
wo<'BS_DIBPATTERN'>,0,3,,2
wo<'BS_DIBPATTERN8X8'>,0,3,,2
wo<'BS_DIBPATTERNPT'>,0,3,,2
wo<'BS_FLAT'>,0,3,,2
wo<'BS_GROUPBOX'>,0,3,,2
wo<'BS_HATCHED'>,0,3,,2
wo<'BS_HOLLOW'>,0,3,,2
wo<'BS_ICON'>,0,3,,2
wo<'BS_INDEXED'>,0,3,,2
wo<'BS_LEFT'>,0,3,,2
wo<'BS_LEFTTEXT'>,0,3,,2
wo<'BS_MULTILINE'>,0,3,,2
wo<'BS_NOTIFY'>,0,3,,2
wo<'BS_NULL'>,0,3,,2
wo<'BS_OWNERDRAW'>,0,3,,2
wo<'BS_PATTERN'>,0,3,,2
wo<'BS_PATTERN8X8'>,0,3,,2
wo<'BS_PUSHBOX'>,0,3,,2
wo<'BS_PUSHBUTTON'>,0,3,,2
wo<'BS_PUSHLIKE'>,0,3,,2
wo<'BS_RADIOBUTTON'>,0,3,,2
wo<'BS_RIGHT'>,0,3,,2
wo<'BS_RIGHTBUTTON'>,0,3,,2
wo<'BS_SOLID'>,f1.345-f1,3,,2
wo<'BS_TEXT'>,0,3,,2
wo<'BS_TOP'>,0,3,,2
wo<'BS_USERBUTTON'>,0,3,,2
wo<'BS_VCENTER'>,0,3,,2
wo<'BeginPath'>,f1.350-f1,3,,6
wo<'BitBlt'>,f1.351-f1,3,,6
wo<'BringWindowToTop'>,0,3,,6
wo<'CBS_AUTOHSCROLL'>,0,3,,2
wo<'CBS_DISABLENOSCROLL'>,f1.354-f1,3,,2
wo<'CBS_DROPDOWN'>,0,3,,2
wo<'CBS_DROPDOWNLIST'>,0,3,,2
wo<'CBS_HASSTRINGS'>,0,3,,2
wo<'CBS_LOWERCASE'>,0,3,,2
wo<'CBS_NOINTEGRALHEIGHT'>,f1.359-f1,3,,2
wo<'CBS_OEMCONVERT'>,0,3,,2
wo<'CBS_OWNERDRAWFIXED'>,0,3,,2
wo<'CBS_OWNERDRAWVARIABLE'>,0,3,,2
wo<'CBS_SIMPLE'>,0,3,,2
wo<'CBS_SORT'>,0,3,,2
wo<'CBS_UPPERCASE'>,0,3,,2
wo<'CB_ADDSTRING'>,f1.366-f1,3,,2
wo<'CB_DELETESTRING'>,f1.367-f1,3,,2
wo<'CB_DIR'>,0,3,,2
wo<'CB_FINDSTRING'>,0,3,,2
wo<'CB_GETCOUNT'>,f1.370-f1,3,,2
wo<'CB_GETCURSEL'>,f1.371-f1,3,,2
wo<'CB_GETEDITSEL'>,0,3,,2
wo<'CB_GETITEMDATA'>,0,3,,2
wo<'CB_GETLBTEXT'>,0,3,,2
wo<'CB_GETLBTEXTLEN'>,0,3,,2
wo<'CB_INSERTSTRING'>,0,3,,2
wo<'CB_LIMITTEXT'>,0,3,,2
wo<'CB_RESETCONTENT'>,0,3,,2
wo<'CB_SELECTSTRING'>,0,3,,2
wo<'CB_SETCURSEL'>,0,3,,2
wo<'CB_SETEDITSEL'>,0,3,,2
wo<'CB_SETITEMDATA'>,0,3,,2
wo<'CB_SHOWDROPDOWN'>,0,3,,2
wo<'CCS_ADJUSTABLE'>,0,3,,2
wo<'CCS_BOTTOM'>,0,3,,2
wo<'CCS_LEFT'>,0,3,,2
wo<'CCS_NODIVIDER'>,0,3,,2
wo<'CCS_NOMOVEX'>,0,3,,2
wo<'CCS_NOMOVEY'>,0,3,,2
wo<'CCS_NOPARENTALIGN'>,0,3,,2
wo<'CCS_NORESIZE'>,0,3,,2
wo<'CCS_RIGHT'>,0,3,,2
wo<'CCS_TOP'>,0,3,,2
wo<'CCS_VERT'>,0,3,,2
wo<'CF_ANSIONLY'>,0,3,,2
wo<'CF_APPLY'>,0,3,,2
wo<'CF_BITMAP'>,0,3,,2
wo<'CF_BOTH'>,f1.398-f1,3,,2
wo<'CF_DIB'>,0,3,,2
wo<'CF_DIF'>,0,3,,2
wo<'CF_DSPBITMAP'>,0,3,,2
wo<'CF_DSPENHMETAFILE'>,0,3,,2
wo<'CF_DSPMETAFILEPICT'>,0,3,,2
wo<'CF_DSPTEXT'>,0,3,,2
wo<'CF_EFFECTS'>,0,3,,2
wo<'CF_ENABLEHOOK'>,0,3,,2
wo<'CF_ENABLETEMPLATE'>,0,3,,2
wo<'CF_ENABLETEMPLATEHANDLE'>,0,3,,2
wo<'CF_ENHMETAFILE'>,0,3,,2
wo<'CF_FIXEDPITCHONLY'>,f1.410-f1,3,,2
wo<'CF_FORCEFONTEXIST'>,0,3,,2
wo<'CF_GDIOBJFIRST'>,0,3,,2
wo<'CF_GDIOBJLAST'>,0,3,,2
wo<'CF_HDROP'>,0,3,,2
wo<'CF_INITTOLOGFONTSTRUCT'>,0,3,,2
wo<'CF_LIMITSIZE'>,0,3,,2
wo<'CF_LOCALE'>,0,3,,2
wo<'CF_MAX'>,0,3,,2
wo<'CF_METAFILEPICT'>,0,3,,2
wo<'CF_NOFACESEL'>,0,3,,2
wo<'CF_NOOEMFONTS'>,f1.421-f1,3,,2
wo<'CF_NOSIMULATIONS'>,0,3,,2
wo<'CF_NOSIZESEL'>,0,3,,2
wo<'CF_NOSTYLESEL'>,0,3,,2
wo<'CF_NOVECTORFONTS'>,0,3,,2
wo<'CF_OEMTEXT'>,0,3,,2
wo<'CF_OWNERDISPLAY'>,0,3,,2
wo<'CF_PALETTE'>,0,3,,2
wo<'CF_PENDATA'>,0,3,,2
wo<'CF_PRINTERFONTS'>,0,3,,2
wo<'CF_PRIVATEFIRST'>,0,3,,2
wo<'CF_PRIVATELAST'>,0,3,,2
wo<'CF_RIFF'>,0,3,,2
wo<'CF_SCALABLEONLY'>,0,3,,2
wo<'CF_SCREENFONTS'>,0,3,,2
wo<'CF_SHOWHELP'>,0,3,,2
wo<'CF_SYLK'>,0,3,,2
wo<'CF_TEXT'>,0,3,,2
wo<'CF_TIFF'>,0,3,,2
wo<'CF_TTONLY'>,0,3,,2
wo<'CF_UNICODETEXT'>,0,3,,2
wo<'CF_USESTYLE'>,0,3,,2
wo<'CF_WAVE'>,0,3,,2
wo<'CF_WYSIWYG'>,0,3,,2
wo<'CHINESEBIG5_CHARSET'>,0,3,,2
wo<'CLIP_DEFAULT_PRECIS'>,0,3,,2
wo<'COLOR_ACTIVEBORDER'>,f1.447-f1,3,,2
wo<'COLOR_ACTIVECAPTION'>,f1.448-f1,3,,2
wo<'COLOR_APPWORKSPACE'>,0,3,,2
wo<'COLOR_BACKGROUND'>,f1.450-f1,3,,2
wo<'COLOR_BTNFACE'>,0,3,,2
wo<'COLOR_BTNHIGHLIGHT'>,0,3,,2
wo<'COLOR_BTNSHADOW'>,0,3,,2
wo<'COLOR_BTNTEXT'>,0,3,,2
wo<'COLOR_CAPTIONTEXT'>,f1.455-f1,3,,2
wo<'COLOR_GRAYTEXT'>,0,3,,2
wo<'COLOR_HIGHLIGHT'>,f1.457-f1,3,,2
wo<'COLOR_HIGHLIGHTTEXT'>,f1.458-f1,3,,2
wo<'COLOR_INACTIVEBORDER'>,f1.459-f1,3,,2
wo<'COLOR_INACTIVECAPTION'>,0,3,,2
wo<'COLOR_MENU'>,0,3,,2
wo<'COLOR_MENUTEXT'>,0,3,,2
wo<'COLOR_SCROLLBAR'>,0,3,,2
wo<'COLOR_WINDOW'>,f1.464-f1,3,,2
wo<'COLOR_WINDOWFRAME'>,0,3,,2
wo<'COLOR_WINDOWTEXT'>,f1.466-f1,3,,2
wo<'CREATE_ALWAYS'>,0,3,,2
wo<'CREATE_NEW'>,0,3,,2
wo<'CS_BYTEALIGNCLIENT'>,0,3,,2
wo<'CS_BYTEALIGNWINDOW'>,0,3,,2
wo<'CS_CLASSDC'>,0,3,,2
wo<'CS_DBLCLKS'>,0,3,,2
wo<'CS_GLOBALCLASS'>,0,3,,2
wo<'CS_HREDRAW'>,f1.474-f1,3,,2
wo<'CS_NOCLOSE'>,0,3,,2
wo<'CS_OWNDC'>,0,3,,2
wo<'CS_PARENTDC'>,f1.477-f1,3,,2
wo<'CS_SAVEBITS'>,f1.478-f1,3,,2
wo<'CS_VREDRAW'>,f1.479-f1,3,,2
wo<'CTLCOLOR_BTN'>,f1.480-f1,3,,2
wo<'CTLCOLOR_DLG'>,f1.481-f1,3,,2
wo<'CTLCOLOR_EDIT'>,f1.482-f1,3,,2
wo<'CTLCOLOR_LISTBOX'>,f1.483-f1,3,,2
wo<'CTLCOLOR_MSGBOX'>,f1.484-f1,3,,2
wo<'CTLCOLOR_SCROLLBAR'>,f1.485-f1,3,,2
wo<'CTLCOLOR_STATIC'>,f1.486-f1,3,,2
wo<'CW_USEDEFAULT'>,0,3,,2
wo<'CanClose'>,0,3,,6
wo<'CheckDlgButton'>,0,3,,6
wo<'CheckRadioButton'>,0,3,,6
wo<'ChildBroadcastMessage'>,0,3,,6
wo<'ChildWindowFromPoint'>,0,3,,6
wo<'ChildWithId'>,0,3,,6
wo<'Chord'>,f1.494-f1,3,,6
wo<'ClearFlag'>,0,3,,6
wo<'ClientToScreen'>,0,3,,6
wo<'CloseFigure'>,0,3,,6
wo<'CloseWindow'>,0,3,,6
wo<'Create'>,0,3,,6
wo<'CreateCaret'>,0,3,,6
wo<'CreateChildren'>,0,3,,6
wo<'CreateDIBSection'>,0,3,,6
wo<'DDL_ARCHIVE'>,0,3,,2
wo<'DDL_DIRECTORY'>,0,3,,2
wo<'DDL_DRIVES'>,0,3,,2
wo<'DECLARE_RESPONSE_TABLE'>,f1.506-f1,3,,2
wo<'DEFAULT_CHARSET'>,0,3,,2
wo<'DEFAULT_PALETTE'>,f1.508-f1,3,,2
wo<'DEFAULT_PITCH'>,0,3,,2
wo<'DEFAULT_QUALITY'>,0,3,,2
wo<'DEFINE_RESPONSE_TABLE'>,f1.511-f1,21,40,2
wo<'DEVICE_DEFAULT_FONT'>,f1.512-f1,3,,2
wo<'DKGRAY_BRUSH'>,f1.513-f1,3,,2
wo<'DLGC_BUTTON'>,0,3,,2
wo<'DLGC_DEFPUSHBUTTON'>,0,3,,2
wo<'DLGC_HASSETSEL'>,0,3,,2
wo<'DLGC_RADIOBUTTON'>,0,3,,2
wo<'DLGC_STATIC'>,0,3,,2
wo<'DLGC_UNDEFPUSHBUTTON'>,0,3,,2
wo<'DLGC_WANTALLKEYS'>,0,3,,2
wo<'DLGC_WANTARROWS'>,0,3,,2
wo<'DLGC_WANTCHARS'>,0,3,,2
wo<'DLGC_WANTMESSAGE'>,0,3,,2
wo<'DLGC_WANTTAB'>,0,3,,2
wo<'DLL_PROCESS_ATTACH'>,0,3,,2
wo<'DLL_PROCESS_DETACH'>,0,3,,2
wo<'DLL_THREAD_ATTACH'>,0,3,,2
wo<'DLL_THREAD_DETACH'>,0,3,,2
wo<'DM_GETDEFID'>,0,3,,2
wo<'DM_SETDEFID'>,0,3,,2
wo<'DPtoLP'>,0,3,,6
wo<'DRIVE_CDROM'>,f1.532-f1,3,,2
wo<'DRIVE_FIXED'>,f1.533-f1,3,,2
wo<'DRIVE_NO_ROOT_DIR'>,f1.534-f1,3,,2
wo<'DRIVE_RAMDISK'>,f1.535-f1,3,,2
wo<'DRIVE_REMOTE'>,f1.536-f1,3,,2
wo<'DRIVE_REMOVABLE'>,f1.537-f1,3,,2
wo<'DRIVE_UNKNOWN'>,f1.538-f1,3,,2
wo<'DSTINVERT'>,f1.539-f1,3,,2
wo<'DS_3DLOOK'>,0,3,,2
wo<'DS_ABSALIGN'>,0,3,,2
wo<'DS_CENTER'>,0,3,,2
wo<'DS_CENTERMOUSE'>,0,3,,2
wo<'DS_CONTEXTHELP'>,0,3,,2
wo<'DS_CONTROL'>,0,3,,2
wo<'DS_FIXEDSYS'>,0,3,,2
wo<'DS_LOCALEDIT'>,0,3,,2
wo<'DS_MODALFRAME'>,0,3,,2
wo<'DS_NOFAILCREATE'>,0,3,,2
wo<'DS_NOIDLEMSG'>,0,3,,2
wo<'DS_SETFONT'>,0,3,,2
wo<'DS_SETFOREGROUND'>,0,3,,2
wo<'DS_SYSMODAL'>,0,3,,2
wo<'DTS_APPCANPARSE'>,0,3,,2
wo<'DTS_LONGDATEFORMAT'>,0,3,,2
wo<'DTS_RIGHTALIGN'>,0,3,,2
wo<'DTS_SHORTDATEFORMAT'>,0,3,,2
wo<'DTS_SHOWNONE'>,0,3,,2
wo<'DTS_TIMEFORMAT'>,0,3,,2
wo<'DTS_UPDOWN'>,0,3,,2
wo<'DT_BOTTOM'>,f1.561-f1,3,,2
wo<'DT_CALCRECT'>,f1.562-f1,3,,2
wo<'DT_CENTER'>,f1.563-f1,3,,2
wo<'DT_EDITCONTROL'>,0,3,,2
wo<'DT_END_ELLIPSIS'>,0,3,,2
wo<'DT_EXPANDTABS'>,f1.566-f1,3,,2
wo<'DT_EXTERNALLEADING'>,f1.567-f1,3,,2
wo<'DT_INTERNAL'>,0,3,,2
wo<'DT_LEFT'>,f1.569-f1,3,,2
wo<'DT_MODIFYSTRING'>,0,3,,2
wo<'DT_NOCLIP'>,f1.571-f1,3,,2
wo<'DT_NOPREFIX'>,f1.572-f1,3,,2
wo<'DT_PATH_ELLIPSIS'>,0,3,,2
wo<'DT_RIGHT'>,f1.574-f1,3,,2
wo<'DT_RTLREADING'>,0,3,,2
wo<'DT_SINGLELINE'>,f1.576-f1,3,,2
wo<'DT_TABSTOP'>,f1.577-f1,3,,2
wo<'DT_TOP'>,f1.578-f1,3,,2
wo<'DT_VCENTER'>,f1.579-f1,3,,2
wo<'DT_WORDBREAK'>,f1.580-f1,3,,2
wo<'DT_WORD_ELLIPSIS'>,0,3,,2
wo<'DefWindowProc'>,0,3,,6
wo<'DefaultProcessing'>,0,3,,6
wo<'Destroy'>,0,3,,6
wo<'DestroyCaret'>,0,3,,6
wo<'DisableAutoCreate'>,0,3,,6
wo<'DisableTransfer'>,0,3,,6
wo<'Dispatch'>,0,3,,6
wo<'DoExecute'>,0,3,,6
wo<'DragAcceptFiles'>,0,3,,6
wo<'DrawFocusRect'>,f1.591-f1,3,,6
wo<'DrawIcon'>,f1.592-f1,3,,6
wo<'DrawMenuBar'>,0,3,,6
wo<'DrawText'>,f1.594-f1,3,,6
wo<'EASTEUROPE_CHARSET'>,0,3,,2
wo<'EDGE_ETCHED'>,0,3,,2
wo<'EDGE_SUNKEN'>,0,3,,2
wo<'EM_CANUNDO'>,0,3,,2
wo<'EM_CHARFROMPOS'>,0,3,,2
wo<'EM_EMPTYUNDOBUFFER'>,0,3,,2
wo<'EM_EXSETSEL'>,0,3,,2
wo<'EM_FMTLINES'>,0,3,,2
wo<'EM_GETFIRSTVISIBLELINE'>,0,3,,2
wo<'EM_GETHANDLE'>,0,3,,2
wo<'EM_GETIMESTATUS'>,0,3,,2
wo<'EM_GETLIMITTEXT'>,0,3,,2
wo<'EM_GETLINE'>,0,3,,2
wo<'EM_GETLINECOUNT'>,0,3,,2
wo<'EM_GETMARGINS'>,0,3,,2
wo<'EM_GETMODIFY'>,0,3,,2
wo<'EM_GETPASSWORDCHAR'>,0,3,,2
wo<'EM_GETRECT'>,0,3,,2
wo<'EM_GETSEL'>,0,3,,2
wo<'EM_GETTHUMB'>,0,3,,2
wo<'EM_GETWORDBREAKPROC'>,0,3,,2
wo<'EM_LIMITTEXT'>,0,3,,2
wo<'EM_LINEFROMCHAR'>,0,3,,2
wo<'EM_LINEINDEX'>,0,3,,2
wo<'EM_LINELENGTH'>,0,3,,2
wo<'EM_LINESCROLL'>,0,3,,2
wo<'EM_POSFROMCHAR'>,0,3,,2
wo<'EM_REPLACESEL'>,0,3,,2
wo<'EM_SCROLL'>,0,3,,2
wo<'EM_SCROLLCARET'>,0,3,,2
wo<'EM_SETHANDLE'>,0,3,,2
wo<'EM_SETIMESTATUS'>,0,3,,2
wo<'EM_SETLIMITTEXT'>,0,3,,2
wo<'EM_SETMARGINS'>,0,3,,2
wo<'EM_SETMODIFY'>,0,3,,2
wo<'EM_SETPASSWORDCHAR'>,0,3,,2
wo<'EM_SETREADONLY'>,0,3,,2
wo<'EM_SETRECT'>,0,3,,2
wo<'EM_SETRECTNP'>,0,3,,2
wo<'EM_SETSEL'>,0,3,,2
wo<'EM_SETTABSTOPS'>,0,3,,2
wo<'EM_SETWORDBREAK'>,0,3,,2
wo<'EM_SETWORDBREAKPROC'>,0,3,,2
wo<'EM_UNDO'>,0,3,,2
wo<'END_RESPONSE_TABLE'>,f1.639-f1,3,,2
wo<'EN_DROPFILES'>,f1.640-f1,3,,2
wo<'EN_PROTECTED'>,f1.641-f1,3,,2
wo<'EN_REQUESTRESIZE'>,f1.642-f1,3,,2
wo<'ES_AUTOHSCROLL'>,f1.643-f1,3,,2
wo<'ES_AUTOVSCROLL'>,f1.644-f1,3,,2
wo<'ES_CENTER'>,f1.645-f1,3,,2
wo<'ES_LEFT'>,0,3,,2
wo<'ES_LOWERCASE'>,0,3,,2
wo<'ES_MULTILINE'>,0,3,,2
wo<'ES_NOHIDESEL'>,0,3,,2
wo<'ES_NUMBER'>,0,3,,2
wo<'ES_OEMCONVERT'>,0,3,,2
wo<'ES_PASSWORD'>,0,3,,2
wo<'ES_READONLY'>,0,3,,2
wo<'ES_RIGHT'>,0,3,,2
wo<'ES_SUNKEN'>,0,3,,2
wo<'ES_UPPERCASE'>,0,3,,2
wo<'ES_WANTRETURN'>,0,3,,2
wo<'EV_CHILD_NOTIFY'>,0,3,,2
wo<'EV_CHILD_NOTIFY_ALL_CODES'>,0,3,,2
wo<'EV_CHILD_NOTIFY_AND_CODE'>,0,3,,2
wo<'EV_COMMAND'>,0,3,,2
wo<'EV_COMMAND_AND_ID'>,0,3,,2
wo<'EV_COMMAND_ENABLE'>,0,3,,2
wo<'EV_MESSAGE'>,0,3,,2
wo<'EV_NOTIFY_AT_CHILD'>,0,3,,2
wo<'EV_OWLDOCUMENT'>,0,3,,2
wo<'EV_OWLNOTIFY'>,0,3,,2
wo<'EV_OWLVIEW'>,0,3,,2
wo<'EV_REGISTERED'>,0,3,,2
wo<'EV_VIEWNOTIFY'>,0,3,,2
wo<'Ellipse'>,f1.671-f1,3,,6
wo<'EnableAutoCreate'>,0,3,,6
wo<'EnableScrollBar'>,0,3,,6
wo<'EnableTransfer'>,0,3,,6
wo<'EnableWindow'>,0,3,,6
wo<'EndPath'>,f1.676-f1,3,,6
wo<'EnumEnhMetaFile'>,0,3,,6
wo<'EnumFontFamilies'>,f1.678-f1,3,,6
wo<'EnumFonts'>,f1.679-f1,3,,6
wo<'EnumMetaFile'>,f1.680-f1,3,,6
wo<'EnumObjects'>,0,3,,6
wo<'EnumProps'>,0,3,,6
wo<'EvActivate'>,0,3,,6
wo<'EvActivateApp'>,0,3,,6
wo<'EvAskCBFormatName'>,0,3,,6
wo<'EvCancelMode'>,0,3,,6
wo<'EvChangeCBChain'>,0,3,,6
wo<'EvChar'>,0,3,,6
wo<'EvCharToItem'>,0,3,,6
wo<'EvChildActivate'>,0,3,,6
wo<'EvChildInvalid'>,0,3,,6
wo<'EvClose'>,0,3,,6
wo<'EvCommNotify'>,0,3,,6
wo<'EvCommand'>,0,3,,6
wo<'EvCommandEnable'>,0,3,,6
wo<'EvCompacting'>,0,3,,6
wo<'EvCompareItem'>,0,3,,6
wo<'EvCreate'>,0,3,,6
wo<'EvCtlColor'>,0,3,,6
wo<'EvDeadChar'>,0,3,,6
wo<'EvDeleteItem'>,0,3,,6
wo<'EvDestroy'>,0,3,,6
wo<'EvDestroyClipboard'>,0,3,,6
wo<'EvDevModeChange'>,0,3,,6
wo<'EvDrawClipboard'>,0,3,,6
wo<'EvDrawItem'>,0,3,,6
wo<'EvDropFiles'>,0,3,,6
wo<'EvEnable'>,0,3,,6
wo<'EvEndSession'>,0,3,,6
wo<'EvEnterIdle'>,0,3,,6
wo<'EvEraseBkgnd'>,0,3,,6
wo<'EvFontChange'>,0,3,,6
wo<'EvGetDlgCode'>,0,3,,6
wo<'EvGetFont'>,0,3,,6
wo<'EvGetMinMaxInfo'>,0,3,,6
wo<'EvGetText'>,0,3,,6
wo<'EvGetTextLength'>,0,3,,6
wo<'EvHScroll'>,0,3,,6
wo<'EvHScrollClipboard'>,0,3,,6
wo<'EvHotKey'>,0,3,,6
wo<'EvIconEraseBkgnd'>,0,3,,6
wo<'EvInitMenu'>,0,3,,6
wo<'EvInitMenuPopup'>,0,3,,6
wo<'EvInputFocus'>,0,3,,6
wo<'EvKeyDown'>,0,3,,6
wo<'EvKeyUp'>,0,3,,6
wo<'EvKillFocus'>,0,3,,6
wo<'EvLButtonDblClk'>,0,3,,6
wo<'EvLButtonDown'>,0,3,,6
wo<'EvLButtonUp'>,0,3,,6
wo<'EvMButtonDblClk'>,0,3,,6
wo<'EvMButtonDown'>,0,3,,6
wo<'EvMButtonUp'>,0,3,,6
wo<'EvMDIActivate'>,0,3,,6
wo<'EvMDICreate'>,0,3,,6
wo<'EvMDIDestroy'>,0,3,,6
wo<'EvMeasureItem'>,0,3,,6
wo<'EvMenuChar'>,0,3,,6
wo<'EvMenuSelect'>,0,3,,6
wo<'EvMouseActivate'>,0,3,,6
wo<'EvMouseMove'>,0,3,,6
wo<'EvMove'>,0,3,,6
wo<'EvNCActivate'>,0,3,,6
wo<'EvNCCalcSize'>,0,3,,6
wo<'EvNCCreate'>,0,3,,6
wo<'EvNCDestroy'>,0,3,,6
wo<'EvNCHitTest'>,0,3,,6
wo<'EvNCLButtonDblClk'>,0,3,,6
wo<'EvNCLButtonDown'>,0,3,,6
wo<'EvNCLButtonUp'>,0,3,,6
wo<'EvNCMButtonDblClk'>,0,3,,6
wo<'EvNCMButtonDown'>,0,3,,6
wo<'EvNCMButtonUp'>,0,3,,6
wo<'EvNCMouseMove'>,0,3,,6
wo<'EvNCPaint'>,0,3,,6
wo<'EvNCRButtonDblClk'>,0,3,,6
wo<'EvNCRButtonDown'>,0,3,,6
wo<'EvNCRButtonUp'>,0,3,,6
wo<'EvNextDlgCtl'>,0,3,,6
wo<'EvNotify'>,0,3,,6
wo<'EvOtherWindowCreated'>,0,3,,6
wo<'EvOtherWindowDestroyed'>,0,3,,6
wo<'EvPaint'>,0,3,,6
wo<'EvPaintClipboard'>,0,3,,6
wo<'EvPaintIcon'>,0,3,,6
wo<'EvPaletteChanged'>,0,3,,6
wo<'EvPaletteIsChanging'>,0,3,,6
wo<'EvParentNotify'>,0,3,,6
wo<'EvPower'>,0,3,,6
wo<'EvQueryDragIcon'>,0,3,,6
wo<'EvQueryEndSession'>,0,3,,6
wo<'EvQueryNewPalette'>,0,3,,6
wo<'EvQueryOpen'>,0,3,,6
wo<'EvQueueSync'>,0,3,,6
wo<'EvRButtonDblClk'>,0,3,,6
wo<'EvRButtonDown'>,0,3,,6
wo<'EvRButtonUp'>,0,3,,6
wo<'EvRenderAllFormats'>,0,3,,6
wo<'EvRenderFormat'>,0,3,,6
wo<'EvSetCursor'>,0,3,,6
wo<'EvSetFocus'>,0,3,,6
wo<'EvSetFont'>,0,3,,6
wo<'EvSetRedraw'>,0,3,,6
wo<'EvSetText'>,0,3,,6
wo<'EvShowWindow'>,0,3,,6
wo<'EvSize'>,0,3,,6
wo<'EvSizeClipboard'>,0,3,,6
wo<'EvSpoolerStatus'>,0,3,,6
wo<'EvSysChar'>,0,3,,6
wo<'EvSysColorChange'>,0,3,,6
wo<'EvSysCommand'>,0,3,,6
wo<'EvSysDeadChar'>,0,3,,6
wo<'EvSysKeyDown'>,0,3,,6
wo<'EvSysKeyUp'>,0,3,,6
wo<'EvSystemError'>,0,3,,6
wo<'EvTimeChange'>,0,3,,6
wo<'EvTimer'>,0,3,,6
wo<'EvVKeyToItem'>,0,3,,6
wo<'EvVScroll'>,0,3,,6
wo<'EvVScrollClipboard'>,0,3,,6
wo<'EvWinIniChange'>,0,3,,6
wo<'EvWindowPosChanged'>,0,3,,6
wo<'EvWindowPosChanging'>,0,3,,6
wo<'ExcludeClipRect'>,0,3,,6
wo<'ExcludeUpdateRgn'>,f1.805-f1,3,,6
wo<'Execute'>,0,3,,6
wo<'ExtFloodFill'>,f1.807-f1,3,,6
wo<'ExtTextOut'>,f1.808-f1,3,,6
wo<'FALSE'>,0,3,,2
wo<'FF_DECORATIVE'>,f1.810-f1,3,,2
wo<'FF_DONTCARE'>,f1.811-f1,3,,2
wo<'FF_MODERN'>,f1.812-f1,3,,2
wo<'FF_ROMAN'>,f1.813-f1,3,,2
wo<'FF_SCRIPT'>,f1.814-f1,3,,2
wo<'FF_SWISS'>,f1.815-f1,3,,2
wo<'FILE_ADD_FILE'>,0,3,,2
wo<'FILE_ADD_SUBDIRECTORY'>,0,3,,2
wo<'FILE_ALL_ACCESS'>,0,3,,2
wo<'FILE_APPEND_DATA'>,0,3,,2
wo<'FILE_ATTRIBUTE_ARCHIVE'>,0,3,,2
wo<'FILE_ATTRIBUTE_COMPRESSED'>,0,3,,2
wo<'FILE_ATTRIBUTE_DIRECTORY'>,0,3,,2
wo<'FILE_ATTRIBUTE_HIDDEN'>,0,3,,2
wo<'FILE_ATTRIBUTE_NORMAL'>,0,3,,2
wo<'FILE_ATTRIBUTE_READONLY'>,0,3,,2
wo<'FILE_ATTRIBUTE_SYSTEM'>,0,3,,2
wo<'FILE_ATTRIBUTE_TEMPORARY'>,0,3,,2
wo<'FILE_BEGIN'>,0,3,,2
wo<'FILE_CREATE_PIPE_INSTANCE'>,0,3,,2
wo<'FILE_CURRENT'>,0,3,,2
wo<'FILE_DELETE_CHILD'>,0,3,,2
wo<'FILE_END'>,0,3,,2
wo<'FILE_EXECUTE'>,0,3,,2
wo<'FILE_FLAG_BACKUP_SEMANTICS'>,0,3,,2
wo<'FILE_FLAG_DELETE_ON_CLOSE'>,0,3,,2
wo<'FILE_FLAG_NO_BUFFERING'>,0,3,,2
wo<'FILE_FLAG_OVERLAPPED'>,0,3,,2
wo<'FILE_FLAG_POSIX_SEMANTICS'>,0,3,,2
wo<'FILE_FLAG_RANDOM_ACCESS'>,0,3,,2
wo<'FILE_FLAG_SEQUENTIAL_SCAN'>,0,3,,2
wo<'FILE_FLAG_WRITE_THROUGH'>,0,3,,2
wo<'FILE_GENERIC_EXECUTE'>,0,3,,2
wo<'FILE_GENERIC_READ'>,0,3,,2
wo<'FILE_GENERIC_WRITE'>,0,3,,2
wo<'FILE_LIST_DIRECTORY'>,0,3,,2
wo<'FILE_NOTIFY_CHANGE_ATTRIBUTES'>,0,3,,2
wo<'FILE_NOTIFY_CHANGE_DIR_NAME'>,0,3,,2
wo<'FILE_NOTIFY_CHANGE_FILE_NAME'>,0,3,,2
wo<'FILE_NOTIFY_CHANGE_LAST_WRITE'>,0,3,,2
wo<'FILE_NOTIFY_CHANGE_SECURITY'>,0,3,,2
wo<'FILE_NOTIFY_CHANGE_SIZE'>,0,3,,2
wo<'FILE_READ_ATTRIBUTES'>,0,3,,2
wo<'FILE_READ_DATA'>,0,3,,2
wo<'FILE_READ_EA'>,0,3,,2
wo<'FILE_READ_PROPERTIES'>,0,3,,2
wo<'FILE_SHARE_READ'>,0,3,,2
wo<'FILE_SHARE_WRITE'>,0,3,,2
wo<'FILE_TRAVERSE'>,0,3,,2
wo<'FILE_WRITE_ATTRIBUTES'>,0,3,,2
wo<'FILE_WRITE_DATA'>,0,3,,2
wo<'FILE_WRITE_EA'>,0,3,,2
wo<'FILE_WRITE_PROPERTIES'>,0,3,,2
wo<'FLOODFILLBORDER'>,f1.863-f1,3,,2
wo<'FLOODFILLSURFACE'>,f1.864-f1,3,,2
wo<'FW_BLACK'>,f1.865-f1,3,,2
wo<'FW_BOLD'>,f1.866-f1,3,,2
wo<'FW_DEMIBOLD'>,f1.867-f1,3,,2
wo<'FW_DONTCARE'>,f1.868-f1,3,,2
wo<'FW_EXTRABOLD'>,f1.869-f1,3,,2
wo<'FW_EXTRALIGHT'>,f1.870-f1,3,,2
wo<'FW_HEAVY'>,f1.871-f1,3,,2
wo<'FW_LIGHT'>,f1.872-f1,3,,2
wo<'FW_MEDIUM'>,f1.873-f1,3,,2
wo<'FW_NORMAL'>,f1.874-f1,3,,2
wo<'FW_REGULAR'>,f1.875-f1,3,,2
wo<'FW_SEMIBOLD'>,f1.876-f1,3,,2
wo<'FW_THIN'>,f1.877-f1,3,,2
wo<'FW_ULTRABOLD'>,f1.878-f1,3,,2
wo<'FW_ULTRALIGHT'>,f1.879-f1,3,,2
wo<'FillPath'>,f1.880-f1,3,,6
wo<'FillRect'>,f1.881-f1,3,,6
wo<'FillRgn'>,f1.882-f1,3,,6
wo<'FirstThat'>,0,3,,6
wo<'FlashWindow'>,0,3,,6
wo<'FlattenPath'>,f1.885-f1,3,,6
wo<'FloodFill'>,f1.886-f1,3,,6
wo<'ForEach'>,0,3,,6
wo<'ForwardMessage'>,0,3,,6
wo<'FrameRect'>,f1.889-f1,3,,6
wo<'FrameRgn'>,0,3,,6
wo<'GB2312_CHARSET'>,0,3,,2
wo<'GENERIC_ALL'>,0,3,,2
wo<'GENERIC_EXECUTE'>,0,3,,2
wo<'GENERIC_READ'>,0,3,,2
wo<'GENERIC_WRITE'>,0,3,,2
wo<'GLU_AUTO_LOAD_MATRIX'>,0,3,,2
wo<'GLU_BEGIN'>,0,3,,2
wo<'GLU_CCW'>,0,3,,2
wo<'GLU_CULLING'>,0,3,,2
wo<'GLU_CW'>,0,3,,2
wo<'GLU_DISPLAY_MODE'>,0,3,,2
wo<'GLU_DOMAIN_DISTANCE'>,0,3,,2
wo<'GLU_EDGE_FLAG'>,0,3,,2
wo<'GLU_END'>,0,3,,2
wo<'GLU_ERROR'>,0,3,,2
wo<'GLU_EXTENSIONS'>,0,3,,2
wo<'GLU_EXTERIOR'>,0,3,,2
wo<'GLU_FALSE'>,0,3,,2
wo<'GLU_FILL'>,f1.909-f1,3,,2
wo<'GLU_FLAT'>,0,3,,2
wo<'GLU_INCOMPATIBLE_GL_VERSION'>,0,3,,2
wo<'GLU_INSIDE'>,0,3,,2
wo<'GLU_INTERIOR'>,0,3,,2
wo<'GLU_INVALID_ENUM'>,0,3,,2
wo<'GLU_INVALID_VALUE'>,0,3,,2
wo<'GLU_LINE'>,f1.916-f1,3,,2
wo<'GLU_MAP1_TRIM_2'>,0,3,,2
wo<'GLU_MAP1_TRIM_3'>,0,3,,2
wo<'GLU_NONE'>,0,3,,2
wo<'GLU_NURBS_ERROR1'>,0,3,,2
wo<'GLU_NURBS_ERROR10'>,0,3,,2
wo<'GLU_NURBS_ERROR11'>,0,3,,2
wo<'GLU_NURBS_ERROR12'>,0,3,,2
wo<'GLU_NURBS_ERROR13'>,0,3,,2
wo<'GLU_NURBS_ERROR14'>,0,3,,2
wo<'GLU_NURBS_ERROR15'>,0,3,,2
wo<'GLU_NURBS_ERROR16'>,0,3,,2
wo<'GLU_NURBS_ERROR17'>,0,3,,2
wo<'GLU_NURBS_ERROR18'>,0,3,,2
wo<'GLU_NURBS_ERROR19'>,0,3,,2
wo<'GLU_NURBS_ERROR2'>,0,3,,2
wo<'GLU_NURBS_ERROR20'>,0,3,,2
wo<'GLU_NURBS_ERROR21'>,0,3,,2
wo<'GLU_NURBS_ERROR22'>,0,3,,2
wo<'GLU_NURBS_ERROR23'>,0,3,,2
wo<'GLU_NURBS_ERROR24'>,0,3,,2
wo<'GLU_NURBS_ERROR25'>,0,3,,2
wo<'GLU_NURBS_ERROR26'>,0,3,,2
wo<'GLU_NURBS_ERROR27'>,0,3,,2
wo<'GLU_NURBS_ERROR28'>,0,3,,2
wo<'GLU_NURBS_ERROR29'>,0,3,,2
wo<'GLU_NURBS_ERROR3'>,0,3,,2
wo<'GLU_NURBS_ERROR30'>,0,3,,2
wo<'GLU_NURBS_ERROR31'>,0,3,,2
wo<'GLU_NURBS_ERROR32'>,0,3,,2
wo<'GLU_NURBS_ERROR33'>,0,3,,2
wo<'GLU_NURBS_ERROR34'>,0,3,,2
wo<'GLU_NURBS_ERROR35'>,0,3,,2
wo<'GLU_NURBS_ERROR36'>,0,3,,2
wo<'GLU_NURBS_ERROR37'>,0,3,,2
wo<'GLU_NURBS_ERROR4'>,0,3,,2
wo<'GLU_NURBS_ERROR5'>,0,3,,2
wo<'GLU_NURBS_ERROR6'>,0,3,,2
wo<'GLU_NURBS_ERROR7'>,0,3,,2
wo<'GLU_NURBS_ERROR8'>,0,3,,2
wo<'GLU_NURBS_ERROR9'>,0,3,,2
wo<'GLU_OUTLINE_PATCH'>,0,3,,2
wo<'GLU_OUTLINE_POLYGON'>,0,3,,2
wo<'GLU_OUTSIDE'>,0,3,,2
wo<'GLU_OUT_OF_MEMORY'>,0,3,,2
wo<'GLU_PARAMETRIC_ERROR'>,0,3,,2
wo<'GLU_PARAMETRIC_TOLERANCE'>,0,3,,2
wo<'GLU_PATH_LENGTH'>,0,3,,2
wo<'GLU_POINT'>,f1.964-f1,3,,2
wo<'GLU_SAMPLING_METHOD'>,0,3,,2
wo<'GLU_SAMPLING_TOLERANCE'>,0,3,,2
wo<'GLU_SILHOUETTE'>,0,3,,2
wo<'GLU_SMOOTH'>,0,3,,2
wo<'GLU_TESS_BEGIN'>,0,3,,2
wo<'GLU_TESS_BEGIN_DATA'>,0,3,,2
wo<'GLU_TESS_BOUNDARY_ONLY'>,0,3,,2
wo<'GLU_TESS_COMBINE'>,0,3,,2
wo<'GLU_TESS_COMBINE_DATA'>,0,3,,2
wo<'GLU_TESS_COORD_TOO_LARGE'>,0,3,,2
wo<'GLU_TESS_EDGE_FLAG'>,0,3,,2
wo<'GLU_TESS_EDGE_FLAG_DATA'>,0,3,,2
wo<'GLU_TESS_END'>,0,3,,2
wo<'GLU_TESS_END_DATA'>,0,3,,2
wo<'GLU_TESS_ERROR'>,0,3,,2
wo<'GLU_TESS_ERROR1'>,0,3,,2
wo<'GLU_TESS_ERROR2'>,0,3,,2
wo<'GLU_TESS_ERROR3'>,0,3,,2
wo<'GLU_TESS_ERROR4'>,0,3,,2
wo<'GLU_TESS_ERROR5'>,0,3,,2
wo<'GLU_TESS_ERROR6'>,0,3,,2
wo<'GLU_TESS_ERROR7'>,0,3,,2
wo<'GLU_TESS_ERROR8'>,0,3,,2
wo<'GLU_TESS_ERROR_DATA'>,0,3,,2
wo<'GLU_TESS_MAX_COORD'>,0,3,,2
wo<'GLU_TESS_MISSING_BEGIN_CONTOUR'>,0,3,,2
wo<'GLU_TESS_MISSING_BEGIN_POLYGON'>,0,3,,2
wo<'GLU_TESS_MISSING_END_CONTOUR'>,0,3,,2
wo<'GLU_TESS_MISSING_END_POLYGON'>,0,3,,2
wo<'GLU_TESS_NEED_COMBINE_CALLBACK'>,0,3,,2
wo<'GLU_TESS_TOLERANCE'>,0,3,,2
wo<'GLU_TESS_VERTEX'>,0,3,,2
wo<'GLU_TESS_VERTEX_DATA'>,0,3,,2
wo<'GLU_TESS_WINDING_ABS_GEQ_TWO'>,0,3,,2
wo<'GLU_TESS_WINDING_NEGATIVE'>,0,3,,2
wo<'GLU_TESS_WINDING_NONZERO'>,0,3,,2
wo<'GLU_TESS_WINDING_ODD'>,0,3,,2
wo<'GLU_TESS_WINDING_POSITIVE'>,0,3,,2
wo<'GLU_TESS_WINDING_RULE'>,0,3,,2
wo<'GLU_TRUE'>,0,3,,2
wo<'GLU_UNKNOWN'>,0,3,,2
wo<'GLU_U_STEP'>,0,3,,2
wo<'GLU_VERSION'>,0,3,,2
wo<'GLU_VERSION_1_1'>,0,3,,2
wo<'GLU_VERSION_1_2'>,0,3,,2
wo<'GLU_VERTEX'>,0,3,,2
wo<'GLU_V_STEP'>,0,3,,2
wo<'GL_ALL_ATTRIB_BITS'>,0,3,,2
wo<'GL_AMBIENT'>,f1.1013-f1,3,,2
wo<'GL_AMBIENT_AND_DIFFUSE'>,f1.1014-f1,3,,2
wo<'GL_COLOR_BUFFER_BIT'>,0,3,,2
wo<'GL_CULL_FACE'>,0,3,,2
wo<'GL_DEPTH_BUFFER_BIT'>,0,3,,2
wo<'GL_DEPTH_TEST'>,0,3,,2
wo<'GL_DIFFUSE'>,f1.1019-f1,3,,2
wo<'GL_EMISSION'>,f1.1020-f1,3,,2
wo<'GL_FRONT'>,0,3,,2
wo<'GL_LIGHT0'>,0,3,,2
wo<'GL_LIGHT1'>,0,3,,2
wo<'GL_LIGHT2'>,0,3,,2
wo<'GL_LIGHTING'>,0,3,,2
wo<'GL_LINE'>,0,3,,2
wo<'GL_LINES'>,0,3,,2
wo<'GL_LINE_LOOP'>,0,3,,2
wo<'GL_LINE_STRIP'>,0,3,,2
wo<'GL_NORMALIZE'>,0,3,,2
wo<'GL_POINTS'>,0,3,,2
wo<'GL_POLYGON'>,0,3,,2
wo<'GL_QUADS'>,0,3,,2
wo<'GL_QUAD_STRIP'>,0,3,,2
wo<'GL_SHININESS'>,f1.1035-f1,3,,2
wo<'GL_SMOOTH'>,0,3,,2
wo<'GL_SPECULAR'>,f1.1037-f1,3,,2
wo<'GL_TRIANGLES'>,0,3,,2
wo<'GL_TRIANGLE_FAN'>,0,3,,2
wo<'GL_TRIANGLE_STRIP'>,0,3,,2
wo<'GMEM_DDESHARE'>,f1.1041-f1,3,,2
wo<'GMEM_DISCARDABLE'>,f1.1042-f1,3,,2
wo<'GMEM_DISCARDED'>,f1.1043-f1,3,,2
wo<'GMEM_FIXED'>,f1.1044-f1,3,,2
wo<'GMEM_INVALID_HANDLE'>,0,3,,2
wo<'GMEM_LOCKCOUNT'>,f1.1046-f1,3,,2
wo<'GMEM_LOWER'>,f1.1047-f1,3,,2
wo<'GMEM_MODIFY'>,f1.1048-f1,3,,2
wo<'GMEM_MOVEABLE'>,f1.1049-f1,3,,2
wo<'GMEM_NOCOMPACT'>,f1.1050-f1,3,,2
wo<'GMEM_NODISCARD'>,f1.1051-f1,3,,2
wo<'GMEM_NOTIFY'>,f1.1052-f1,3,,2
wo<'GMEM_NOT_BANKED'>,f1.1053-f1,3,,2
wo<'GMEM_SHARE'>,f1.1054-f1,3,,2
wo<'GMEM_VALID_FLAGS'>,0,3,,2
wo<'GMEM_ZEROINIT'>,f1.1056-f1,3,,2
wo<'GRAY_BRUSH'>,f1.1057-f1,3,,2
wo<'GREEK_CHARSET'>,0,3,,2
wo<'GWL_EXSTYLE'>,0,3,,2
wo<'GWL_HINSTANCE'>,0,3,,2
wo<'GWL_ID'>,0,3,,2
wo<'GWL_STYLE'>,0,3,,2
wo<'GWL_USERDATA'>,0,3,,2
wo<'GWL_WNDPROC'>,0,3,,2
wo<'GW_HWNDFIRST'>,0,3,,2
wo<'GW_HWNDLAST'>,0,3,,2
wo<'GW_HWNDNEXT'>,0,3,,2
wo<'GW_HWNDPREV'>,0,3,,2
wo<'GetActiveWindow'>,0,3,,6
wo<'GetApplication'>,0,3,,6
wo<'GetAspectRatioFilter'>,f1.1071-f1,3,,6
wo<'GetAttributeHDC'>,f1.1072-f1,3,,6
wo<'GetBkColor'>,f1.1073-f1,3,,6
wo<'GetBkMode'>,f1.1074-f1,3,,6
wo<'GetBoundsRect'>,f1.1075-f1,3,,6
wo<'GetBrushOrg'>,f1.1076-f1,3,,6
wo<'GetCapture'>,0,3,,6
wo<'GetCaretBlinkTime'>,0,3,,6
wo<'GetCaretPos'>,0,3,,6
wo<'GetCharABCWidths'>,f1.1080-f1,3,,6
wo<'GetCharWidth'>,f1.1081-f1,3,,6
wo<'GetClassLong'>,0,3,,6
wo<'GetClassName'>,0,3,,6
wo<'GetClassWord'>,0,3,,6
wo<'GetClientRect'>,0,3,,6
wo<'GetClipBox'>,f1.1086-f1,3,,6
wo<'GetClipRgn'>,f1.1087-f1,3,,6
wo<'GetContextMenu'>,0,3,,6
wo<'GetCurrentEvent'>,0,3,,6
wo<'GetCurrentObject'>,f1.1090-f1,3,,6
wo<'GetCurrentPosition'>,f1.1091-f1,3,,6
wo<'GetCursorPos'>,0,3,,6
wo<'GetDCOrg'>,f1.1093-f1,3,,6
wo<'GetDIBits'>,0,3,,6
wo<'GetDesktopWindow'>,0,3,,6
wo<'GetDeviceCaps'>,f1.1096-f1,3,,6
wo<'GetDlgCtrlID'>,0,3,,6
wo<'GetDlgItem'>,0,3,,6
wo<'GetDlgItemInt'>,0,3,,6
wo<'GetDlgItemText'>,0,3,,6
wo<'GetExStyle'>,0,3,,6
wo<'GetFirstChild'>,0,3,,6
wo<'GetFocus'>,0,3,,6
wo<'GetFontData'>,f1.1104-f1,3,,6
wo<'GetGlyphOutline'>,0,3,,6
wo<'GetHDC'>,f1.1106-f1,3,,6
wo<'GetHWndState'>,0,3,,6
wo<'GetHandle'>,0,3,,6
wo<'GetId'>,0,3,,6
wo<'GetKerningPairs'>,f1.1110-f1,3,,6
wo<'GetLastActivePopup'>,0,3,,6
wo<'GetLastChild'>,0,3,,6
wo<'GetMapMode'>,f1.1113-f1,3,,6
wo<'GetMenu'>,0,3,,6
wo<'GetModule'>,0,3,,6
wo<'GetNearestColor'>,f1.1116-f1,3,,6
wo<'GetNextDlgGroupItem'>,0,3,,6
wo<'GetNextDlgTabItem'>,0,3,,6
wo<'GetNextWindow'>,0,3,,6
wo<'GetOutlineTextMetrics'>,f1.1120-f1,3,,6
wo<'GetParent'>,0,3,,6
wo<'GetParentH'>,0,3,,6
wo<'GetParentO'>,0,3,,6
wo<'GetPixel'>,f1.1124-f1,3,,6
wo<'GetPolyFillMode'>,f1.1125-f1,3,,6
wo<'GetProp'>,0,3,,6
wo<'GetROP2'>,0,3,,6
wo<'GetScrollPos'>,0,3,,6
wo<'GetScrollRange'>,0,3,,6
wo<'GetScroller'>,0,3,,6
wo<'GetStretchBltMode'>,f1.1131-f1,3,,6
wo<'GetStyle'>,0,3,,6
wo<'GetSysModalWindow'>,0,3,,6
wo<'GetSystemMenu'>,0,3,,6
wo<'GetSystemMetrics'>,0,3,,6
wo<'GetSystemPaletteEntries'>,f1.1136-f1,3,,6
wo<'GetSystemPaletteUse'>,f1.1137-f1,3,,6
wo<'GetTabbedTextExtent'>,f1.1138-f1,3,,6
wo<'GetTextAlign'>,f1.1139-f1,3,,6
wo<'GetTextCharacterExtra'>,f1.1140-f1,3,,6
wo<'GetTextColor'>,f1.1141-f1,3,,6
wo<'GetTextExtent'>,f1.1142-f1,3,,6
wo<'GetTextFace'>,f1.1143-f1,3,,6
wo<'GetTextMetrics'>,0,3,,6
wo<'GetThunk'>,0,3,,6
wo<'GetTopWindow'>,0,3,,6
wo<'GetUpdateRect'>,0,3,,6
wo<'GetUpdateRgn'>,0,3,,6
wo<'GetViewportExt'>,0,3,,6
wo<'GetViewportOrg'>,f1.1150-f1,3,,6
wo<'GetWindow'>,0,3,,6
wo<'GetWindowExt'>,f1.1152-f1,3,,6
wo<'GetWindowFont'>,0,3,,6
wo<'GetWindowLong'>,0,3,,6
wo<'GetWindowOrg'>,0,3,,6
wo<'GetWindowPlacement'>,0,3,,6
wo<'GetWindowRect'>,0,3,,6
wo<'GetWindowTask'>,0,3,,6
wo<'GetWindowText'>,0,3,,6
wo<'GetWindowTextLength'>,0,3,,6
wo<'GetWindowTextTitle'>,0,3,,6
wo<'GetWindowWord'>,0,3,,6
wo<'GrayString'>,f1.1163-f1,3,,6
wo<'HANGEUL_CHARSET'>,0,3,,2
wo<'HDC'>,0,3,,6
wo<'HDS_BUTTONS'>,0,3,,2
wo<'HDS_DRAGDROP'>,0,3,,2
wo<'HDS_FULLDRAG'>,0,3,,2
wo<'HDS_HIDDEN'>,0,3,,2
wo<'HDS_HORZ'>,0,3,,2
wo<'HDS_HOTTRACK'>,0,3,,2
wo<'HEBREW_CHARSET'>,0,3,,2
wo<'HELP_COMMAND'>,0,3,,2
wo<'HELP_CONTENTS'>,0,3,,2
wo<'HELP_CONTEXT'>,0,3,,2
wo<'HELP_CONTEXTMENU'>,0,3,,2
wo<'HELP_CONTEXTPOPUP'>,0,3,,2
wo<'HELP_FINDER'>,0,3,,2
wo<'HELP_FORCEFILE'>,0,3,,2
wo<'HELP_HELPONHELP'>,0,3,,2
wo<'HELP_INDEX'>,0,3,,2
wo<'HELP_KEY'>,0,3,,2
wo<'HELP_MULTIKEY'>,0,3,,2
wo<'HELP_PARTIALKEY'>,0,3,,2
wo<'HELP_QUIT'>,0,3,,2
wo<'HELP_SETCONTENTS'>,0,3,,2
wo<'HELP_SETINDEX'>,0,3,,2
wo<'HELP_SETPOPUP_POS'>,0,3,,2
wo<'HELP_SETWINPOS'>,0,3,,2
wo<'HELP_TCARD'>,0,3,,2
wo<'HELP_TCARD_DATA'>,0,3,,2
wo<'HELP_TCARD_OTHER_CALLER'>,0,3,,2
wo<'HELP_WM_HELP'>,0,3,,2
wo<'HIDE_WINDOW'>,0,3,,2
wo<'HOLLOW_BRUSH'>,f1.1195-f1,3,,2
wo<'HOVER_DEFAULT'>,0,3,,2
wo<'HTBORDER'>,0,3,,2
wo<'HTBOTTOM'>,0,3,,2
wo<'HTBOTTOMLEFT'>,0,3,,2
wo<'HTBOTTOMRIGHT'>,0,3,,2
wo<'HTCAPTION'>,0,3,,2
wo<'HTCLIENT'>,0,3,,2
wo<'HTCLOSE'>,0,3,,2
wo<'HTERROR'>,0,3,,2
wo<'HTGROWBOX'>,0,3,,2
wo<'HTHELP'>,0,3,,2
wo<'HTHSCROLL'>,0,3,,2
wo<'HTLEFT'>,0,3,,2
wo<'HTMAXBUTTON'>,0,3,,2
wo<'HTMENU'>,0,3,,2
wo<'HTMINBUTTON'>,0,3,,2
wo<'HTNOWHERE'>,0,3,,2
wo<'HTOBJECT'>,0,3,,2
wo<'HTREDUCE'>,0,3,,2
wo<'HTRIGHT'>,0,3,,2
wo<'HTSIZE'>,0,3,,2
wo<'HTSIZEFIRST'>,0,3,,2
wo<'HTSIZELAST'>,0,3,,2
wo<'HTSYSMENU'>,0,3,,2
wo<'HTTOP'>,0,3,,2
wo<'HTTOPLEFT'>,0,3,,2
wo<'HTTOPRIGHT'>,0,3,,2
wo<'HTTRANSPARENT'>,0,3,,2
wo<'HTVSCROLL'>,0,3,,2
wo<'HTZOOM'>,0,3,,2
wo<'HWND'>,f1.1226-f1,3,,6
wo<'HandleMessage'>,0,3,,6
wo<'Height'>,f1.1228-f1,3,,6
wo<'HideCaret'>,0,3,,6
wo<'HiliteMenuItem'>,0,3,,6
wo<'HoldFocusHWnd'>,0,3,,6
wo<'ICON_BIG'>,0,3,,2
wo<'ICON_SMALL'>,0,3,,2
wo<'IDABORT'>,f1.1234-f1,3,,2
wo<'IDCANCEL'>,f1.1235-f1,3,,2
wo<'IDCLOSE'>,0,3,,2
wo<'IDCONTINUE'>,f1.1237-f1,3,,2
wo<'IDC_APPSTARTING'>,f1.1238-f1,3,,2
wo<'IDC_ARROW'>,f1.1239-f1,3,,2
wo<'IDC_CROSS'>,f1.1240-f1,3,,2
wo<'IDC_NO'>,f1.1241-f1,3,,2
wo<'IDC_SIZEALL'>,f1.1242-f1,3,,2
wo<'IDC_SIZENESW'>,f1.1243-f1,3,,2
wo<'IDC_SIZENS'>,f1.1244-f1,3,,2
wo<'IDC_SIZENWSE'>,f1.1245-f1,3,,2
wo<'IDC_SIZEWE'>,f1.1246-f1,3,,2
wo<'IDC_STATIC'>,0,3,,2
wo<'IDC_WAIT'>,f1.1248-f1,3,,2
wo<'IDHELP'>,0,3,,2
wo<'IDH_CANCEL'>,0,3,,2
wo<'IDH_GENERIC_HELP_BUTTON'>,0,3,,2
wo<'IDH_HELP'>,0,3,,2
wo<'IDH_MISSING_CONTEXT'>,0,3,,2
wo<'IDH_NO_HELP'>,0,3,,2
wo<'IDH_OK'>,0,3,,2
wo<'IDIGNORE'>,f1.1256-f1,3,,2
wo<'IDI_APPLICATION'>,0,3,,2
wo<'IDI_ASTERISK'>,0,3,,2
wo<'IDI_ERROR'>,0,3,,2
wo<'IDI_EXCLAMATION'>,0,3,,2
wo<'IDI_HAND'>,0,3,,2
wo<'IDI_INFORMATION'>,0,3,,2
wo<'IDI_QUESTION'>,0,3,,2
wo<'IDI_TRAY'>,0,3,,2
wo<'IDI_WARNING'>,0,3,,2
wo<'IDI_WINLOGO'>,0,3,,2
wo<'IDNO'>,f1.1267-f1,3,,2
wo<'IDOK'>,f1.1268-f1,3,,2
wo<'IDRETRY'>,f1.1269-f1,3,,2
wo<'IDTRYAGAIN'>,f1.1270-f1,3,,2
wo<'IDYES'>,f1.1271-f1,3,,2
wo<'ILC_COLOR'>,0,3,,2
wo<'ILC_COLOR16'>,f1.1273-f1,3,,2
wo<'ILC_COLOR24'>,f1.1274-f1,3,,2
wo<'ILC_COLOR32'>,f1.1275-f1,3,,2
wo<'ILC_COLOR4'>,f1.1276-f1,3,,2
wo<'ILC_COLOR8'>,f1.1277-f1,3,,2
wo<'ILC_COLORDDB'>,0,3,,2
wo<'ILC_MASK'>,0,3,,2
wo<'ILC_PALETTE'>,0,3,,2
wo<'ILD_BLEND'>,0,3,,2
wo<'ILD_BLEND25'>,0,3,,2
wo<'ILD_BLEND50'>,0,3,,2
wo<'ILD_FOCUS'>,0,3,,2
wo<'ILD_IMAGE'>,0,3,,2
wo<'ILD_MASK'>,0,3,,2
wo<'ILD_NORMAL'>,0,3,,2
wo<'ILD_OVERLAYMASK'>,0,3,,2
wo<'ILD_SELECTED'>,0,3,,2
wo<'ILD_TRANSPARENT'>,f1.1290-f1,3,,2
wo<'IdleAction'>,0,3,,6
wo<'InitApplication'>,f1.1292-f1,3,,6
wo<'InitInstance'>,f1.1293-f1,3,,6
wo<'InitMainWindow'>,f1.1294-f1,3,,6
wo<'Insert'>,0,3,,6
wo<'InsertColumn'>,0,3,,6
wo<'InsertItem'>,0,3,,6
wo<'IntersectClipRect'>,0,3,,6
wo<'Invalidate'>,0,3,,6
wo<'InvalidateRect'>,0,3,,6
wo<'InvalidateRgn'>,0,3,,6
wo<'InvertRect'>,f1.1302-f1,3,,6
wo<'InvertRgn'>,f1.1303-f1,3,,6
wo<'IsChild'>,0,3,,6
wo<'IsDlgButtonChecked'>,0,3,,6
wo<'IsFlagSet'>,0,3,,6
wo<'IsIconic'>,0,3,,6
wo<'IsWindow'>,0,3,,6
wo<'IsWindowEnabled'>,0,3,,6
wo<'IsWindowVisible'>,0,3,,6
wo<'IsZoomed'>,0,3,,6
wo<'JOHAB_CHARSET'>,0,3,,2
wo<'KillTimer'>,0,3,,6
wo<'LBN_DBLCLK'>,0,3,,2
wo<'LBN_ERRSPACE'>,0,3,,2
wo<'LBN_KILLFOCUS'>,0,3,,2
wo<'LBN_SELCANCEL'>,0,3,,2
wo<'LBN_SELCHANGE'>,0,3,,2
wo<'LBN_SETFOCUS'>,0,3,,2
wo<'LBS_DISABLENOSCROLL'>,0,3,,2
wo<'LBS_EXTENDEDSEL'>,0,3,,2
wo<'LBS_HASSTRINGS'>,0,3,,2
wo<'LBS_MULTICOLUMN'>,0,3,,2
wo<'LBS_MULTIPLESEL'>,0,3,,2
wo<'LBS_NODATA'>,0,3,,2
wo<'LBS_NOINTEGRALHEIGHT'>,0,3,,2
wo<'LBS_NOREDRAW'>,0,3,,2
wo<'LBS_NOSEL'>,0,3,,2
wo<'LBS_NOTIFY'>,0,3,,2
wo<'LBS_OWNERDRAWFIXED'>,0,3,,2
wo<'LBS_OWNERDRAWVARIABLE'>,0,3,,2
wo<'LBS_SORT'>,0,3,,2
wo<'LBS_STANDARD'>,0,3,,2
wo<'LBS_USETABSTOPS'>,0,3,,2
wo<'LBS_WANTKEYBOARDINPUT'>,0,3,,2
wo<'LB_ADDFILE'>,0,3,,2
wo<'LB_ADDSTRING'>,f1.1337-f1,3,,2
wo<'LB_CTLCODE'>,0,3,,2
wo<'LB_DELETESTRING'>,f1.1339-f1,3,,2
wo<'LB_DIR'>,0,3,,2
wo<'LB_ERR'>,0,3,,2
wo<'LB_ERRSPACE'>,0,3,,2
wo<'LB_FINDSTRING'>,f1.1343-f1,3,,2
wo<'LB_FINDSTRINGEXACT'>,0,3,,2
wo<'LB_GETANCHORINDEX'>,0,3,,2
wo<'LB_GETCARETINDEX'>,0,3,,2
wo<'LB_GETCOUNT'>,f1.1347-f1,3,,2
wo<'LB_GETCURSEL'>,0,3,,2
wo<'LB_GETHORIZONTALEXTENT'>,0,3,,2
wo<'LB_GETITEMDATA'>,0,3,,2
wo<'LB_GETITEMHEIGHT'>,0,3,,2
wo<'LB_GETITEMRECT'>,0,3,,2
wo<'LB_GETLOCALE'>,0,3,,2
wo<'LB_GETSEL'>,0,3,,2
wo<'LB_GETSELCOUNT'>,0,3,,2
wo<'LB_GETSELITEMS'>,0,3,,2
wo<'LB_GETTEXT'>,0,3,,2
wo<'LB_GETTEXTLEN'>,0,3,,2
wo<'LB_GETTOPINDEX'>,0,3,,2
wo<'LB_INSERTSTRING'>,0,3,,2
wo<'LB_MSGMAX'>,0,3,,2
wo<'LB_OKAY'>,0,3,,2
wo<'LB_RESETCONTENT'>,0,3,,2
wo<'LB_SELECTSTRING'>,0,3,,2
wo<'LB_SELITEMRANGE'>,0,3,,2
wo<'LB_SELITEMRANGEEX'>,0,3,,2
wo<'LB_SETANCHORINDEX'>,0,3,,2
wo<'LB_SETCARETINDEX'>,0,3,,2
wo<'LB_SETCOLUMNWIDTH'>,f1.1369-f1,3,,2
wo<'LB_SETCOUNT'>,0,3,,2
wo<'LB_SETCURSEL'>,0,3,,2
wo<'LB_SETHORIZONTALEXTENT'>,0,3,,2
wo<'LB_SETITEMDATA'>,0,3,,2
wo<'LB_SETITEMHEIGHT'>,0,3,,2
wo<'LB_SETLOCALE'>,0,3,,2
wo<'LB_SETSEL'>,f1.1376-f1,3,,2
wo<'LB_SETTABSTOPS'>,0,3,,2
wo<'LB_SETTOPINDEX'>,0,3,,2
wo<'LPtoDP'>,f1.1379-f1,3,,6
wo<'LR_COLOR'>,0,3,,2
wo<'LR_COPYDELETEORG'>,0,3,,2
wo<'LR_COPYFROMRESOURCE'>,0,3,,2
wo<'LR_COPYRETURNORG'>,0,3,,2
wo<'LR_CREATEDIBSECTION'>,0,3,,2
wo<'LR_DEFAULTCOLOR'>,0,3,,2
wo<'LR_DEFAULTSIZE'>,0,3,,2
wo<'LR_LOADFROMFILE'>,0,3,,2
wo<'LR_LOADMAP3DCOLORS'>,0,3,,2
wo<'LR_LOADTRANSPARENT'>,0,3,,2
wo<'LR_MONOCHROME'>,0,3,,2
wo<'LR_SHARED'>,0,3,,2
wo<'LR_VGACOLOR'>,0,3,,2
wo<'LTGRAY_BRUSH'>,f1.1393-f1,3,,2
wo<'LVS_ALIGNLEFT'>,0,3,,2
wo<'LVS_ALIGNMASK'>,0,3,,2
wo<'LVS_ALIGNTOP'>,0,3,,2
wo<'LVS_AUTOARRANGE'>,0,3,,2
wo<'LVS_EDITLABELS'>,0,3,,2
wo<'LVS_ICON'>,0,3,,2
wo<'LVS_LIST'>,0,3,,2
wo<'LVS_NOCOLUMNHEADER'>,0,3,,2
wo<'LVS_NOLABELWRAP'>,0,3,,2
wo<'LVS_NOSCROLL'>,0,3,,2
wo<'LVS_NOSORTHEADER'>,0,3,,2
wo<'LVS_OWNERDATA'>,f1.1405-f1,3,,2
wo<'LVS_OWNERDRAWFIXED'>,0,3,,2
wo<'LVS_REPORT'>,0,3,,2
wo<'LVS_SHAREIMAGELISTS'>,0,3,,2
wo<'LVS_SHOWSELALWAYS'>,0,3,,2
wo<'LVS_SINGLESEL'>,0,3,,2
wo<'LVS_SMALLICON'>,0,3,,2
wo<'LVS_SORTASCENDING'>,0,3,,2
wo<'LVS_SORTDESCENDING'>,0,3,,2
wo<'LVS_TYPEMASK'>,0,3,,2
wo<'LVS_TYPESTYLEMASK'>,0,3,,2
wo<'Layout'>,0,3,,6
wo<'LineTo'>,f1.1417-f1,3,,6
wo<'LoadAccelerators'>,0,3,,6
wo<'LockWindowUpdate'>,0,3,,6
wo<'MAC_CHARSET'>,0,3,,2
wo<'MA_ACTIVATE'>,0,3,,2
wo<'MA_ACTIVATEANDEAT'>,0,3,,2
wo<'MA_NOACTIVATE'>,0,3,,2
wo<'MA_NOACTIVATEANDEAT'>,0,3,,2
wo<'MB_ABORTRETRYIGNORE'>,f1.1425-f1,3,,2
wo<'MB_CANCELTRYCONTINUE'>,f1.1426-f1,3,,2
wo<'MB_ICONASTERISK'>,f1.1427-f1,3,,2
wo<'MB_ICONERROR'>,f1.1428-f1,3,,2
wo<'MB_ICONEXCLAMATION'>,f1.1429-f1,3,,2
wo<'MB_ICONHAND'>,f1.1430-f1,3,,2
wo<'MB_ICONINFORMATION'>,f1.1431-f1,3,,2
wo<'MB_ICONQUESTION'>,f1.1432-f1,3,,2
wo<'MB_ICONSTOP'>,f1.1433-f1,3,,2
wo<'MB_ICONWARNING'>,f1.1434-f1,3,,2
wo<'MB_OK'>,f1.1435-f1,3,,2
wo<'MB_OKCANCEL'>,f1.1436-f1,3,,2
wo<'MB_RETRYCANCEL'>,f1.1437-f1,3,,2
wo<'MB_YESNO'>,f1.1438-f1,3,,2
wo<'MB_YESNOCANCEL'>,f1.1439-f1,3,,2
wo<'MERGECOPY'>,f1.1440-f1,3,,2
wo<'MERGEPAINT'>,f1.1441-f1,3,,2
wo<'MFS_BOTTOMGAPDROP'>,0,3,,2
wo<'MFS_CACHEDBMP'>,0,3,,2
wo<'MFS_CHECKED'>,0,3,,2
wo<'MFS_DEFAULT'>,0,3,,2
wo<'MFS_DISABLED'>,0,3,,2
wo<'MFS_ENABLED'>,0,3,,2
wo<'MFS_GAPDROP'>,0,3,,2
wo<'MFS_GRAYED'>,0,3,,2
wo<'MFS_HILITE'>,0,3,,2
wo<'MFS_HOTTRACKDRAWN'>,0,3,,2
wo<'MFS_MASK'>,0,3,,2
wo<'MFS_TOPGAPDROP'>,0,3,,2
wo<'MFS_UNCHECKED'>,0,3,,2
wo<'MFS_UNHILITE'>,0,3,,2
wo<'MFT_BITMAP'>,0,3,,2
wo<'MFT_MENUBARBREAK'>,0,3,,2
wo<'MFT_MENUBREAK'>,0,3,,2
wo<'MFT_OWNERDRAW'>,0,3,,2
wo<'MFT_RADIOCHECK'>,0,3,,2
wo<'MFT_RIGHTJUSTIFY'>,0,3,,2
wo<'MFT_RIGHTORDER'>,0,3,,2
wo<'MFT_SEPARATOR'>,0,3,,2
wo<'MFT_STRING'>,0,3,,2
wo<'MF_APPEND'>,0,3,,2
wo<'MF_BITMAP'>,f1.1466-f1,3,,2
wo<'MF_BYCOMMAND'>,0,3,,2
wo<'MF_BYPOSITION'>,0,3,,2
wo<'MF_CHANGE'>,0,3,,2
wo<'MF_CHECKED'>,f1.1470-f1,3,,2
wo<'MF_DEFAULT'>,0,3,,2
wo<'MF_DELETE'>,0,3,,2
wo<'MF_DISABLED'>,f1.1473-f1,3,,2
wo<'MF_ENABLED'>,0,3,,2
wo<'MF_END'>,0,3,,2
wo<'MF_GRAYED'>,0,3,,2
wo<'MF_HELP'>,0,3,,2
wo<'MF_HILITE'>,0,3,,2
wo<'MF_INSERT'>,0,3,,2
wo<'MF_MENUBARBREAK'>,0,3,,2
wo<'MF_MENUBREAK'>,0,3,,2
wo<'MF_MOUSESELECT'>,0,3,,2
wo<'MF_OWNERDRAW'>,f1.1483-f1,3,,2
wo<'MF_POPUP'>,f1.1484-f1,3,,2
wo<'MF_REMOVE'>,0,3,,2
wo<'MF_RIGHTJUSTIFY'>,0,3,,2
wo<'MF_SEPARATOR'>,f1.1487-f1,3,,2
wo<'MF_STRING'>,0,3,,2
wo<'MF_SYSMENU'>,0,3,,2
wo<'MF_UNCHECKED'>,0,3,,2
wo<'MF_UNHILITE'>,0,3,,2
wo<'MF_USECHECKBITMAPS'>,0,3,,2
wo<'MK_CONTROL'>,0,3,,2
wo<'MK_LBUTTON'>,0,3,,2
wo<'MK_MBUTTON'>,0,3,,2
wo<'MK_RBUTTON'>,0,3,,2
wo<'MK_SHIFT'>,0,3,,2
wo<'MainWindow'>,f1.1498-f1,3,,6
wo<'MapWindowPoints'>,0,3,,6
wo<'MaskBlt'>,0,3,,6
wo<'MessageBox'>,0,3,,6
wo<'ModifyExStyle'>,0,3,,6
wo<'ModifyStyle'>,0,3,,6
wo<'ModifyWorldTransform'>,f1.1504-f1,3,,6
wo<'MoveSplitter'>,0,3,,6
wo<'MoveTo'>,0,3,,6
wo<'MoveWindow'>,0,3,,6
wo<'NFR_ANSI'>,0,3,,2
wo<'NFR_UNICODE'>,0,3,,2
wo<'NFS_ALL'>,0,3,,2
wo<'NFS_BUTTON'>,0,3,,2
wo<'NFS_EDIT'>,0,3,,2
wo<'NFS_LISTCOMBO'>,0,3,,2
wo<'NFS_STATIC'>,0,3,,2
wo<'NF_QUERY'>,0,3,,2
wo<'NF_REQUERY'>,0,3,,2
wo<'NIF_ICON'>,0,3,,2
wo<'NIF_MESSAGE'>,0,3,,2
wo<'NIF_TIP'>,0,3,,2
wo<'NIM_ADD'>,0,3,,2
wo<'NIM_DELETE'>,0,3,,2
wo<'NOTSRCCOPY'>,f1.1522-f1,3,,2
wo<'NOTSRCERASE'>,f1.1523-f1,3,,2
wo<'NULL'>,0,3,,2
wo<'NULL_BRUSH'>,f1.1525-f1,3,,2
wo<'NULL_PEN'>,f1.1526-f1,3,,2
wo<'Next'>,0,3,,6
wo<'NumChildren'>,0,3,,6
wo<'OBM_BTNCORNERS'>,0,3,,2
wo<'OBM_BTSIZE'>,0,3,,2
wo<'OBM_CHECK'>,0,3,,2
wo<'OBM_CHECKBOXES'>,0,3,,2
wo<'OBM_CLOSE'>,0,3,,2
wo<'OBM_COMBO'>,0,3,,2
wo<'OBM_DNARROW'>,0,3,,2
wo<'OBM_DNARROWD'>,0,3,,2
wo<'OBM_DNARROWI'>,0,3,,2
wo<'OBM_LFARROW'>,0,3,,2
wo<'OBM_LFARROWD'>,0,3,,2
wo<'OBM_LFARROWI'>,0,3,,2
wo<'OBM_MNARROW'>,0,3,,2
wo<'OBM_OLD_CLOSE'>,0,3,,2
wo<'OBM_OLD_DNARROW'>,0,3,,2
wo<'OBM_OLD_LFARROW'>,0,3,,2
wo<'OBM_OLD_REDUCE'>,0,3,,2
wo<'OBM_OLD_RESTORE'>,0,3,,2
wo<'OBM_OLD_RGARROW'>,0,3,,2
wo<'OBM_OLD_UPARROW'>,0,3,,2
wo<'OBM_OLD_ZOOM'>,0,3,,2
wo<'OBM_REDUCE'>,0,3,,2
wo<'OBM_REDUCED'>,0,3,,2
wo<'OBM_RESTORE'>,0,3,,2
wo<'OBM_RESTORED'>,0,3,,2
wo<'OBM_RGARROW'>,0,3,,2
wo<'OBM_RGARROWD'>,0,3,,2
wo<'OBM_RGARROWI'>,0,3,,2
wo<'OBM_SIZE'>,0,3,,2
wo<'OBM_UPARROW'>,0,3,,2
wo<'OBM_UPARROWD'>,0,3,,2
wo<'OBM_UPARROWI'>,0,3,,2
wo<'OBM_ZOOM'>,0,3,,2
wo<'OBM_ZOOMD'>,0,3,,2
wo<'OCR_APPSTARTING'>,0,3,,2
wo<'OCR_CROSS'>,0,3,,2
wo<'OCR_HAND'>,0,3,,2
wo<'OCR_IBEAM'>,0,3,,2
wo<'OCR_ICOCUR'>,0,3,,2
wo<'OCR_ICON'>,0,3,,2
wo<'OCR_NO'>,0,3,,2
wo<'OCR_NORMAL'>,0,3,,2
wo<'OCR_SIZE'>,0,3,,2
wo<'OCR_SIZEALL'>,0,3,,2
wo<'OCR_SIZENESW'>,0,3,,2
wo<'OCR_SIZENS'>,0,3,,2
wo<'OCR_SIZENWSE'>,0,3,,2
wo<'OCR_SIZEWE'>,0,3,,2
wo<'OCR_UP'>,0,3,,2
wo<'OCR_WAIT'>,0,3,,2
wo<'OEM_CHARSET'>,0,3,,2
wo<'OEM_FIXED_FONT'>,f1.1580-f1,3,,2
wo<'OFN_ALLOWMULTISELECT'>,0,3,,2
wo<'OFN_CREATEPROMPT'>,0,3,,2
wo<'OFN_ENABLEHOOK'>,0,3,,2
wo<'OFN_ENABLETEMPLATE'>,0,3,,2
wo<'OFN_ENABLETEMPLATEHANDLE'>,0,3,,2
wo<'OFN_EXPLORER'>,0,3,,2
wo<'OFN_EXTENSIONDIFFERENT'>,0,3,,2
wo<'OFN_FILEMUSTEXIST'>,f1.1588-f1,3,,2
wo<'OFN_HIDEREADONLY'>,f1.1589-f1,3,,2
wo<'OFN_LONGNAMES'>,0,3,,2
wo<'OFN_NOCHANGEDIR'>,0,3,,2
wo<'OFN_NODEREFERENCELINKS'>,0,3,,2
wo<'OFN_NOLONGNAMES'>,0,3,,2
wo<'OFN_NONETWORKBUTTON'>,0,3,,2
wo<'OFN_NOREADONLYRETURN'>,0,3,,2
wo<'OFN_NOTESTFILECREATE'>,0,3,,2
wo<'OFN_NOVALIDATE'>,0,3,,2
wo<'OFN_OVERWRITEPROMPT'>,0,3,,2
wo<'OFN_PATHMUSTEXIST'>,0,3,,2
wo<'OFN_READONLY'>,0,3,,2
wo<'OFN_SHAREAWARE'>,0,3,,2
wo<'OFN_SHOWHELP'>,0,3,,2
wo<'OIC_BANG'>,0,3,,2
wo<'OIC_ERROR'>,0,3,,2
wo<'OIC_HAND'>,0,3,,2
wo<'OIC_INFORMATION'>,0,3,,2
wo<'OIC_NOTE'>,0,3,,2
wo<'OIC_QUES'>,0,3,,2
wo<'OIC_SAMPLE'>,0,3,,2
wo<'OIC_WARNING'>,0,3,,2
wo<'OIC_WINLOGO'>,0,3,,2
wo<'OPEN_ALWAYS'>,0,3,,2
wo<'OPEN_EXISTING'>,0,3,,2
wo<'OUT_CHARACTER_PRECIS'>,0,3,,2
wo<'OUT_DEFAULT_PRECIS'>,0,3,,2
wo<'OUT_DEVICE_PRECIS'>,0,3,,2
wo<'OUT_OUTLINE_PRECIS'>,0,3,,2
wo<'OUT_RASTER_PRECIS'>,0,3,,2
wo<'OUT_STRING_PRECIS'>,0,3,,2
wo<'OUT_STROKE_PRECIS'>,0,3,,2
wo<'OUT_TT_ONLY_PRECIS'>,0,3,,2
wo<'OUT_TT_PRECIS'>,0,3,,2
wo<'OWLFastWindowFrame'>,0,3,,6
wo<'OffsetClipRgn'>,f1.1624-f1,3,,6
wo<'OffsetViewportOrg'>,f1.1625-f1,3,,6
wo<'OffsetWindowOrg'>,f1.1626-f1,3,,6
wo<'OpenClipboard'>,0,3,,6
wo<'OwlMain'>,0,3,,6
wo<'PATCOPY'>,f1.1629-f1,3,,2
wo<'PATINVERT'>,f1.1630-f1,3,,2
wo<'PATPAINT'>,f1.1631-f1,3,,2
wo<'PBS_SMOOTH'>,0,3,,2
wo<'PBS_VERTICAL'>,0,3,,2
wo<'PFD_DEPTH_DONTCARE'>,0,3,,2
wo<'PFD_DOUBLEBUFFER'>,0,3,,2
wo<'PFD_DOUBLEBUFFER_DONTCARE'>,0,3,,2
wo<'PFD_DRAW_TO_BITMAP'>,0,3,,2
wo<'PFD_DRAW_TO_WINDOW'>,0,3,,2
wo<'PFD_GENERIC_ACCELERATED'>,0,3,,2
wo<'PFD_GENERIC_FORMAT'>,0,3,,2
wo<'PFD_MAIN_PLANE'>,0,3,,2
wo<'PFD_NEED_PALETTE'>,0,3,,2
wo<'PFD_NEED_SYSTEM_PALETTE'>,0,3,,2
wo<'PFD_OVERLAY_PLANE'>,0,3,,2
wo<'PFD_STEREO'>,0,3,,2
wo<'PFD_STEREO_DONTCARE'>,0,3,,2
wo<'PFD_SUPPORT_DIRECTDRAW'>,0,3,,2
wo<'PFD_SUPPORT_GDI'>,0,3,,2
wo<'PFD_SUPPORT_OPENGL'>,0,3,,2
wo<'PFD_SWAP_COPY'>,0,3,,2
wo<'PFD_SWAP_EXCHANGE'>,0,3,,2
wo<'PFD_SWAP_LAYER_BUFFERS'>,0,3,,2
wo<'PFD_TYPE_COLORINDEX'>,0,3,,2
wo<'PFD_TYPE_RGBA'>,0,3,,2
wo<'PFD_UNDERLAY_PLANE'>,0,3,,2
wo<'PGS_AUTOSCROLL'>,0,3,,2
wo<'PGS_DRAGNDROP'>,0,3,,2
wo<'PGS_HORZ'>,0,3,,2
wo<'PGS_VERT'>,0,3,,2
wo<'PROOF_QUALITY'>,0,3,,2
wo<'PS_ALTERNATE'>,0,3,,2
wo<'PS_COSMETIC'>,0,3,,2
wo<'PS_DASH'>,0,3,,2
wo<'PS_DASHDOT'>,0,3,,2
wo<'PS_DASHDOTDOT'>,0,3,,2
wo<'PS_DOT'>,0,3,,2
wo<'PS_ENDCAP_FLAT'>,0,3,,2
wo<'PS_ENDCAP_MASK'>,0,3,,2
wo<'PS_ENDCAP_ROUND'>,0,3,,2
wo<'PS_ENDCAP_SQUARE'>,0,3,,2
wo<'PS_GEOMETRIC'>,0,3,,2
wo<'PS_INSIDEFRAME'>,0,3,,2
wo<'PS_JOIN_BEVEL'>,0,3,,2
wo<'PS_JOIN_MASK'>,0,3,,2
wo<'PS_JOIN_MITER'>,0,3,,2
wo<'PS_JOIN_ROUND'>,0,3,,2
wo<'PS_NULL'>,0,3,,2
wo<'PS_SOLID'>,0,3,,2
wo<'PS_STYLE_MASK'>,0,3,,2
wo<'PS_TYPE_MASK'>,0,3,,2
wo<'PS_USERSTYLE'>,0,3,,2
wo<'PWR_CRITICALRESUME'>,0,3,,2
wo<'PWR_FAIL'>,0,3,,2
wo<'PWR_OK'>,0,3,,2
wo<'PWR_SUSPENDREQUEST'>,0,3,,2
wo<'PWR_SUSPENDRESUME'>,0,3,,2
wo<'Paint'>,0,3,,6
wo<'PaintRgn'>,f1.1688-f1,3,,6
wo<'PatBlt'>,f1.1689-f1,3,,6
wo<'PathToRegion'>,0,3,,6
wo<'PerformCreate'>,0,3,,6
wo<'Pie'>,f1.1692-f1,3,,6
wo<'PlayEnhMetaFile'>,0,3,,6
wo<'PlayEnhMetaFileRecord'>,0,3,,6
wo<'PlayMetaFile'>,f1.1695-f1,3,,6
wo<'PlayMetaFileRecord'>,f1.1696-f1,3,,6
wo<'PlgBlt'>,f1.1697-f1,3,,6
wo<'PolyBezier'>,0,3,,6
wo<'PolyBezierTo'>,f1.1699-f1,3,,6
wo<'PolyDraw'>,f1.1700-f1,3,,6
wo<'PolyPolygon'>,f1.1701-f1,3,,6
wo<'PolyPolyline'>,f1.1702-f1,3,,6
wo<'Polygon'>,f1.1703-f1,3,,6
wo<'Polyline'>,0,3,,6
wo<'PolylineTo'>,0,3,,6
wo<'PostMessage'>,0,3,,6
wo<'PreProcessMsg'>,0,3,,6
wo<'Previous'>,0,3,,6
wo<'PtVisible'>,f1.1709-f1,3,,6
wo<'R2_BLACK'>,f1.1710-f1,3,,2
wo<'R2_COPYPEN'>,f1.1711-f1,3,,2
wo<'R2_MASKNOTPEN'>,f1.1712-f1,3,,2
wo<'R2_MASKPEN'>,f1.1713-f1,3,,2
wo<'R2_MASKPENNOT'>,f1.1714-f1,3,,2
wo<'R2_MERGENOTPEN'>,f1.1715-f1,3,,2
wo<'R2_MERGEPEN'>,f1.1716-f1,3,,2
wo<'R2_MERGEPENNOT'>,f1.1717-f1,3,,2
wo<'R2_NOP'>,f1.1718-f1,3,,2
wo<'R2_NOT'>,f1.1719-f1,3,,2
wo<'R2_NOTCOPYPEN'>,f1.1720-f1,3,,2
wo<'R2_NOTMASKPEN'>,f1.1721-f1,3,,2
wo<'R2_NOTMERGEPEN'>,f1.1722-f1,3,,2
wo<'R2_NOTXORPEN'>,f1.1723-f1,3,,2
wo<'R2_WHITE'>,f1.1724-f1,3,,2
wo<'R2_XORPEN'>,f1.1725-f1,3,,2
wo<'RBS_AUTOSIZE'>,0,3,,2
wo<'RBS_BANDBORDERS'>,0,3,,2
wo<'RBS_DBLCLKTOGGLE'>,0,3,,2
wo<'RBS_FIXEDORDER'>,0,3,,2
wo<'RBS_REGISTERDROP'>,0,3,,2
wo<'RBS_TOOLTIPS'>,0,3,,2
wo<'RBS_VARHEIGHT'>,0,3,,2
wo<'RBS_VERTICALGRIPPER'>,0,3,,2
wo<'RGN_AND'>,0,3,,2
wo<'RGN_COPY'>,0,3,,2
wo<'RGN_DIFF'>,0,3,,2
wo<'RGN_MAX'>,0,3,,2
wo<'RGN_MIN'>,0,3,,2
wo<'RGN_OR'>,0,3,,2
wo<'RGN_XOR'>,0,3,,2
wo<'RUSSIAN_CHARSET'>,0,3,,2
wo<'RealizePalette'>,f1.1742-f1,3,,6
wo<'ReceiveMessage'>,0,3,,6
wo<'RectVisible'>,0,3,,6
wo<'Rectangle'>,0,3,,6
wo<'RedrawWindow'>,0,3,,6
wo<'Register'>,0,3,,6
wo<'RegisterHotKey'>,0,3,,6
wo<'ReleaseCapture'>,0,3,,6
wo<'RemovePane'>,0,3,,6
wo<'RemoveProp'>,0,3,,6
wo<'ResetDC'>,0,3,,6
wo<'RestoreBitmap'>,0,3,,6
wo<'RestoreBrush'>,0,3,,6
wo<'RestoreDC'>,0,3,,6
wo<'RestoreFont'>,0,3,,6
wo<'RestoreObjects'>,0,3,,6
wo<'RestorePalette'>,0,3,,6
wo<'RestorePen'>,f1.1759-f1,3,,6
wo<'RestoreTextBrush'>,f1.1760-f1,3,,6
wo<'RoundRect'>,f1.1761-f1,3,,6
wo<'RouteCommandEnable'>,0,3,,6
wo<'Run'>,0,3,,6
wo<'SBARS_SIZEGRIP'>,0,3,,2
wo<'SBS_BOTTOMALIGN'>,f1.1765-f1,3,,2
wo<'SBS_HORZ'>,f1.1766-f1,3,,2
wo<'SBS_LEFTALIGN'>,f1.1767-f1,3,,2
wo<'SBS_RIGHTALIGN'>,f1.1768-f1,3,,2
wo<'SBS_SIZEBOX'>,f1.1769-f1,3,,2
wo<'SBS_SIZEBOXBOTTOMRIGHTALIGN'>,f1.1770-f1,3,,2
wo<'SBS_SIZEBOXTOPLEFTALIGN'>,f1.1771-f1,3,,2
wo<'SBS_SIZEGRIP'>,0,3,,2
wo<'SBS_TOPALIGN'>,f1.1773-f1,3,,2
wo<'SBS_VERT'>,f1.1774-f1,3,,2
wo<'SB_BOTH'>,f1.1775-f1,3,,2
wo<'SB_BOTTOM'>,f1.1776-f1,3,,2
wo<'SB_CTL'>,f1.1777-f1,3,,2
wo<'SB_ENDSCROLL'>,f1.1778-f1,3,,2
wo<'SB_HORZ'>,f1.1779-f1,3,,2
wo<'SB_LEFT'>,f1.1780-f1,3,,2
wo<'SB_LINEDOWN'>,f1.1781-f1,3,,2
wo<'SB_LINELEFT'>,f1.1782-f1,3,,2
wo<'SB_LINERIGHT'>,f1.1783-f1,3,,2
wo<'SB_LINEUP'>,f1.1784-f1,3,,2
wo<'SB_PAGEDOWN'>,f1.1785-f1,3,,2
wo<'SB_PAGELEFT'>,f1.1786-f1,3,,2
wo<'SB_PAGERIGHT'>,f1.1787-f1,3,,2
wo<'SB_PAGEUP'>,f1.1788-f1,3,,2
wo<'SB_RIGHT'>,f1.1789-f1,3,,2
wo<'SB_SETPARTS'>,f1.1790-f1,3,,2
wo<'SB_SETTEXT'>,f1.1791-f1,3,,2
wo<'SB_THUMBPOSITION'>,f1.1792-f1,3,,2
wo<'SB_THUMBTRACK'>,f1.1793-f1,3,,2
wo<'SB_TOP'>,f1.1794-f1,3,,2
wo<'SB_VERT'>,f1.1795-f1,3,,2
wo<'SC_ARRANGE'>,0,3,,2
wo<'SC_CLOSE'>,0,3,,2
wo<'SC_CONTEXTHELP'>,0,3,,2
wo<'SC_DEFAULT'>,0,3,,2
wo<'SC_HOTKEY'>,0,3,,2
wo<'SC_HSCROLL'>,0,3,,2
wo<'SC_ICON'>,0,3,,2
wo<'SC_KEYMENU'>,0,3,,2
wo<'SC_MAXIMIZE'>,0,3,,2
wo<'SC_MINIMIZE'>,0,3,,2
wo<'SC_MONITORPOWER'>,0,3,,2
wo<'SC_MOUSEMENU'>,0,3,,2
wo<'SC_MOVE'>,0,3,,2
wo<'SC_NEXTWINDOW'>,0,3,,2
wo<'SC_PREVWINDOW'>,0,3,,2
wo<'SC_RESTORE'>,0,3,,2
wo<'SC_SCREENSAVE'>,0,3,,2
wo<'SC_SEPARATOR'>,0,3,,2
wo<'SC_SIZE'>,0,3,,2
wo<'SC_TASKLIST'>,0,3,,2
wo<'SC_VSCROLL'>,0,3,,2
wo<'SC_ZOOM'>,0,3,,2
wo<'SHIFTJIS_CHARSET'>,0,3,,2
wo<'SHOW_FULLSCREEN'>,0,3,,2
wo<'SHOW_ICONWINDOW'>,0,3,,2
wo<'SHOW_OPENNOACTIVATE'>,0,3,,2
wo<'SHOW_OPENWINDOW'>,0,3,,2
wo<'SIZEFULLSCREEN'>,0,3,,2
wo<'SIZEICONIC'>,0,3,,2
wo<'SIZENORMAL'>,0,3,,2
wo<'SIZEZOOMHIDE'>,0,3,,2
wo<'SIZEZOOMSHOW'>,0,3,,2
wo<'SIZE_MAXHIDE'>,0,3,,2
wo<'SIZE_MAXIMIZED'>,0,3,,2
wo<'SIZE_MAXSHOW'>,0,3,,2
wo<'SIZE_MINIMIZED'>,0,3,,2
wo<'SIZE_RESTORED'>,0,3,,2
wo<'SMTO_ABORTIFHUNG'>,0,3,,2
wo<'SMTO_BLOCK'>,0,3,,2
wo<'SMTO_NORMAL'>,0,3,,2
wo<'SMTO_NOTIMEOUTIFNOTHUNG'>,0,3,,2
wo<'SM_ARRANGE'>,0,3,,2
wo<'SM_CLEANBOOT'>,0,3,,2
wo<'SM_CMETRICS'>,0,3,,2
wo<'SM_CMONITORS'>,0,3,,2
wo<'SM_CMOUSEBUTTONS'>,f1.1841-f1,3,,2
wo<'SM_CXBORDER'>,f1.1842-f1,3,,2
wo<'SM_CXCURSOR'>,0,3,,2
wo<'SM_CXDLGFRAME'>,f1.1844-f1,3,,2
wo<'SM_CXDOUBLECLK'>,0,3,,2
wo<'SM_CXDRAG'>,0,3,,2
wo<'SM_CXEDGE'>,f1.1847-f1,3,,2
wo<'SM_CXFIXEDFRAME'>,0,3,,2
wo<'SM_CXFRAME'>,0,3,,2
wo<'SM_CXFULLSCREEN'>,0,3,,2
wo<'SM_CXHSCROLL'>,0,3,,2
wo<'SM_CXHTHUMB'>,0,3,,2
wo<'SM_CXICON'>,0,3,,2
wo<'SM_CXICONSPACING'>,0,3,,2
wo<'SM_CXMAXIMIZED'>,0,3,,2
wo<'SM_CXMAXTRACK'>,0,3,,2
wo<'SM_CXMENUCHECK'>,0,3,,2
wo<'SM_CXMENUSIZE'>,0,3,,2
wo<'SM_CXMIN'>,0,3,,2
wo<'SM_CXMINIMIZED'>,0,3,,2
wo<'SM_CXMINSPACING'>,0,3,,2
wo<'SM_CXMINTRACK'>,0,3,,2
wo<'SM_CXSCREEN'>,0,3,,2
wo<'SM_CXSIZE'>,0,3,,2
wo<'SM_CXSIZEFRAME'>,0,3,,2
wo<'SM_CXSMICON'>,0,3,,2
wo<'SM_CXSMSIZE'>,0,3,,2
wo<'SM_CXVIRTUALSCREEN'>,0,3,,2
wo<'SM_CXVSCROLL'>,0,3,,2
wo<'SM_CYBORDER'>,f1.1870-f1,3,,2
wo<'SM_CYCAPTION'>,f1.1871-f1,3,,2
wo<'SM_CYCURSOR'>,0,3,,2
wo<'SM_CYDLGFRAME'>,f1.1873-f1,3,,2
wo<'SM_CYDOUBLECLK'>,0,3,,2
wo<'SM_CYDRAG'>,0,3,,2
wo<'SM_CYEDGE'>,f1.1876-f1,3,,2
wo<'SM_CYFIXEDFRAME'>,0,3,,2
wo<'SM_CYFRAME'>,0,3,,2
wo<'SM_CYFULLSCREEN'>,0,3,,2
wo<'SM_CYHSCROLL'>,0,3,,2
wo<'SM_CYICON'>,0,3,,2
wo<'SM_CYICONSPACING'>,0,3,,2
wo<'SM_CYKANJIWINDOW'>,0,3,,2
wo<'SM_CYMAXIMIZED'>,0,3,,2
wo<'SM_CYMAXTRACK'>,0,3,,2
wo<'SM_CYMENU'>,0,3,,2
wo<'SM_CYMENUCHECK'>,0,3,,2
wo<'SM_CYMENUSIZE'>,0,3,,2
wo<'SM_CYMIN'>,0,3,,2
wo<'SM_CYMINIMIZED'>,0,3,,2
wo<'SM_CYMINSPACING'>,0,3,,2
wo<'SM_CYMINTRACK'>,0,3,,2
wo<'SM_CYSCREEN'>,0,3,,2
wo<'SM_CYSIZE'>,0,3,,2
wo<'SM_CYSIZEFRAME'>,0,3,,2
wo<'SM_CYSMCAPTION'>,0,3,,2
wo<'SM_CYSMICON'>,0,3,,2
wo<'SM_CYSMSIZE'>,0,3,,2
wo<'SM_CYVIRTUALSCREEN'>,0,3,,2
wo<'SM_CYVSCROLL'>,0,3,,2
wo<'SM_CYVTHUMB'>,0,3,,2
wo<'SM_DBCSENABLED'>,0,3,,2
wo<'SM_DEBUG'>,0,3,,2
wo<'SM_MENUDROPALIGNMENT'>,0,3,,2
wo<'SM_MIDEASTENABLED'>,0,3,,2
wo<'SM_MOUSEPRESENT'>,0,3,,2
wo<'SM_MOUSEWHEELPRESENT'>,0,3,,2
wo<'SM_NETWORK'>,0,3,,2
wo<'SM_PENWINDOWS'>,0,3,,2
wo<'SM_RESERVED1'>,0,3,,2
wo<'SM_RESERVED2'>,0,3,,2
wo<'SM_RESERVED3'>,0,3,,2
wo<'SM_RESERVED4'>,0,3,,2
wo<'SM_SAMEDISPLAYFORMAT'>,0,3,,2
wo<'SM_SECURE'>,0,3,,2
wo<'SM_SHOWSOUNDS'>,0,3,,2
wo<'SM_SLOWMACHINE'>,0,3,,2
wo<'SM_SWAPBUTTON'>,0,3,,2
wo<'SM_XVIRTUALSCREEN'>,0,3,,2
wo<'SM_YVIRTUALSCREEN'>,0,3,,2
wo<'SPIF_SENDCHANGE'>,f1.1921-f1,3,,2
wo<'SPI_GETMOUSE'>,f1.1922-f1,3,,2
wo<'SPI_SETDESKWALLPAPER'>,f1.1923-f1,3,,2
wo<'SPI_SETMOUSE'>,f1.1924-f1,3,,2
wo<'SRCAND'>,f1.1925-f1,3,,2
wo<'SRCCOPY'>,f1.1926-f1,3,,2
wo<'SRCERASE'>,f1.1927-f1,3,,2
wo<'SRCINVERT'>,f1.1928-f1,3,,2
wo<'SRCPAINT'>,f1.1929-f1,3,,2
wo<'SS_BITMAP'>,0,3,,2
wo<'SS_BLACKFRAME'>,f1.1931-f1,3,,2
wo<'SS_BLACKRECT'>,f1.1932-f1,3,,2
wo<'SS_CENTER'>,f1.1933-f1,3,,2
wo<'SS_CENTERIMAGE'>,0,3,,2
wo<'SS_ELLIPSISMASK'>,0,3,,2
wo<'SS_ENDELLIPSIS'>,0,3,,2
wo<'SS_ENHMETAFILE'>,0,3,,2
wo<'SS_ETCHEDFRAME'>,0,3,,2
wo<'SS_ETCHEDHORZ'>,0,3,,2
wo<'SS_ETCHEDVERT'>,0,3,,2
wo<'SS_GRAYFRAME'>,f1.1941-f1,3,,2
wo<'SS_GRAYRECT'>,f1.1942-f1,3,,2
wo<'SS_ICON'>,f1.1943-f1,3,,2
wo<'SS_LEFT'>,f1.1944-f1,3,,2
wo<'SS_LEFTNOWORDWRAP'>,f1.1945-f1,3,,2
wo<'SS_NOPREFIX'>,f1.1946-f1,3,,2
wo<'SS_NOTIFY'>,0,3,,2
wo<'SS_OWNERDRAW'>,0,3,,2
wo<'SS_PATHELLIPSIS'>,0,3,,2
wo<'SS_REALSIZEIMAGE'>,0,3,,2
wo<'SS_RIGHT'>,f1.1951-f1,3,,2
wo<'SS_RIGHTJUST'>,0,3,,2
wo<'SS_SIMPLE'>,f1.1953-f1,3,,2
wo<'SS_SUNKEN'>,0,3,,2
wo<'SS_TYPEMASK'>,0,3,,2
wo<'SS_USERITEM'>,f1.1956-f1,3,,2
wo<'SS_WHITEFRAME'>,f1.1957-f1,3,,2
wo<'SS_WHITERECT'>,f1.1958-f1,3,,2
wo<'SS_WORDELLIPSIS'>,0,3,,2
wo<'STD_PRINT'>,0,3,,2
wo<'SW_FORCEMINIMIZE'>,0,3,,2
wo<'SW_HIDE'>,0,3,,2
wo<'SW_MAX'>,0,3,,2
wo<'SW_MAXIMIZE'>,0,3,,2
wo<'SW_MINIMIZE'>,0,3,,2
wo<'SW_NORMAL'>,0,3,,2
wo<'SW_OTHERUNZOOM'>,0,3,,2
wo<'SW_OTHERZOOM'>,0,3,,2
wo<'SW_PARENTCLOSING'>,0,3,,2
wo<'SW_PARENTOPENING'>,0,3,,2
wo<'SW_RESTORE'>,0,3,,2
wo<'SW_SHOW'>,0,3,,2
wo<'SW_SHOWDEFAULT'>,0,3,,2
wo<'SW_SHOWMAXIMIZED'>,0,3,,2
wo<'SW_SHOWMINIMIZED'>,0,3,,2
wo<'SW_SHOWMINNOACTIVE'>,0,3,,2
wo<'SW_SHOWNA'>,0,3,,2
wo<'SW_SHOWNOACTIVATE'>,0,3,,2
wo<'SW_SHOWNORMAL'>,0,3,,2
wo<'SYMBOL_CHARSET'>,0,3,,2
wo<'SYSTEM_FIXED_FONT'>,f1.1981-f1,3,,2
wo<'SYSTEM_FONT'>,f1.1982-f1,3,,2
wo<'SaveDC'>,f1.1983-f1,3,,6
wo<'ScaleViewportExt'>,0,3,,6
wo<'ScaleWindowExt'>,f1.1985-f1,3,,6
wo<'ScreenToClient'>,0,3,,6
wo<'ScrollDC'>,f1.1987-f1,3,,6
wo<'ScrollWindow'>,0,3,,6
wo<'ScrollWindowEx'>,0,3,,6
wo<'SelectClipPath'>,f1.1990-f1,3,,6
wo<'SelectClipRgn'>,f1.1991-f1,3,,6
wo<'SelectObject'>,f1.1992-f1,3,,6
wo<'SelectStockObject'>,f1.1993-f1,3,,6
wo<'SendDlgItemMessage'>,0,3,,6
wo<'SendMessage'>,0,3,,6
wo<'SendNotification'>,0,3,,6
wo<'SetActiveWindow'>,0,3,,6
wo<'SetBkColor'>,f1.1998-f1,3,,6
wo<'SetBkMode'>,f1.1999-f1,3,,6
wo<'SetBkgndColor'>,0,3,,6
wo<'SetBoundsRect'>,f1.2001-f1,3,,6
wo<'SetBrushOrg'>,f1.2002-f1,3,,6
wo<'SetCaption'>,0,3,,6
wo<'SetCapture'>,0,3,,6
wo<'SetCaretBlinkTime'>,0,3,,6
wo<'SetCaretPos'>,0,3,,6
wo<'SetClassLong'>,0,3,,6
wo<'SetClassWord'>,0,3,,6
wo<'SetClientWindow'>,0,3,,6
wo<'SetColumnWidth'>,0,3,,6
wo<'SetCursor'>,0,3,,6
wo<'SetDIBits'>,0,3,,6
wo<'SetDIBitsToDevice'>,0,3,,6
wo<'SetDlgItemInt'>,0,3,,6
wo<'SetDlgItemText'>,0,3,,6
wo<'SetDocTitle'>,0,3,,6
wo<'SetExStyle'>,0,3,,6
wo<'SetFlag'>,0,3,,6
wo<'SetFocus'>,0,3,,6
wo<'SetGlyph'>,f1.2020-f1,3,,6
wo<'SetHintMode'>,0,3,,6
wo<'SetImageIndex'>,0,3,,6
wo<'SetImageList'>,0,3,,6
wo<'SetMapMode'>,f1.2024-f1,3,,6
wo<'SetMapperFlags'>,f1.2025-f1,3,,6
wo<'SetMenu'>,0,3,,6
wo<'SetMiterLimit'>,f1.2027-f1,3,,6
wo<'SetModule'>,0,3,,6
wo<'SetNext'>,0,3,,6
wo<'SetParent'>,0,3,,6
wo<'SetPixel'>,f1.2031-f1,3,,6
wo<'SetPolyFillMode'>,f1.2032-f1,3,,6
wo<'SetProp'>,0,3,,6
wo<'SetROP2'>,f1.2034-f1,3,,6
wo<'SetRedraw'>,0,3,,6
wo<'SetScrollPos'>,0,3,,6
wo<'SetScrollRange'>,0,3,,6
wo<'SetScroller'>,0,3,,6
wo<'SetSelIndex'>,0,3,,6
wo<'SetSplitterCushion'>,0,3,,6
wo<'SetStretchBltMode'>,f1.2041-f1,3,,6
wo<'SetStyle'>,0,3,,6
wo<'SetSysModalWindow'>,0,3,,6
wo<'SetSystemPaletteUse'>,f1.2044-f1,3,,6
wo<'SetTextAlign'>,f1.2045-f1,3,,6
wo<'SetTextCharacterExtra'>,f1.2046-f1,3,,6
wo<'SetTextColor'>,f1.2047-f1,3,,6
wo<'SetTextJustification'>,f1.2048-f1,3,,6
wo<'SetTimer'>,0,3,,6
wo<'SetTransferBuffer'>,0,3,,6
wo<'SetValidator'>,0,3,,6
wo<'SetViewportExt'>,0,3,,6
wo<'SetViewportOrg'>,f1.2053-f1,3,,6
wo<'SetWindowExt'>,f1.2054-f1,3,,6
wo<'SetWindowFont'>,0,3,,6
wo<'SetWindowLong'>,0,3,,6
wo<'SetWindowOrg'>,f1.2057-f1,3,,6
wo<'SetWindowPlacement'>,0,3,,6
wo<'SetWindowPos'>,0,3,,6
wo<'SetWindowText'>,0,3,,6
wo<'SetWindowWord'>,0,3,,6
wo<'SetWorldTransform'>,f1.2062-f1,3,,6
wo<'SetupWindow'>,0,3,,6
wo<'Show'>,0,3,,6
wo<'ShowCaret'>,0,3,,6
wo<'ShowOwnedPopups'>,0,3,,6
wo<'ShowScrollBar'>,0,3,,6
wo<'ShowWindow'>,0,3,,6
wo<'ShutDownWindow'>,0,3,,6
wo<'SplitPane'>,0,3,,6
wo<'Status'>,0,3,,6
wo<'StretchBlt'>,f1.2072-f1,3,,6
wo<'StretchDIBits'>,0,3,,6
wo<'StrokeAndFillPath'>,f1.2074-f1,3,,6
wo<'StrokePath'>,f1.2075-f1,3,,6
wo<'SubclassWindowFunction'>,0,3,,6
wo<'TA_BASELINE'>,0,3,,2
wo<'TA_BOTTOM'>,0,3,,2
wo<'TA_CENTER'>,f1.2079-f1,3,,2
wo<'TA_LEFT'>,f1.2080-f1,3,,2
wo<'TA_MASK'>,0,3,,2
wo<'TA_NOUPDATECP'>,0,3,,2
wo<'TA_RIGHT'>,f1.2083-f1,3,,2
wo<'TA_TOP'>,0,3,,2
wo<'TA_UPDATECP'>,0,3,,2
wo<'TApplication'>,f1.2086-f1,3,,6
wo<'TBM_CLEARSEL'>,f1.2087-f1,3,,2
wo<'TBM_CLEARTICS'>,f1.2088-f1,3,,2
wo<'TBM_GETCHANNELRECT'>,f1.2089-f1,3,,2
wo<'TBM_GETLINESIZE'>,f1.2090-f1,3,,2
wo<'TBM_GETNUMTICS'>,f1.2091-f1,3,,2
wo<'TBM_GETPAGESIZE'>,f1.2092-f1,3,,2
wo<'TBM_GETPOS'>,f1.2093-f1,3,,2
wo<'TBM_GETPTICS'>,f1.2094-f1,3,,2
wo<'TBM_GETRANGEMAX'>,f1.2095-f1,3,,2
wo<'TBM_GETRANGEMIN'>,f1.2096-f1,3,,2
wo<'TBM_GETSELEND'>,f1.2097-f1,3,,2
wo<'TBM_GETSELSTART'>,f1.2098-f1,3,,2
wo<'TBM_GETTHUMBLENGTH'>,f1.2099-f1,3,,2
wo<'TBM_GETTHUMBRECT'>,f1.2100-f1,3,,2
wo<'TBM_GETTIC'>,f1.2101-f1,3,,2
wo<'TBM_GETTICPOS'>,f1.2102-f1,3,,2
wo<'TBM_SETLINESIZE'>,f1.2103-f1,3,,2
wo<'TBM_SETPAGESIZE'>,f1.2104-f1,3,,2
wo<'TBM_SETPOS'>,f1.2105-f1,3,,2
wo<'TBM_SETRANGE'>,f1.2106-f1,3,,2
wo<'TBM_SETRANGEMAX'>,f1.2107-f1,3,,2
wo<'TBM_SETRANGEMIN'>,f1.2108-f1,3,,2
wo<'TBM_SETSEL'>,f1.2109-f1,3,,2
wo<'TBM_SETSELEND'>,f1.2110-f1,3,,2
wo<'TBM_SETSELSTART'>,f1.2111-f1,3,,2
wo<'TBM_SETTHUMBLENGTH'>,f1.2112-f1,3,,2
wo<'TBM_SETTIC'>,f1.2113-f1,3,,2
wo<'TBM_SETTICFREQ'>,f1.2114-f1,3,,2
wo<'TBSTATE_CHECKED'>,0,3,,2
wo<'TBSTATE_ENABLED'>,0,3,,2
wo<'TBSTATE_HIDDEN'>,0,3,,2
wo<'TBSTATE_INDETERMINATE'>,0,3,,2
wo<'TBSTATE_PRESSED'>,0,3,,2
wo<'TBSTATE_WRAP'>,0,3,,2
wo<'TBSTYLE_ALTDRAG'>,0,3,,2
wo<'TBSTYLE_BUTTON'>,0,3,,2
wo<'TBSTYLE_CHECK'>,0,3,,2
wo<'TBSTYLE_CHECKGROUP'>,0,3,,2
wo<'TBSTYLE_CUSTOMERASE'>,0,3,,2
wo<'TBSTYLE_FLAT'>,0,3,,2
wo<'TBSTYLE_GROUP'>,0,3,,2
wo<'TBSTYLE_LIST'>,0,3,,2
wo<'TBSTYLE_REGISTERDROP'>,0,3,,2
wo<'TBSTYLE_SEP'>,0,3,,2
wo<'TBSTYLE_TOOLTIPS'>,0,3,,2
wo<'TBSTYLE_TRANSPARENT'>,0,3,,2
wo<'TBSTYLE_WRAPABLE'>,0,3,,2
wo<'TBS_AUTOTICKS'>,f1.2134-f1,3,,2
wo<'TBS_BOTH'>,f1.2135-f1,3,,2
wo<'TBS_BOTTOM'>,f1.2136-f1,3,,2
wo<'TBS_ENABLESELRANGE'>,f1.2137-f1,3,,2
wo<'TBS_FIXEDLENGTH'>,0,3,,2
wo<'TBS_HORZ'>,f1.2139-f1,3,,2
wo<'TBS_LEFT'>,f1.2140-f1,3,,2
wo<'TBS_NOTHUMB'>,0,3,,2
wo<'TBS_NOTICKS'>,f1.2142-f1,3,,2
wo<'TBS_RIGHT'>,f1.2143-f1,3,,2
wo<'TBS_TOOLTIPS'>,0,3,,2
wo<'TBS_TOP'>,f1.2145-f1,3,,2
wo<'TBS_VERT'>,f1.2146-f1,3,,2
wo<'TB_ADDBITMAP'>,0,3,,2
wo<'TB_ADDBUTTONS'>,0,3,,2
wo<'TB_ADDSTRING'>,0,3,,2
wo<'TB_AUTOSIZE'>,0,3,,2
wo<'TB_BUTTONCOUNT'>,0,3,,2
wo<'TB_BUTTONSTRUCTSIZE'>,0,3,,2
wo<'TB_CHANGEBITMAP'>,0,3,,2
wo<'TB_COMMANDTOINDEX'>,0,3,,2
wo<'TB_CUSTOMIZE'>,0,3,,2
wo<'TB_DELETEBUTTON'>,0,3,,2
wo<'TB_GETBITMAP'>,0,3,,2
wo<'TB_GETBUTTON'>,0,3,,2
wo<'TB_GETBUTTONTEXT'>,0,3,,2
wo<'TB_GETITEMRECT'>,0,3,,2
wo<'TB_GETROWS'>,0,3,,2
wo<'TB_GETSTATE'>,0,3,,2
wo<'TB_GETSTYLE'>,0,3,,2
wo<'TB_GETTOOLTIPS'>,0,3,,2
wo<'TB_INSERTBUTTON'>,0,3,,2
wo<'TB_REPLACEBITMAP'>,0,3,,2
wo<'TB_SAVERESTORE'>,0,3,,2
wo<'TB_SETBITMAPSIZE'>,0,3,,2
wo<'TB_SETBUTTONSIZE'>,0,3,,2
wo<'TB_SETCMDID'>,0,3,,2
wo<'TB_SETPARENT'>,0,3,,2
wo<'TB_SETROWS'>,0,3,,2
wo<'TB_SETSTATE'>,0,3,,2
wo<'TB_SETSTYLE'>,0,3,,2
wo<'TB_SETTOOLTIPS'>,0,3,,2
wo<'TBandInfo'>,0,3,,6
wo<'TBitMap'>,0,3,,6
wo<'TBitMapGadget'>,0,3,,6
wo<'TBitSet'>,0,3,,6
wo<'TBitmap'>,0,3,,6
wo<'TBrush'>,0,3,,6
wo<'TBtnBitmap'>,f1.2182-f1,3,,6
wo<'TButton'>,0,3,,6
wo<'TButtonGadget'>,f1.2184-f1,3,,6
wo<'TCS_BOTTOM'>,0,3,,2
wo<'TCS_BUTTONS'>,0,3,,2
wo<'TCS_FIXEDWIDTH'>,0,3,,2
wo<'TCS_FLATBUTTONS'>,0,3,,2
wo<'TCS_FOCUSNEVER'>,0,3,,2
wo<'TCS_FOCUSONBUTTONDOWN'>,0,3,,2
wo<'TCS_FORCEICONLEFT'>,0,3,,2
wo<'TCS_FORCELABELLEFT'>,0,3,,2
wo<'TCS_HOTTRACK'>,0,3,,2
wo<'TCS_MULTILINE'>,0,3,,2
wo<'TCS_MULTISELECT'>,0,3,,2
wo<'TCS_OWNERDRAWFIXED'>,0,3,,2
wo<'TCS_RAGGEDRIGHT'>,0,3,,2
wo<'TCS_RIGHT'>,0,3,,2
wo<'TCS_RIGHTJUSTIFY'>,0,3,,2
wo<'TCS_SCROLLOPPOSITE'>,0,3,,2
wo<'TCS_SINGLELINE'>,0,3,,2
wo<'TCS_TABS'>,0,3,,2
wo<'TCS_TOOLTIPS'>,0,3,,2
wo<'TCS_VERTICAL'>,0,3,,2
wo<'TCelArray'>,0,3,,6
wo<'TCharSet'>,0,3,,6
wo<'TCheckBox'>,0,3,,6
wo<'TChooseColor'>,0,3,,6
wo<'TChooseColorDialog'>,0,3,,6
wo<'TChooseFont'>,0,3,,6
wo<'TClientDC'>,0,3,,6
wo<'TClipboard'>,0,3,,6
wo<'TClipboardViewer'>,0,3,,6
wo<'TColor'>,0,3,,6
wo<'TComboBox'>,0,3,,6
wo<'TCommonDial'>,0,3,,6
wo<'TControl'>,0,3,,6
wo<'TControlBar'>,0,3,,6
wo<'TControlGadget'>,0,3,,6
wo<'TCreatedDC'>,0,3,,6
wo<'TCursor'>,0,3,,6
wo<'TCusor'>,0,3,,6
wo<'TDC'>,f1.2223-f1,3,,6
wo<'TDecoratedFrame'>,0,3,,6
wo<'TDecoratedMDIFrame'>,0,3,,6
wo<'TDeskTopDC'>,f1.2226-f1,3,,6
wo<'TDesktopDC'>,0,3,,6
wo<'TDialog'>,0,3,,6
wo<'TDialogAttr'>,0,3,,6
wo<'TDib'>,0,3,,6
wo<'TDibDC'>,f1.2231-f1,3,,6
wo<'TDocManager'>,0,3,,6
wo<'TDocTemplate'>,0,3,,6
wo<'TDocument'>,0,3,,6
wo<'TDropinfo'>,0,3,,6
wo<'TEdit'>,0,3,,6
wo<'TEditFile'>,0,3,,6
wo<'TEditSearch'>,0,3,,6
wo<'TEditView'>,0,3,,6
wo<'TEnhMetaFilePict'>,0,3,,6
wo<'TEventHandler'>,0,3,,6
wo<'TFileDocument'>,0,3,,6
wo<'TFileOpenDialog'>,0,3,,6
wo<'TFileSaveDialog'>,0,3,,6
wo<'TFilterValidator'>,0,3,,6
wo<'TFindDialog'>,0,3,,6
wo<'TFindReplaceDialog'>,0,3,,6
wo<'TFloatingFrame'>,0,3,,6
wo<'TFont'>,0,3,,6
wo<'TFrameWindow'>,f1.2250-f1,3,,6
wo<'TGadget'>,0,3,,6
wo<'TGadgetWindow'>,0,3,,6
wo<'TGauge'>,0,3,,6
wo<'TGdiBase'>,0,3,,6
wo<'TGdiObject'>,0,3,,6
wo<'TGlyphButton'>,f1.2256-f1,3,,6
wo<'TGroupBox'>,0,3,,6
wo<'THAI_CHARSET'>,0,3,,2
wo<'THSlider'>,0,3,,6
wo<'THandle'>,0,3,,6
wo<'TIC'>,f1.2261-f1,3,,6
wo<'TIcon'>,0,3,,6
wo<'TImageList'>,0,3,,6
wo<'TInStream'>,0,3,,6
wo<'TInputDialog'>,0,3,,6
wo<'TKeybardModeTracker'>,0,3,,6
wo<'TLayoutConstraint'>,0,3,,6
wo<'TLayoutMetrics'>,0,3,,6
wo<'TListBox'>,0,3,,6
wo<'TListBoxData'>,0,3,,6
wo<'TListView'>,0,3,,6
wo<'TListWindColumn'>,0,3,,6
wo<'TListWindItem'>,0,3,,6
wo<'TListWindow'>,0,3,,6
wo<'TLookupValidator'>,0,3,,6
wo<'TMDIChild'>,0,3,,6
wo<'TMDIClient'>,0,3,,6
wo<'TMDIFrame'>,0,3,,6
wo<'TME_CANCEL'>,0,3,,2
wo<'TME_HOVER'>,0,3,,2
wo<'TME_LEAVE'>,0,3,,2
wo<'TME_QUERY'>,0,3,,2
wo<'TMemoryDC'>,0,3,,6
wo<'TMenu'>,0,3,,6
wo<'TMenuDesc'>,0,3,,6
wo<'TMessageBar'>,0,3,,6
wo<'TMetaFileBar'>,0,3,,6
wo<'TMetaFileDC'>,0,3,,6
wo<'TMetaFilePict'>,0,3,,6
wo<'TModule'>,0,3,,6
wo<'TOpenSave'>,0,3,,6
wo<'TOutStream'>,0,3,,6
wo<'TPM_RIGHTALIGN'>,0,3,,2
wo<'TPaintDC'>,0,3,,6
wo<'TPalette'>,0,3,,6
wo<'TPaneSplitter'>,0,3,,6
wo<'TPen'>,0,3,,6
wo<'TPictureValidator'>,0,3,,6
wo<'TPoint'>,0,3,,6
wo<'TPointer'>,0,3,,6
wo<'TPopupMenu'>,0,3,,6
wo<'TPreviewPage'>,0,3,,6
wo<'TPrintDC'>,0,3,,6
wo<'TPrintDialog'>,0,3,,6
wo<'TPrinter'>,0,3,,6
wo<'TPrinterAbortDlg'>,0,3,,6
wo<'TPrintout'>,0,3,,6
wo<'TProcInstance'>,0,3,,6
wo<'TRANSPARENT'>,0,3,,2
wo<'TRUE'>,0,3,,2
wo<'TRadioButton'>,0,3,,6
wo<'TRangeValidator'>,0,3,,6
wo<'TRect'>,0,3,,6
wo<'TRegion'>,0,3,,6
wo<'TResId'>,0,3,,6
wo<'TScreenDC'>,0,3,,6
wo<'TScrollBar'>,0,3,,6
wo<'TScrollBarData'>,0,3,,6
wo<'TScroller'>,0,3,,6
wo<'TSeparatorGadget'>,0,3,,6
wo<'TSize'>,0,3,,6
wo<'TSlider'>,0,3,,6
wo<'TStatic'>,0,3,,6
wo<'TStatus'>,0,3,,6
wo<'TStatusBar'>,0,3,,6
wo<'TStream'>,0,3,,6
wo<'TStringLookupValidator'>,0,3,,6
wo<'TSystemMenu'>,0,3,,6
wo<'TTS_ALWAYSTIP'>,0,3,,2
wo<'TTS_NOPREFIX'>,0,3,,2
wo<'TTextGadget'>,0,3,,6
wo<'TTinyCaption'>,0,3,,6
wo<'TToolBox'>,0,3,,6
wo<'TURKISH_CHARSET'>,0,3,,2
wo<'TVBXControl'>,0,3,,6
wo<'TVS_CHECKBOXES'>,0,3,,2
wo<'TVS_DISABLEDRAGDROP'>,0,3,,2
wo<'TVS_EDITLABELS'>,0,3,,2
wo<'TVS_FULLROWSELECT'>,0,3,,2
wo<'TVS_HASBUTTONS'>,f1.2340-f1,3,,2
wo<'TVS_HASLINES'>,f1.2341-f1,3,,2
wo<'TVS_INFOTIP'>,0,3,,2
wo<'TVS_LINESATROOT'>,f1.2343-f1,3,,2
wo<'TVS_NONEVENHEIGHT'>,0,3,,2
wo<'TVS_NOSCROLL'>,0,3,,2
wo<'TVS_NOTOOLTIPS'>,0,3,,2
wo<'TVS_RTLREADING'>,0,3,,2
wo<'TVS_SHOWSELALWAYS'>,0,3,,2
wo<'TVS_SINGLEEXPAND'>,0,3,,2
wo<'TVS_TRACKSELECT'>,0,3,,2
wo<'TVSlider'>,0,3,,6
wo<'TValidator'>,0,3,,6
wo<'TVbxControl'>,0,3,,6
wo<'TVbxEventHandler'>,0,3,,6
wo<'TView'>,0,3,,6
wo<'TWindow'>,f1.2356-f1,3,,6
wo<'TWindowDC'>,f1.2357-f1,3,,6
wo<'TWindowView'>,0,3,,6
wo<'TXCompatibility'>,0,3,,6
wo<'TXOutOfMemory'>,0,3,,6
wo<'TXOwl'>,0,3,,6
wo<'TabbedTextOut'>,f1.2362-f1,3,,6
wo<'Tcontrol'>,0,3,,6
wo<'TextOut'>,f1.2364-f1,3,,6
wo<'TextRect'>,f1.2365-f1,3,,6
wo<'Transfer'>,0,3,,6
wo<'TransferData'>,0,3,,6
wo<'UDS_ALIGNLEFT'>,0,3,,2
wo<'UDS_ALIGNRIGHT'>,0,3,,2
wo<'UDS_ARROWKEYS'>,0,3,,2
wo<'UDS_AUTOBUDDY'>,0,3,,2
wo<'UDS_HORZ'>,0,3,,2
wo<'UDS_HOTTRACK'>,0,3,,2
wo<'UDS_NOTHOUSANDS'>,0,3,,2
wo<'UDS_SETBUDDYINT'>,0,3,,2
wo<'UDS_WRAP'>,0,3,,2
wo<'UnregisterHotKey'>,0,3,,6
wo<'UpdateColors'>,f1.2378-f1,3,,6
wo<'UpdateWindow'>,0,3,,6
wo<'VK_0'>,0,3,,2
wo<'VK_1'>,0,3,,2
wo<'VK_2'>,0,3,,2
wo<'VK_3'>,0,3,,2
wo<'VK_4'>,0,3,,2
wo<'VK_5'>,0,3,,2
wo<'VK_6'>,0,3,,2
wo<'VK_7'>,0,3,,2
wo<'VK_8'>,0,3,,2
wo<'VK_9'>,0,3,,2
wo<'VK_A'>,0,3,,2
wo<'VK_ACCEPT'>,0,3,,2
wo<'VK_ADD'>,0,3,,2
wo<'VK_APPS'>,0,3,,2
wo<'VK_ATTN'>,0,3,,2
wo<'VK_B'>,0,3,,2
wo<'VK_BACK'>,0,3,,2
wo<'VK_C'>,0,3,,2
wo<'VK_CANCEL'>,0,3,,2
wo<'VK_CAPITAL'>,0,3,,2
wo<'VK_CLEAR'>,0,3,,2
wo<'VK_CONTROL'>,0,3,,2
wo<'VK_CONVERT'>,0,3,,2
wo<'VK_CRSEL'>,0,3,,2
wo<'VK_D'>,0,3,,2
wo<'VK_DECIMAL'>,0,3,,2
wo<'VK_DELETE'>,0,3,,2
wo<'VK_DIVIDE'>,0,3,,2
wo<'VK_DOWN'>,f1.2408-f1,3,,2
wo<'VK_E'>,0,3,,2
wo<'VK_END'>,0,3,,2
wo<'VK_EREOF'>,0,3,,2
wo<'VK_ESCAPE'>,0,3,,2
wo<'VK_EXECUTE'>,0,3,,2
wo<'VK_EXSEL'>,0,3,,2
wo<'VK_F'>,0,3,,2
wo<'VK_F1'>,0,3,,2
wo<'VK_F10'>,0,3,,2
wo<'VK_F11'>,0,3,,2
wo<'VK_F12'>,0,3,,2
wo<'VK_F13'>,0,3,,2
wo<'VK_F14'>,0,3,,2
wo<'VK_F15'>,0,3,,2
wo<'VK_F16'>,0,3,,2
wo<'VK_F17'>,0,3,,2
wo<'VK_F18'>,0,3,,2
wo<'VK_F19'>,0,3,,2
wo<'VK_F2'>,0,3,,2
wo<'VK_F20'>,0,3,,2
wo<'VK_F21'>,0,3,,2
wo<'VK_F22'>,0,3,,2
wo<'VK_F23'>,0,3,,2
wo<'VK_F24'>,0,3,,2
wo<'VK_F3'>,0,3,,2
wo<'VK_F4'>,0,3,,2
wo<'VK_F5'>,0,3,,2
wo<'VK_F6'>,0,3,,2
wo<'VK_F7'>,0,3,,2
wo<'VK_F8'>,0,3,,2
wo<'VK_F9'>,0,3,,2
wo<'VK_FINAL'>,0,3,,2
wo<'VK_G'>,0,3,,2
wo<'VK_H'>,0,3,,2
wo<'VK_HANGEUL'>,0,3,,2
wo<'VK_HANGUL'>,0,3,,2
wo<'VK_HANJA'>,0,3,,2
wo<'VK_HELP'>,0,3,,2
wo<'VK_HOME'>,0,3,,2
wo<'VK_I'>,0,3,,2
wo<'VK_INSERT'>,0,3,,2
wo<'VK_J'>,0,3,,2
wo<'VK_JUNJA'>,0,3,,2
wo<'VK_K'>,0,3,,2
wo<'VK_KANA'>,0,3,,2
wo<'VK_KANJI'>,0,3,,2
wo<'VK_L'>,0,3,,2
wo<'VK_LBUTTON'>,f1.2456-f1,3,,2
wo<'VK_LCONTROL'>,0,3,,2
wo<'VK_LEFT'>,f1.2458-f1,3,,2
wo<'VK_LMENU'>,0,3,,2
wo<'VK_LSHIFT'>,0,3,,2
wo<'VK_LWIN'>,0,3,,2
wo<'VK_M'>,0,3,,2
wo<'VK_MBUTTON'>,f1.2463-f1,3,,2
wo<'VK_MENU'>,0,3,,2
wo<'VK_MODECHANGE'>,0,3,,2
wo<'VK_MULTIPLY'>,0,3,,2
wo<'VK_N'>,0,3,,2
wo<'VK_NEXT'>,0,3,,2
wo<'VK_NONAME'>,0,3,,2
wo<'VK_NONCONVERT'>,0,3,,2
wo<'VK_NUMLOCK'>,0,3,,2
wo<'VK_NUMPAD0'>,0,3,,2
wo<'VK_NUMPAD1'>,0,3,,2
wo<'VK_NUMPAD2'>,0,3,,2
wo<'VK_NUMPAD3'>,0,3,,2
wo<'VK_NUMPAD4'>,0,3,,2
wo<'VK_NUMPAD5'>,0,3,,2
wo<'VK_NUMPAD6'>,0,3,,2
wo<'VK_NUMPAD7'>,0,3,,2
wo<'VK_NUMPAD8'>,0,3,,2
wo<'VK_NUMPAD9'>,0,3,,2
wo<'VK_O'>,0,3,,2
wo<'VK_OEM_CLEAR'>,0,3,,2
wo<'VK_P'>,0,3,,2
wo<'VK_PA1'>,0,3,,2
wo<'VK_PAUSE'>,0,3,,2
wo<'VK_PGDN'>,0,3,,2
wo<'VK_PGUP'>,0,3,,2
wo<'VK_PLAY'>,0,3,,2
wo<'VK_PRINT'>,0,3,,2
wo<'VK_PRIOR'>,0,3,,2
wo<'VK_PROCESSKEY'>,0,3,,2
wo<'VK_Q'>,0,3,,2
wo<'VK_R'>,0,3,,2
wo<'VK_RBUTTON'>,f1.2495-f1,3,,2
wo<'VK_RCONTROL'>,0,3,,2
wo<'VK_RETURN'>,0,3,,2
wo<'VK_RIGHT'>,f1.2498-f1,3,,2
wo<'VK_RMENU'>,0,3,,2
wo<'VK_RSHIFT'>,0,3,,2
wo<'VK_RWIN'>,0,3,,2
wo<'VK_S'>,0,3,,2
wo<'VK_SCROLL'>,0,3,,2
wo<'VK_SELECT'>,0,3,,2
wo<'VK_SEPARATOR'>,0,3,,2
wo<'VK_SHIFT'>,0,3,,2
wo<'VK_SNAPSHOT'>,0,3,,2
wo<'VK_SPACE'>,0,3,,2
wo<'VK_SUBTRACT'>,0,3,,2
wo<'VK_T'>,0,3,,2
wo<'VK_TAB'>,0,3,,2
wo<'VK_U'>,0,3,,2
wo<'VK_UP'>,f1.2513-f1,3,,2
wo<'VK_V'>,0,3,,2
wo<'VK_W'>,0,3,,2
wo<'VK_X'>,0,3,,2
wo<'VK_Y'>,0,3,,2
wo<'VK_Z'>,0,3,,2
wo<'VK_ZOOM'>,0,3,,2
wo<'Validate'>,0,3,,6
wo<'ValidateRect'>,0,3,,6
wo<'ValidateRgn'>,0,3,,6
wo<'WA_ACTIVE'>,0,3,,2
wo<'WA_CLICKACTIVE'>,0,3,,2
wo<'WA_INACTIVE'>,0,3,,2
wo<'WHEEL_DELTA'>,0,3,,2
wo<'WHEEL_PAGESCROLL'>,0,3,,2
wo<'WHITENESS'>,f1.2528-f1,3,,2
wo<'WHITE_BRUSH'>,f1.2529-f1,3,,2
wo<'WHITE_PEN'>,f1.2530-f1,3,,2
wo<'WMSZ_BOTTOM'>,0,3,,2
wo<'WMSZ_BOTTOMLEFT'>,0,3,,2
wo<'WMSZ_BOTTOMRIGHT'>,0,3,,2
wo<'WMSZ_LEFT'>,0,3,,2
wo<'WMSZ_RIGHT'>,0,3,,2
wo<'WMSZ_TOP'>,0,3,,2
wo<'WMSZ_TOPLEFT'>,0,3,,2
wo<'WMSZ_TOPRIGHT'>,0,3,,2
wo<'WM_ACTIVATE'>,f1.2539-f1,3,,2
wo<'WM_ACTIVATEAPP'>,f1.2540-f1,3,,2
wo<'WM_AFXFIRST'>,0,3,,2
wo<'WM_AFXLAST'>,0,3,,2
wo<'WM_APP'>,0,3,,2
wo<'WM_ASKCBFORMATNAME'>,f1.2544-f1,3,,2
wo<'WM_CANCELJOURNAL'>,0,3,,2
wo<'WM_CANCELMODE'>,f1.2546-f1,3,,2
wo<'WM_CAPTURECHANGED'>,0,3,,2
wo<'WM_CHANGECBCHAIN'>,0,3,,2
wo<'WM_CHAR'>,0,3,,2
wo<'WM_CHARTOITEM'>,0,3,,2
wo<'WM_CHILDACTIVATE'>,f1.2551-f1,3,,2
wo<'WM_CLEAR'>,0,3,,2
wo<'WM_CLOSE'>,f1.2553-f1,3,,2
wo<'WM_COMMAND'>,0,3,,2
wo<'WM_COMMANDIDLE'>,0,3,,2
wo<'WM_COMMNOTIFY'>,0,3,,2
wo<'WM_COMPACTING'>,0,3,,2
wo<'WM_COMPAREITEM'>,0,3,,2
wo<'WM_CONTEXTMENU'>,0,3,,2
wo<'WM_COPY'>,0,3,,2
wo<'WM_COPYDATA'>,0,3,,2
wo<'WM_CREATE'>,f1.2562-f1,3,,2
wo<'WM_CTLCOLOR'>,f1.2563-f1,3,,2
wo<'WM_CTLCOLORBTN'>,0,3,,2
wo<'WM_CTLCOLORDLG'>,0,3,,2
wo<'WM_CTLCOLOREDIT'>,0,3,,2
wo<'WM_CTLCOLORLISTBOX'>,0,3,,2
wo<'WM_CTLCOLORMSGBOX'>,0,3,,2
wo<'WM_CTLCOLORSCROLLBAR'>,0,3,,2
wo<'WM_CTLCOLORSTATIC'>,0,3,,2
wo<'WM_CUSTOMMESSAGE'>,0,3,,2
wo<'WM_CUT'>,0,3,,2
wo<'WM_DDE_ACK'>,0,3,,2
wo<'WM_DDE_ADVISE'>,0,3,,2
wo<'WM_DDE_DATA'>,0,3,,2
wo<'WM_DDE_EXECUTE'>,0,3,,2
wo<'WM_DDE_FIRST'>,0,3,,2
wo<'WM_DDE_INITIATE'>,0,3,,2
wo<'WM_DDE_LAST'>,0,3,,2
wo<'WM_DDE_POKE'>,0,3,,2
wo<'WM_DDE_REQUEST'>,0,3,,2
wo<'WM_DDE_TERMINATE'>,0,3,,2
wo<'WM_DDE_UNADVISE'>,0,3,,2
wo<'WM_DEADCHAR'>,0,3,,2
wo<'WM_DELETEITEM'>,0,3,,2
wo<'WM_DESTROY'>,f1.2586-f1,3,,2
wo<'WM_DESTROYCLIPBOARD'>,0,3,,2
wo<'WM_DEVICECHANGE'>,0,3,,2
wo<'WM_DEVMODECHANGE'>,0,3,,2
wo<'WM_DISPLAYCHANGE'>,0,3,,2
wo<'WM_DRAWCITEM'>,0,3,,2
wo<'WM_DRAWCLIPBOARD'>,0,3,,2
wo<'WM_DRAWITEM'>,0,3,,2
wo<'WM_DROPFILES'>,0,3,,2
wo<'WM_ENABLE'>,f1.2595-f1,3,,2
wo<'WM_ENDSESSION'>,f1.2596-f1,3,,2
wo<'WM_ENTERIDLE'>,f1.2597-f1,3,,2
wo<'WM_ENTERMENULOOP'>,0,3,,2
wo<'WM_ENTERSIZEMOVE'>,0,3,,2
wo<'WM_ERASEBKGND'>,f1.2600-f1,3,,2
wo<'WM_EXITMENULOOP'>,0,3,,2
wo<'WM_EXITSIZEMOVE'>,0,3,,2
wo<'WM_FONTCHANGE'>,0,3,,2
wo<'WM_GETDLGCODE'>,0,3,,2
wo<'WM_GETFONT'>,0,3,,2
wo<'WM_GETHOTKEY'>,0,3,,2
wo<'WM_GETICON'>,0,3,,2
wo<'WM_GETMINMAXINFO'>,f1.2608-f1,3,,2
wo<'WM_GETOBJECT'>,0,3,,2
wo<'WM_GETTEXT'>,f1.2610-f1,3,,2
wo<'WM_GETTEXTLEN'>,0,3,,2
wo<'WM_GETTEXTLENGTH'>,f1.2612-f1,3,,2
wo<'WM_HANDHELDFIRST'>,0,3,,2
wo<'WM_HANDHELDLAST'>,0,3,,2
wo<'WM_HELP'>,0,3,,2
wo<'WM_HOTKEY'>,0,3,,2
wo<'WM_HSCROLL'>,0,3,,2
wo<'WM_HSCROLLCLIPBOARD'>,0,3,,2
wo<'WM_ICONERASEBKGND'>,f1.2619-f1,3,,2
wo<'WM_IME_CHAR'>,0,3,,2
wo<'WM_IME_COMPOSITION'>,0,3,,2
wo<'WM_IME_COMPOSITIONFULL'>,0,3,,2
wo<'WM_IME_CONTROL'>,0,3,,2
wo<'WM_IME_ENDCOMPOSITION'>,0,3,,2
wo<'WM_IME_KEYDOWN'>,0,3,,2
wo<'WM_IME_KEYLAST'>,0,3,,2
wo<'WM_IME_KEYUP'>,0,3,,2
wo<'WM_IME_NOTIFY'>,0,3,,2
wo<'WM_IME_REQUEST'>,0,3,,2
wo<'WM_IME_SELECT'>,0,3,,2
wo<'WM_IME_SETCONTEXT'>,0,3,,2
wo<'WM_IME_STARTCOMPOSITION'>,0,3,,2
wo<'WM_INITDIALOG'>,0,3,,2
wo<'WM_INITMENU'>,0,3,,2
wo<'WM_INITMENUPOPUP'>,0,3,,2
wo<'WM_INPUTLANGCHANGE'>,0,3,,2
wo<'WM_INPUTLANGCHANGEREQUEST'>,0,3,,2
wo<'WM_KEYDOWN'>,f1.2638-f1,3,,2
wo<'WM_KEYFIRST'>,0,3,,2
wo<'WM_KEYLAST'>,0,3,,2
wo<'WM_KEYUP'>,f1.2641-f1,3,,2
wo<'WM_KILLFOCUS'>,f1.2642-f1,3,,2
wo<'WM_LBUTTONDBLCLK'>,0,3,,2
wo<'WM_LBUTTONDOWN'>,0,3,,2
wo<'WM_LBUTTONUP'>,0,3,,2
wo<'WM_MBUTTONDBLCLK'>,0,3,,2
wo<'WM_MBUTTONDOWN'>,0,3,,2
wo<'WM_MBUTTONUP'>,0,3,,2
wo<'WM_MDIACTIVATE'>,0,3,,2
wo<'WM_MDICASCADE'>,0,3,,2
wo<'WM_MDICREATE'>,0,3,,2
wo<'WM_MDIDESTROY'>,0,3,,2
wo<'WM_MDIGETACTIVE'>,0,3,,2
wo<'WM_MDIICONARRANGE'>,0,3,,2
wo<'WM_MDIMAXIMIZE'>,0,3,,2
wo<'WM_MDINEXT'>,0,3,,2
wo<'WM_MDIREFRESHMENU'>,0,3,,2
wo<'WM_MDIRESTORE'>,0,3,,2
wo<'WM_MDISETMENU'>,0,3,,2
wo<'WM_MDITILE'>,0,3,,2
wo<'WM_MEASUREITEM'>,0,3,,2
wo<'WM_MENUCHAR'>,f1.2662-f1,3,,2
wo<'WM_MENUCOMMAND'>,0,3,,2
wo<'WM_MENUDRAG'>,0,3,,2
wo<'WM_MENUGETOBJECT'>,0,3,,2
wo<'WM_MENURBUTTONUP'>,0,3,,2
wo<'WM_MENUSELECT'>,f1.2667-f1,3,,2
wo<'WM_MOUSEACTIVATE'>,f1.2668-f1,3,,2
wo<'WM_MOUSEFIRST'>,0,3,,2
wo<'WM_MOUSEHOVER'>,0,3,,2
wo<'WM_MOUSELEAVE'>,0,3,,2
wo<'WM_MOUSEMOVE'>,f1.2672-f1,3,,2
wo<'WM_MOVE'>,f1.2673-f1,3,,2
wo<'WM_MOVING'>,0,3,,2
wo<'WM_NCACTIVATE'>,0,3,,2
wo<'WM_NCCALCSIZE'>,0,3,,2
wo<'WM_NCCREATE'>,0,3,,2
wo<'WM_NCDESTROY'>,0,3,,2
wo<'WM_NCHITTEST'>,0,3,,2
wo<'WM_NCLBUTTONDBLCLK'>,0,3,,2
wo<'WM_NCLBUTTONDOWN'>,0,3,,2
wo<'WM_NCLBUTTONUP'>,0,3,,2
wo<'WM_NCMBUTTONDBLCLK'>,0,3,,2
wo<'WM_NCMBUTTONDOWN'>,0,3,,2
wo<'WM_NCMBUTTONUP'>,0,3,,2
wo<'WM_NCMOUSEMOVE'>,0,3,,2
wo<'WM_NCPAINT'>,0,3,,2
wo<'WM_NCRBUTTONDBLCLK'>,0,3,,2
wo<'WM_NCRBUTTONDOWN'>,0,3,,2
wo<'WM_NCRBUTTONUP'>,0,3,,2
wo<'WM_NEXTDLGCTL'>,0,3,,2
wo<'WM_NEXTMENU'>,0,3,,2
wo<'WM_NOTIFY'>,0,3,,2
wo<'WM_NOTIFYFORMAT'>,0,3,,2
wo<'WM_NULL'>,0,3,,2
wo<'WM_PAINT'>,f1.2696-f1,3,,2
wo<'WM_PAINTCLIPBOARD'>,0,3,,2
wo<'WM_PAINTICON'>,f1.2698-f1,3,,2
wo<'WM_PALETTECHANGED'>,0,3,,2
wo<'WM_PALETTEISCHANGING'>,0,3,,2
wo<'WM_PARENTNOTIFY'>,f1.2701-f1,3,,2
wo<'WM_PASTE'>,0,3,,2
wo<'WM_PENWINFIRST'>,0,3,,2
wo<'WM_PENWINLAST'>,0,3,,2
wo<'WM_POWER'>,0,3,,2
wo<'WM_POWERBROADCAST'>,0,3,,2
wo<'WM_PRINT'>,0,3,,2
wo<'WM_PRINTCLIENT'>,0,3,,2
wo<'WM_QUERYDRAGICON'>,0,3,,2
wo<'WM_QUERYENDSESSION'>,f1.2710-f1,3,,2
wo<'WM_QUERYNEWPALETTE'>,0,3,,2
wo<'WM_QUERYOPEN'>,0,3,,2
wo<'WM_QUEUESYNC'>,0,3,,2
wo<'WM_QUIT'>,f1.2714-f1,3,,2
wo<'WM_RBUTTONDBLCLK'>,0,3,,2
wo<'WM_RBUTTONDOWN'>,0,3,,2
wo<'WM_RBUTTONUP'>,0,3,,2
wo<'WM_RENDERALLFORMATS'>,0,3,,2
wo<'WM_RENDERFORMAT'>,0,3,,2
wo<'WM_SETCURSOR'>,0,3,,2
wo<'WM_SETFOCUS'>,f1.2721-f1,3,,2
wo<'WM_SETFONT'>,f1.2722-f1,3,,2
wo<'WM_SETHOTKEY'>,0,3,,2
wo<'WM_SETICON'>,0,3,,2
wo<'WM_SETREDRAW'>,f1.2725-f1,3,,2
wo<'WM_SETTEXT'>,f1.2726-f1,3,,2
wo<'WM_SETTINGCHANGE'>,0,3,,2
wo<'WM_SHELLNOTIFY'>,0,3,,2
wo<'WM_SHOWWINDOW'>,f1.2729-f1,3,,2
wo<'WM_SIZE'>,f1.2730-f1,3,,2
wo<'WM_SIZECLIPBOARD'>,0,3,,2
wo<'WM_SIZING'>,0,3,,2
wo<'WM_SPOOLERSTATUS'>,0,3,,2
wo<'WM_STYLECHANGED'>,0,3,,2
wo<'WM_STYLECHANGING'>,0,3,,2
wo<'WM_SYNCPAINT'>,0,3,,2
wo<'WM_SYSCHAR'>,0,3,,2
wo<'WM_SYSCOLORCHANGE'>,0,3,,2
wo<'WM_SYSCOMMAND'>,0,3,,2
wo<'WM_SYSDEADCHAR'>,0,3,,2
wo<'WM_SYSKEYDOWN'>,0,3,,2
wo<'WM_SYSKEYUP'>,0,3,,2
wo<'WM_TCARD'>,0,3,,2
wo<'WM_TIMECHANGE'>,0,3,,2
wo<'WM_TIMER'>,f1.2745-f1,3,,2
wo<'WM_UNDO'>,f1.2746-f1,3,,2
wo<'WM_UNINITMENUPOPUP'>,0,3,,2
wo<'WM_USERCHANGED'>,0,3,,2
wo<'WM_VKEYTOITEM'>,0,3,,2
wo<'WM_VSCROLL'>,0,3,,2
wo<'WM_VSCROLLCLIPBOARD'>,0,3,,2
wo<'WM_WINDOWPOSCHANGED'>,0,3,,2
wo<'WM_WINDOWPOSCHANGING'>,0,3,,2
wo<'WM_WININICHANGE'>,0,3,,2
wo<'WS_BORDER'>,f1.2755-f1,3,,2
wo<'WS_CAPTION'>,f1.2756-f1,3,,2
wo<'WS_CHILD'>,f1.2757-f1,3,,2
wo<'WS_CHILDWINDOW'>,f1.2758-f1,3,,2
wo<'WS_CLIPCHILDREN'>,f1.2759-f1,3,,2
wo<'WS_CLIPSIBLINGS'>,f1.2760-f1,3,,2
wo<'WS_DISABLED'>,f1.2761-f1,3,,2
wo<'WS_DLGFRAME'>,f1.2762-f1,3,,2
wo<'WS_EX_ACCEPTFILES'>,0,3,,2
wo<'WS_EX_APPWINDOW'>,0,3,,2
wo<'WS_EX_CLIENTEDGE'>,0,3,,2
wo<'WS_EX_CONTEXTHELP'>,0,3,,2
wo<'WS_EX_CONTROLPARENT'>,0,3,,2
wo<'WS_EX_DLGMODALFRAME'>,0,3,,2
wo<'WS_EX_LEFT'>,0,3,,2
wo<'WS_EX_LEFTSCROLLBAR'>,0,3,,2
wo<'WS_EX_LTRREADING'>,0,3,,2
wo<'WS_EX_MDICHILD'>,0,3,,2
wo<'WS_EX_NOPARENTNOTIFY'>,0,3,,2
wo<'WS_EX_OVERLAPPEDWINDOW'>,0,3,,2
wo<'WS_EX_PALETTEWINDOW'>,0,3,,2
wo<'WS_EX_RIGHT'>,0,3,,2
wo<'WS_EX_RIGHTSCROLLBAR'>,0,3,,2
wo<'WS_EX_RTLREADING'>,0,3,,2
wo<'WS_EX_STATICEDGE'>,0,3,,2
wo<'WS_EX_TOOLWINDOW'>,0,3,,2
wo<'WS_EX_TOPMOST'>,0,3,,2
wo<'WS_EX_TRANSPARENT'>,0,3,,2
wo<'WS_EX_WINDOWEDGE'>,0,3,,2
wo<'WS_GROUP'>,f1.2784-f1,3,,2
wo<'WS_HSCROLL'>,f1.2785-f1,3,,2
wo<'WS_ICONIC'>,f1.2786-f1,3,,2
wo<'WS_MAXIMIZE'>,f1.2787-f1,3,,2
wo<'WS_MAXIMIZEBOX'>,f1.2788-f1,3,,2
wo<'WS_MINIMIZE'>,f1.2789-f1,3,,2
wo<'WS_MINIMIZEBOX'>,f1.2790-f1,3,,2
wo<'WS_OVERLAPPED'>,f1.2791-f1,3,,2
wo<'WS_OVERLAPPEDWINDOW'>,f1.2792-f1,3,,2
wo<'WS_POPUP'>,f1.2793-f1,3,,2
wo<'WS_POPUPWINDOW'>,f1.2794-f1,3,,2
wo<'WS_SIZEBOX'>,f1.2795-f1,3,,2
wo<'WS_SYSMENU'>,f1.2796-f1,3,,2
wo<'WS_TABSTOP'>,0,3,,2
wo<'WS_THICKFRAME'>,f1.2798-f1,3,,2
wo<'WS_TILED'>,0,3,,2
wo<'WS_TILEDWINDOW'>,f1.2800-f1,3,,2
wo<'WS_VISIBLE'>,f1.2801-f1,3,,2
wo<'WS_VSCROLL'>,f1.2802-f1,3,,2
wo<'WVR_ALIGNBOTTOM'>,0,3,,2
wo<'WVR_ALIGNLEFT'>,0,3,,2
wo<'WVR_ALIGNRIGHT'>,0,3,,2
wo<'WVR_ALIGNTOP'>,0,3,,2
wo<'WVR_HREDRAW'>,0,3,,2
wo<'WVR_REDRAW'>,0,3,,2
wo<'WVR_VREDRAW'>,0,3,,2
wo<'WidenPath'>,f1.2810-f1,3,,6
wo<'Width'>,f1.2811-f1,3,,6
wo<'WinHelp'>,0,3,,6
wo<'WindowFromPoint'>,0,3,,6
wo<'WindowProc'>,0,3,,6
wo<'['>,0,0,,1
wo<']'>,0,0,,1
wo<'^'>,f1.2817-f1,0,,1
wo<'asm'>,0,3,,1
wo<'bool'>,f1.2819-f1,3,,1
wo<'break'>,f1.2820-f1,3,,1
wo<'case'>,0,3,,1
wo<'catch'>,0,3,,1
wo<'char'>,f1.2823-f1,3,,1
wo<'class'>,f1.2824-f1,3,,1
wo<'const'>,f1.2825-f1,3,,1
wo<'const_cast'>,0,3,,1
wo<'continue'>,f1.2827-f1,3,,1
wo<'default'>,0,3,,1
wo<'delete'>,f1.2829-f1,3,,1
wo<'do'>,f1.2830-f1,3,,1
wo<'double'>,f1.2831-f1,3,,1
wo<'else'>,0,3,,1
wo<'enum'>,0,3,,1
wo<'extern'>,0,3,,1
wo<'false'>,0,3,,1
wo<'far'>,0,3,,1
wo<'float'>,f1.2837-f1,3,,1
wo<'for'>,f1.2838-f1,3,,1
wo<'friend'>,0,3,,1
wo<'if'>,f1.2840-f1,3,,1
wo<'inline'>,0,3,,1
wo<'int'>,f1.2842-f1,3,,1
wo<'long'>,f1.2843-f1,3,,1
wo<'new'>,f1.2844-f1,3,,1
wo<'operator'>,0,3,,1
wo<'private'>,f1.2846-f1,3,,1
wo<'protected'>,f1.2847-f1,3,,1
wo<'public'>,f1.2848-f1,3,,1
wo<'register'>,0,3,,1
wo<'return'>,f1.2850-f1,3,,1
wo<'short'>,f1.2851-f1,3,,1
wo<'signed'>,f1.2852-f1,3,,1
wo<'sizeof'>,0,3,,1
wo<'static'>,0,3,,1
wo<'struct'>,f1.2855-f1,3,,1
wo<'switch'>,0,3,,1
wo<'this'>,0,3,,1
wo<'throw'>,0,3,,1
wo<'true'>,0,3,,1
wo<'try'>,0,3,,1
wo<'typedef'>,f1.2861-f1,3,,1
wo<'union'>,0,3,,1
wo<'unsigned'>,f1.2863-f1,3,,1
wo<'virtual'>,f1.2864-f1,3,,1
wo<'void'>,0,3,,1
wo<'wchar_t'>,0,3,,1
wo<'while'>,f1.2867-f1,3,,1
wo<'{'>,0,0,,1
wo<'|'>,f1.2869-f1,0,,1
wo<'|='>,0,0,,1
wo<'||'>,f1.2871-f1,0,,1
wo<'}'>,0,0,,1
wo<'~'>,f1.2873-f1,0,,1
f1: db 0
.69:db '�����᪮� ',39,'���栭��',39,'',0
.70:db '�� ࠢ��',0
.71:db '��ப���� ����⠭�',0
.79:db '��४⨢� ����祭�� 䠩���',0
.82:db '����⮢�� ',39,'�',39,'',0
.83:db '�����᪮� ',39,'�',39,'',0
.95:db '���室 �� 㪠��⥫�',0
.98:db '����� ����� ������਩',0
.99:db '���� ����� ������਩',0
.113:db '�����',0
.114:db '����� ��� ࠢ��',0
.123:db 'ᮤ�ন� ����� ',39,'�ਫ������',39,' TApplication',0
.126:db 'ᮤ�ন� ����� ',39,'������',39,' TButton',0
.127:db 'ᮤ�ন� ����� ',39,'������ ��� ������ �����㬥�⮢',39,' TButtonGadget',0
.129:db 'ᮤ�ন� ����� ',39,'䫠���',39,' TCheckBox',0
.131:db 'ᮤ�ন� ����� ������� TChooseColorDialog (��� �롮� 梥�)',0
.143:db 'ᮤ�ন� ����� ',39,'������ �ࠢ�����',39,' TControlBar',0
.145:db 'ᮤ�ন� ������: TClientDC, TCreatedDC, TDC, TDeskTopDC, TDibDC, TEnhMetaFilePict, TIC, TMemoryDC, TMetaFileDC, TPaintDC, TPrintDC, TScreenDC, TWindowDC',0
.146:db 'ᮤ�ন� ����� ',39,'�����஢����� ����',39,' TDecoratedFrame',0
.166:db 'ᮤ�ন� ����� ',39,'���� � ࠬ���',39,' TFrameWindow',0
.171:db 'ᮤ�ন� ����� TGdiObject, ������ ���: TBitmap, TBrush, TPen, TRegion, TIcon, TCursor, TDib, ...',0
.172:db 'ᮤ�ন� ����� ',39,'������ � ��㭪�� � ⥪�⮬',39,' TGlyphButton',0
.180:db 'ᮤ�ন� ����� ',39,'ᯨ᪠',39,' TListBox',0
.202:db 'ᮤ�ন� ���� �������� TFileOpenDialog � TFileSaveDialog (��� ������ � ������� 䠩���)',0
.263:db '��ᢠ������',0
.264:db 'ࠢ��',0
.265:db '�����',0
.266:db '����� ��� ࠢ��',0
.272:db '���ய��樮����� ��⥬�� ���� ANSI',0
.273:db '�ய��樮����� ��⥬�� ���� ANSI',0
.278:db '����� � ������ DC ������� ᥣ���� � ���, �ᯮ���� ��� �⮣� ⥪�騩 ��࠭�� ��ꥪ� ���. ����� ������ �� ⥪�饩 ����樨 �� ��砫� �㣨.',0
.279:db '����� � ������ DC �������� ���, �ᯮ���� ��� �⮣� ⥪�騩 ��࠭�� ��ꥪ� ���.',0
.281:db '������祭� ����',0
.284:db '�� ��஭� ��אַ㣮�쭨��',0
.285:db '������ ��஭� ��אַ㣮�쭨��',0
.286:db '�������� ������� ����訢����� ᯫ��� ��� 梥⮬',0
.287:db '��ୠ� �����',0
.288:db '���� ��࠭���',0
.290:db '��।����, ���� �� ᥫ��⨢��� ������ ��� ���� �஢�ન ����祭��',0
.292:db '��।���� ���ﭨ� �࣠�� �ࠢ����� ������ �� ����⨨ ������ ��� ��� ������ �஡���',0
.293:db '����砥� ��� 㤠��� �⬥�� �� ᥫ��⨢��� ������ ��� ����� �஢�ન',0
.295:db '������� ���ﭨ� ������ ��� ����� �஢�ન',0
.296:db '������� �⨫� ������',0
.345:db '����譠� �����',0
.350:db '���뢠�� ��� ������� DC ���� ������� � ����뢠�� �।��騩 �������. ��᫥ ������ ������� �ਫ������ ����� ��뢠�� �㭪樨 �ᮢ���� � ������ ���⥪��.',0
.351:db '���⠪��: bool BitBlt(int, int, int, int, const TDC&, int, int, uint32 rop); bool BitBlt(const TRect&, const TDC&, const TPoint&, uint32 rop);',0
.354:db '���᮪ �뢮��� �� �࠭ ������ ���⨪��쭮� �ப��⪨ ���� �᫨ ��� �����筮�� ������⢠ ����⮢ ��� �ப��⪨ (CBS - Combo Box Style)',0
.359:db '������ �������஢������ ᯨ᪠ �筮 ᮮ⢥����� ��������� �� ��� ᮧ�����; ���筮 Windows ������� ࠧ��� �������஢������ ᯨ᪠ ⠪, �⮡� � ��� ���� �뫨 ����� ������ ᯨ᪠ 楫���� (CBS - Combo Box Style)',0
.366:db '�������� ��ப� � ����� ᯨ᪠ (CB - Combo Box)',0
.367:db '������ ��ப� �� ����� ᯨ᪠ (CB - Combo Box)',0
.370:db '�����頥� �᫮ ����⮢ � ����� ᯨ᪠ (CB - Combo Box)',0
.371:db '�����頥� ������ ⥪�饣� ��࠭���� ����� � ����� ᯨ᪠ (CB - Combo Box)',0
.398:db '�� ��, �� � CF_SCREENFONTS + CF_PRINTERFONTS',0
.410:db '��।����, �� �㭪�� ChooseFont ������ �롨��� ⮫쪮 �����ਭ�� �����',0
.421:db '�� ��, �� � CF_NOVECTORFONTS',0
.447:db '����. 梥�: �࠭�� ��⨢���� ����',0
.448:db '����. 梥�: ��������� ��⨢���� ���� (� ����� ��஭� � WinXP)',0
.450:db '����. 梥�: ����稩 �⮫',0
.455:db '����. 梥�: ��������� ��⨢���� ���� (⥪��)',0
.457:db '����. 梥�: �뤥����� �㭪� ����',0
.458:db '����. 梥�: �뤥����� �㭪� ���� (⥪��)',0
.459:db '����. 梥�: �࠭�� �� ��⨢���� ����',0
.464:db '����. 梥�: ���� (䮭)',0
.466:db '����. 梥�: ���� (⥪��)',0
.474:db '����ᮢ뢠�� ���� �� ��������� ��ਧ��⠫��� ࠧ��஢ (CS - Class Style)',0
.477:db '���� �ᯮ���� ���⥪�� ��ᯫ�� த�⥫�᪮�� ���� (CS - Class Style)',0
.478:db '�᫨ ᮤ�ন��� ���� � ����� ������ �� �⮡ࠦ�����, ��� ��࠭���� � ���� ���. �� ���� ��� �ᯮ������ ��� ����୮�� �⮡ࠦ���� ᮤ�ন����. (CS - Class Style)',0
.479:db '����ᮢ뢠�� ���� �� ��������� ���⨪����� ࠧ��஢ (CS - Class Style)',0
.480:db '������ ',0
.481:db '������ ',0
.482:db '������� ।���஢���� ',0
.483:db '���᮪ ',0
.484:db '���� ᮮ�饭�� ',0
.485:db '��஫��� ',0
.486:db '����᪨� ⥪�� ',0
.494:db '����� ����������� ��� (�������, ��࠭�祭��� ����祭��� ���� � ������� ᥣ���⮬).',0
.506:db '������� ⠡���� �⪫����',0
.508:db '�᭮���� 梥⮢�� ������, ᮤ�ঠ�� 20 梥⮢',0
.511:db '��砫� ⠡���� �⪫����',0
.512:db '����, ����ᨬ� �� ���ன�⢠',0
.513:db '�����-��� �����',0
.532:db 'CD-ROM ��� (�. GetDriveType)',0
.533:db '����஢���� ��� (�. GetDriveType)',0
.534:db '�� �ࠢ���� ���� (�. GetDriveType)',0
.535:db 'RAM ��� (�. GetDriveType)',0
.536:db '������ ��� network ��� (�. GetDriveType)',0
.537:db '���� ��� (�. GetDriveType)',0
.538:db '�� ������� ⨯ (�. GetDriveType)',0
.539:db '������஢���� ⥪�饣� ����������',0
.561:db '��⠭�������� ��ࠢ������� �� ������ �࠭�� �������� ��אַ㣮�쭮� ������; ������ �������஢����� � DT_SINGLELINE',0
.562:db '��㦨� ��� ��।������ �ਭ� � ����� ��אַ㣮�쭮� ������ �뢮��; ��� ⥪�� � ��᪮�쪮 ��ப �ᯮ������ �ਭ� ��אַ㣮�쭮� ������, �������� ��ࠬ��஬ IpRect, � ����ࠨ������ ���� ⠪, �⮡� �������� ���� ⥪��;',0
.563:db '��⠭�������� ��ࠢ������� �� 業��� (�� ��ਧ��⠫�) �������� ������',0
.566:db '�ந������ �� �뢮�� ������ ᨬ����� ⠡��樨 �� �஡���; ������ ⠡���� ����� ࠧ��� � ��ᥬ� ᨬ�����',0
.567:db '�� ��।������ ����� ��ப� ���뢠���� ࠧ��� ����� ��ப���; �� ����� �ᯮ�짮������ ᮢ���⭮ � DT_TABSTOP',0
.569:db '��⠭�������� ��ࠢ������� �� ������ ��� �������� ��אַ㣮�쭮� ������',0
.571:db '�����⢫�� �뢮� ��� ��� �ਭ� ���������� ��אַ㣮�쭮� ������; �� ����� �ᯮ�짮������ ᮢ���⭮ � DT_TABSTOP',0
.572:db '�� �뢮�� �����ᠭ� (&) �� ����������� ��� 㪠����� ����ન����� ᫥���饣� �� ��� ᨬ����, � ������� �����ᠭ� (&&) - ��� 㪠����� �� �뢮� ⮫쪮 ������; �� ����� �ᯮ�짮������ ᮢ���⭮ � DT_TABSTOP',0
.574:db '��⠭�������� ��ࠢ������� �� �ࠢ��� ��� �������� ��אַ㣮�쭮� ������',0
.576:db '����뢠�� �� �뢮� ����� ��ப� ⥪��, �� �⮬ ᨬ���� ��ॢ��� ��ப� � ������ ���⪨ �� ࠧ������ ��ப�',0
.577:db '��⠭�������� ࠧ��� ⠡��樨, � ���襬 ���� ��ࠬ��� nFormat ᮤ�ন��� ������⢮ ᨬ����� �� ⠡���� (�� 㬮�砭�� ࠧ��� ⠡��樨 8 ᨬ�����)',0
.578:db '��⠭�������� ��ࠢ������� �� ���孥� �࠭�� �������� ��אַ㣮�쭮� ������; ������ �������஢����� � DT_SINGLELINE',0
.579:db '��⠭�������� ��ࠢ������� �� 業��� (�� ���⨪���) �������� ������; ������ �������஢����� � DT_SINGLELINE',0
.580:db '��⠭�������� ०�� ��⮬���᪮�� ��७�� ��ப� ����� ᫮����, �᫨ ᫥���饥 ᫮�� ��室�� �� ࠬ�� �������� ��אַ㣮�쭮� ������; ᨬ���� ��७�� ��ப� ������������ ��� ���筮',0
.591:db '����� � ������ DC 㪠����� ��אַ㣮�쭨�, �������� �⨫�� ��� ��⨢�����.',0
.592:db '����� � ������ DC 㪠������ ���⮣ࠬ�� � ������묨 ���न��⠬�.',0
.594:db '��ଠ���� � ���� � �������� ��אַ㣮�쭨�� 㪠������ �᫮ ᨬ�����.',0
.639:db '����� ⠡���� �⪫����',0
.640:db '���짮��⥫� ��६��⨫ � ���� �࣠�� �ࠢ����� Rich Edit ���⮣ࠬ�� 䠩�� �� ����� ����樨 "drag and drop"',0
.641:db '���짮��⥫� ����⠫�� ��।���஢��� ���饭�� ⥪�� � Rich Edit',0
.642:db '���������� ࠧ���� ���� �࣠�� �ࠢ����� Rich Edit',0
.643:db '�⨫� ��⮬���᪨ �ப��稢��� ⥪�� ��ࠢ� �� 10 ᨬ����� �� ����� ᨬ���� � ���� ��ப�. �� ����⨨ Enter ⥪�� �ப��稢����� ����� �� �㫥��� ����樨 (ES - Edit Style)',0
.644:db '�⨫� �࣠�� �ࠢ����� ।���஢����� ��⮬���᪨ �ப��稢��� ⥪�� �� ���� ��࠭��� �����, ����� �� ��⠢�� � ���� ��ப� ���������� Enter (ES - Edit Style)',0
.645:db '�⨫� 業����� ⥪��. �ᯮ������� ⮫쪮 ����� � �⨫�� ES_MULTILINE (ES - Edit Style)',0
.671:db '����� � �������� � ⥪�饬 DC ����, �ᯮ���� ��࠭��� ��� � �����.',0
.676:db '���몠�� ������� � �롨ࠥ� ������� � ������ DC.',0
.678:db '������� �����, ����㯭� ��� ������� DC, � �������� ᥬ���⢥ ���⮢.',0
.679:db '������� ����㯭� � ������ DC �����.',0
.680:db '������� �맮�� GDI � �������� ���䠩��. ���� �� ���� ��ࠡ�⠭� �� �맮��, ����� ⠪�� �맮� ��।����� �㭪樨 ���⭮�� �맮�� � ������묨 ����묨.',0
.805:db '�।���頥� �ᮢ���� � �������⨬�� �������� ����, �᪫��� ����������� ������� ���� ������� DC �� ��१����� ������.',0
.807:db '�������� ������� � ������ DC, ��稭�� � �������� �窨 � �ᯮ���� ��࠭�� ��ꥪ� ����.',0
.808:db '����� � �������� DC ᨬ���� (�� 㪠������� �᫠) �������� ��ப� � �������騬 �㫥�.',0
.810:db '�����⨢�� ���� (FF - Font Family)',0
.811:db '������⢮ ���� �� ����� ���祭�� (FF - Font Family)',0
.812:db '���ய��樮����� ���� (FF - Font Family)',0
.813:db '�ய��樮����� ���� � ���窠�� (FF - Font Family)',0
.814:db '����, ��室�騩 �� �㪮���� ��� (FF - Font Family)',0
.815:db '�ய��樮����� ���� ��� ���祪 (FF - Font Family)',0
.863:db '������� ����訢���� ��।������ �࠭�楩',0
.864:db '������� ����訢���� ��।������ 梥⮬',0
.865:db '��ਭ� ���� 900 (FW - Font Weight)',0
.866:db '��ਭ� ���� 700 (FW - Font Weight)',0
.867:db '��ਭ� ���� 700 (FW - Font Weight)',0
.868:db '��ਭ� ���� 0 (FW - Font Weight)',0
.869:db '��ਭ� ���� 800 (FW - Font Weight)',0
.870:db '��ਭ� ���� 200 (FW - Font Weight)',0
.871:db '��ਭ� ���� 900 (FW - Font Weight)',0
.872:db '��ਭ� ���� 300 (FW - Font Weight)',0
.873:db '��ਭ� ���� 500 (FW - Font Weight)',0
.874:db '��ਭ� ���� 400 (FW - Font Weight)',0
.875:db '��ਭ� ���� 400 (FW - Font Weight)',0
.876:db '��ਭ� ���� 600 (FW - Font Weight)',0
.877:db '��ਭ� ���� 100 (FW - Font Weight)',0
.878:db '��ਭ� ���� 800 (FW - Font Weight)',0
.879:db '��ਭ� ���� 200 (FW - Font Weight)',0
.880:db '���몠�� ������ 䨣��� � ⥪�饬 ������� ������� DC � �������� ����७��� ������� �������, �ᯮ���� ⥪���� ����� � ०�� ����᪨ �����㣮�쭨��.',0
.881:db '�������� ������� ��אַ㣮�쭨� � 㪠������ DC, �ᯮ���� 㪠������ �����.',0
.882:db '�������� �������� ������� ������� DC, �ᯮ���� 㪠������ �����.',0
.885:db '�८�ࠧ�� ��� �ਢ� � ⥪�饬 ��࠭��� ������� ������� DC. �� ⠪�� �ਢ� ���������� �� ��᫥����⥫쭮�� �������� ᥣ���⮢.',0
.886:db '����訢��� ������� � ������ DC, ��稭�� � �������� �窨 � �ᯮ���� ⥪�騩 ��࠭�� ��ꥪ� ����. ��㬥�� 梥� ������ 梥� �࠭��� ��� ������.',0
.889:db '����� ࠬ�� � ������ DC ����� ������� ��אַ㣮�쭨��, �ᯮ���� �������� �����.',0
.909:db '������ ᯫ�譮� ��ꥪ� (�. gluQuadricDrawStyle)',0
.916:db '������ �஢����� ��ꥪ� (�. gluQuadricDrawStyle)',0
.964:db '������� ⮫쪮 �窨 (�. gluQuadricDrawStyle)',0
.1013:db '���ﭭ� ᢥ�',0
.1014:db '��� ���ﭭ�� ᢥ�',0
.1019:db '⮦� ���ﭭ� ᢥ�',0
.1020:db '����砥�� ᢥ�',0
.1035:db '�⥯��� ��ࠦ������ ᢥ�',0
.1037:db '��ࠦ���� ᢥ�',0
.1041:db '���� ����� ����� ࠧ�������� ������� �ਪ����묨 ����砬�, �᫨ �ᯮ������ ��⮪�� DDE. ����� �ਪ������ �����, ��।������ ���� �����, �����蠥���, ���� 㭨�⮦�����',0
.1042:db '���� ����� ����� ���� 㭨�⮦��. ������ �ᯮ�짮������ � GMEM_MOVEABLE',0
.1043:db '���� ����� �� 㭨�⮦�� (⮫쪮 ��� �㭪樨 GlobalFlags)',0
.1044:db '���� ����� ��䨪�஢�� � ����� ���� �����',0
.1046:db '�� �������஢���� � ����訬 ���⮬ ���祭��, �����頥���� �㭪樥� GlobalFlags, �����頥� ���稪 ��뫮� ����� ����� (⮫쪮 ��� �㭪樨 GlobalFlags)',0
.1047:db '�� ��, �� � GMEM_NOT_BANKED',0
.1048:db '����� ����砥��� ��� 䫠�, 䫠�� ������쭮� ����� ���� �������� (⮫쪮 GlobalReAlloc)',0
.1049:db '���� ����� ��䨪�஢�� � ����� ���� �����',0
.1050:db '�� ��।������ ����� ��� ���� ����� ������� ��㣨� ����� ����� �� ���� ᦨ������ ��� 㭨�⮦�����',0
.1051:db '�� ��।������ ����� ��� ���� ����� ������� ��㣨� ����� ����� �� ���� 㭨�⮦�����',0
.1052:db '�᫨ ������ ��ࠥ���, � �㤥� �맢��� �ணࠬ�� 㢥��������',0
.1053:db '���� ����� �뤥����� � ����㯯���� �����',0
.1054:db '�� ��, �� � GMEM_DDESHARE',0
.1056:db '���樠������� ᮤ�ন��� ����� ����� � ���',0
.1057:db '���� �����',0
.1071:db '����砥� ���祭�� ⥪�饣� 䨫��� �����樥�� �⭮�⥫쭮�� 㤫������ ��� ������� DC.',0
.1072:db '�����頥� ��ਡ��� ��ꥪ� DC.',0
.1073:db '�����頥� ��� ������� DC ⥪�騩 䮭��� 梥�.',0
.1074:db '�����頥� ��� ������� DC ०�� 䮭�.',0
.1075:db '� ����ᨬ��� �� ��㬥�� ᮮ�頥� �� ��࠭����饬 ��אַ㣮�쭨�� ��� ������� DC ��� ����������� Windows.',0
.1076:db '����頥� � ⥪�騩 ���⥪�� ��室��� ��� ����.',0
.1080:db '����砥� ��� ⥪�饣� ���� TrueType ������� DC �ਭ� ��᫥����⥫��� ᨬ����� � �������� ���������.',0
.1081:db '����砥� ��� ������� DC �ਭ� (� �����᪨� �������) ��� �������� ���������� ��᫥����⥫쭮�� ᨬ����� � ⥪�饬 ����.',0
.1086:db '����頥� � ������� ��אַ㣮�쭨� ⥪���� ࠬ�� ���祭�� � ������ DC.',0
.1087:db '����砥� ⥪���� ࠬ�� ���祭�� ��� ������� DC � ����頥� ��� ����� � ������� ��㬥��.',0
.1090:db '�����頥� ����⥫� �� ⥪�騩 �뤥����� ��ꥪ�, �易��� � ⥪�騬 DC.',0
.1091:db '����頥� �����᪨� ���न���� ⥪�饩 ����樨 ������� DC.',0
.1093:db '����砥� �⮣���� �࠭���� ��砫� ������� ���⥪��. �� ���祭�� ������ ᬥ饭��, �ᯮ��㥬�� ��� �࠭��樨 ���न��� ���ன�⢠ � ������� ���न���� �窨 ���� �ਫ������.',0
.1096:db '�����頥� ���ଠ�� � ������������ ������� DC.',0
.1104:db '����砥� ��� ��������� ����⠡��㥬��� ���� TrueType ���ଠ�� � ����.',0
.1106:db '�����頥� ����⥫� ������� DC.',0
.1110:db '����砥� ��� ⥪�饣� ���� ������� DC ���� ��୨��� � ������� �� � ���ᨢ.',0
.1113:db '�����頥� ०�� �⮡ࠦ���� ������� ���� ⥪�饣� DC.',0
.1116:db '�����頥� ��� ������� ��㬥�� Color ������訩 梥� � ⥪�饩 ������.',0
.1120:db '���뢠�� �������� ���ଠ�� ��� ���⮢ TrueType ������� DC.',0
.1124:db '�����頥� 梥� ����� ����ࠦ���� � ������ �窥.',0
.1125:db '�����頥� ��� ������� DC ⥪�騩 ०�� �� ��᪨ �����㣮�쭨��.',0
.1131:db '�����頥� ��� ������� DC ⥪�騩 ०�� ��������.',0
.1136:db '���뢠�� ������� �������� ����ᥩ ������� �� ��⥬��� ������� � ���ᨢ �������.',0
.1137:db '��।����, ����� �� ����� DC ����� � ������ ��⥬��� ������.',0
.1138:db '������ ����� � �ਭ� (� �����᪨� �������) ⥪�⮢�� ��ப� �������� ����� � ��ப� � �������騬 �㫥�.',0
.1139:db '�����頥� ��� ������� DC ⥪�騥 䫠�� ��ࠢ������� ⥪��.',0
.1140:db '�����頥� ���祭�� ⥪�饣� ���ࢠ�� ����� ᨬ������ � �����᪨� ������� (��� ������� DC).',0
.1141:db '�����頥� ��� ������� DC ⥪�騩 梥�.',0
.1142:db '������ ����� � �ਭ� (� �����᪨� �������) �������� ⥪�⮢�� ��ப� � ��ப� � �������騬 �㫥�.',0
.1143:db '��� ������� DC ���뢠�� ��� ��୨���� ⥪�饣� ����.',0
.1150:db '��⠭�������� ����稭� ࠧ��஢ x � y (� ������� ���ன�⢠) ⥪�饩 ������ ��ᬮ��.',0
.1152:db '���뢠�� ⥪�騥 ࠧ���� x � y (� ������� ���ன�⢠) ���� � ⥪�饬 DC.',0
.1163:db '����� ��� 梥⮬ � �������� ��אַ㣮�쭨�� 㪠������ �᫮ ᨬ�����, �ᯮ���� �������� ����� � ⥪�騩 ���� ������� DC.',0
.1195:db '����� ��� �஧�筠� �����',0
.1226:db 'operator HWND()const;',0
.1228:db '�㭪�� ����� TRect, �����頥� ����� ��אַ㣮�쭨��',0
.1234:db '��࠭� ������ ��ࢠ�� [Abort]',0
.1235:db '��࠭� ������ �⬥�� [Cancel]',0
.1237:db '��࠭� ������ �த������ [Continue]',0
.1238:db '�����⥫� ��� ',39,'��५�� � �����쪨�� �ᠬ�',39,'',0
.1239:db '�����⥫� ��� ',39,'��५�',39,'',0
.1240:db '�����⥫� ��� ',39,'����',39,'',0
.1241:db '�����⥫� ��� ',39,'����ભ��� ���',39,'',0
.1242:db '�����⥫� ��� ',39,'����� ��६�饭��',39,' (4 ��५��)',0
.1243:db '�����⥫� ��� ',39,'���������� ��५��',39,' (ᥢ��-���⮪ - -�����)/',0
.1244:db '�����⥫� ��� ',39,'���⨪���� ��५��',39,' (ᥢ�� - �)',0
.1245:db '�����⥫� ��� ',39,'���������� ��५��',39,' (ᥢ��-����� - -���⮪)',0
.1246:db '�����⥫� ��� ',39,'��ਧ��⠫�� ��५��',39,' (����� - ���⮪)',0
.1248:db '�����⥫� ��� ',39,'����� ���',39,'',0
.1256:db '��࠭� ������ �ய����� [Ignore]',0
.1267:db '��࠭� ������ ��� [No]',0
.1268:db '��࠭� ������ [OK]',0
.1269:db '��࠭� ������ ����� [Retry]',0
.1270:db '��࠭� ������ ������� [Try Again]',0
.1271:db '��࠭� ������ �� [Yes]',0
.1273:db '����ࠦ���� ����� ��㡨�� 梥� 16 ��� (ILC - Image List Create)',0
.1274:db '����ࠦ���� ����� ��㡨�� 梥� 24 ��� (ILC - Image List Create)',0
.1275:db '����ࠦ���� ����� ��㡨�� 梥� 32 ��� (ILC - Image List Create)',0
.1276:db '����ࠦ���� ����� ��㡨�� 梥� 4 ��� (ILC - Image List Create)',0
.1277:db '����ࠦ���� ����� ��㡨�� 梥� 8 ��� (ILC - Image List Create)',0
.1290:db '�뢮� ����ࠦ���� � ��⮬ �஧�筮�� (ILD - Image List Draw)',0
.1292:db '�㭪�� ����� TApplication, ��뢠���� ⮫쪮 ��� ��ࢮ�� �।�⠢�⥫� �ணࠬ��.',0
.1293:db '�㭪�� ����� TApplication, ��뢠���� ��� ������� �।�⠢�⥫� �ணࠬ��.',0
.1294:db '�㭪�� ����� TApplication, ��뢠���� ��� ᮧ����� �������� ���� �ணࠬ��.',0
.1302:db '��������� ������� ��אַ㣮�쭨� � ������ DC.',0
.1303:db '��������� �������� ������� � ������ DC.',0
.1337:db '�������� ��ப� � ����� ᯨ᪠ (LB - List Box)',0
.1339:db '������ ��ப� �� ����� ᯨ᪠ (LB - List Box)',0
.1343:db '��室�� ���� ����� ����� ᯨ᪠, ᮮ⢥�����騩 ��䨪᭮� ��ப� (LB - List Box)',0
.1347:db '�����頥� �᫮ ����⮢ � ����� ᯨ᪠ (LB - List Box)',0
.1369:db '��⠭�������� �ਭ� �⮫�� ����� ᯨ᪠ (LB - List Box)',0
.1376:db '�롨ࠥ� ��� �⬥��� �롮� ����� � ����� ᯨ᪠ (LB - List Box)',0
.1379:db '���������� ������ �� ��������� �᫠ �祪 � ���ᨢ� �� �����᪨� �祪 � �窨 ���ன�⢠. �८�ࠧ������ ������ �� ०��� �⮡ࠦ���� ⥪�饣� DC.',0
.1393:db '���⫮-��� �����',0
.1405:db '����� ����㠫쭮�� ᯨ᪠ (LVS - List View Style)',0
.1417:db '����� ����� � ������ DC, �ᯮ���� ⥪�騩 ���쥢�� ��ꥪ�.',0
.1425:db '������ [��ࢠ��],[�����],[�ய�����] �� ᮮ�饭��',0
.1426:db '������ [�⬥��],[�����],[�த������] �� ᮮ�饭��',0
.1427:db '������ ᮮ�饭�� � ���ଠ樥� (i)',0
.1428:db '������ ᮮ�饭�� � �訡��� (x)',0
.1429:db '������ ᮮ�饭�� � �।�०������ (!)',0
.1430:db '������ ᮮ�饭�� � �訡��� (x)',0
.1431:db '������ ᮮ�饭�� � ���ଠ樥� (i)',0
.1432:db '������ ᮮ�饭�� � ����ᮬ (?)',0
.1433:db '������ ᮮ�饭�� � �訡��� (x)',0
.1434:db '������ ᮮ�饭�� � �।�०������ (!)',0
.1435:db '������ [OK] �� ᮮ�饭��',0
.1436:db '������ [OK],[�⬥��] �� ᮮ�饭��',0
.1437:db '������ [�����],[�⬥��] �� ᮮ�饭��',0
.1438:db '������ [��],[���] �� ᮮ�饭��',0
.1439:db '������ [��],[���],[�⬥��] �� ᮮ�饭��',0
.1440:db '�������஢���� ⥪�饣� ���������� � ⥪�饩 ������ �� ����� ������ AND',0
.1441:db '�������஢���� ������஢������ ��⮢��� ���ᨢ� � ⥪�騬 ����������� �� ����� ������ OR',0
.1466:db '���� � ��⮢� ���ᨢ��',0
.1470:db '���� � ����窮�',0
.1473:db '���� �������஢��� �� �⮡ࠦ����� ��� ���筮',0
.1483:db '���� ������ �����, ᮧ���訬 ���',0
.1484:db '�ᯫ뢠�饥 ����',0
.1487:db '�������⥫쭠� ����� � ����',0
.1498:db '㪠��⥫� �� ������� ���� (� ����� TApplication)',0
.1504:db '�ᯮ���� ������� ��㬥���, ������� ⥪�饥 ������쭮� �८�ࠧ������ ��� ������� DC.',0
.1522:db '����஢���� ������஢������ ��⮢��� ���ᨢ�',0
.1523:db '������஢���� १���� �������஢���� ⥪�饣� ���������� � ��⮢� ���ᨢ�� �� ����� ������ OR',0
.1525:db '�� ��, �� � HOLLOW_BRUSH',0
.1526:db '���⮩ ��� �஧��� ��࠭���',0
.1580:db '���ய��樮����� ���� OEM',0
.1588:db '���� ⮫쪮 ��������� ���� 䠩��� � ���� "��� 䠩��" �������. �ᯮ������ ����� � OFN_PATHMUSTEXIST.',0
.1589:db '���뢠�� 䫠��� ������� "���쪮 ��� �⥭��".',0
.1624:db '��६�頥� ������� ���祭�� � ������ DC �� �������� ᬥ饭��.',0
.1625:db '��������� ��砫� ������ ��ᬮ�� � ������ DC �� ������� ����稭� x � y.',0
.1626:db '������� ��砫� ���� ������� DC �� ������� ����稭�.',0
.1629:db '���������� ������ �� ����� ⥪�饩 ����',0
.1630:db '�������஢���� ⥪�饣� ���������� � ������ �� ����� ������ XOR',0
.1631:db '�������஢���� ������஢������ ��⮢��� ���ᨢ� � ⥪�饩 ������ �� ����� ������ OR � �������஢���� १���� �⮩ ����樨 � ⥪�騬 ����������� �� ����� ������ OR',0
.1688:db '����訢��� �������� ������� ⥪�饣� DC, �ᯮ���� ⥪���� �����.',0
.1689:db '����訢��� ������� ��אַ㣮�쭨�, �ᯮ���� ⥪���� ����� ������� DC.',0
.1692:db '�ᯮ���� ��࠭�� ��ꥪ�� ��� � ����, ���� � ����訢��� ᥪ��.',0
.1695:db '�믮���� � ⥪�饬 DC ᮤ�ন��� ��������� ���䠩��. ���䠩� ����� �믮����� �� �᫮ ࠧ.',0
.1696:db '�믮���� � ������ DC ������ ���䠩��.',0
.1697:db '�믮���� ����⮢�� ��।��� ����� �� 㪠������� ��室���� DC � ����� DC.',0
.1699:db '����� ���� ��� ����� �易���� �㡨�᪨� ᯫ����� ���� �� �窠�, ������� � ���ᨢ� �祪, �ᯮ���� ⥪�騩 ��࠭�� ��ꥪ� ���.',0
.1700:db '����� � ������ DC ���� ��� ����� ����஢ �������� ᥣ���⮢ (�� ��易⥫쭮 ᬥ����), �ᯮ���� ⥪�騩 ��ꥪ� ���.',0
.1701:db '� ������ DC ���� � ����訢��� ��᫥����⥫쭮��� �����㣮�쭨��� (�������� ��४�뢠�����), �ᯮ���� ⥪�騩 ��ꥪ� ��� � ०�� ����᪨.',0
.1702:db '����� � ������ DC ��᫥����⥫쭮��� ������� �����, �ᯮ���� ⥪�饥 ���.',0
.1703:db '����� � ����訢��� ������� ������묨 ᥣ���⠬� �����㣮�쭨�.',0
.1709:db '�����頥� true, �᫨ �������� �窠 ��室���� � ������ ���祭�� ������� DC.',0
.1710:db '���������騩 梥� �ᥣ�� ���',0
.1711:db '���������騩 梥� �ᥣ�� ���� 梥⮬ ���',0
.1712:db '���������騩 梥� ���� �������樥� 梥⮢, ������ ��� �������饣� �⮡ࠦ���� � ������ ��� ⥪�饣� ���',0
.1713:db '���������騩 梥� ���� �������樥� 梥⮢, ������ ��� �������饣� �⮡ࠦ���� � ��� ⥪�饣� ���',0
.1714:db '���������騩 梥� ���� ����� 梥��, ������饬��� �� R2_MASKPEN',0
.1715:db '���������騩 梥� ���� �������樥� ���⭮�� 梥� ⥪�饣� ��� � 梥� �������饣� ����ࠦ����',0
.1716:db '���������騩 梥� ���� �������樥� 梥� ⥪�饣� ��� � 梥� �������饣� ����ࠦ����',0
.1717:db '���������騩 梥� ���� �������樥� 梥� ⥪�饣� ��� � ���⭮�� 梥� �������饣� ����ࠦ����',0
.1718:db '�������饥 ����ࠦ���� �� ���������',0
.1719:db '���������騩 梥� ���� ����� 梥�� �������饣� ����ࠦ����',0
.1720:db '���������騩 梥� ���� ����� 梥�� ⥪�饣� ���',0
.1721:db '���������騩 梥� ���� ����� 梥��, ������饬��� �� R2_MASKPEN',0
.1722:db '���������騩 梥� ���� ����� 梥��, ������饬��� �� R2_MERGEPEN',0
.1723:db '���������騩 梥� ���� ����� 梥��, ������饬��� �� R2_XORPEN',0
.1724:db '���������騩 梥� ���� ����',0
.1725:db '���������騩 梥� ���� �������樥� 梥⮢ � �������饬 ����ࠦ���� � ⥪�饬 ���, �� �� 梥⮬ ����� ��������⮢',0
.1742:db '����⠭�������� � ������ DC ��ࢮ��砫��� ������� GDI ��ꥪ�.',0
.1759:db '����⠭�������� � ������ DC ��ࢮ��砫쭮� ��� GDI.',0
.1760:db '����⠭�������� � ������ DC ��ࢮ��砫�� ��ꥪ� ⥪�⮢�� ���� GDI.',0
.1761:db '����� � ����訢��� � ������ DC ����㣫���� ��אַ㣮�쭨� ������� ࠧ���.',0
.1765:db '�⨫� ������ �ப��⪨ ����� �⠭������ ����� � ������ �ࠩ, ��஢����� � ������ �࠭�楩 ��אַ㣮�쭨��, �ᯮ��㥬��� ��� �� ᮧ�����. �ᯮ������� ����� � SBS_HORZ. (SBS - Scroll Bar Style)',0
.1766:db '�⨫� ������ �ப��⪨ ���� ��ਧ��⠫��. �᫨ �� �ᯮ������ �� �⨫� SBS_BOTTOMALIGN, �� �⨫� SBS_TOPALIGN, ����� �ப��⪨ �㤥� ����� ��� ࠧ���, ����� �� ����襭 �� �� ᮧ�����. (SBS - Scroll Bar Style)',0
.1767:db '�⨫� ������ �ப��⪨ ����� �⠭������ �ਭ� � ���� �ࠩ, ��஢����� � ����� �࠭�楩 ��אַ㣮�쭨��, �ᯮ��㥬��� ��� �� ᮧ�����. �ᯮ������� ����� � SBS_VERT. (SBS - Scroll Bar Style)',0
.1768:db '�⨫� ������ �ப��⪨ ����� �⠭������ �ਭ� � �ࠢ� �ࠩ, ��஢����� � �ࠢ�� �࠭�楩 ��אַ㣮�쭨��, �ᯮ��㥬��� ��� �� ᮧ�����. �ᯮ������� ����� � SBS_VERT. (SBS - Scroll Bar Style)',0
.1769:db '�⨫� ������ �ப��⪨ ���� ������ ࠧ���. �᫨ �� �ᯮ������ SBS_SIZEBOXBOTTOMRIGHTALIGN, �� SBS_SIZEBOXTOPLEFTALIGN, � ����� �ப��⪨ �㤥� ����� ��� ࠧ���, ����� �� ����襭 �� �� ᮧ�����. (SBS - Scroll Bar Style)',0
.1770:db '�⨫� ������ �ப��⪨ ���� �⠭����� ࠧ��஬ ��� ��⥬��� ������ ࠧ��� � ����� �ࠢ� ������ 㣮�, ��ࠢ����� � �ࠢ� ������ 㣫�� ��אַ㣮�쭨��, �ᯮ��㥬��� ��� �� ᮧ�����. �ᯮ������� ����� � SBS_SIZEBOX. (SBS - Scroll Bar Style)',0
.1771:db '�⨫� ������ �ப��⪨ ���� �⠭����� ࠧ��஬ ��� ��⥬��� ������ ࠧ��� � ����� ���孨� ���� 㣮�, ��ࠢ����� � ���� ���孨� 㣫�� ��אַ㣮�쭨��, �ᯮ��㥬��� ��� �� ᮧ�����. �ᯮ������� ����� � SBS_SIZEBOX. (SBS - Scroll Bar Style)',0
.1773:db '�⨫� ������ �ப��⪨ ����� �⠭������ ����� � ���孨� �ࠩ, ��஢����� �� ���孥� �࠭�� ��אַ㣮�쭨��, �ᯮ��㥬��� ��� �� ᮧ�����. �ᯮ������� ����� � SBS_HORZ. (SBS - Scroll Bar Style)',0
.1774:db '�⨫� ������ �ப��⪨ ���� ���⨪����. �᫨ �� �ᯮ������ �� �⨫� SBS_RIGHTALIGN, �� �⨫� SBS_LEFTALIGN, ����� �ப��⪨ �㤥� ����� ��� ࠧ���, ����� �� ����襭 �� �� ᮧ�����. (SBS - Scroll Bar Style)',0
.1775:db '(SB - Scroll Bar)',0
.1776:db '�ᯮ������ ⮫쪮 � ���⨪���묨 ����ᠬ� �ப��⪨, ॠ��������묨 ��� ���୨� ����, ���짮��⥫� ����� ������� "End" (SB - Scroll Bar)',0
.1777:db '(SB - Scroll Bar)',0
.1778:db '���짮��⥫� ����⨫ ������� ��� ��᫥ 㤥ঠ��� �� ����⮩ �� ��५�� ��� �� ����� �ப��⪥ (SB - Scroll Bar)',0
.1779:db '��ਧ��⠫쭠� ����� �ப��⪨ (SB - Scroll Bar)',0
.1780:db '�ᯮ������ ⮫쪮 � ��ਧ��⠫�묨 ����ᠬ� �ப��⪨, ॠ��������묨 ��� ���୨� ����, ���짮��⥫� ����� ������� "Home" (SB - Scroll Bar)',0
.1781:db '�ᯮ������ ⮫쪮 � WM_VSCROLL, 饫箪 ����� �� ��५�� ����, �ப��稢��� �� ���� "��ப�" ���� (SB - Scroll Bar)',0
.1782:db '�ᯮ������ ⮫쪮 � WM_HSCROLL, 饫箪 ����� �� ��५�� �����, �ப��稢��� �� ���� "�������" ����� (SB - Scroll Bar)',0
.1783:db '�ᯮ������ ⮫쪮 � WM_HSCROLL, 饫箪 ����� �� ��५�� ��ࠢ�, �ப��稢��� �� ���� "�������" ��ࠢ� (SB - Scroll Bar)',0
.1784:db '�ᯮ������ ⮫쪮 � WM_VSCROLL, 饫箪 ����� �� ��५�� �����, �ப��稢��� �� ���� "��ப�" ����� (SB - Scroll Bar)',0
.1785:db '�ᯮ������ ⮫쪮 � WM_VSCROLL, 饫箪 ����� �� ����� �ப��⪨ ���� ᫠����, �ப��稢��� �� ���� "��࠭���" ���� (SB - Scroll Bar)',0
.1786:db '�ᯮ������ ⮫쪮 � WM_HSCROLL, 饫箪 ����� �� ����� �ப��⪨ ����� ᫠����, �ப��稢��� �� ���� "��࠭���" ����� (SB - Scroll Bar)',0
.1787:db '�ᯮ������ ⮫쪮 � WM_HSCROLL, 饫箪 ����� �� ����� �ப��⪨ �ࠢ�� ᫠����, �ப��稢��� �� ���� "��࠭���" ��ࠢ� (SB - Scroll Bar)',0
.1788:db '�ᯮ������ ⮫쪮 � WM_VSCROLL, 饫箪 ����� �� ����� �ப��⪨ ��� ᫠����, �ப��稢��� �� ���� "��࠭���" ����� (SB - Scroll Bar)',0
.1789:db '�ᯮ������ ⮫쪮 � ��ਧ��⠫�묨 ����ᠬ� �ப��⪨, ॠ��������묨 ��� ���୨� ����, ���짮��⥫� ����� ������� "End" (SB - Scroll Bar)',0
.1790:db '(SB - Status Bar)',0
.1791:db '(SB - Status Bar)',0
.1792:db '����᪨����� ᫠���� �����祭�, ���짮��⥫� �⦠� ������� ��� (SB - Scroll Bar)',0
.1793:db '������� ����᪨������ � ������� ���, �ਢ���� � ��६�饭�� ᮤ�ন���� �࠭� (SB - Scroll Bar)',0
.1794:db '�ᯮ������ ⮫쪮 � ���⨪���묨 ����ᠬ� �ப��⪨, ॠ��������묨 ��� ���୨� ����, ���짮��⥫� ����� ������� "Home" (SB - Scroll Bar)',0
.1795:db '���⨪��쭠� ����� �ப��⪨ (SB - Scroll Bar)',0
.1841:db '��᫮ ������ ���; �᫨ ���� �� ��⠭������, � �����頥��� 0',0
.1842:db '��ਭ� ࠬ�� ����',0
.1844:db '��ਭ� ����� ���� ��� ��㪫�� ࠬ�� (���᪮�� ����������� ����)',0
.1847:db '��ਭ� ��嬥୮� ��㪫�� ࠬ�� ����',0
.1870:db '���� ࠬ�� ����',0
.1871:db '���� ��������� ����',0
.1873:db '���� ����� ���� ��� ��㪫�� ࠬ�� (���᪮�� ����������� ����)',0
.1876:db '���� ��嬥୮� ��㪫�� ࠬ�� ����',0
.1921:db '�������� win.ini (�. SystemParametersInfo)',0
.1922:db '����砥� ���ଠ�� � ��誥, �ᯮ���� SystemParametersInfo',0
.1923:db '���塞 䮭��� ��㭮� ࠡ�祣� �⮫�, �ᯮ���� SystemParametersInfo',0
.1924:db '�����塞 ���ଠ�� ��誨, �ᯮ���� SystemParametersInfo',0
.1925:db '�������஢���� ⥪�饣� ���������� � ��⮢��� ���ᨢ� �� ����� ����樨 AND',0
.1926:db '���������� �������� ������ ��⮢� ���ᨢ��',0
.1927:db '�������஢���� ��⮢��� ���ᨢ� � ������஢������ ⥪�饣� ���������� �� ����� ����樨 AND',0
.1928:db '�������஢���� ��⮢��� ���ᨢ� � ⥪�饣� ���������� �� ����� ����樨 XOR',0
.1929:db '�������஢���� ��⮢��� ���ᨢ� � ⥪�饣� ���������� �� ����� ����樨 OR',0
.1931:db '�⨫� ����᪮�� ����� �ࠢ����� ����� ���� � ⥬ �� 梥⮬, �� � ������ �����. (SS - Static Style)',0
.1932:db '�⨫� ����᪮�� ����� �ࠢ����� �������� ⥬ �� 梥⮬, ����� �뫨 ���ᮢ��� ������ �����. (SS - Static Style)',0
.1933:db '�⨫� ����᪮�� ����� �ࠢ����� �⮡ࠦ��� ⥪�� �� 業���. �᫨ ����� ⥪�� �����, 祬 �ਭ� ����� �ࠢ�����, ��㬥�⨢���� ��ப� ��७����� �� ����� ��ப�. (SS - Static Style)',0
.1941:db '�⨫� ����᪮�� ����� �ࠢ����� ����� ���� � ⥬ �� 梥⮬, �� � 䮭 �࠭�. (SS - Static Style)',0
.1942:db '�⨫� ����᪮�� ����� �ࠢ����� ���������� ⥬ �� 梥⮬, ����� �ᯮ������ ��� 䮭� �࠭�. (SS - Static Style)',0
.1943:db '�⨫� ����᪮�� ����� �ࠢ����� - ���⮣ࠬ��. ����� � ����� �ࠢ����� - ��� ���⮣ࠬ��, ᮮ⢥�����饥 ����ᠭ���� � 䠩�� ����ᮢ. ���⮣ࠬ�� ᠬ� ��⮬���᪨ ��⠭�������� ᢮� ࠧ����. (SS - Static Style)',0
.1944:db '�⨫� ����᪮�� ����� �ࠢ����� �⮡ࠦ��� ⥪�� ᫥��. �᫨ ����� ⥪�� �����, 祬 �ਭ� ����� �ࠢ�����, ��㬥�⨢���� ��ப� ��७����� �� ����� ��ப�. (SS - Static Style)',0
.1945:db '�⨫� ����᪮�� ����� �ࠢ����� �⮡ࠦ��� ⥪�� ᫥��. �᫨ ����� ⥪�� �����, 祬 �ਭ� ����� �ࠢ�����, ��㬥�⨢訩�� ⥪�� ��१�����. (SS - Static Style)',0
.1946:db '�⨫� ����᪮�� ����� �ࠢ����� �������� ᨬ���� ',39,'&',39,' � ��� ⥪��. ���筮 ᨬ��� ',39,'&',39,' �ᯮ������ ��� ��䨪�� ᨬ��� ����⨢��� ������, ����� 㤠�����, � ᫥���騩 ᨬ��� � ��ப� ����ન������. (SS - Static Style)',0
.1951:db '�⨫� ����᪮�� ����� �ࠢ����� �⮡ࠦ��� ⥪�� �ࠢ�. �᫨ ����� ⥪�� �����, 祬 �ਭ� ����� �ࠢ�����, ��㬥�⨢���� ��ப� ��७����� �� ����� ��ப�. (SS - Static Style)',0
.1953:db '�⨫� ����᪮�� ����� �ࠢ����� �⮡ࠦ��� ���� ��ப� ⥪��, ᬥ饭��� �����. ����� �� ����� ���� �������. ��஦���騩 ��ꥪ� ����� �ࠢ����� �� ������ ��ࠡ��뢠�� ᮮ�饭�� WM_CTLCOLOR. (SS - Static Style)',0
.1956:db '�⨫� ����᪮�� ����� �ࠢ����� ���� ����᪨� ����⮬ �ࠢ�����, ��।������ ���짮��⥫��. (SS - Static Style)',0
.1957:db '�⨫� ����᪮�� ����� �ࠢ����� ����� ���� � ⥬ �� 梥⮬, �� � 䮭 ����. (SS - Static Style)',0
.1958:db '�⨫� ����᪮�� ����� �ࠢ����� �������� ⥬ �� 梥⮬, ����� �� �������� 䮭 ����. (SS - Static Style)',0
.1981:db '���ய��樮����� ��⥬�� ����, �ᯮ������ ��� ᮢ���⨬��� � �।��騬� ����ﬨ Windows ',0
.1982:db '���⥬�� ����, �ᯮ��㥬� �� 㬮�砭�� ��� �⮡ࠦ���� �㭪⮢ ����, ⥪�� ����⮢ �ࠢ����� � �. �.',0
.1983:db '���࠭�� ⥪�饥 ���ﭨ� ������� DC � �⥪� ���⥪��.',0
.1985:db '��������� ࠧ���� ���� ������� DC �⭮�⥫쭮 ⥪��� ࠧ��஢.',0
.1987:db '�ப��稢��� ��⮢� ��אַ㣮�쭨� ��ਧ��⠫쭮 � ���⨪��쭮 �� �������� ����稭�.',0
.1990:db '�롨ࠥ� ⥪�騩 ������� ������� DC � ����⢥ ������ ���祭��, �ᯮ���� ������� ०��.',0
.1991:db '�롨ࠥ� � ����⢥ ⥪�饩 ������ ���祭�� ������� DC �������� �������, �������� ����� �� �� ������� ��� ��㣨� ��ꥪ⮢ DC.',0
.1992:db '�롨ࠥ� � ������ DC ������� ��ꥪ� GDI.',0
.1993:db '�롨ࠥ� � DC �।��।������ ��ꥪ�� ���, ����, ���� ��� �������.',0
.1998:db '��⠭�������� ��� ������� DC ⥪�騩 䮭��� 梥�.',0
.1999:db '��⠭�������� 䮭��� ०��.',0
.2001:db '��ࠢ��� ��� ������� DC ����������� ���ଠ樨 ��࠭�稢��饣� ��אַ㣮�쭨��.',0
.2002:db '��⠭�������� ��砫� ⥪�饩 ��࠭��� ���� ������� DC.',0
.2020:db '�㭪�� ����� TGlyphButton. ���⠪��: void SetGlyph(TResId, TModule*, TGlyphType); void SetGlyph(TBitmap*, TGlyphType); void SetGlyph(HBITMAP, TGlyphType, TAutoDelete);',0
.2024:db '��⠭�������� ०�� �⮡ࠦ���� ⥪�饣� ���� ������� DC.',0
.2025:db '������� ������, �ᯮ��㥬� �।�⢮� �⮡ࠦ���� ����, �⮡ࠦ��騬 � ������ DC �����᪨� ����� � 䨧��᪨�.',0
.2027:db '��⠭�������� �।�� ᮥ������� � ���� ���祭��.',0
.2031:db '��⠭�������� 梥� ����� ����ࠦ���� � ������묨 ���न��⠬� � 㪠����� 梥�.',0
.2032:db '��⠭�������� ��� ������� DC ०�� ����᪨ �����㣮�쭨��.',0
.2034:db '��⠭�������� ��� ������� DC ⥪�騩 ०�� ᬥ訢���� 䮭�.',0
.2041:db '��⠭�������� ०�� ���殮��� ��� ������� DC.',0
.2044:db '������� �ᯮ�짮����� ��⥬��� ������� ������� DC.',0
.2045:db '��⠭�������� ��� ������� DC 䫠�� ��ࠢ������� ⥪��.',0
.2046:db '��⠭�������� ���祭�� ⥪�饣� ���ࢠ�� ����� ᨬ������ � �����᪨� ������� (��� ������� DC).',0
.2047:db '��⠭�������� ��� ������� DC ⥪�騩 梥� ⥪��.',0
.2048:db '������ ࠧ��� ��饣� �������⥫쭮�� ����࠭�⢠, ������塞�� �� �뢮�� ⥪�⮢�� ��ப �� ��� ᨬ����� ��ॢ��� ��ப� (� �����᪨� �������).',0
.2053:db '��⠭�������� ������� ��ᬮ�� ������� DC � 㪠������ ���祭��, ��࠭�� �।��饥 ���祭��.',0
.2054:db '��⠭�������� ࠧ���� x � y ���� ������� DC.',0
.2057:db '��⠭�������� ��砫� �易����� � ����� ����� DC � �������� ���祭�� � ��࠭�� �।��饥.',0
.2062:db '������ ��㬥୮� �������� �८�ࠧ������ ����� �������� ����࠭�⢮� � ����࠭�⢮� ��࠭��� ������� DC.',0
.2072:db '������� ��⮢� ���ᨢ �� ��室���� DC � 楫���� ��אַ㣮�쭨� � ������ DC. ���筨� ����� ᦨ������ ��� ��������� � ᮮ⢥��⢨� � ०���� ���殮���.',0
.2074:db '���몠�� �� ������ 䨣��� ⥪�饣� ������� � ������ DC, �ᯮ���� ��� �⮣� ⥪�饥 ��� � ����訢�� ����७��� ������� � ������� ⥪�饩 ����, �ਬ���� ०�� ����᪨ �����㣮�쭨��.',0
.2075:db '��ॢ���� ⥪�騩 �������� ������� � ������ DC, �ᯮ���� ⥪�饥 ���.',0
.2079:db '��ࠢ������� ⥪�� �� 業��� (�. SetTextAlign, TA - Text Align)',0
.2080:db '��ࠢ������� ⥪�� �� ������ ��� (�. SetTextAlign, TA - Text Align)',0
.2083:db '��ࠢ������� ⥪�� �� �ࠢ��� ��� (�. SetTextAlign, TA - Text Align)',0
.2086:db '����� ',39,'�ਫ������',39,'',0
.2087:db '�⬥�� �뤥����� ��������� ���祭�� � ���� Trackbar (TBM - Track Bar Message)',0
.2088:db '�������� �᮪ (TBM - Track Bar Message)',0
.2089:db '��।������ ���न��� � ࠧ��஢ ����ࠦ������ ��אַ㣮�쭨��, ��࠭�稢��饣� ������� ��६�饭�� ������ (TBM - Track Bar Message)',0
.2090:db '��।������ ����稭� (� �᪠�) �� ������ ��६�頥��� ������, �᫨ ���짮��⥫� �������� ������ � ������ VK_LEFT, VK_RIGHT, VK_UP ��� VK_DOWN (TBM - Track Bar Message)',0
.2091:db '��।������ ������⢠ �᮪ � ���� Trackbar (TBM - Track Bar Message)',0
.2092:db '��।������ ����稭� (� �᪠�) �� ������ ��६�頥��� ������, �᫨ ���짮��⥫� �������� ������ � ������ VK_PRIOR ��� VK_NEXT (TBM - Track Bar Message)',0
.2093:db '��।������ ⥪�饣� ��������� ������ (TBM - Track Bar Message)',0
.2094:db '�㭪�� SendMessage, ���뫠��� �� ᮮ�饭��, �����頥� 㪠��⥫� �� ���ᨢ ����権 �᮪ (TBM - Track Bar Message)',0
.2095:db '��।������ ���ᨬ��쭮� ����樨 ������ (TBM - Track Bar Message)',0
.2096:db '��।������ �������쭮� ����樨 ������ (TBM - Track Bar Message)',0
.2097:db '��।������ ����筮� ����樨 �뤥������ ������ (TBM - Track Bar Message)',0
.2098:db '��।������ ��砫쭮� ����樨 �뤥������ ������ (TBM - Track Bar Message)',0
.2099:db '��।������ ����� ������ � ���ᥫ�� (TBM - Track Bar Message)',0
.2100:db '��।������ �ᯮ������� � ࠧ��஢ ������ (TBM - Track Bar Message)',0
.2101:db '��।������ ����樨 �᪨ � ������� ����஬ (TBM - Track Bar Message)',0
.2102:db '��।������ 䨧��᪮�� �ᯮ������� 㪠������ �᪨ � ��⥬� ���न��� ����७��� ������ ���� (TBM - Track Bar Message)',0
.2103:db '��⠭���� ����稭� (� �᪠�) �� ������ ��६�頥��� ������, �᫨ ���짮��⥫� �������� ������ � ������ VK_LEFT, VK_RIGHT, VK_UP ��� VK_DOWN (TBM - Track Bar Message)',0
.2104:db '��⠭���� ����稭� (� �᪠�) �� ������ ��६�頥��� ������, �᫨ ���짮��⥫� �������� ������ � ������ VK_PRIOR ��� VK_NEXT (TBM - Track Bar Message)',0
.2105:db '��⠭���� ⥪�饣� ��������� ������ (TBM - Track Bar Message)',0
.2106:db '��⠭���� �������쭮� � ���ᨬ��쭮� ����樨 ������ � ���� Trackbar (TBM - Track Bar Message)',0
.2107:db '��⠭���� ���ᨬ��쭮� ����樨 ������ � ���� Trackbar (TBM - Track Bar Message)',0
.2108:db '��⠭���� �������쭮� ����樨 ������ � ���� Trackbar (TBM - Track Bar Message)',0
.2109:db '��⠭���� ��砫쭮� � ����筮� ����樨 �뤥������ ������ (TBM - Track Bar Message)',0
.2110:db '��⠭���� ����筮� ����樨 �뤥������ ������ (TBM - Track Bar Message)',0
.2111:db '��⠭���� ��砫쭮� ����樨 �뤥������ ������ (TBM - Track Bar Message)',0
.2112:db '��⠭���� ����� ������ � ���ᥫ�� (TBM - Track Bar Message)',0
.2113:db '��⠭���� ����樨 �᪨ � ������� ����஬. �� �ᯮ������ ��� ��ࢮ� � ��᫥���� �᪨, ����� ᮧ������ ��⮬���᪨ (TBM - Track Bar Message)',0
.2114:db '��⠭���� ���ࢠ��, � ����� �ᯮ�������� �᪨ (TBM - Track Bar Message)',0
.2134:db '������� ����� ��⪨ ��� ��� ���祭�� � �������� ��������� ���祭�� (TBS - Track Bar Style)',0
.2135:db '��᪨ ���� �ᯮ������ � ����� ��஭ (TBS - Track Bar Style)',0
.2136:db '��᪨ ���� �ᯮ������ ᭨�� (�ᯮ������ ����� � �⨫�� TBS_HORZ, TBS - Track Bar Style)',0
.2137:db '�������� �뤥����� ��������� ���祭�� (TBS - Track Bar Style)',0
.2139:db '��ਧ��⠫쭮� �ᯮ������� ���� Trackbar (TBS - Track Bar Style)',0
.2140:db '��᪨ ���� �ᯮ������ ᫥�� (�ᯮ������ ����� � �⨫�� TBS_VERT, TBS - Track Bar Style)',0
.2142:db '�ࠧ� ��᫥ ᮧ����� ���� Trackbar �᪨ �� �⮡ࠦ�����, ������ �� ����� ��।����� �� ����� ᮮ�饭�� TBM_SETTIC � TBM_SETTICFREQ (TBS - Track Bar Style)',0
.2143:db '��᪨ ���� �ᯮ������ �ࠢ� (�ᯮ������ ����� � �⨫�� TBS_VERT, TBS - Track Bar Style) ',0
.2145:db '��᪨ ���� �ᯮ������ ᢥ��� (�ᯮ������ ����� � �⨫�� TBS_HORZ, TBS - Track Bar Style)',0
.2146:db '���⨪��쭮� �ᯮ������� ���� Trackbar (TBS - Track Bar Style)',0
.2182:db '������� ��㭮� ��� ������ TGlyphButton. ����������: TBtnBitmap(HINSTANCE, TResId, const TColor&); TBtnBitmap(HBITMAP, const TColor&, TAutoDelete);. ��� const TColor& - �㤥� �஧��� 梥⮬ ��㭪�.',0
.2184:db '������ ��� ������ TControlBar. ���������: TButtonGadget(TResId, int, TType, bool, TState, bool);',0
.2223:db '������ ����� ��� ��㣨� ����ᮢ ���⥪�� ���ன�⢠; ������㫨��� � ����⢥ ����⮢ �㭪樨 GDI',0
.2226:db '������ ����᪮�� ���⥪��, ���ᯥ稢��� ����� �� �ᥬ� �࠭�',0
.2231:db '���ᯥ稢��� ����� � ��⮢� ����栬, �� ������騬 �� ���ன�⢠; �ॡ�� ������ �ࠩ��� ��ૠ��� DIB.DRV',0
.2250:db '����� ',39,'���� � ࠬ���',39,'',0
.2256:db '������ � ��㭪�� � ⥪�⮬. ����������: TGlyphButton(TWindow*, int, const char far*, int, int, int, int, bool, TModule*); TGlyphButton(TWindow*, int, TModule*);. ���㭮� �������� �१ �㭪�� SetGlyph � ����� TBtnBitmap.',0
.2261:db '���쪮 ���ଠ樮��� ���⥪�� ���ன�⢠; �� �ᯮ������ ��� �뢮�� ��䨪�',0
.2340:db '������ �ࠢ� �� ����� ��ॢ�, ����⨥� �� ������ ����� ���뢠�� ��ॢ�� (TVS - Tree View Style)',0
.2341:db '���뢠�� த�⥫�� � ���쬨 ����ﬨ (TVS - Tree View Style)',0
.2343:db '�������� ����� � ��୥���� ��ॢ� (TVS - Tree View Style)',0
.2356:db '����� ',39,'����',39,'',0
.2357:db '����� ����᪮�� ���⥪��, ���ᯥ稢��� ����� � 楫��� ����',0
.2362:db '����� �������� �᫮ ᨬ����� ������ ��ப� � �������騬 �㫥�, �ᯮ���� ⥪�騩 ���� ������� DC. �����樨 ��������� ᮣ��᭮ ��㬥�⠬.',0
.2364:db '����� �������� �᫮ ᨬ����� 㪠������ ��ப� � �������騬 �㫥�, �ᯮ���� ⥪�騩 ���� ������� DC.',0
.2365:db '����訢��� 㪠����� ��אַ㣮�쭨�, ��뢠� ExtTextOut.',0
.2378:db '�������� �������� ������� ������� DC, ᮯ��⠢��� ⥪�騩 梥� � �����⭮� ������ � ��⥬��� �����ன (�� ����⠬ ����ࠦ����).',0
.2408:db '��� ������ [����]',0
.2456:db '����� ������ ���',0
.2458:db '��� ������ [�����]',0
.2463:db '�।��� ������ ���',0
.2495:db '�ࠢ�� ������ ���',0
.2498:db '��� ������ [��ࠢ�]',0
.2513:db '��� ������ [�����]',0
.2528:db '�������� ������� ����訢����� ᯫ��� ���� 梥⮬',0
.2529:db '����� �����',0
.2530:db '���� ��࠭���',0
.2539:db '���� ��⨢�������� ��� ����⨢�������� (WM - Window Message)',0
.2540:db '��⨢����㥬�� ���� �ਭ������� ��㣮�� �ਫ������ (WM - Window Message)',0
.2544:db '����饭�� ���뫠�� ���� ������ �ਫ������, �⮡� 㧭��� �ଠ� (WM - Window Message)',0
.2546:db '�⬥�� ��⥬���� ०��� (WM - Window Message)',0
.2551:db '��⨢�������� ���୥� ���� (WM - Window Message)',0
.2553:db '���� ����뢠���� (������ �� ������ �������) (WM - Window Message)',0
.2562:db '���� ᮧ������, �� �� �� ������� (WM - Window Message)',0
.2563:db '�������� த�⥫�᪮�� ���� �������� 梥� 䮭� � ⥪��, ����묨 ���ᮢ�� ���୨� ����� (WM - Window Message)',0
.2586:db '���� ������ ���� 㭨�⮦��� (WM - Window Message)',0
.2595:db '���� �뫮 �������஢��� ��� ࠧ�����஢��� (WM - Window Message)',0
.2596:db '����� ࠡ��� �����蠥��� (WM - Window Message)',0
.2597:db '��砫� ���ᨢ���� 横��, ����� ����� ��ᯮ�짮������ ��� �㦤 �ணࠬ�� (WM - Window Message)',0
.2600:db '����室��� ����� 䮭 ���� (WM - Window Message)',0
.2608:db '����祭�� ���ଠ樨 � ࠧ���� ���� (WM - Window Message)',0
.2610:db '����祭�� ⥪��, �易����� � ����� (WM - Window Message)',0
.2612:db '����祭�� ����� ⥪��, �易����� � ����� (WM - Window Message)',0
.2619:db '��������� ��������஢����� ����, �� ��� 䮭 ������ ���� �������� ��� �����⮢�� � �ᮢ����� ���⮣ࠬ�� (WM - Window Message)',0
.2638:db '����� ������ ���������� (WM - Window Message)',0
.2641:db '���饭� ������ ���������� (WM - Window Message)',0
.2642:db '���� ���� 䮪�� ����� (WM - Window Message)',0
.2662:db '���짮��⥫� ����� �������, �� �ᯮ��㥬�� � ⥪�饬 ���� (WM - Window Message)',0
.2667:db '��������� �������� ����, �� ��࠭ ����� ���� (WM - Window Message)',0
.2668:db '� ����⨢��� ���� �� ᤥ��� 饫箪 ����� (WM - Window Message)',0
.2672:db '��६�饭 ����� ��� (WM - Window Message)',0
.2673:db '���� ��६�頥��� (WM - Window Message)',0
.2696:db '����室��� ����ᮢ�� ���� (WM - Window Message)',0
.2698:db '����ᮢ��� ���� ���窠 �ਫ������ (WM - Window Message)',0
.2701:db '���� ᮧ������ ��� 㭨�⮦����� (WM - Window Message)',0
.2710:db '����祭� ������� �� �����襭�� ᥠ�� (WM - Window Message)',0
.2714:db '�ਫ������ �����蠥��� (WM - Window Message)',0
.2721:db '���� ����稫� 䮪�� ����� (WM - Window Message)',0
.2722:db '��������� ���� (WM - Window Message)',0
.2725:db '������� 䫠� ����ᮢ�� (WM - Window Message)',0
.2726:db '������ ⥪�� � ��������� ���� (WM - Window Message)',0
.2729:db '���� ����室��� ����� ��� �뢥�� �� �࠭ (WM - Window Message)',0
.2730:db '���������� ࠧ���� ���� (WM - Window Message)',0
.2745:db '����饭�� ⠩��� (WM - Window Message)',0
.2746:db '�⬥��� ��᫥���� ������ � �࣠��� �ࠢ����� ।���஢����� (WM - Window Message)',0
.2755:db '�⨫� ���� � ⮭��� ࠬ��� ����� ������᪮� ������ (WS - Window Style)',0
.2756:db '�⨫� ���� � ��ப�� ���������, �� ����� �ᯮ�짮������ � �⨫�� WS_DLGFRAME (WS - Window Style)',0
.2757:db '�⨫� ᮧ����� ���୥�� ����. �� �ᯮ������ � ������ �⨫� WS_POPUP. (WS - Window Style)',0
.2758:db '�� ��, �� � WS_CHILD (WS - Window Style)',0
.2759:db '�⨫� ���� �ᯮ������ �� ᮧ����� த�⥫�᪮�� ����, �᪫�砥� �������, ������� ���୨�� ������ �� ���ᮢ�� ����� த�⥫�᪮�� ����. (WS - Window Style)',0
.2760:db '�⨫� ���� �ᯮ������ ����� � WS_CHILD, �� ᮧ����� ���୨� ����. �� �������� ��४�뢠�騬�� ���୨� ����� �ᮢ��� � ��४��⮩ ������. (WS - Window Style)',0
.2761:db '�⨫� ���� ��ࢮ��砫쭮 �������஢������ (�� ����� �ਭ����� �������� ���ଠ�� �� ���짮��⥫�) (WS - Window Style)',0
.2762:db '�⨫� ���� ��� ��������� � ����饣� ������� ࠬ�� (WS - Window Style)',0
.2784:db '�⨫� ���� ��।����騩 1-� ����� ��㯯� ���� (WS - Window Style)',0
.2785:db '�⨫� ���� � ��ਧ��⠫쭮� ����ᮩ �ப��⪨ (WS - Window Style)',0
.2786:db '�⨫� ���� ��������஢����� ���� (WS - Window Style)',0
.2787:db '�⨫� ���� ���ᨬ���஢����� ���� (WS - Window Style)',0
.2788:db '�⨫� ���� � ������� ���ᨬ���樨 (WS - Window Style)',0
.2789:db '�⨫� ���� ��������஢����� ���� (� ��, �� � WS_ICONIC) (WS - Window Style)',0
.2790:db '�⨫� ���� � ������� ��������樨 (WS - Window Style)',0
.2791:db '�⨫� ���� ��४�뢠���� ���� � ࠬ��� (WS - Window Style)',0
.2792:db '��������� �⨫�� WS_OVERLAPPED, WS_CAPTION, WS_SYSMENU, WS_THICKFRAME, WS_MINIMIZEBOX, WS_MAXIMIZEBOX (WS - Window Style)',0
.2793:db '�⨫� ���� �ᯫ뢠�饥 ����, �� �ᯮ������ ����� � WS_CHILD (WS - Window Style)',0
.2794:db '��������� �⨫�� WS_POPUP, WS_BORDER, WS_SYSMENU (WS - Window Style)',0
.2795:db '�⨫� ���� � �����塞� �� ࠬ�� ࠧ��஬ (WS - Window Style)',0
.2796:db '�⨫� ���� � ������� ��⥬���� ���� (WS - Window Style)',0
.2798:db '������� ���� � ࠬ���, ����� �������� �������� ࠧ��� ���� (WS - Window Style)',0
.2800:db '�� ��, �� � WS_OVERLAPPEDWINDOW (WS - Window Style)',0
.2801:db '�⨫� ���� ����砫쭮 ������� (WS - Window Style)',0
.2802:db '�⨫� ���� � ���⨪��쭮� ����ᮩ �ப��⪨ (WS - Window Style)',0
.2810:db '��८�।���� ⥪�騩 �������� ������� ������� DC ��� �������, ����ࠦ����� ⥪�騬 ��஬ ������� DC.',0
.2811:db '�㭪�� ����� TRect, �����頥� �ਭ� ��אַ㣮�쭨��',0
.2817:db '����⮢�� ',39,'�᪫���饥 ���',39,'',0
.2819:db '�����᪨� ⨯ ������',0
.2820:db '��室 �� 横��',0
.2823:db 'ᨬ����� ⨯ ������ 1 ����',0
.2824:db '�����',0
.2825:db '����⠭�',0
.2827:db '�ய�� ⥫� 横�� � ���室 �� ��� ��砫�',0
.2829:db '㤠���� ��ꥪ� �� �����',0
.2830:db '横� � �᫮����, �믮������ ������ 1 ࠧ',0
.2831:db '�᫮ � ������饩 ����⮩ ������� �筮�� 8 ����',0
.2837:db '�᫮ � ������饩 ����⮩ 4 ����',0
.2838:db '������ 横��',0
.2840:db '�᫮��� ������',0
.2842:db '楫� ⨯ ������ 2 ����',0
.2843:db '������ ⨯ ������',0
.2844:db 'ᮧ���� ��ꥪ� � �����',0
.2846:db '����� �����. �������� ����㯭� ⮫쪮 ������ � ��� ��㦥�⢥��� ����ᠬ (�������� �१ friend)',0
.2847:db '���饭��� ᥪ�� �����. �������� ����㯭� ������ � ��� �ந������ ����ᠬ.',0
.2848:db '��饤���㯭�� ᥪ�� �����. �������� ����㯭� �����.',0
.2850:db '��室 �� �㭪樨',0
.2851:db '���⪨� ⨯ ������',0
.2852:db '������� ⨯ ������',0
.2855:db '�������',0
.2861:db '��८�।������ ⨯�� ������',0
.2863:db '���������� ⨯ ������',0
.2864:db '�ᯮ������ ��� ᮧ����� ����㠫��� �㭪権',0
.2867:db '横� � �᫮����',0
.2869:db '����⮢�� ',39,'���',39,'',0
.2871:db '�����᪮� ',39,'���',39,'',0
.2873:db '����⮢�� ',39,'���栭��',39,'',0
