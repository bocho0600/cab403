#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* Simulated UART hardware register */
#define UART_DATA_REGISTER (*(volatile uint8_t *)0x40013800)

/* UART status register for checking if UART is ready to transmit */
#define UART_STATUS_REGISTER (*(volatile uint8_t *)0x40013804)
#define UART_TX_READY (1 << 7) // Assuming the 7th bit represents TX ready

/* Function to check if UART is ready to transmit */
int uart_transmit_ready() {
    return (UART_STATUS_REGISTER & UART_TX_READY); // Check if TX ready bit is set
}

/* Custom function to send a single character via UART */
void uart_putc(char c) {
    while (!uart_transmit_ready()); // Wait until UART is ready
    UART_DATA_REGISTER = c;         // Write character to UART data register
}

/* Custom printf function using UART for output */
void uart_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[128];
    vsnprintf(buffer, sizeof(buffer), format, args); // Safely format the string
    va_end(args);

    // Send each character in the buffer to UART
    for (int i = 0; buffer[i] != '\0'; i++) {
        uart_putc(buffer[i]);
    }
}

/* Example application using custom UART printf */
int main(void) {
    /* Sample data to print */
    int temperature = 25;
    float voltage = 3.3;
    const char *status = "OK";

    // Print values via UART
    uart_printf("System Booting...\n");
    uart_printf("Temperature: %d°C\n", temperature);
    uart_printf("Voltage: %.2fV\n", voltage);
    uart_printf("Status: %s\n", status);

    /* Loop forever, sending a message periodically (in a real embedded system) */
    while (1) {
        uart_printf("Heartbeat: System is alive...\n");
        // Add a delay or sleep based on your system's timing requirements
    }

    return 0;
}
