
%ifndef _COMMON_INC_INCLUDED
%define _COMMON_INC_INCLUDED

; where the kernel is to be loaded to in protected mode
;;; %define IMAGE_PMODE_BASE 0x100000
%define IMAGE_PMODE_BASE 0xC0000000
;;; %define IMAGE_PMODE_BASE 0x08048220



; where the kernel is to be loaded to in real mode
%define IMAGE_RMODE_BASE 0x3000

; kernel name
ImageName     db "KERNEL  BIN"

ImageSize     db 0

%endif
