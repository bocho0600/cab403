#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define MAX_FLOOR_NAME 4
#define MAX_MESSAGE_LENGTH 256

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char current_floor[MAX_FLOOR_NAME];
    char destination_floor[MAX_FLOOR_NAME];
    char status[8];
    uint8_t open_button;
    uint8_t close_button;
    uint8_t door_obstruction;
    uint8_t overload;
    uint8_t emergency_stop;
    uint8_t individual_service_mode;
    uint8_t emergency_mode;
} car_shared_mem;

car_shared_mem *shm;
int shm_fd;
char car_name[16];
int delay_ms;
int timer = 0;
int controller_fd;
int delay_flag = 0;
char lowest_floor[MAX_FLOOR_NAME];
char highest_floor[MAX_FLOOR_NAME];

void* handle_controller_messages(void* arg);
void cleanup(void);
void signal_handler(int signum);
void init_shared_memory(void);
void* connect_to_controller(void* arg);
// void connect_to_controller(void);
void send_message(int sock, const char* message);
char* receive_message(int sock);
void elevator_operation(void);
int compare_floors(const char* floor1, const char* floor2);
void opening_doors(void);
void opening_to_open(void);
void close_doors(void);
void move_to_floor(const char* destination);
void update_status(void);

void cleanup(void) {
    if (shm) {
        pthread_mutex_destroy(&shm->mutex);
        pthread_cond_destroy(&shm->cond);
        munmap(shm, sizeof(car_shared_mem));
    }
    if (shm_fd != -1) close(shm_fd);
    if (controller_fd != -1) close(controller_fd);
    char shm_name[32];
    snprintf(shm_name, sizeof(shm_name), "/car%s", car_name);
    shm_unlink(shm_name);
}

void signal_handler(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        printf("Received signal %d. Cleaning up and exiting.\n", signum);
        cleanup();
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <name> <lowest_floor> <highest_floor> <delay>\n", argv[0]);
        exit(1);
    } else {
        // Print the usage message
        printf("Usage %s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);
    }

    strncpy(car_name, argv[1], sizeof(car_name) - 1);
    strncpy(lowest_floor, argv[2], MAX_FLOOR_NAME - 1);
    strncpy(highest_floor, argv[3], MAX_FLOOR_NAME - 1);
    delay_ms = atoi(argv[4]);

    // Initialize shared memory
    init_shared_memory();

    // Initialize the shared memory structure
    pthread_mutex_lock(&shm->mutex);
    strncpy(shm->current_floor, lowest_floor, MAX_FLOOR_NAME - 1);
    strncpy(shm->destination_floor, lowest_floor, MAX_FLOOR_NAME - 1);
    strcpy(shm->status, "Closed");
    memset((char*)shm + offsetof(car_shared_mem, open_button), 0, 
           sizeof(car_shared_mem) - offsetof(car_shared_mem, open_button));
    pthread_mutex_unlock(&shm->mutex);

    // Set up signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Connect to controller
    pthread_t controller_thread;
    pthread_create(&controller_thread, NULL, connect_to_controller, NULL);

    // update_status();

    // Main elevator operation loop
    printf("Start elevator operation\n");
    elevator_operation();

    pthread_join(controller_thread, NULL);
    // This point should never be reached under normal operation
    cleanup();
    return 0;
}

