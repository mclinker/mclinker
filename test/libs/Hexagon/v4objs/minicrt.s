        .section .start, "awx", @progbits
        .p2align 3
        .subsection 0
	.type event_handle_nmi, @function
event_handle_nmi:
		jumpr r28
	.size event_handle_nmi, . - event_handle_nmi

	.type event_handle_reset, @function
event_handle_reset:
		jumpr r28
	.size event_handle_reset, . - event_handle_reset

	.text

        .subsection 0

/* Next we have the event vectors */
/* See the HEXAGON Architecture System-Level Specification  */
/* for more information.*/

  .p2align 14

	.type .EventVectors, @function

.EventVectors:
	jump event_handle_reset
	jump event_handle_nmi
