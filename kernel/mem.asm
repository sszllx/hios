
	global write_mem8
	
write_mem8:
		MOV	ECX,[ESP+4]	
		MOV	AL,[ESP+8]	
		MOV	[ECX],AL
		RET