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

#ifdef _MGNCS_GUIBUILDER_SUPPORT
#include "iconview_content_data.c"

static PBITMAP* get_default_bmparray(void) {
    static PBITMAP pbmps[2] = { NULL, NULL };
    if(pbmps[0] == NULL)
        pbmps[0] = get_iconview_content();
    return pbmps;
}
#endif

#include "manimate.h"
#include "mswiper.h"

static void mSwiper_construct(mSwiper *self, DWORD addData)
{
    _SUPER(mAnimate, self, construct, addData);
    self->pagination_bullet_width = 20;
    self->pagination_bullet_margin = 0;
    self->pagination_bullet_padding = 0;
}


static mAnimate* mSwiper_createBody(mSwiper *self)
{
    DWORD dwStyle = GetWindowStyle(self->hwnd);
    mSwiperPiece *body = NEWPIECE(mSwiperPiece);

    if(dwStyle & NCSS_ANMT_AUTOLOOP)
        _c(body)->setProperty(body, NCSP_ANIMATEPIECE_AUTOLOOP, 1);
    if(dwStyle & NCSS_ANMT_SCALE)
        _c(body)->setProperty(body, NCSP_ANIMATEPIECE_SCALE, 1);
    if(dwStyle & NCSS_ANMT_AUTOFIT)
        _c(body)->setProperty(body, NCSP_ANIMATEPIECE_AUTOFIT, 1);
    if(dwStyle & NCSS_ANMT_AUTOPLAY)
        _c(body)->setProperty(body, NCSP_ANIMATEPIECE_AUTOPLAY, 1);
#ifdef _MGNCS_GUIBUILDER_SUPPORT
    _c(body)->setProperty(body, NCSP_ANIMATEPIECE_BMPARRAY, (DWORD)get_default_bmparray());
#endif

    return (mAnimate*)body;
}


static BOOL mSwiper_setProperty(mSwiper *self, int id, DWORD value)
{
    if(id >= NCSP_SWIPER_MAX)
        return FALSE;

    switch(id)
    {
    case NCSP_SWIPER_PAGINATION_BULLET_WIDTH:
        {
            int width = (int)value;
            if(width <= 0)
                return FALSE;
            return _c((mSwiperPiece*)(self->body))->setProperty((mSwiperPiece*)(self->body), NCSP_SWIPERPIECE_PAGINATION_BULLET_WIDTH, value);
        }
    case NCSP_SWIPER_PAGINATION_BULLET_COLOR:
        return Body?SetBodyProp(NCSP_SWIPERPIECE_PAGINATION_BULLET_COLOR, value):FALSE;
    case NCSP_SWIPER_PAGINATION_BULLET_HILIGHT_COLOR:
        return Body?SetBodyProp(NCSP_SWIPERPIECE_PAGINATION_BULLET_HILIGHT_COLOR, value):FALSE;
    case NCSP_SWIPER_PAGINATION_BGCOLOR:
        return Body?SetBodyProp(NCSP_SWIPERPIECE_PAGINATION_BGCOLOR, value):FALSE;
    case NCSP_SWIPER_PAGINATION_BULLET_MARGIN:
        return Body?SetBodyProp(NCSP_SWIPERPIECE_PAGINATION_BULLET_MARGIN, value):FALSE;
    case NCSP_SWIPER_PAGINATION_BULLET_PADDING:
        return Body?SetBodyProp(NCSP_SWIPERPIECE_PAGINATION_BULLET_PADDING, value):FALSE;
    }

    return Class(mAnimate).setProperty((mAnimate*)self, id, value);
}

static DWORD mSwiper_getProperty(mSwiper *self, int id)
{
    if(id >= NCSP_SWIPER_MAX)
        return 0;

    switch(id)
    {
    case NCSP_SWIPER_PAGINATION_BULLET_WIDTH:
        return Body?GetBodyProp(NCSP_SWIPERPIECE_PAGINATION_BULLET_WIDTH):0;
    case NCSP_SWIPER_PAGINATION_BULLET_MARGIN:
        return Body?GetBodyProp(NCSP_SWIPERPIECE_PAGINATION_BULLET_MARGIN):0;
    case NCSP_SWIPER_PAGINATION_BULLET_PADDING:
        return Body?GetBodyProp(NCSP_SWIPERPIECE_PAGINATION_BULLET_PADDING):0;
    case NCSP_SWIPER_PAGINATION_BULLET_COLOR:
        return Body?GetBodyProp(NCSP_SWIPERPIECE_PAGINATION_BULLET_COLOR):0;
    case NCSP_SWIPER_PAGINATION_BULLET_HILIGHT_COLOR:
        return Body?GetBodyProp(NCSP_SWIPERPIECE_PAGINATION_BULLET_HILIGHT_COLOR):0;
    case NCSP_SWIPER_PAGINATION_BGCOLOR:
        return Body?GetBodyProp(NCSP_SWIPERPIECE_PAGINATION_BGCOLOR):0;
    }

    return Class(mAnimate).getProperty((mAnimate*)self, id);
}

BEGIN_CMPT_CLASS(mSwiper, mAnimate)
    CLASS_METHOD_MAP(mSwiper, construct)
    CLASS_METHOD_MAP(mSwiper, setProperty)
    CLASS_METHOD_MAP(mSwiper, getProperty)
    CLASS_METHOD_MAP(mSwiper, createBody)
END_CMPT_CLASS

#endif        //_MGNCSCTRL_ANIMATE
