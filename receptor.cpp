#include <stdio.h>
#include <wiringPi.h>
#include <string.h>
#include <stdlib.h>
#include "receptor.h"

volatile int mensajes_recibidos=0;
volatile int errors = 0;
volatile int nbits = 0;
volatile int nbytes = 0;
bool transmissionStarted = false;
bool parity = 0;
int nones = 0;

bool parityError = 0;
char nombre[LEN]={0};
int caso=0;

bool p_10=false;
bool estado = true;

int msj=0;
int m_error=0;
int m_error_n_e=0;
int m_correcto=0;
bool error_data=false;
bool limpio=true;
bool seguridad=false;

frame aux;
int max_len=31; //

int main(){
    while(estado){
        BYTE buff[100]={0};
        recibir_bytes(buff);

        frame recuperado;
        recuperado=desempaquetar(buff);

        if ((seguridad==true && recuperado.cmd != 1))
        {
            recuperado.cmd=1;
            printf("Existio un error en mensajes de prueba, forzando cambio de cmd\n");
        }

        if(recuperado.cmd==1){
            msj++;
            seguridad=true;
        }else{
            if(parityError==true){
            printf("Error de paridad detectado\n");
            recuperado.cmd=0;
            parityError=false;
            errors++;
            }
            int x=recuperado.FCS;
            if(x != fcs(buff,(recuperado.len+1))){
                printf("Error fcs detectado\n");
                recuperado.cmd=0;
                errors++;
            }
            if((recuperado.len != '0') && (recuperado.cmd==4 | recuperado.cmd==6 | recuperado.cmd==7)){
                printf("Incongruencia len y cmd\n");
                recuperado.cmd=0;
                errors++;
            }
        }


        mensajes_recibidos++;
        switch (recuperado.cmd)
        {
        case 1: //Mensajes de prueba contar mensajes buenos, malos, m no encnotrados y mostrar %

            break;
        case 2: //Guardar mensaje recibido en mensaje.txt
            guardar_mensaje_txt(recuperado);
            break;
        case 3: //CMD 3 Buscar nombre de archivo TXT recibido, si esta abrirlo, si no informarlo
            buscar_nombre_archivo(recuperado, nombre);
            break;
        case 4: //CMD 4 mostrar mensajes recibidos(excepto los de prueba) y mostrar % de correctors y erroneos
            imprimir_pp_mensajes();
            break;
        case 5: //CMD 5 Crear un archivo con nombre recibido y registrar el siguiente mensaje recibido en ese archivo usar a+
            crear_archivo_guardar_mensaje(recuperado,nombre);
            break;
        case 6: //CMD 6 listar archivos txt de su carpeta
            listar_archivos_txt_carpeta();
            break;
        case 7: //CMD 7 cerrar el programa
            cerrar_receptor();
            break;
        default: //CMD 0 o erroneo
            printf("CMD no declarado, BUFF desechado\n");
            break;
        }
    }
    return 0;
}
void empaquetar(BYTE *buff, frame morsa){
    buff[0]=buff[0]|(morsa.cmd&0x07)|((morsa.len&0x1F)<<3);
        for (int i = 0; i < morsa.len; i++)
    {
        buff[i+1]=morsa.data[i];
    }
    int fcstotal=fcs(buff,(morsa.len+1));
    buff[morsa.len+1]=(fcstotal)&0x01;
    buff[morsa.len+2]=(fcstotal>>1)&0xFF;

}

int fcs(BYTE* buff, int len){
    int fcs_value=0;
    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            fcs_value += (buff[i]>>j)&0x01;
        }

    }
    return fcs_value;
}

frame desempaquetar(BYTE *buff){
    frame recuperado;
    recuperado.cmd=0;
    recuperado.len=0;
    recuperado.FCS=0;
    recuperado.cmd=recuperado.cmd|(buff[0]&0x07);
    recuperado.len=recuperado.len|((buff[0]>>3)&0x1F);
    recuperado.FCS=recuperado.FCS|(buff[recuperado.len+2]&0xFF)<<1|(buff[recuperado.len+1]&0x01);
    //LIMPIAR DATA
    for (int i = 0; i < LEN; i++)
    {
        recuperado.data[i]=0;
    }

    for (int i = 0; i < recuperado.len; i++)
    {
        recuperado.data[i]=buff[1+i];
    }
    return recuperado;
}

void mensajes_de_prueba(frame recuperado, BYTE* buff){
    printf("Mensaje de prueba [%d]\n", msj);
    mensajes_recibidos--;
    char msj_esperado[]="prueba";
    for(int i=0; i<7; i++){
        if(recuperado.data[i] != msj_esperado[i]){
            error_data=true;
        }
    }
    int x10=recuperado.FCS;
    if( (parityError==true) | (x10 != fcs(buff,(recuperado.len+1))) | (recuperado.len != '7')){
        limpio=false;
        m_error++;
    }
    if(error_data==true){
        m_error_n_e++;
        limpio=false;
    }
    if(limpio){
        m_correcto++;
    }
    limpio=true;
    error_data=false;
    if(msj==10){
        printf("%d ; %d% ----> Mensajes correctos\n", m_correcto, (m_correcto*100/10));
        printf("%d ; %d% ----> Mensajes erroneo\n", m_error, (m_error*100/10));
        printf("%d ; %d% ----> Mensajes con error no encontrado\n", m_error_n_e, (m_error_n_e*100/10));
        msj=0;
        m_error=0;
        m_error_n_e=0;
        m_correcto=0;
        seguridad=false;
    }
}

