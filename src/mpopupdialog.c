/*
    This file is part of mGNCS, a component for MiniGUI.

    Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSCTRL_DIALOGBOX

//#define ROUND_RECT_RADIUS 10

static void mPopupDialog_construct(mPopupDialog *self, DWORD addData)
{
	self->first_idle = 1;
	Class(mDialogBox).construct((mDialogBox *)self, addData);
}


static BOOL mPopupDialog_onEraseBkgnd(mWidget* self, HDC hdc, const RECT *pinv)
{
	RECT rc;
	GetWindowRect(self->hwnd, &rc);
	rc.right = RECTW(rc);
	rc.bottom = RECTH(rc);
	rc.left = rc.top = 0;
	/*gal_pixel bg_color = GetWindowBkColor(self->hwnd);
	gal_pixel bg_old = SetBrushColor(hdc, bg_color);
	FillBox(hdc, 0, 0, RECTW(rc), RECTH(rc));
	SetMemDCColorKey(hdc, MEMDC_FLAG_SRCCOLORKEY, bg_color);
	SetBrushColor(hdc, bg_old);*/

    //ncsCommRDRFillHalfRoundRect(hdc, &rc, ROUND_RECT_RADIUS, ROUND_RECT_RADIUS,
      //                    PIXEL_lightwhite, NCS_ROUND_CORNER_ALL | NCS_BORDER_ALL);
	return NCSR_CONTINUE_MSG; /*Don't call default EraseBkgrnd*/
}

static DWORD mPopupDialog_doModal(mPopupDialog* self, BOOL bAutoDestroy)
{
	HWND hwnd;
	HWND howner;

	hwnd = self->hwnd;
	howner = GetHosting(hwnd);
	Class(mDialogBox).doModal((mDialogBox *)self, bAutoDestroy);
	InvalidateRect(howner, 0, TRUE);
}

static LRESULT mPopupDialog_wndProc(mPopupDialog *self, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwnd;
	HWND howner;
	hwnd = self->hwnd;

	HDC sec_dc;

	switch(message){
		case MSG_CREATE:
			sec_dc = GetSecondaryDC (hwnd);
			SetSecondaryDC (hwnd, sec_dc, ON_UPDSECDC_DONOTHING);
			break;
		case MSG_IDLE:
			if(self->first_idle) {
				self->first_idle = 0;

				hwnd = self->hwnd;
				howner = GetHosting(hwnd);

				RECT rc0, rc1;
				int w0, h0, w1, h1;

				GetWindowRect(howner, &rc0);
				GetWindowRect(hwnd, &rc1);

				w0 = RECTW(rc0);
				h0 = RECTH(rc0);

				w1 = RECTW(rc1);
				h1 = RECTH(rc1);

				HDC mem_dc = CreateCompatibleDCEx(HDC_SCREEN, w0, h0);
				if(mem_dc) {
					//SetBrushColor(mem_dc, 0x50000000);
					SetBrushColor(mem_dc, 0xff000000);
					SetMemDCAlpha (mem_dc, MEMDC_FLAG_SRCALPHA, 128);

					FillBox(mem_dc, 0, 0, w0, h0);
                    
					BitBlt (mem_dc, 0, 0, w0, h0, HDC_SCREEN, rc0.left, rc0.top, 0);
					DeleteMemDC (mem_dc);
				}

				/* restore to default behavior */
				SetSecondaryDC (hwnd, sec_dc, ON_UPDSECDC_DEFAULT);
				InvalidateRect (hwnd, 0, TRUE);
			}
			break;
	}
	return Class(mDialogBox).wndProc((mDialogBox*)self, message, wParam, lParam);
}


BEGIN_CMPT_CLASS(mPopupDialog, mDialogBox)
	CLASS_METHOD_MAP(mPopupDialog, construct)
	CLASS_METHOD_MAP(mPopupDialog, doModal)
	CLASS_METHOD_MAP(mPopupDialog, wndProc);
	//CLASS_METHOD_MAP(mPopupDialog, onEraseBkgnd)
END_CMPT_CLASS

#endif
