.extern W
lea W, r6
 inc r6
 mov r3, K
 sub r1, r4
 bne END
 cmp K, #-6
 bne %END
 dec W r3
 add L3, L3
END: stop
K: .data 31
.extern L3 