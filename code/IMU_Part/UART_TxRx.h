#ifndef UART_TXRX_H
#define UART_TXRX_H
class UART_TR{
    public:
    static void UART_init(uint txpin,uint rxpin,uint baud);
    static void U_tx(char c[]);
    static char U_rx();
};
#endif