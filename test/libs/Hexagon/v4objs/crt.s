        .section .start, "awx", @progbits
        .p2align 3
        .subsection 0
	.type event_handle_temp, @function
event_handle_temp:
		jumpr r28
	.size event_handle_temp, . - event_handle_temp
