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
    bwprintf(COM2, "%d %d %d\r\n", signalen, buffer->length, offset);
    int i;
    int length = maxlen < signalen ? maxlen : signalen; 
    for(i=0; i<length; i++){
        dest[i] = buffer->buffer[getPhysicalBufferIndex(offset+i)];
    }
    return i;
}
