#include <stdio.h>
#include "bootpack.h"

char hankaku[16][8] = 
  {"........",
   "...**...",
   "...**...",
   "...**...",
   "...**...",
   "..*..*..",
   "..*..*..",
   "..*..*..",
   "..*..*..",
   ".******.",
   ".*....*.",
   ".*....*.",
   ".*....*.",
   "***..***",
   "........",
   "........"};

unsigned char table_rgb[16 * 3] = {
    0x00, 0x00, 0x00,	/*  0:黒 */
    0xff, 0x00, 0x00,	/*  1:明るい赤 */
    0x00, 0xff, 0x00,	/*  2:明るい緑 */
    0xff, 0xff, 0x00,	/*  3:明るい黄色 */
    0x00, 0x00, 0xff,	/*  4:明るい青 */
    0xff, 0x00, 0xff,	/*  5:明るい紫 */
    0x00, 0xff, 0xff,	/*  6:明るい水色 */
    0xff, 0xff, 0xff,	/*  7:白 */
    0xc6, 0xc6, 0xc6,	/*  8:明るい灰色 */
    0x84, 0x00, 0x00,	/*  9:暗い赤 */
    0x00, 0x84, 0x00,	/* 10:暗い緑 */
    0x84, 0x84, 0x00,	/* 11:暗い黄色 */
    0x00, 0x00, 0x84,	/* 12:暗い青 */
    0x84, 0x00, 0x84,	/* 13:暗い紫 */
    0x00, 0x84, 0x84,	/* 14:暗い水色 */
    0x84, 0x84, 0x84	/* 15:暗い灰色 */
};

char cursor[16][16] = {
  "**************..",
  "*OOOOOOOOOOO*...",
  "*OOOOOOOOOO*....",
  "*OOOOOOOOO*.....",
  "*OOOOOOOO*......",
  "*OOOOOOO*.......",
  "*OOOOOOO*.......",
  "*OOOOOOOO*......",
  "*OOOO**OOO*.....",
  "*OOO*..*OOO*....",
  "*OO*....*OOO*...",
  "*O*......*OOO*..",
  "**........*OOO*.",
  "*..........*OOO*",
  "............*OO*",
  ".............***"
};

void KernelMain() {
  struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
  char s[40];
  char mcursor[256];
  int mx, my;
  char mousebuf[128];
  struct MOUSE_DEC mdec;
  int i;

  // write_screen(binfo->vram);
  init_gdtidt();
  init_pic();
  _io_sti();
  fifo8_init(&mousefifo, 128, mousebuf);
  init_pit();
  // TODO: _io_out8(PIC0_IMR, 0xf8) is error!
  // _io_out8(PIC0_IMR, 0xf8); /* PITとPIC1とキーボードを許可(11111000) */
  _io_out8(PIC1_IMR, 0xef); /* マウスを許可(11101111) */
  
  enable_mouse(&mdec);

  init_palette();
  init_screen8(binfo->vram, binfo->scrnx, binfo->scrny);
  mx = (binfo->scrnx - 16) / 2; /* 画面中央になるように座標計算 */
  my = (binfo->scrny - 28 - 16) / 2;
  init_mouse_cursor8(mcursor, COL8_008484);
  putblock8_8(binfo->vram, binfo->scrnx, 16, 16, mx, my, mcursor, 16);
  // sprintf(s, "(%d, %d)", mx, my);

  static char font_A[16] = {
    0x00, 0x18, 0x18, 0x18, 0x18, 0x24, 0x24, 0x24,
    0x24, 0x7e, 0x42, 0x42, 0x42, 0xe7, 0x00, 0x00
  };
  putfont8(binfo->vram, binfo->scrnx, 0, 0, COL8_FFFFFF, font_A);

  while (1) {
    _io_cli();
    if (fifo8_status(&mousefifo) == 0) {
      _io_sti();
    } else if (fifo8_status(&mousefifo) != 0) {
      i = fifo8_get(&mousefifo);
      _io_sti();
      if (mouse_decode(&mdec, i) != 0) {
	/* データが3バイト揃ったので表示 */
	/*
	sprintf(s, "[lcr %4d %4d]", mdec.x, mdec.y);
	if ((mdec.btn & 0x01) != 0) {
	  s[1] = 'L';
	}
	if ((mdec.btn & 0x02) != 0) {
	  s[3] = 'R';
	}
	if ((mdec.btn & 0x04) != 0) {
	  s[2] = 'C';
	}
	boxfill8(buf_back, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);
	putfonts8_asc(buf_back, binfo->scrnx, 32, 16, COL8_FFFFFF, s);
	sheet_refresh(sht_back, 32, 16, 32 + 15 * 8, 32);
	*/
	/* マウスカーソルの移動 */
        //	boxfill8(buf_back, binfo->scrnx, COL8_008484, 32, 16, 32 + 15 * 8 - 1, 31);
	mx += mdec.x;
	my += mdec.y;
	if (mx < 0) {
	  mx = 0;
	}
	if (my < 0) {
	  my = 0;
	}
	if (mx > binfo->scrnx - 1) {
	  mx = binfo->scrnx - 1;
	}
	if (my > binfo->scrny - 1) {
	  my = binfo->scrny - 1;
	}
	// sprintf(s, "(%3d, %3d)", mx, my);
	// boxfill8(buf_back, binfo->scrnx, COL8_008484, 0, 0, 79, 15); /* 座標消す */
	// putfonts8_asc(buf_back, binfo->scrnx, 0, 0, COL8_FFFFFF, s); /* 座標書く */
	// sheet_refresh(sht_back, 0, 0, 80, 16);
	// sheet_slide(sht_mouse, mx, my);
      }
    }
  }
}

