/*
 *   This file is part of mGNCS, a component for MiniGUI.
 *
 *   Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Or,
 *
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 *
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 *
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"
#include "mstaticpiece.h"
#include "mlabelpiece.h"
#include "mmarqueepiece.h"

#ifdef _MGNCSCTRL_ANIMATE

static void mMarqueePiece_construct(mMarqueePiece *self, DWORD add_data)
{
    Class(mLabelPiece).construct((mLabelPiece*)self, add_data);
    self->flags = 0;
    self->mem_dc = 0;
    self->scrollamount = 1;
    self->view_x = 0;
    self->view_y = 0;
}

static void mMarqueePiece_destroy(mMarqueePiece *self)
{
    if(self->mem_dc)
        DeleteMemDC(self->mem_dc);
    Class(mLabelPiece).destroy((mLabelPiece*)self);
}

static void mMarqueePiece_alternate(mMarqueePiece *self)
{
}

static void mMarqueePiece_scroll(mMarqueePiece *self)
{
}

static void mMarqueePiece_paint(mMarqueePiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
    RECT rc;
    _c(self)->getRect(self, &rc);

    if(self->mem_dc == 0) {
        DWORD uFormat = DT_WORDBREAK;
        DWORD fg_color = 0, bg_color = 0;
        //如果为参数uFormat指定DT_CALCRECT值，必须为lpRect指向的RECT结构设置right和bottom成员
        RECT rcText={0, 0, INT16_MAX, INT16_MAX};

        const char* str = self->str;
        if(owner == NULL || str == NULL)
            return ;

        SelectFont(hdc, GetWindowFont(owner->hwnd));
        fg_color = ncsGetElement(owner, NCS_FGC_WINDOW);

        //if(self->align == NCS_ALIGN_CENTER)
        //    uFormat |= DT_CENTER;
        //else if(self->align == NCS_ALIGN_RIGHT)
        //    uFormat |= DT_RIGHT;
        //else
        //    uFormat |= DT_LEFT;

        //if(self->valign == NCS_VALIGN_CENTER)
       //     uFormat |= DT_VCENTER;
        //else if(self->valign == NCS_VALIGN_BOTTOM)
        //    uFormat |= DT_BOTTOM;
        //else
        //    uFormat |= DT_TOP;

        if(!mLabelPiece_isAutoWrap(self))
            uFormat |= DT_SINGLELINE;
        else
            rcText.right = RECTW(rc);

        //if(!mLabelPiece_isPrefix(self))
        //    uFormat |= DT_NOPREFIX;

        /*
         * for vertical text
         */
        if (mLabelPiece_isWordBreak(self))
            uFormat |= DT_WORDBREAK;

        DrawText(hdc, str, -1, &rcText, uFormat | DT_CALCRECT);
        self->mem_w = RECTW(rcText);
        self->mem_h = RECTH(rcText);
        //self->mem_dc = CreateMemDC(self->mem_w, self->mem_h, 18, MEMDC_FLAG_HWSURFACE | MEMDC_FLAG_SRCALPHA, 0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F);
        self->mem_dc = CreateCompatibleDCEx(hdc, self->mem_w, self->mem_h);
        FillBox(self->mem_dc, 0, 0, self->mem_w, self->mem_h);
        
        SelectFont(self->mem_dc, GetWindowFont(owner->hwnd));
        SetTextColor(self->mem_dc, ncsColor2Pixel(self->mem_dc, fg_color));
        SetBkColor( self->mem_dc, COLOR_transparent );
        SetBkMode(self->mem_dc, BM_TRANSPARENT);

        SetRect(&rcText, 0, 0, self->mem_w, self->mem_h);
        DrawText (self->mem_dc, str, -1, &rcText, uFormat);
		
		printf("mem_dc w:%d h:%d rc: w:%d h%d\n", self->mem_w, self->mem_h, RECTW(rc), RECTH(rc));
    }

    int minWidth, maxWidth;
    int minHeight, maxHeight;
    int x1 = -1,  y1 = -1, w1 = -1, h1 = -1;
    int x2 = -1,  y2 = -1, w2 = -1, h2 = -1;
    if(self->mem_w > RECTW(rc)) {
        minWidth = RECTW(rc);
        maxWidth = self->mem_w;
        x2 = 0;
    } else {
        minWidth = self->mem_w;
        maxWidth = RECTW(rc);
        x1 = 0;
    }

    if(self->mem_h > RECTH(rc)) {
        minHeight = RECTH(rc);
        maxHeight = self->mem_h;
        y2 = 0;
    } else {
        minHeight = self->mem_h;
        maxHeight = RECTH(rc);
        y1 = 0;
    }
    
    if(add_data && (mMarqueePiece_isPlay(self) || mMarqueePiece_isAutoplay(self))) {
        if(mMarqueePiece_isDirectionLeft(self))
            self->view_x += self->scrollamount;
        else if(mMarqueePiece_isDirectionRight(self))
            self->view_x -= self->scrollamount;
        
        if(mMarqueePiece_isDirectionUp(self))
            self->view_y += self->scrollamount;
        else if(mMarqueePiece_isDirectionDown(self))
            self->view_y -= self->scrollamount;
    }
    
    if(mMarqueePiece_isBehaviorAlternate(self)) {
        if(self->view_x + minWidth > maxWidth) {
            mMarqueePiece_setDirectionLeft(self, 0);
            mMarqueePiece_setDirectionRight(self, 1);
            self->view_x = maxWidth - minWidth;
        } else if(self->view_x < 0) {
            mMarqueePiece_setDirectionRight(self, 0);
            mMarqueePiece_setDirectionLeft(self, 1);
            self->view_x = 0;
        }

        if(self->view_y + minHeight > maxHeight) {
            mMarqueePiece_setDirectionUp(self, 1);
            mMarqueePiece_setDirectionDown(self, 0);
            self->view_y = maxHeight - minHeight;
        } else if(self->view_y < 0) {
            mMarqueePiece_setDirectionDown(self, 1);
            mMarqueePiece_setDirectionUp(self, 0);
            self->view_y = 0;
        }
        
        w1 = minWidth;
        if(x1 == 0) {
            x2 = self->view_x;
        } else {
            x1 = self->view_x;
        }
        
        h1 = minHeight;
        if(y1 == 0) {
            y2 = self->view_y;
        } else {
            y1 = self->view_y;
        }
    } else if(mMarqueePiece_isBehaviorScroll(self)) {
        if(self->view_x > maxWidth) {
            self->view_x = 0;

        } else if(self->view_x < 0) {
            self->view_x = maxWidth;
        }

        if(self->view_y > maxHeight) {
            self->view_y = 0;
        } else if(self->view_y < 0) {
            self->view_y = maxHeight;
        }

        w1 = maxWidth - self->view_x;
        if(w1 > minWidth)
            w1 = minWidth;
        else
            w2 = minWidth - w1;

        if(x1 == 0) {
            x2 = self->view_x;
        } else {
            x1 = self->view_x;
        }

        h1 = maxHeight - self->view_y;
        if(h1 > minHeight)
            h1 = minHeight;
        else
            h2 = minHeight - h1;

        if(y1 == 0) {
            y2 = self->view_y;
        } else {
            y1 = self->view_y;
        }
    }
    
    //printf("x1=%d y1=%d w1=%d h1=%d ", x1, y1, w1, h1);
    //printf("x2=%d y2=%d w2=%d h2=%d\n", x2, y2, w2, h2);
    
    BitBlt(self->mem_dc, x1, y1, w1, h1, hdc, x2, y2, 0);
    if(w2 > 0)
        if(x2 == 0) BitBlt(self->mem_dc, 0, y1, w2, h1, hdc, w1, 0, 0);
        else BitBlt(self->mem_dc, x1+w1, y1, w2, h1, hdc, 0, 0, 0);
    if(h2 > 0)
        if(y2 == 0) BitBlt(self->mem_dc, x1, 0, w1, h2, hdc, 0, h1, 0);
        else BitBlt(self->mem_dc, x1, y1+h1, w1, h2, hdc, 0, 0, 0);
        
}