void guardar_mensaje_txt(frame recuperado){
    FILE* mensaje = fopen("mensaje.txt","a");
    fprintf(mensaje, "%s\n", recuperado.data);
    fclose(mensaje);
}

void buscar_nombre_archivo(frame recuperado, char* nombre){
    for (int i = 0; i < recuperado.len; i++)
    {
        nombre[i]=recuperado.data[i];
    }
    char * ptr;
    ptr = strcat(nombre,".txt");
    FILE* archivo = fopen(nombre, "r");
    if(archivo == NULL){
    printf("No existe el archivo\n");
    }else{
        char linea[32];
        while (!feof(archivo))
        {
            if(fgets(linea, sizeof(linea),archivo)){
            printf("%s", linea);
            }
        }
    }
    fclose(archivo);  //fgets para leer la linea completa
    for(int i=0; i<LEN;i++){
        nombre[i]=0;
    }
}

void imprimir_pp_mensajes(){
    printf("%d ---> Mensajes recibidos\n",mensajes_recibidos);
    printf("%d% ---> Porcentaje de error\n",(errors*100/mensajes_recibidos));
    printf("%d% ---->Porcentaje de correctos\n", ((mensajes_recibidos-errors)*100/mensajes_recibidos));
}

void crear_archivo_guardar_mensaje(frame recuperado, char* nombre){
    if(caso == 0){  //recibiria nombre, caso,
        for (int i = 0; i < recuperado.len; i++){
            nombre[i]=recuperado.data[i];
        }
        char * ptr;
        ptr = strcat(nombre,".txt");
        FILE* archivo = fopen(nombre, "a+");
        caso++;
        fclose(archivo);
    }else{
        char* ptr;
        ptr = strcat(nombre,".txt");
        FILE* archivo = fopen(nombre, "a+");
        fprintf(archivo, "%s\n", recuperado.data);
        fclose(archivo);
        caso=0;
        for(int i=0; i<LEN;i++){
            nombre[i]=0;
        }
    }
}

void listar_archivos_txt_carpeta(){
    printf( "La lista de ficheros en el directorio actual, segun el comando \"dir\":" );
    system( "dir *.txt" );
    }

void cerrar_receptor(){
    printf("|------CERRANDO PROGRAMA------|");
    estado=false;
}

void cb(BYTE* bytes){
    bool level = digitalRead(RX_PIN);
    //  printf("%d",level);
    if (transmissionStarted){
        processBit(level, bytes);
    }else if(level == 0 && !transmissionStarted){
        transmissionStarted  = true;
        nbits                 = 1;
    }
}

void processBit(bool level, BYTE* bytes){
    if(nbits < 9){
        bytes[nbytes] |= (level << (nbits-1));
    }
    if(nbytes == 0){
        aux.cmd|=(buff[0]&0x07);
        aux.len|=((buff[0]>>3)&0x1F);
        max_len=aux.len+3;
    }
    else if (nbits==9){
    //    printf("\n");
        parity = level;
        nones = (bytes[nbytes]&0x01) + ((bytes[nbytes]&0x02)>>1) + ((bytes[nbytes]&0x04)>>2) + ((bytes[nbytes]&0x08)>>3)
      + ((bytes[nbytes]&0x10)>>4) + ((bytes[nbytes]&0x20)>>5) + ((bytes[nbytes]&0x40)>>6) + ((bytes[nbytes]&0x80)>>7);
        if(parity != (nones%2==0)){
            parityError = true;
        }
        nbytes++;
        transmissionStarted = false;
    }
    nbits++;
}

void recibir_bytes(BYTE* bytes){
    if(wiringPiSetup() == -1)
    exit(1);
    //  piHiPri(99);
    //CONFIGURA PINES DE ENTRADA SALIDA
    pinMode(RX_PIN, INPUT);

    //CONFIGURA INTERRUPCION PIN CLOCK (PUENTEADO A PIN PWM)
    if(wiringPiISR(DELAY_PIN, INT_EDGE_RISING, &cb) < 0){
        printf("Unable to start interrupt function\n");
    }

    printf("Delay\n");
    while(nbytes < max_len){
        cb(bytes);
        delay(300);
    }


    for(int i = 0; i<nbytes; i++){
        printf("Byte %d: %d\n", i, bytes[i]);
    }
    printf("Errores: %d\n", errors);
    //printf("Nbits: %d. Errors: %d\n", nbits, errors);
    nbytes=0;

}