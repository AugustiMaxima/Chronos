#ifndef TRANSMIT_BUFFER
#define TRANSMIT_BUFFER
/**
 * A ring buffer heavily optimized for tranmission
 * features better memory utilization and io optimized api
*/

#define TRANSMIT_BUFFER_SIZE 256


typedef struct transmit_Queue{
    int cursor;
    int length;
    char buffer[TRANSMIT_BUFFER_SIZE];
} TransmitBuffer;

void initializeTransmitBuffer(TransmitBuffer* transmitBuffer);

//Returns:
// >=0: # of characters copied
// -1: error/unavailable resources

int fillBuffer(TransmitBuffer* buffer, char* source, int length);

int fetchBuffer(TransmitBuffer* buffer, char* dest, int length);

int getBufferFill(TransmitBuffer* buffer);

int getBufferCapacity(TransmitBuffer* buffer);

//Fetches all the way up to the latest from a given offset, does not modify the READ/consumed cursor
int glean(TransmitBuffer* buffer, char* dest, int offset, int maxlen);

// -2: Reached max length but found no delimiter

int readUntilDelimiter(TransmitBuffer* buffer, char* dest, int maxlen, char delimiter);

#endif