void init_shared_memory() {
    char shm_name[32];
    snprintf(shm_name, sizeof(shm_name), "/car%s", car_name);
    
    shm_fd = shm_open(shm_name, O_RDWR | O_CREAT, 0600);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    if (ftruncate(shm_fd, sizeof(car_shared_mem)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    shm = mmap(NULL, sizeof(car_shared_mem), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    pthread_mutex_init(&shm->mutex, NULL);
    pthread_cond_init(&shm->cond, NULL);
}

void* connect_to_controller(void* arg) {
    struct sockaddr_in server_addr;
    while (1) {
        // Create a TCP socket
        if ((controller_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("socket");
            exit(1);
        }

        int opt = 1;
        if (setsockopt(controller_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            perror("setsockopt");
            exit(1);
        }

        // Set up the server address structure
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(3000);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        // Connect to the server
        if (connect(controller_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("connect");
            close(controller_fd);
            controller_fd = -1;
            usleep(delay_ms * 1000);
            continue;
        }

        // Send initial CAR message to controller
        char init_message[MAX_MESSAGE_LENGTH];
        snprintf(init_message, sizeof(init_message), "CAR %s %s %s", car_name, lowest_floor, highest_floor);
        send_message(controller_fd, init_message);
        update_status();

        pthread_t message_thread;       
        pthread_create(&message_thread, NULL, handle_controller_messages, NULL);
        pthread_detach(message_thread);

        break;
    }
    return NULL;
}

void send_message(int sock, const char* message) {
    if (sock < 0) {
        fprintf(stderr, "Invalid socket descriptor\n");
        return;
    }

    uint32_t length = htonl(strlen(message));
    if (send(sock, &length, sizeof(length), 0) < 0) {
        perror("send");
        return;
    }
    if (send(sock, message, strlen(message), 0) < 0) {
        perror("send");
    }
    // printf("Sent initial message to controller: %s\n", message);
}
char* receive_message(int sock) {
    uint32_t length;
    ssize_t bytes_received = recv(sock, &length, sizeof(length), 0);
    if (bytes_received <= 0) {
        return NULL;
    }
    
    length = ntohl(length);
    char* buffer = malloc(length + 1);
    if (!buffer) {
        perror("malloc");
        return NULL;
    }
    
    bytes_received = recv(sock, buffer, length, 0);
    if (bytes_received <= 0) {
        free(buffer);
        return NULL;
    }
    
    buffer[length] = '\0';
    return buffer;
}

void elevator_operation() {
    int open_count = 0;
    printf("delay_ms: %d\n", delay_ms);
    while ( 1 ) {
        if (strcmp(shm-> status, "Open") == 0 && open_count < delay_ms && !shm->close_button) {
            // Wait for a while before closing the doors
            open_count += 1;
            // printf("Counting open time: %d\n", open_count);
        } else if ((strcmp(shm->status, "Open") == 0 && open_count >= delay_ms) || shm->close_button) {
            printf("Door opened a while, Closing doors, time: %d\n", timer);
            close_doors();
            open_count = 0;
        }
        if (strcmp(shm->status, "Opening") == 0) {
            opening_to_open();
            printf("Opening to Open, time: %d\n", timer);
        }
        // Check for manual controls
        if (shm->close_button && strcmp(shm->status, "Open") != 0) {
            printf("Close Btn pressed, Closing doors, time: %d\n", timer);
            close_doors();
        }
        if (shm->open_button && strcmp(shm->status, "Closed") == 0) {
            printf("Open Btn pressed, Opening doors, time: %d\n", timer);
            open_count = 0;
            opening_doors();
        } 

        // Check for emergency stop
        if (shm->emergency_stop) {
            printf("Stopping\n");
            strcpy(shm->status, "Stopped");
            shm->emergency_mode = 1;
        }

        // Move to destination if not in emergency mode
        if (!shm->emergency_mode && strcmp(shm->current_floor, shm->destination_floor) != 0) {
            move_to_floor(shm->destination_floor);
        }


        if (delay_flag == 1) {
            usleep(delay_ms * 1000);
            timer += delay_ms;
            delay_flag = 0;
        } else {
            usleep(1000);
            timer += 1;
        }
    }
}

int compare_floors(const char* floor1, const char* floor2) {
    int f1 = 0, f2 = 0;
    
    if (floor1[0] == 'B') f1 = -atoi(floor1 + 1);
    else f1 = atoi(floor1);
    
    if (floor2[0] == 'B') f2 = -atoi(floor2 + 1);
    else f2 = atoi(floor2);
    
    return f1 - f2; // Destination - Current floor,  negative if current floor is higher ->  move down
}

void opening_doors() {
    pthread_mutex_lock(&shm->mutex); // Lock the mutex
    shm->open_button = 0;
    // printf("Opening, time: %d\n", timer);
    strcpy(shm->status, "Opening");
    pthread_cond_broadcast(&shm->cond);
    pthread_mutex_unlock(&shm->mutex); // Unlock the mutex before sleeping
    
    update_status();
    delay_flag = 1;
}

void opening_to_open() {
    // printf("Open, and close after delay_ms, time: %d\n", timer); 
    pthread_mutex_lock(&shm->mutex); // Lock the mutex again before modifying the status
    shm->open_button = 0;
    strcpy(shm->status, "Open");
    pthread_cond_broadcast(&shm->cond);
    pthread_mutex_unlock(&shm->mutex); // Unlock the mutex after modifications
    update_status();
}

void close_doors() {
    pthread_mutex_lock(&shm->mutex); // Lock the mutex
    shm->close_button = 0;

    if (shm->door_obstruction) {
        pthread_cond_broadcast(&shm->cond);
        pthread_mutex_unlock(&shm->mutex); // Unlock the mutex before returning
        opening_doors();
        return;
    }

    // printf("Closing status, time: %d\n", timer);
    strcpy(shm->status, "Closing");
    pthread_cond_broadcast(&shm->cond);
    pthread_mutex_unlock(&shm->mutex); // Unlock the mutex before sleeping
    update_status();

    usleep(delay_ms * 1000);
    timer += delay_ms;

    pthread_mutex_lock(&shm->mutex); // Lock the mutex again before modifying the status
    // printf("Closed status, time: %d\n", timer);
    strcpy(shm->status, "Closed");
    pthread_cond_broadcast(&shm->cond);
    pthread_mutex_unlock(&shm->mutex); // Unlock the mutex after modifications
    
    delay_flag = 1;
    update_status();
}

void move_to_floor(const char* destination) {
    int lowest = 0;
    int highest = 0;
    int floor;
    int destination_floor = 0;
    char current[MAX_FLOOR_NAME];

    // Check if the destination is within the limits
    if ( lowest_floor[0] == 'B')
    {
        lowest = -atoi(lowest_floor);
    } else {
        lowest = atoi(lowest_floor);
    }
    if ( highest_floor[0] == 'B')
    {
        highest = -atoi(highest_floor);
    } else {
        highest = atoi(highest_floor);
    }
    if (destination[0] == 'B')
    {
        destination_floor = -atoi(destination);
    } else {
        destination_floor = atoi(destination);
    }
    // printf("Moving: %s to %s, ", shm->current_floor, destination);
    // printf("Limit: %d to %d, destination: %d\n", lowest, highest, destination_floor);
    if (destination_floor < lowest || destination_floor > highest) {
        // printf("Destination exceed limit\n");
        pthread_mutex_lock(&shm->mutex); // Lock the mutex
        strcpy(shm->destination_floor, shm->current_floor); 
        pthread_cond_broadcast(&shm->cond);
        pthread_mutex_unlock(&shm->mutex); // Unlock the mutex before sleeping
        return;
    } else {
        delay_flag = 1;
        // move to destination
        if (strcmp(shm->status, "Between") != 0){
            // printf("Start moving\n");
            pthread_mutex_lock(&shm->mutex); // Lock the mutex
            strcpy(shm->status, "Between");
            pthread_cond_broadcast(&shm->cond);
            pthread_mutex_unlock(&shm->mutex); // Unlock the mutex before sleeping
        } else {
            // Simulate movement
            // usleep(delay_ms * 1000);
            // timer += delay_ms;
            char* current = shm->current_floor;
            printf("Current floor: %s\n", current);
            pthread_mutex_lock(&shm->mutex); // Lock the mutex again before modifying the status
            if (compare_floors(destination, shm->current_floor) > 0) {
                printf("Moving up\n");
                
                if (current[0] == 'B') {
                    floor = -atoi(current + 1) + 1;
                    if (floor == 0) {
                        floor = 1;
                    }
                } else {
                    floor = atoi(current) + 1;
                }

            } else if (compare_floors(destination, shm->current_floor) < 0) {
                printf("Moving down\n");
                if (shm->current_floor[0] == 'B') { // Neg floor
                    floor = -atoi(current + 1) - 1;
                } else {
                    floor = atoi(current) - 1;
                    if (floor == 0) {
                        floor = -1;
                    }
                }
            } 
            if (floor < 0) {
                sprintf(current, "B%d", abs(floor));
            } else {
                sprintf(current, "%d", floor);
            }
            strcpy(shm->current_floor, current);
            pthread_cond_broadcast(&shm->cond);
            pthread_mutex_unlock(&shm->mutex); // Unlock the mutex after modifications
            if (compare_floors(destination, shm->current_floor) == 0) {
                // Already at destination
                pthread_mutex_lock(&shm->mutex); // Lock the mutex again before modifying the status
                // strcpy(shm->current_floor, destination);
                if (shm->individual_service_mode) {
                    strcpy(shm->status, "Closed");
                } else {
                    strcpy(shm->status, "Opening");
                }
                pthread_cond_broadcast(&shm->cond);
                pthread_mutex_unlock(&shm->mutex); // Unlock the mutex after modifications
                delay_flag = 0;
            } 
        }
        update_status();
    }
}

void update_status() {
    char status_message[64];
    snprintf(status_message, sizeof(status_message), "STATUS %s %s %s", shm->status, shm->current_floor, shm->destination_floor);
    printf("%s\n", status_message);
    send_message(controller_fd, status_message);
}

void* handle_controller_messages(void* arg) {
    while (1) {
        char* message = receive_message(controller_fd);
        if (message == NULL) {
            // Connection lost, try to reconnect
            close(controller_fd);
            sleep(delay_ms / 1000);  // Wait before attempting to reconnect
            controller_fd = -1;
            pthread_t controller_thread;
            pthread_create(&controller_thread, NULL, connect_to_controller, NULL);
            pthread_detach(controller_thread);
            continue;
        } else {
            printf("Received message: %s\n", message);
        }

        if (strncmp(message, "FLOOR ", 6) == 0) {
            char new_destination[MAX_FLOOR_NAME];
            sscanf(message + 6, "%s", new_destination);
            
            if (strcmp(shm->destination_floor, new_destination) == 0) {
                printf("Destination already set to %s, Opening door\n", new_destination);
                opening_doors();
            } else
            {
                pthread_mutex_lock(&shm->mutex);
                printf("Update destination to %s\n", new_destination);
                strncpy(shm->destination_floor, new_destination, MAX_FLOOR_NAME - 1);
                pthread_cond_broadcast(&shm->cond);
                pthread_mutex_unlock(&shm->mutex);
            }

            
        }
        // Add handling for other types of messages if needed
    }
    return NULL;
}
