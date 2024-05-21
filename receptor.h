#ifndef EMISOR_H
#define EMISOR_H

#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <stdlib.h>

#define DELAY_PIN 0
#define TX_PIN 2
#define BYTE unsigned char
#define LEN 31

struct frame
{
    BYTE cmd;   //3 bits  0x07
    BYTE len;   //5 bits    0x1F
    BYTE data[LEN]; //32 bytes
    BYTE FCS; //136 bits 1 BYTE + 1bit + 7relleno 0x01 + 0xFF
};

void empaquetar(BYTE *buff, frame morsa);
int fcs(BYTE* buff, int len);
frame desempaquetar(BYTE *buff);

void mensaje_de_prueba(); //CMD 1
void enviar_mensaje();  //CMD 2
void buscar_archivo();  //CMD 3
void Imprimir_cont_mnsj(); //CMD 4
void crear_archivo(); //CMD 5
void listar_archivos_txt(); //CMD 6
void cerrar_receptor(); //CMD 7

//ENVIAR BYTES
void cb(BYTE* bytes, int len);
void startTransmission();   //PROBAR

void envia_bytes(BYTE* bytes, int len);

#endif //EMISOR_H