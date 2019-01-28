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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"
#include "mstaticpiece.h"
#include "manimatepiece.h"
#include "mswiperpiece.h"

#ifdef _MGNCSCTRL_ANIMATE

static void mSwiperPiece_construct(mSwiperPiece *self, DWORD add_data)
{
    Class(mAnimatePiece).construct((mAnimatePiece*)self, add_data);
    self->pagination = 0;
    self->pagination_bgcolor = 0x40C9C2BD;
    self->pagination_bullet_color = 0xFFFFFFFF;
    self->pagination_bullet_hilight_color = 0xFFF48924;
    self->pagination_bullet_width = 20;
    self->pagination_bullet_margin = 2;
    self->pagination_bullet_padding = 2;
}

static void mSwiperPiece_destroy(mSwiperPiece *self)
{
    Class(mAnimatePiece).destroy((mAnimatePiece*)self);
}

static void mSwiperPiece_paint(mSwiperPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
    _SUPER(mAnimatePiece, self, paint, hdc, owner, add_data);

    if(self->frame && self->frame->nr_frames > 0 && add_data
        && (mAnimatePiece_isPlay(self)|| mAnimatePiece_isAutoplay(self))) {
        self->pagination++;
        if(self->pagination > self->frame->nr_frames)
            self->pagination = 1;
    }

    if(self->pagination == 0)
        return;

    RECT rcClient, rcPagination;
    //DWORD uFormat = DT_WORDBREAK | DT_CENTER | DT_VCENTER | DT_SINGLELINE;
    //char str[64];
    //sprintf(str, "Page:%d", self->pagination);
    //SetTextColor(hdc, ncsColor2Pixel(hdc, 0));
    //SetBkMode (hdc, BM_TRANSPARENT);
    _c(self)->getRect(self, &rcClient);
    //DrawText (hdc, str, -1, &rcClient, uFormat);
    int w = self->pagination_bullet_width / 2 + self->pagination_bullet_margin;

    int paginationWidth = (self->frame->nr_frames + 2) * 2 * w;

    rcPagination.left = 0;
    rcPagination.top = 0;
    rcPagination.bottom = self->pagination_bullet_width;
    rcPagination.right =  paginationWidth;

    //HDC mem_dc = CreateMemDC (RECTW(rcPagination), RECTH(rcPagination), 18,
      //                          MEMDC_FLAG_HWSURFACE | MEMDC_FLAG_SRCALPHA,
        //                        0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F);
    HDC mem_dc = CreateCompatibleDCEx(hdc, RECTW(rcPagination), RECTH(rcPagination));

    ncsCommRDRFillHalfRoundRect(mem_dc, &rcPagination,
                            self->pagination_bullet_width / 2, self->pagination_bullet_width / 2,
                            self->pagination_bgcolor, NCS_ROUND_CORNER_ALL | NCS_BORDER_ALL);

     SetBrushColor(mem_dc, ncsColor2Pixel(mem_dc, self->pagination_bullet_color));
    int x = w;
    int y = self->pagination_bullet_width / 2;
    int r = self->pagination_bullet_width / 2 - self->pagination_bullet_padding;

    //先填充第一个圆，后续其他圆用拷贝提高效率
    x += w * 2;
    FillCircle(mem_dc, x, y, r);

    x -= w;
    int i;
    for(i = 1; i < self->frame->nr_frames; i++)
    {
        x += w * 2;
        /* Copy the circle in other posititon of client area by BitBlt function */
        BitBlt (mem_dc, 2*w, 0, self->pagination_bullet_width, self->pagination_bullet_width, mem_dc, x, 0, 0);
        //FillCircle(mem_dc, x, y, r);
    }

    SetBrushColor(mem_dc, ncsColor2Pixel(mem_dc, self->pagination_bullet_hilight_color));

    x = self->pagination * 2 * w + w;
    FillCircle(mem_dc, x, y, r);

    BitBlt(mem_dc, 0, 0, RECTW(rcPagination), RECTH(rcPagination), hdc,
        (RECTW(rcClient) - paginationWidth)/2, RECTH(rcClient)*7/8, 0);

    DeleteMemDC (mem_dc);
}

static void mSwiperPiece_reset(mSwiperPiece *self)
{
    _SUPER(mAnimatePiece, self, reset);
    self->pagination = 0;
}

static BOOL mSwiperPiece_setProperty(mSwiperPiece *self, int id, DWORD value)
{
    BOOL ret;

    switch(id)
    {
    case NCSP_SWIPERPIECE_PAGINATION_BULLET_WIDTH:
        self->pagination_bullet_width = value;
        break;
    case NCSP_SWIPERPIECE_PAGINATION_BULLET_MARGIN:
        self->pagination_bullet_margin = value;
        break;
    case NCSP_SWIPERPIECE_PAGINATION_BULLET_PADDING:
        if(value > self->pagination_bullet_width)
            return FALSE;
        self->pagination_bullet_padding = value;
        break;
    case NCSP_SWIPERPIECE_PAGINATION_BULLET_COLOR:
        self->pagination_bullet_color = value;
        break;
    case NCSP_SWIPERPIECE_PAGINATION_BULLET_HILIGHT_COLOR:
        self->pagination_bullet_hilight_color = value;
        break;
    case NCSP_SWIPERPIECE_PAGINATION_BGCOLOR:
        self->pagination_bgcolor = value;
        break;
    default:
        return Class(mAnimatePiece).setProperty((mAnimatePiece*)self, id, value);
    }
    return TRUE;
}

static DWORD mSwiperPiece_getProperty(mSwiperPiece* self, int id)
{
    switch(id)
    {
    case NCSP_SWIPERPIECE_PAGINATION_BULLET_WIDTH:
        return self->pagination_bullet_width;
    case NCSP_SWIPERPIECE_PAGINATION_BULLET_MARGIN:
        return self->pagination_bullet_margin;
    case NCSP_SWIPERPIECE_PAGINATION_BULLET_PADDING:
        return self->pagination_bullet_padding;
    case NCSP_SWIPERPIECE_PAGINATION_BULLET_COLOR:
        return self->pagination_bullet_color;
    case NCSP_SWIPERPIECE_PAGINATION_BGCOLOR:
        return self->pagination_bgcolor;
    case NCSP_SWIPERPIECE_PAGINATION_BULLET_HILIGHT_COLOR:
        return self->pagination_bullet_hilight_color;
    }
    return Class(mAnimatePiece).getProperty((mAnimatePiece*)self, id);
}


BEGIN_MINI_CLASS(mSwiperPiece, mAnimatePiece)
    CLASS_METHOD_MAP(mSwiperPiece, construct)
    CLASS_METHOD_MAP(mSwiperPiece, destroy)
    CLASS_METHOD_MAP(mSwiperPiece, paint)
    CLASS_METHOD_MAP(mSwiperPiece, reset)
    CLASS_METHOD_MAP(mSwiperPiece, getProperty)
    CLASS_METHOD_MAP(mSwiperPiece, setProperty)
END_MINI_CLASS

#endif        //_MGNCSCTRL_ANIMATE
