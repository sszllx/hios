bits    16
        
BOTPAK	EQU		0x00280000		; bootpackのロード先
DSKCAC	EQU		0x00100000		; ディスクキャッシュの場所
DSKCAC0	EQU		0x00008000		; ディスクキャッシュの場所（リアルモード）

; BOOT_INFO関係
CYLS	EQU		0x0ff0			; ブートセクタが設定する
LEDS	EQU		0x0ff1
VMODE	EQU		0x0ff2			; 色数に関する情報。何ビットカラーか？
SCRNX	EQU		0x0ff4			; 解像度のX
SCRNY	EQU		0x0ff6			; 解像度のY
VRAM	EQU		0x0ff8			; グラフィックバッファの開始番地
        
        ;;; 对org的理解还是不够
        org	0x500
        jmp     main
        
%include "stdio.inc"
%include "A20.inc"
%include "common.inc"
%include "Fat12.inc"
%include "Gdt.inc"

main:
        xor     ax, ax
        mov     ds, ax
        mov     es, ax
        mov	ax, 0x0000         ; stack begins at 0x9000-0xffff
	mov	ss, ax
	mov	sp, 0xFFFF
        
	mov	AL,0x13			; VGAグラフィックス、320x200x8bitカラー
	mov	AH,0x00
	int	0x10
	mov	BYTE [VMODE],8	; 画面モードをメモする（C言語が参照する）
	mov	WORD [SCRNX],320
	mov	WORD [SCRNY],200
	mov	DWORD [VRAM],0x000a0000

	mov	AH,0x02
	int	0x16 			; keyboard BIOS
	mov	[LEDS],AL

        ;;; enable A20
	call	_EnableA20
        ;;; enter protected mode        
	call 	InstallGDT

	call	LoadRoot
   	mov    	bx, 0
   	mov	bp, IMAGE_RMODE_BASE
   	mov 	si, ImageName
	call	LoadFile		; load our file
   	mov   	dword [ImageSize], ecx
	cmp	ax, 0
	je	EnterStage3
        jmp     $
;;; 需要容错处理

EnterStage3:
        mov	AL,0xff
 	out	0x21,AL
 	nop
 	out	0xa1,AL
;;; 	cli
        
	cli	                           ; clear interrupts	
	mov	eax, cr0                 ; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax

	jmp	CODE_DESC:Stage3                ; far jump to fix CS. Remember that the code selector is 0x8!

;;; Protected Mode

bits 32

%include "Paging.inc"

BadImage db "*** FATAL: Invalid or corrupt kernel image. Halting system.", 0
ImageSig db "ELF"

Stage3:
	
	;-------------------------------;
	;   Set registers				;
	;-------------------------------;

	mov	ax, 0x10		; set data segments to data selector (0x10)
	mov	ds, ax
 	mov	ss, ax
 	mov	fs, ax
 	mov	gs, ax
	mov	es, ax
 	mov	esp, 90000h		; stack begins from 90000h

	call	ClrScr32

        call	EnablePaging

CopyImage:
  	 mov	eax, dword [ImageSize]
  	 movzx	ebx, word [bpbBytesPerSector]
  	 mul	ebx
;;; 为什么要除4?
  	 mov	ebx, 4
  	 div	ebx
   	 cld
   	 mov    esi, IMAGE_RMODE_BASE
    	 mov	edi, IMAGE_PMODE_BASE
   	 mov	ecx, eax
   	 rep	movsd                   ; copy image to its protected mode address

TestImage:
   	mov   	ebx, IMAGE_PMODE_BASE
	add	ebx, 1
;;;   	  and    ebx, 0xffffff00  ; ebx now points to file sig (PE00)
;;;   	  mov    esi, ebx
;;;   	  mov    edi, ImageSig
;;;   	  cmpsw
	mov	ecx, 3
	mov	esi, ebx
	mov	edi, ImageSig
	rep cmpsb
  	je     EXECUTE
  	mov	ebx, BadImage
  	call	Puts32
  	cli
  	hlt

;;; execute kernel
EXECUTE:
         mov     eax, IMAGE_PMODE_BASE + 0x1000
;;;         jmp     IMAGE_PMODE_BASE + 0x400
        mov     ebp, eax
        call    ebp
