        extern  KernelMain
        extern  write_screen

	global _start
_start:
;;;         call    write_screen
   	call 	KernelMain
	jmp	$