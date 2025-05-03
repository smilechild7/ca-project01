8454149   // lw 0 1 5    => reg1 = 0x0F0F0F0F
8454214   // lw 0 2 6    => reg2 = 0xF0F0F0F0
720899    // nor 1 2 3   => reg3 = ~(reg1 | reg2) = 0x00000000
25165824  // halt
252645135 // .fill 0x0F0F0F0F
4042322160 // .fill 0xF0F0F0F0