static void mMarqueePiece_reset(mMarqueePiece *self)
{
    if (self->mem_dc == 0)
        return;

    if(self->mem_dc) {
        DeleteMemDC(self->mem_dc);
        self->mem_dc = 0;
        self->view_x = 0;
        self->view_y = 0;
    }
    mMarqueePiece_setPlay(self, 1);
}

static BOOL mMarqueePiece_setProperty(mMarqueePiece *self, int id, DWORD value)
{
    switch(id)
    {
    case NCSS_MARQUEEPIECE_DIRECTION_LEFT:
        mMarqueePiece_setDirectionLeft(self, value);
        return TRUE;
    case NCSS_MARQUEEPIECE_DIRECTION_RIGHT:
        mMarqueePiece_setDirectionRight(self, value);
        return TRUE;
    case NCSS_MARQUEEPIECE_DIRECTION_UP:
        mMarqueePiece_setDirectionUp(self, value);
        return TRUE;
    case NCSS_MARQUEEPIECE_DIRECTION_DOWN:
        mMarqueePiece_setDirectionDown(self, value);
        return TRUE;
    case NCSS_MARQUEEPIECE_BEHAVIOR_ALTERNATE:
        mMarqueePiece_setBehaviorAlternate(self, value);
        return TRUE;
    case NCSS_MARQUEEPIECE_BEHAVIOR_SCROLL:
        mMarqueePiece_setBehaviorScroll(self, value);
        return TRUE;
    case NCSS_MARQUEEPIECE_BEHAVIOR_SLIDE:
        mMarqueePiece_setBehaviorSlide(self, value);
        return TRUE;
    case NCSP_MARQUEEPIECE_AUTOPLAY:
        mMarqueePiece_setAutoplay(self, value);
        return TRUE;
    case NCSP_MARQUEEPIECE_SCROLL_AMOUNT:
        self->scrollamount = value;
        return TRUE;
    default:
        return Class(mLabelPiece).setProperty((mLabelPiece*)self, id, value);
    }
    return TRUE;
}

