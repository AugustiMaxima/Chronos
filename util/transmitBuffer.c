#include <transmitBuffer.h>

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
        buffer->buffer[buffer->length + i];
    }
    buffer->length += length;
    return i;
}

int fetchBuffer(TransmitBuffer* buffer, char* dest, int length){
    if(length > getBufferFill(buffer)){
        return -1;
    }
    int i;
    for(i=0;i<length;i++){
        dest[i] = buffer->buffer[buffer->cursor + i];
    }
    buffer->cursor += length;
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
    int length = maxlen > signalen ? maxlen : signalen; 
    for(i=0; i<length; i++){
        dest[i] = buffer->buffer[offset+i];
    }
    return i;
}
