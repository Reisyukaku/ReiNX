/*
 * Copyright (c) 2018 naehrwert
 * Copyright (C) 2018 CTCaer
 * Copyright (C) 2018 Reisyukaku
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdarg.h>
#include <string.h>
#include "gfx.h"

/*
*   Init functions
*/
void gfx_init_ctxt(gfx_ctxt_t *ctxt, u32 *fb, u32 width, u32 height, u32 stride)
{
	ctxt->fb = fb;
	ctxt->width = width;
	ctxt->height = height;
	ctxt->stride = stride;
}

void gfx_con_init(gfx_con_t *con, gfx_ctxt_t *ctxt)
{
	con->gfx_ctxt = ctxt;
	con->x = 0;
	con->y = 0;
	con->savedx = 0;
	con->savedy = 0;
	con->fgcol = 0xFFCCCCCC;
	con->fillbg = 0;
	con->bgcol = 0;
	con->mute = 1;
}

/*
*   Getters/Setters
*/
void gfx_con_setcol(gfx_con_t *con, u32 fgcol, int fillbg, u32 bgcol)
{
	con->fgcol = fgcol;
	con->fillbg = fillbg;
	con->bgcol = bgcol;
}

void gfx_con_getpos(gfx_con_t *con, u32 *x, u32 *y)
{
	*x = con->x;
	*y = con->y;
}

void gfx_con_setpos(gfx_con_t *con, u32 x, u32 y)
{
	con->x = x;
	con->y = y;
}

/*
*   Draw/Print functions
*/
void gfx_set_pixel(gfx_ctxt_t *ctxt, u32 x, u32 y, u32 color)
{
    ctxt->fb[y + x * ctxt->stride] = color;
}

void gfx_clear_color(gfx_ctxt_t *ctxt, u32 color)
{
	for (u32 x = 0; x < ctxt->width; x++)
		for (u32 y = 0; y < ctxt->height; y++)
			gfx_set_pixel(ctxt, x, y, color);
}

void gfx_putc(gfx_con_t *con, char c)
{
    if (c >= 32 && c <= 126)
    {
        u8 *cbuf = (u8 *)&_gfx_font[8 * (c - 32)];
        for (u32 i = 0; i < 8; i++)
        {
            u8 v = *cbuf++;
            for (u32 j = 0; j < 8; j++)
            {
                if (v & 1) 
                    gfx_set_pixel(con->gfx_ctxt, con->x + j, con->y + i, con->fgcol);
                else if (con->fillbg) 
                    gfx_set_pixel(con->gfx_ctxt, con->x + j, con->y + i, con->bgcol);
                v >>= 1;
            }
        }
        con->x += 8;
    }
    else if (c == '\n')
    {
        con->x = 0;
        con->y += 8;
        if (con->y > con->gfx_ctxt->height - 8)
            con->y = 0;
    }
}

void gfx_puts(gfx_con_t *con, const char *s)
{
	if (!s || con->mute)
		return;

	for (; *s; s++)
		gfx_putc(con, *s);
}

static void _gfx_putn(gfx_con_t *con, u32 v, int base, char fill, int fcnt)
{
	char buf[65];
	static const char digits[] = "0123456789ABCDEFghijklmnopqrstuvwxyz";
	char *p;
	int c = fcnt;

	if (base > 36) return;

	p = buf + 64;
	*p = 0;
	do
	{
		c--;
		*--p = digits[v % base];
		v /= base;
	} while (v);

	if (fill != 0)
	{
		while (c > 0)
		{
			*--p = fill;
			c--;
		}
	}

	gfx_puts(con, p);
}

void gfx_printf(gfx_con_t *con, const char *fmt, ...)
{
	if (con->mute)
		return;

	va_list ap;
	int fill, fcnt;

	va_start(ap, fmt);
	while(*fmt)
	{
		if(*fmt == '%')
		{
			fmt++;
			fill = 0;
			fcnt = 0;
			if ((*fmt >= '0' && *fmt <= '9') || *fmt == ' ')
			{
				fcnt = *fmt;
				fmt++;
				if (*fmt >= '0' && *fmt <= '9')
				{
					fill = fcnt;
					fcnt = *fmt - '0';
					fmt++;
				}
				else
				{
					fill = ' ';
					fcnt -= '0';
				}
			}
			switch(*fmt)
			{
                case 'c': { gfx_putc(con, va_arg(ap, u32)); break; }
                case 's': { gfx_puts(con, va_arg(ap, char *)); break; }
                case 'd': { _gfx_putn(con, va_arg(ap, u32), 10, fill, fcnt); break; }
                case 'p':
                case 'P':
                case 'x':
                case 'X': { _gfx_putn(con, va_arg(ap, u32), 16, fill, fcnt); break; }
                case 'k': { con->fgcol = va_arg(ap, u32); break; }
                case 'K': { con->bgcol = va_arg(ap, u32); con->fillbg = 1; break; }
                case '%': { gfx_putc(con, '%'); break; }
                case '\0': goto out;
                default: { gfx_putc(con, '%'); gfx_putc(con, *fmt); break; }
			}
		}
		else {
			gfx_putc(con, *fmt);
        }
		fmt++;
	}

	out:
	va_end(ap);
}

void gfx_hexdump(gfx_con_t *con, const u8 *buf, u32 len)
{
	if (con->mute)
		return;

	for(u32 i = 0; i < len; i++)
	{
		if(i % 0x10 == 0)
		{
			if(i != 0)
			{
				gfx_puts(con, "| ");
				for(u32 j = 0; j < 0x10; j++)
				{
					u8 c = buf[i - 0x10 + j];
					if(c >= 32 && c <= 126)
						gfx_putc(con, c);
					else
						gfx_putc(con, '.');
				}
				gfx_putc(con, '\n');
			}
			gfx_printf(con, "%08x: ", &buf + i);
		}
		gfx_printf(con, "%02x ", buf[i]);
		if (i == len - 1)
		{
			int ln = len % 0x10 != 0;
			u32 k = 0x10 - 1;
			if (ln)
			{
				k = (len & 0xF) - 1;
				for (u32 j = 0; j < 0x10 - k; j++)
					gfx_puts(con, "   ");
			}
			gfx_puts(con, "| ");
			for(u32 j = 0; j < (ln ? k : k + 1); j++)
			{
				u8 c = buf[i - k + j];
				if(c >= 32 && c <= 126)
					gfx_putc(con, c);
				else
					gfx_putc(con, '.');
			}
			gfx_putc(con, '\n');
		}
	}
	gfx_putc(con, '\n');
}

void gfx_load_splash(const u32 *buf)
{
    u32 width = 1280, height = 720;
    u32 x,y;
    u32 i = 0;
    for (x = 0; x < width; x++)
        for (y = 0; y < height; y++){
			gfx_ctxt.fb[y + x * gfx_ctxt.stride] = (u32*)buf[(((height-1) - y ) * width + x)];
        }
}