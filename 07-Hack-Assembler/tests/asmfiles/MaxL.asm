//Thisfileispartofwww.nand2tetris.org
//andthebook"TheElementsofComputingSystems"
//byNisanandSchocken,MITPress.
//Filename:projects/6/max/MaxL.asm

//Symbol-lessversionoftheMax.asmprogram.
//Designedfortestingthebasicversionoftheassembler.

@0
D=M
@1
D=D-M
@10
D;JGT
@1
D=M
@12
0;JMP
@0
D=M
@2
M=D
@14
0;JMP