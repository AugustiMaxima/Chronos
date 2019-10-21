#include <transmitBuffer.h>
#include <bwio.h>


int getPhysicalBufferIndex(int index){
    return index % TRANSMIT_BUFFER_SIZE;
}

void initializeTransmitBuffer(TransmitBuffer* transmitBuffer){
    transmitBuffer->cursor = 0;
    transmitBuffer->length = 0;
}

//Returns:
// >=0: # of characters copied
// -1: error/unavailable resources

int fillBuffer(TransmitBuffer* buffer, char* source, int length){
    if(length > getBufferCapacity(buffer)){
        return -1;
    }
    int i=0;
    for(i=0; i<length; i++){
        buffer->buffer[getPhysicalBufferIndex(buffer->length + i)] = source[i];
    }
    buffer->length += i;
    return i;
}

int fetchBuffer(TransmitBuffer* buffer, char* dest, int length){
    if(length > TRANSMIT_BUFFER_SIZE){
        return -2;
    }
    if(length > getBufferFill(buffer)){
        return -1;
    }
    int i;
    for(i=0;i<length;i++){
        dest[i] = buffer->buffer[getPhysicalBufferIndex(buffer->cursor + i)];
    }
    buffer->cursor += i;
    return i;
}

int getBufferFill(TransmitBuffer* buffer){
    return buffer->length - buffer->cursor;
}

int getBufferCapacity(TransmitBuffer* buffer){
    return TRANSMIT_BUFFER_SIZE - getBufferFill(buffer);

}

//Fetches all the way up to the latest from a given offset, does not modify the READ/consumed cursor
int glean(TransmitBuffer* buffer, char* dest, int offset, int maxlen){
    int signalen = buffer->length - offset;
    if(signalen > TRANSMIT_BUFFER_SIZE){
        //indicates character loss
        return -1;
    }
    int i;
    int length = maxlen < signalen ? maxlen : signalen; 
    for(i=0; i<length; i++){
        dest[i] = buffer->buffer[getPhysicalBufferIndex(offset+i)];
    }
    return i;
}

//don't set maxlen greater than TRANSMIT_BUFFER_SIZE
//if you do you won't ever get the chance to realize that your delimiter isn't in the buffer
int readUntilDelimiter(TransmitBuffer* buffer, char* dest, int maxlen, char delimiter){
    int signalen = getBufferFill(buffer);
    int length = signalen < maxlen? signalen : maxlen;
    int delimiterFound = 0;
    int i;
    for(i=0;i<length;i++){
        dest[i] = buffer->buffer[getPhysicalBufferIndex(buffer->cursor+i)];
        if(dest[i] == delimiter){
            delimiterFound++;
            break;
        }
    }
    if(delimiterFound){
        buffer->cursor+=i;
        return i;
    } else if(i==maxlen){
        //exhausted maxlen, this request wont ever succeed ever
        return -2;
    }
    //no delimter (yet) found
    return -1;
}
