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

#ifdef _MGNCSCTRL_ANIMATE

#define ID_TIMER 10001

static void mMarquee_construct(mMarquee *self, DWORD addData)
{
    self->scrolldelay = 10;
    self->loop = -1;
    self->hspace = 0;
    self->vspace = 0;
    self->mem_dc = 0;
    Class(mStatic).construct((mStatic*)self, addData);
}

static void mMarquee_destroy(mMarquee *self)
{
    if(self->mem_dc)
        DeleteMemDC(self->mem_dc);

    Class(mStatic).destroy((mStatic*)self);
}

static mStaticPiece* mMarquee_createBody(mMarquee *self)
{
    DWORD dwStyle = GetWindowStyle(self->hwnd);
    mMarqueePiece *body = NEWPIECE(mMarqueePiece);

    if(dwStyle & NCSS_MARQUEE_DIRECTION_LEFT)
        _c(body)->setProperty(body, NCSS_MARQUEEPIECE_DIRECTION_LEFT, 1);
    if(dwStyle & NCSS_MARQUEE_DIRECTION_RIGHT)
        _c(body)->setProperty(body, NCSS_MARQUEEPIECE_DIRECTION_RIGHT, 1);
    if(dwStyle & NCSS_MARQUEE_DIRECTION_UP)
        _c(body)->setProperty(body, NCSS_MARQUEEPIECE_DIRECTION_UP, 1);
    if(dwStyle & NCSS_MARQUEE_DIRECTION_DOWN)
        _c(body)->setProperty(body, NCSS_MARQUEEPIECE_DIRECTION_DOWN, 1);
    if(dwStyle & NCSS_MARQUEE_BEHAVIOR_ALTERNATE)
        _c(body)->setProperty(body, NCSS_MARQUEEPIECE_BEHAVIOR_ALTERNATE, 1);
    if(dwStyle & NCSS_MARQUEE_BEHAVIOR_SCROLL)
        _c(body)->setProperty(body, NCSS_MARQUEEPIECE_BEHAVIOR_SCROLL, 1);
    if(dwStyle & NCSS_MARQUEE_BEHAVIOR_SLIDE)
        _c(body)->setProperty(body, NCSS_MARQUEEPIECE_BEHAVIOR_SLIDE, 1);
    if(dwStyle & NCSS_MARQUEE_AUTOPLAY)
        _c(body)->setProperty(body, NCSP_MARQUEEPIECE_AUTOPLAY, 1);

    return (mStaticPiece*)body;
}


static BOOL mMarquee_setProperty(mMarquee *self, int id, DWORD value)
{
    if(id >= NCSP_MARQUEE_MAX)
        return FALSE;

    switch(id)
    {
        case NCSP_MARQUEE_SCROLL_DELAY:
            {
                int scrolldelay = (int)value;
                if(scrolldelay < 0)
                    return FALSE;
                self->scrolldelay = scrolldelay;
                if(self->state == MARQUEE_PLAY || self->state == MARQUEE_PAUSE)
                {
                    ResetTimer (self->hwnd, ID_TIMER, self->scrolldelay);
                }
            }
            return TRUE;
        case NCSP_MARQUEE_SCROLL_AMOUNT:
            {
                int scrollamount = (int)value;
                if(scrollamount < 0)
                    return FALSE;
                if(Body && SetBodyProp(NCSP_MARQUEEPIECE_SCROLL_AMOUNT, scrollamount))
                {
                	return TRUE;
                }
            }
            return FALSE;
        case NCSP_MARQUEE_HSPACE:
            {
                int hspace = (int)value;
                if(hspace < 0)
                    return FALSE;
                self->hspace = hspace;
                InvalidateRect(self->hwnd, NULL, TRUE);
            }
            return TRUE;
        case NCSP_MARQUEE_VSPACE:
            {
                int vspace = (int)value;
                if(vspace < 0)
                    return FALSE;
                self->vspace = vspace;
                InvalidateRect(self->hwnd, NULL, TRUE);
            }
            return TRUE;
    }
    return Class(mStatic).setProperty((mStatic*)self, id, value);
}

