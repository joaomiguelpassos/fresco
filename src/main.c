#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct Message {
    uint8_t id;
    uint16_t size;
    uint8_t* payload;
    uint16_t crc;
};

struct DataPoint {
    uint8_t id;
    uint8_t type;
    union {
        uint8_t uint8_val;
        int16_t int16_val;
        char* string_val;
    } value;
};

struct Message parse_message(const uint8_t* data) {
    struct Message msg;
    msg.id = data[0];
    msg.size = *((uint16_t*)&data[1]);
    msg.payload = malloc(msg.size);
    memcpy(msg.payload, &data[3], msg.size);
    msg.crc = *((uint16_t*)&data[3 + msg.size]);
    return msg;
}

struct DataPoint parse_data_point(const uint8_t* payload) {
    struct DataPoint dp;
    dp.id = payload[0];
    dp.type = payload[1];
    
    switch (dp.type) {
        case 0x00:
            dp.value.uint8_val = payload[2];
            break;
        case 0x01:
            dp.value.int16_val = *((int16_t*)&payload[2]);
            break;
        case 0x02:
            dp.value.string_val = malloc(payload[2] + 1);
            memcpy(dp.value.string_val, &payload[3], payload[2]);
            dp.value.string_val[payload[2]] = '\0';
            break;
        default:
            // Handle unknown data point type
            break;
    }
    return dp;
}

void print_data_point(const struct DataPoint* dp) {
    switch (dp->type) {
        case 0x00:
            printf("DP_ID %d | DP_VALUE %u\n", dp->id, dp->value.uint8_val);
            break;
        case 0x01:
            printf("DP_ID %d | DP_VALUE %d\n", dp->id, dp->value.int16_val);
            break;
        case 0x02:
            printf("DP_ID %d | DP_VALUE %s\n", dp->id, dp->value.string_val);
            break;
        default:
            // Handle unknown data point type
            break;
    }
}

void handle_message(const struct Message* msg) {
    // Parse data point from payload
    struct DataPoint dp = parse_data_point(msg->payload);

    // Data Point ID out of bounds
    if (dp.id < 1 || dp.id > 3) {
        printf("Invalid Data Point ID\n");
        return;
    }
    // Print data point information
    print_data_point(&dp);

    // Free allocated memory
    if (dp.type == 0x02) {
        free(dp.value.string_val);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "bind") != 0) {
        fprintf(stderr, "Usage: %s bind <fifo_path>\n", argv[0]);
        return 1;
    }

    const char* fifoPath = argv[2];

    // Create the FIFO if it doesn't exist
    mkfifo(fifoPath, 0666);

    int fd = open(fifoPath, O_RDWR);
    if (fd == -1) {
        perror("Error opening FIFO");
        return 1;
    }

    while (1) {
        uint8_t buffer[512];
        ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
        if (bytes_read > 0) {
            struct Message msg = parse_message(buffer);
            handle_message(&msg);
            free(msg.payload);
        }
    }

    close(fd);
    return 0;
}
