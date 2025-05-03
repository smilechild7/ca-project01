        lw  0 1 a
        lw  0 2 b
loop    beq 1 2 done
        add 1 2 1
        beq 0 0 loop
done    halt
a       .fill 1
b       .fill 2