static DWORD mMarquee_getProperty(mMarquee *self, int id)
{
    if(id >= NCSP_MARQUEE_MAX)
        return 0;

    switch(id)
    {
        case NCSP_MARQUEE_SCROLL_DELAY:
            return self->scrolldelay;
        case NCSP_MARQUEE_SCROLL_AMOUNT:
            return Body?GetBodyProp(NCSP_MARQUEEPIECE_SCROLL_AMOUNT):0;
        case NCSP_MARQUEE_HSPACE:
            return self->hspace;
        case NCSP_MARQUEE_VSPACE:
            return self->vspace;
    }

    return Class(mStatic).getProperty((mStatic*)self, id);
}

static LRESULT mMarquee_wndProc(mMarquee* self, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
            SetTimer (self->hwnd, ID_TIMER, self->scrolldelay);
            if (GetWindowStyle (self->hwnd) & NCSS_MARQUEE_AUTOPLAY) {
                self->state = MARQUEE_PLAY;
            }
            break;
        case MSG_TIMER:
            if(self->state == MARQUEE_PAUSE || self->state == MARQUEE_STOP)
            {
                return 0;
            }
            self->paint_flag = 1; //set the flags
            InvalidateRect(self->hwnd, NULL, TRUE);
            return 0;
        case MARQUEE_STARTPLAY:
            return _c(self)->play(self);
        case MARQUEE_PAUSE_RESUME:
            return _c(self)->pauseResume(self);
        case MARQUEE_STOPPLAY:
            return _c(self)->stop(self);
        case MSG_SIZECHANGING:
        case MSG_FONTCHANGED:
        case MSG_SETTEXT:
            _c(self)->play(self);
            //need not break;
        default:
            break;
    }
    return Class(mStatic).wndProc((mStatic*)self, message, wParam, lParam);
}

static BOOL mMarquee_play(mMarquee* self)
{
    self->state = MARQUEE_PLAY;
    _c((mMarqueePiece*)(self->body))->reset((mMarqueePiece*)self->body);
    return TRUE;
}

static BOOL mMarquee_pauseResume(mMarquee* self)
{
    if(self->state == MARQUEE_STOP)
        return FALSE;
    if(self->state == MARQUEE_PLAY)
        self->state = MARQUEE_PAUSE;
    else
        self->state = MARQUEE_PLAY;
    return TRUE;
}

static BOOL mMarquee_stop(mMarquee* self)
{
    self->state = MARQUEE_STOP;
    _c((mMarqueePiece*)(self->body))->reset((mMarqueePiece*)(self->body));
    InvalidateRect(self->hwnd, NULL, FALSE);
    return TRUE;
}

static BOOL mMarquee_onEraseBkgnd(mMarquee* self, HDC hdc, const RECT *pinv)
{
    if(self->mem_dc)
    {
        SetMemDCColorKey(self->mem_dc, 0, 0);
        return  Class(mStatic).onEraseBkgnd((mStatic*)self, self->mem_dc, pinv);
    }
    else
        return  Class(mStatic).onEraseBkgnd((mStatic*)self, hdc, pinv);
}

static int mMarquee_onSizeChanged(mMarquee* self, RECT *rtClient)
{
    int width, height;
    if(self->mem_dc)
    {
        width = GetGDCapability(self->mem_dc, GDCAP_HPIXEL) + self->hspace*2;
        height = GetGDCapability(self->mem_dc, GDCAP_VPIXEL) + self->vspace*2;
        if(width == RECTWP(rtClient) && height == RECTHP(rtClient))
            goto END;

        DeleteMemDC(self->mem_dc);
        self->mem_dc = 0;
    }
    width = RECTWP(rtClient) - self->hspace*2;
    height = RECTHP(rtClient) - self->vspace*2;
    if(width <= 0 || height <= 0)
        goto END;
    rtClient->left = 0;
    rtClient->top = 0;
    rtClient->right = width;
    rtClient->bottom = height;
    self->mem_dc = CreateCompatibleDCEx(HDC_SCREEN, width, height);
END:
    return Class(mStatic).onSizeChanged((mStatic*)self, rtClient);
}

