#define SERVICE_LENGTH 8

typedef struct
{
 char signature[2];
 unsigned char marker[2];
 unsigned int length;
} service_information;