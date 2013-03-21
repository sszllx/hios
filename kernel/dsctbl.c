/* GDTÇ‚IDTÇ»Ç«ÇÃÅA descriptor table ä÷åW */

#include "bootpack.h"

struct SEGMENT_DESCRIPTOR _gdt[MAX_DESCRIPTORS];
struct gdtr _gdtr;

void init_gdtidt(void)
{
  //	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
  struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) (_gdt);
  struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) ADR_IDT;
  // 18 idt
  struct GATE_DESCRIPTOR idtVector[256];
  unsigned char idt_ptr[6];
  int i;

  /* GDTÇÃèâä˙âª */
  //  for (i = 0; i <= LIMIT_GDT / 8; i++) {
  /* for (i = 0; i <= 2; i++) { */
  /*   set_segmdesc(gdt + i, 0, 0, 0); */
  /* } */

  _gdtr.m_limit = (sizeof (struct SEGMENT_DESCRIPTOR) * MAX_DESCRIPTORS)-1;
  _gdtr.m_base = (unsigned int)&_gdt[0];
  
  set_segmdesc(_gdt, 0, 0, 0);
  set_segmdesc(_gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
  set_segmdesc(_gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
  _load_gdtr(_gdtr.m_limit, _gdtr.m_base);

  /* IDTÇÃèâä˙âª */
  /* for (i = 0; i <= LIMIT_IDT / 8; i++) { */
  /*   set_gatedesc(idtVector + i, 0, 0, 0); */
  /* } */
  _load_idtr(LIMIT_IDT, (int)(idt_ptr));
  unsigned short* p_idt_limit = (unsigned short*)(&idt_ptr[0]);
  unsigned int* p_idt_base  = (unsigned int*)(&idt_ptr[2]);
  *p_idt_limit = 256 * sizeof(struct GATE_DESCRIPTOR) - 1;
  *p_idt_base  = (unsigned int)&idtVector;

  /* IDTÇÃê›íË */
  set_gatedesc(idtVector + 0x20, (int) _asm_inthandler20, 2 * 8, AR_INTGATE32);
  set_gatedesc(idtVector + 0x21, (int) _asm_inthandler21, 2 * 8, AR_INTGATE32);
  set_gatedesc(idtVector + 0x27, (int) _asm_inthandler27, 2 * 8, AR_INTGATE32);
  set_gatedesc(idtVector + 0x2c, (int) _asm_inthandler2c, 2 * 8, AR_INTGATE32);

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
