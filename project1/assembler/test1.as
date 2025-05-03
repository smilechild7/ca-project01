start   lw  0 1 num1     ; reg1 = memory[num1]
        lw  0 2 num2     ; reg2 = memory[num2]
        add 1 2 3        ; reg3 = reg1 + reg2
        sw  0 3 result   ; memory[result] = reg3
        halt
num1    .fill 5
num2    .fill 6
result  .fill 0
