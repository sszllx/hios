
void write_mem8(int addr, int data);

void write_screen() {
  int i = 0;
  for (i = 0xa0000; i <= 0xaffff; i++)
    write_mem8 (i, i & 0xf);
}