static void mMarquee_onPaint(mMarquee* self, HDC hdc, const PCLIPRGN* pclip)
{
    RECT rc;
    DWORD add_data  = 0;
	HDC sub_dc;
    if(!self->body)
        return;
    if(self->mem_dc == 0)
        return;

    GetClientRect(self->hwnd, &rc);
    if(GetWindowExStyle(self->hwnd) & WS_EX_TRANSPARENT)
    {
        //clear the memdc
        gal_pixel bg_color = GetWindowBkColor(self->hwnd);
        gal_pixel bg_old = SetBrushColor(self->mem_dc, bg_color);
        FillBox(self->mem_dc, 0, 0, RECTW(rc), RECTH(rc));
        SetMemDCColorKey(self->mem_dc, MEMDC_FLAG_SRCCOLORKEY, bg_color);
        //SetBrushColor(self->mem_dc, bg_old);//不要设置回去，mmarqueepiece.c需要用到该画刷颜色，以支持透明
        //mmarqueepiece.c会复制该mem_dc
    }

    if(!(GetWindowStyle(self->hwnd)&WS_DISABLED))
        add_data = self->paint_flag;

    //sub_dc = GetSubDC(hdc, self->hspace, self->vspace, RECTW(rc)-2*self->hspace, RECTH(rc)-2*self->vspace);
    //_c(Body)->paint(Body, hdc, (mObject*)self, add_data);
    //ReleaseDC(sub_dc);

    //_c(Body)->paint(Body, hdc, (mObject*)self, add_data);

    _c(Body)->paint(Body, self->mem_dc, (mObject*)self, add_data);
    BitBlt(self->mem_dc, 0, 0, RECTW(rc)-2*self->hspace, RECTH(rc)-2*self->vspace, hdc, self->hspace, self->vspace, 0);
    self->paint_flag = 0;
}

#ifdef _MGNCS_GUIBUILDER_SUPPORT
static BOOL mMarquee_refresh(mMarquee* self)
{
	DWORD dwStyle;
	if(!Body)
		return FALSE;

	dwStyle = GetWindowStyle(self->hwnd);

	if(dwStyle & NCSS_MARQUEE_AUTOPLAY)
	{
		_c(Body)->setProperty(Body, NCSP_MARQUEEPIECE_AUTOPLAY, 1);
		self->state = MARQUEE_PLAY;
	}
	else {
		_c(Body)->setProperty(Body, NCSP_MARQUEEPIECE_AUTOPLAY, 0);
		self->state = MARQUEE_STOP;
	}

	if(self->mem_dc)
	{
	}

	InvalidateRect(self->hwnd, 0, TRUE);
	return TRUE;
}
#endif

BEGIN_CMPT_CLASS(mMarquee, mStatic)
    CLASS_METHOD_MAP(mMarquee, construct)
    CLASS_METHOD_MAP(mMarquee, wndProc)
    CLASS_METHOD_MAP(mMarquee, createBody)
    CLASS_METHOD_MAP(mMarquee, destroy)
    CLASS_METHOD_MAP(mMarquee, setProperty)
    CLASS_METHOD_MAP(mMarquee, getProperty)
    CLASS_METHOD_MAP(mMarquee, onEraseBkgnd)
    CLASS_METHOD_MAP(mMarquee, play)
    CLASS_METHOD_MAP(mMarquee, pauseResume)
    CLASS_METHOD_MAP(mMarquee, stop)
    CLASS_METHOD_MAP(mMarquee, onSizeChanged)
    CLASS_METHOD_MAP(mMarquee, onPaint)
#ifdef _MGNCS_GUIBUILDER_SUPPORT
    CLASS_METHOD_MAP(mMarquee, refresh)
#endif
END_CMPT_CLASS

#endif        //_MGNCSCTRL_ANIMATE
