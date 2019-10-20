
#include <charay.h>
#include <bwio.h>

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
        } else{
            return result;
        }
        result *= base;
        result += digit;
    }
    return result;
}

// https://stackoverflow.com/questions/34873209/implementation-of-strcmp
int strcmp(const char* s1, const char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
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

int strncpy(const char* source, char* dest, int length){
    int i;
    for(i=0;i<length && source[i]; i++){
        dest[i] = source[i];
    }
    dest[i] = 0;
    return i;
}

int strlen(const char* str){
    int i;
    for(i=0;str[i];i++);
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

// https://code.woboq.org/userspace/glibc/string/test-strlen.c.htmlsize_t
int chos_strlen (const char *s) {
  const char *p;
  for (p = s; *p; ++p);
  return p - s;
}
