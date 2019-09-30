#include <kern.h>
#include <bwio.h>
#include <dump.h>

#include <charay.h>

// charray.c

int noneZeroIntString(char* writeBuffer, int buflen, int num, int base){
    int remainder = num;
    int divider = 1;
    int cursor = 0;
    int length = 1;
    buflen--;
    while(remainder /= base) {
	    divider *= base;
	    length++;
    }
    remainder = num;
    int digit;
    while(length-- && buflen--){
        digit = remainder/divider;
	    remainder %= divider;
        divider /= base;
	    writeBuffer[cursor++] = digit + (digit < 10? '0':('a' - 10));
    }
    writeBuffer[cursor] = 0;
    return cursor;
}

int stringToNum(char* strint, int base){
    int i;
    int result = 0;
    for(i=0;strint[i];i++){
        int digit;
        char dig = strint[i];
        if(dig<='9' && dig>='0'){
            digit = dig - '0';
        } else if (dig<='f' && dig>='a'){
            digit = dig - 'a' + 10;
        }
        result *= base;
        result += digit;
    }
    return result;
}

int strlcmp(char* str1, char* str2){
    int i=0;
    for(i=0;str1[i]&&str2[i];i++){
        if(str1[i]!=str2[i]){
            return str1[i] - str2[i];
        }
    }
    if(str1[i]){
        return -1;
    }
    else if(str2[i]){
        return 1;
    }
    return 0;
}

int getCharMap(char a){
    int offset = 0;
    if(a>='0' && a<='9'){
        return a-'0';
    }
    offset += 10;
    if(a>='a' && a<='z'){
        return a-'a' + offset;
    } 
    offset += 26;
    if(a>='A' && a<='Z'){
        return a -'A' + offset;
    }
    offset += 26;
    if(a=='-'){
        return offset + 1;
    }
    offset += 1;
    if(a=='_'){
        return offset + 1;
    }
    offset += 1;
    if(a==':'){
        return offset + 1;
    }
    offset += 1;
    if(a=='='){
        return offset + 1;
    }
    offset += 1;
    if(a=='!'){
        return offset + 1;
    }
    offset += 1;
    if(a=='$'){
        return offset + 1;
    }
    offset += 1;
    if(a=='@'){
        return offset + 1;
    }
    offset += 1;    
    if(a=='*'){
        return offset + 1;
    }
    offset += 1;
    return offset;
}

//AlphaNumeric Hash:
//Accepts 0-9, a-z, A-Z, -, _, :, =, !
//Range:   10 + 26 + 26 + 5 = 67
int alphaNumericHash(char* name){
    int i, hash = 0;
    int base = getCharMap(' ');
    for(i=0;name[i];i++){
        hash*=base;
        hash+=getCharMap(name[i]);
    }
    return hash;
}

void split(char** tokens, int length, char* base, char divider){
    int i;
    int chunks = 0;
    tokens[chunks++] = base;
    for(i=0; base[i] && chunks<length; i++){
        if(base[i]==divider){
            tokens[chunks++] = base + i + 1;
        }
    }
}

int strncpy(char* source, char* dest, int length){
    int i;
    for(i=0;i<length && source[i]; i++){
        dest[i] = source[i];
    }
    dest[i] = 0;
    return i;
}

void formatStrn(char* result, int length, char* format, ...){
    int fi;
    int i = 0;
    int mode = 0;
    va_list varag;
    va_start(varag, format);
    /**
     * Minidocumentation:
     * 0: Literal mode: Takes char from formatter
     * 1: Decimal int mode: Grabs token from varag and serialize
     * 2: Hex mode: Serialize
     * 3: String
     * 4: Prime : The next character will indicate mode, activated with %
    **/
    for(fi=0; format[fi]&& i<length - 1;){
        char operand = format[fi++];
        if(operand=='%' && mode==0){
            mode = 4;
        } else {
            int op = 0;
            char* ops = 0;
            if(mode == 4){
                if(operand == '%'){
                    result[i++] = operand;
                } else if (operand == 'd') {
                    op = va_arg(varag, int);
                    i += noneZeroIntString(result + i, length - i, op, 10);
                } else if (operand == 'x') {
                    op = va_arg(varag, int);
                    i += noneZeroIntString(result + i, length - i, op, 16);
                } else if (operand == 's') {
                    ops = va_arg(varag, char*);
                    i += strncpy(ops, result + i, length - i);
                }
                result[i] = ' ';
                mode = 0;
            } else {
                result[i++] = operand;
            }
        }
    }
    va_end(varag);
    result[i] = 0;
}

// end charray.c

void exitKernel(void* processStackPtr){
    // save kernel registers on kernel stack
    asm("SUB SP, SP, #64");
    asm("STR R0, [SP]");
    asm("STR R1, [SP, #4]");
    asm("STR R2, [SP, #8]");
    asm("STR R3, [SP, #12]");
    asm("STR R4, [SP, #16]");
    asm("STR R5, [SP, #20]");
    asm("STR R6, [SP, #24]");
    asm("STR R7, [SP, #28]");
    asm("STR R8, [SP, #32]");
    asm("STR R9, [SP, #36]");
    asm("STR R10, [SP, #40]");
    asm("STR R11, [SP, #44]");
    asm("STR R12, [SP, #48]");
    asm("STR R13, [SP, #52]");
    asm("STR R14, [SP, #56]");
    asm("ADD R2, PC, #80");
    //sets where the return address should be
    asm("STR R2, [SP, #60]");

    asm("ldr r1, [r0]");
    asm("msr cpsr_c, r1");

    //restores the stack pointer, minus the cpsr
    // sp <- r0 - 4
    asm("add sp, r0, #4");

    //loads user mode registers
    //includes the PC register, and starts executing

    asm("LDR R0, [SP]");
    asm("LDR R1, [SP, #4]");
    asm("LDR R2, [SP, #8]");
    asm("LDR R3, [SP, #12]");
    asm("LDR R4, [SP, #16]");
    asm("LDR R5, [SP, #20]");
    asm("LDR R6, [SP, #24]");
    asm("LDR R7, [SP, #28]");
    asm("LDR R8, [SP, #32]");
    asm("LDR R9, [SP, #36]");
    asm("LDR R10, [SP, #40]");
    asm("LDR R11, [SP, #44]");
    asm("LDR R12, [SP, #48]");
    asm("LDR R13, [SP, #52]");
    asm("LDR R14, [SP, #56]");
    asm("ADD SP, SP, #64");
    asm("LDR R15, [SP, #-4]");

    // bwprintf(COM2, "end of exitKernel\r\n");
}

void __attribute__((naked)) enterKernel(){
    // bwprintf(COM2, "entering Kernel\r\n");
    asm("LDR R0, [SP]");
    asm("LDR R1, [SP, #4]");
    asm("LDR R2, [SP, #8]");
    asm("LDR R3, [SP, #12]");
    asm("LDR R4, [SP, #16]");
    asm("LDR R5, [SP, #20]");
    asm("LDR R6, [SP, #24]");
    asm("LDR R7, [SP, #28]");
    asm("LDR R8, [SP, #32]");
    asm("LDR R9, [SP, #36]");
    asm("LDR R10, [SP, #40]");
    asm("LDR R11, [SP, #44]");
    asm("LDR R12, [SP, #48]");
    asm("LDR R13, [SP, #52]");
    asm("LDR R14, [SP, #56]");
    asm("ADD SP, SP, #64");
    asm("LDR R15, [SP, #-4]");
}