static DWORD mMarqueePiece_getProperty(mMarqueePiece* self, int id)
{
    switch(id)
    {
    case NCSS_MARQUEEPIECE_DIRECTION_LEFT:
        return mMarqueePiece_isDirectionLeft(self);
    case NCSS_MARQUEEPIECE_DIRECTION_RIGHT:
        return mMarqueePiece_isDirectionRight(self);
    case NCSS_MARQUEEPIECE_DIRECTION_UP:
        return mMarqueePiece_isDirectionUp(self);
    case NCSS_MARQUEEPIECE_DIRECTION_DOWN:
        return mMarqueePiece_isDirectionDown(self);
    case NCSS_MARQUEEPIECE_BEHAVIOR_ALTERNATE:
        return mMarqueePiece_isBehaviorAlternate(self);
    case NCSS_MARQUEEPIECE_BEHAVIOR_SCROLL:
        return mMarqueePiece_isBehaviorScroll(self);
    case NCSS_MARQUEEPIECE_BEHAVIOR_SLIDE:
        return mMarqueePiece_isBehaviorSlide(self);
    case NCSP_MARQUEEPIECE_AUTOPLAY:
        return mMarqueePiece_isAutoplay(self);
    }
    return Class(mLabelPiece).getProperty((mLabelPiece*)self, id);
}


BEGIN_MINI_CLASS(mMarqueePiece, mLabelPiece)
    CLASS_METHOD_MAP(mMarqueePiece, construct)
    CLASS_METHOD_MAP(mMarqueePiece, destroy)
    CLASS_METHOD_MAP(mMarqueePiece, paint)
    CLASS_METHOD_MAP(mMarqueePiece, reset)
    CLASS_METHOD_MAP(mMarqueePiece, getProperty)
    CLASS_METHOD_MAP(mMarqueePiece, setProperty)
END_MINI_CLASS

#endif        //_MGNCSCTRL_ANIMATE
