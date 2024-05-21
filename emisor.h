#ifndef MODULO_H
#define MODULO_H

#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <stdlib.h>

#define DELAY_PIN 0
#define TX_PIN 2
#define BYTE unsigned char
#define LEN 31

struct frame {
    BYTE cmd;   //3 bits  0x07
    BYTE len;   //5 bits    0x1F
    BYTE data[LEN]; //32 bytes
    BYTE FCS; //136 bits 1 BYTE + 1bit + 7relleno 0x01 + 0xFF
};

void empaquetar(BYTE *buff, frame morsa);
int fcs(BYTE* buff, int len);
frame desempaquetar(BYTE *buff);

void mensaje_de_prueba();
void enviar_mensaje();
void buscar_archivo();
void Imprimir_cont_mnsj();
void crear_archivo();
void listar_archivos_txt();
void cerrar_receptor();

void cb(BYTE* bytes, int len);
void startTransmission();
void envia_bytes(BYTE* bytes, int len);

#endif