void write_screen(char* addr) {
  /*
  int i = 0;
  for (i = addr; i <= 0xaffff; i++)
    write_mem8 (i, i & 0xf);
  */
}

void init_palette(void)
{
  set_palette(0, 15, table_rgb);
  return;
}

void set_palette(int start, int end, unsigned char *rgb)
{
  int i, eflags;
  eflags = _io_load_eflags();	/* 割り込み許可フラグの値を記録する */
  _io_cli(); 					/* 許可フラグを0にして割り込み禁止にする */
  _io_out8(0x03c8, start);
  for (i = start; i <= end; i++) {
    _io_out8(0x03c9, rgb[0] / 4);
    _io_out8(0x03c9, rgb[1] / 4);
    _io_out8(0x03c9, rgb[2] / 4);
    rgb += 3;
  }
  _io_store_eflags(eflags);	/* 割り込み許可フラグを元に戻す */
  return;
}

void init_screen8(char *vram, int x, int y) {
  boxfill8(vram, x, COL8_008484,  0,     0,      x -  1, y - 29);
  boxfill8(vram, x, COL8_C6C6C6,  0,     y - 28, x -  1, y - 28);
  boxfill8(vram, x, COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
  boxfill8(vram, x, COL8_C6C6C6,  0,     y - 26, x -  1, y -  1);

  boxfill8(vram, x, COL8_FFFFFF,  3,     y - 24, 59,     y - 24);
  boxfill8(vram, x, COL8_FFFFFF,  2,     y - 24,  2,     y -  4);
  boxfill8(vram, x, COL8_848484,  3,     y -  4, 59,     y -  4);
  boxfill8(vram, x, COL8_848484, 59,     y - 23, 59,     y -  5);
  boxfill8(vram, x, COL8_000000,  2,     y -  3, 59,     y -  3);
  boxfill8(vram, x, COL8_000000, 60,     y - 24, 60,     y -  3);

  boxfill8(vram, x, COL8_848484, x - 47, y - 24, x -  4, y - 24);
  boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y -  4);
  boxfill8(vram, x, COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
  boxfill8(vram, x, COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);
  return;
}

void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1) {
  int x, y;
  for (y = y0; y <= y1; y++) {
    for (x = x0; x <= x1; x++)
      vram[y * xsize + x] = c;
  }
  return;
}

void init_mouse_cursor8(char *mouse, char bc) {
  int x, y;

  for (y = 0; y < 16; y++) {
    for (x = 0; x < 16; x++) {
      if (cursor[y][x] == '*') {
        mouse[y * 16 + x] = COL8_000000;
      }
      if (cursor[y][x] == 'O') {
        mouse[y * 16 + x] = COL8_FFFFFF;
      }
      if (cursor[y][x] == '.') {
        mouse[y * 16 + x] = bc;
      }
    }
  }
  return;
}

void putblock8_8(char *vram, int vxsize, int pxsize,
                 int pysize, int px0, int py0, char *buf, int bxsize)
{
  int x, y;
  for (y = 0; y < pysize; y++) {
    for (x = 0; x < pxsize; x++) {
      vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
    }
  }
  return;
}

void putfont8(char *vram, int xsize, int x, int y, char c, char *font) {
  int i;
  char *p, d /* data */;
  for (i = 0; i < 16; i++) {
    p = vram + (y + i) * xsize + x;
    d = font[i];
    if ((d & 0x80) != 0) { p[0] = c; }
    if ((d & 0x40) != 0) { p[1] = c; }
    if ((d & 0x20) != 0) { p[2] = c; }
    if ((d & 0x10) != 0) { p[3] = c; }
    if ((d & 0x08) != 0) { p[4] = c; }
    if ((d & 0x04) != 0) { p[5] = c; }
    if ((d & 0x02) != 0) { p[6] = c; }
    if ((d & 0x01) != 0) { p[7] = c; }
  }
  return;
}

void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s) {
  for (; *s != 0x00; s++) {
    putfont8(vram, xsize, x, y, c, (char*)(hankaku + *s * 16));
    x += 8;
  }
  return;
}

void init_gdtidt(void)
{
  struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
  struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) 0x0026f800;
  int i;

  /* GDT initailize */
  for (i = 0; i < 8192; i++) {
    set_segmdesc(gdt + i, 0, 0, 0);
  }
  set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, 0x4092);
  set_segmdesc(gdt + 2, 0x0007ffff, 0x00280000, 0x409a);
  _load_gdtr(0xffff, 0x00270000);

  /* IDT initailize */
  for (i = 0; i < 256; i++) {
    set_gatedesc(idt + i, 0, 0, 0);
  }
  _load_idtr(0x7ff, 0x0026f800);

  return;
}

void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar)
{
  if (limit > 0xfffff) {
    ar |= 0x8000; /* G_bit = 1 */
    limit /= 0x1000;
  }
  sd->limit_low    = limit & 0xffff;
  sd->base_low     = base & 0xffff;
  sd->base_mid     = (base >> 16) & 0xff;
  sd->access_right = ar & 0xff;
  sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
  sd->base_high    = (base >> 24) & 0xff;
  return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar)
{
  gd->offset_low   = offset & 0xffff;
  gd->selector     = selector;
  gd->dw_count     = (ar >> 8) & 0xff;
  gd->access_right = ar & 0xff;
  gd->offset_high  = (offset >> 16) & 0xffff;
  return;
}
