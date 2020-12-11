#include <stdio.h>
#include <unistd.h>    // Used for UART
#include <sys/fcntl.h> // Used for UART
#include <termios.h>   // Used for UART
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Define Constants
const char *uart_target = "/dev/ttyTHS1";
#define NSERIAL_CHAR 256
#define VMINX 1
#define BAUDRATE B9600


int fid = -1;
struct termios port_options; // Create the structure


#define SERIAL_RUNTIME_ERROR(...) {                                                                       \
    fprintf(stderr, "A runtime error related to the serial connection '%s' has occured.\n", uart_target); \
    fprintf(stderr, "Encountered on %s:%d\n", __FILE__, __LINE__);                                        \
    fprintf(stderr, "In function %s\n", __func__);                                                        \
    fprintf(stderr, __VA_ARGS__);                                                                         \
}                                                                                                         \

int serial_init(){

    // SETUP SERIAL WORLD

    tcgetattr(fid, &port_options); // Get the current attributes of the Serial port

    //------------------------------------------------
    //  OPEN THE UART
    //------------------------------------------------
    // The flags (defined in fcntl.h):
    //	Access modes (use 1 of these):
    //		O_RDONLY - Open for reading only.
    //		O_RDWR   - Open for reading and writing.
    //		O_WRONLY - Open for writing only.
    //	    O_NDELAY / O_NONBLOCK (same function)
    //               - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
    //                 if there is no input immediately available (instead of blocking). Likewise, write requests can also return
    //				   immediately with a failure status if the output can't be written immediately.
    //                 Caution: VMIN and VTIME flags are ignored if O_NONBLOCK flag is set.
    //	    O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.fid = open("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode

    fid = open(uart_target, O_RDWR | O_NOCTTY);

    tcflush(fid, TCIFLUSH);
    tcflush(fid, TCIOFLUSH);

    if (fid == -1)
    {
        SERIAL_RUNTIME_ERROR("Error - Unable to open UART.  Ensure it is not in use by another application\n");
        return -1;
    }

    //------------------------------------------------
    // CONFIGURE THE UART
    //------------------------------------------------
    // flags defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html
    //	Baud rate:
    //         - B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200,
    //           B230400, B460800, B500000, B576000, B921600, B1000000, B1152000,
    //           B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
    //	CSIZE: - CS5, CS6, CS7, CS8
    //	CLOCAL - Ignore modem status lines
    //	CREAD  - Enable receiver
    //	IGNPAR = Ignore characters with parity errors
    //	ICRNL  - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
    //	PARENB - Parity enable
    //	PARODD - Odd parity (else even)

    port_options.c_cflag &= ~PARENB; // Disables the Parity Enable bit(PARENB),So No Parity
    port_options.c_cflag &= ~CSTOPB; // CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit
    port_options.c_cflag &= ~CSIZE;  // Clears the mask for setting the data size
    port_options.c_cflag |= CS8;     // Set the data bits = 8
    //port_options.c_cflag &= ~CRTSCTS;           // No Hardware flow Control
    port_options.c_cflag |= CREAD | CLOCAL;                  // Enable receiver,Ignore Modem Control lines
    port_options.c_iflag &= ~(IXON | IXOFF | IXANY);         // Disable XON/XOFF flow control both input & output
    port_options.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Non Cannonical mode
    port_options.c_oflag &= ~OPOST;                          // No Output Processing

    port_options.c_lflag = 0; //  enable raw input instead of canonical,

    port_options.c_cc[VMIN] = VMINX; // Read at least 1 character
    port_options.c_cc[VTIME] = 0;    // Wait indefinetly

    cfsetispeed(&port_options, BAUDRATE); // Set Read  Speed
    cfsetospeed(&port_options, BAUDRATE); // Set Write Speed

    // Set the attributes to the termios structure
    int att = tcsetattr(fid, TCSANOW, &port_options);

    if (att != 0)
    {
        SERIAL_RUNTIME_ERROR("Error in Setting port attributes\n");
        return -2;
    }

    // Flush Buffers
    tcflush(fid, TCIFLUSH);
    tcflush(fid, TCIOFLUSH);

    return 0;
}


int serial_write(char* message, size_t message_length){
    if(fid<0){
        SERIAL_RUNTIME_ERROR("Connection not initialized. fid = %d\n", fid);
        return -1;
    }
    return write(fid, &message, message_length); //Filestream, bytes to write, number of bytes to write
}

ssize_t serial_readln(char* destination, size_t max_message_length){
    
    if(fid<0){
        SERIAL_RUNTIME_ERROR("Connection not initialized. fid = %d\n", fid);
        return -1;
    }

    char* i = destination;
    char* end_of_message = destination+max_message_length;
    ssize_t size_read = 0;


    while(i<end_of_message){
        ssize_t read_result = read(fid, (void *)i, 1);
        if(i==destination && (isspace(*i) ||  iscntrl(*i))) continue; //Strip beginning whitespace and null characters
        if(read_result!=1) {
            SERIAL_RUNTIME_ERROR("Failed to read a character. read() returned %ld\n", read_result);
            return -1;
        }
        if(*i=='\n') break;
        if(*i=='\r') break;
        if(*i=='\0') break;
        i++;
        size_read++;
    }
    if(i!=end_of_message){
        i++;
        size_read++;
    }
    *i='\0';
    return size_read-1;
}

int serial_destroy(){
    return close(fid);
}


int main()
{
    printf("Hello World\n");
    
    char buff[1024];

    serial_init();

    printf("SERIAL Port Good to Go.\n");

    for(int i = 0; i < 100; i++){
        ssize_t message_length = serial_readln(buff, 1024);
        if(message_length<0){
            i--;
            continue;
        }
        printf("Message Received: %s\n", buff);
    }

    serial_destroy();

    printf("Goodbye World\n");
}