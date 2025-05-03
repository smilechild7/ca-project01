        lw      0       1       ptrSub  reg1 = 7
        lw      0       2       ptrRet  reg2 = 4
        jalr    1       2               reg2 = PC + 1 = 3, PC = reg1 = 7
        noop                 
retAdd  add     3       2       1       reg1 = reg3 + reg2 = 10 + 3 = 13
        sw      0       1       100     mem[100] = reg1 = 13
        beq     0       0       3       PC = 6 + 1 + 3 = 10
sub     lw      0       3       five    reg3 = 5
        add     3       3       3       reg3 = reg3 + reg3 = 10
        beq     0       0       -6      PC = 4 = 9 + 1 - 6
done    halt                                   
five    .fill   5 
ptrSub  .fill   7 
ptrRet  .fill   4     
