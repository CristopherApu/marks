#include "emisor.h"

volatile int nbits = 0;
volatile int nbytes = 0;
int nones = 0;
bool transmissionStarted = false;

int main(){


    int seleccion=0;
    int contador_mensaje=0;
    int m_prueba=0;
    do
    {
        printf("-----------Seleccione una opcion------------\n");
        printf("0 --> Cerrar programa emisor\n");
        printf("1 --> Enviar mensaje de prueba\n");   //comando 1
        printf("2 --> Enviar mensaje de texto\n");    //comando 2
        printf("3 --> Buscar nombre de un archivo .txt\n");   //comando 3
        printf("4 --> Imprimir contador de mensajes en el receptor\n"); //comando 4
        printf("5 --> Crear archivo\n");    //comando 2
        printf("6 --> Listar archivos .txt\n");    //comando 2
        printf("7 --> Cerrar programa receptor\n");    //comando 7.

        scanf("%d",&seleccion);
        switch (seleccion)
        {
        case 0: //cerrando
            printf("|---------Cerrando programa emisor---------|\n");
            break;
        case 1: //Enviar mensaje de prueba 10 veces
            printf("---------Enviando mensaje de prueba---------\n");
            mensaje_de_prueba();
            contador_mensaje+=10;
            m_prueba+=10;
            break;
        case 2: //enviar un mensaje de texto
            printf("----------Enviando mensaje de texto---------\n");
            enviar_mensaje();
            contador_mensaje++;
            break;
        case 3: //Buscar nombre de un archivo
            printf("-------Buscando nombre de un archivo--------\n");
            buscar_archivo();
            contador_mensaje++;
            break;
        case 4: //Imprimir contador de mensajes en el receptor
            printf("---Imprimir contador de mensajes en el receptor---\n");
            Imprimir_cont_mnsj();
            contador_mensaje++;
            break;
        case 5: //Crear archivo
            printf("---------------Crear archivo----------------\n");
            crear_archivo();
            contador_mensaje++;
            break;
        case 6: //Listar archivos
            printf("--------------Listar archivos---------------\n");
            listar_archivos_txt();
            contador_mensaje++;
            break;
        case 7: //Cerrar programa receptor
            printf("----------Cerrar programa receptor----------\n");
            cerrar_receptor();
            contador_mensaje++;
            break;
        default:
            break;
        }
        printf("Contador de mensajes enviados -->  %d\n",contador_mensaje);
        printf("Contador de mensajes de prueba enviado -->  %d\n",m_prueba);
    } while (seleccion != 0);

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

void mensaje_de_prueba(){
    BYTE buff[100]={0};
    frame prueba;
    prueba.cmd=1;
    prueba.len=7;
    int max_len=prueba.len+3;

    char hola[]="prueba";
    for(int i=0; i<prueba.len; i++){
        prueba.data[i]=hola[i];
    }
    empaquetar(buff,prueba);
    //  ENVIAR BITS 10 VECES//  
    for (int i = 0; i < 10; i++)
    {
        envia_bytes(buff,max_len);
        delay(2000);
    }

}

void enviar_mensaje(){
    BYTE buff[100]={0};
    frame prueba;
    prueba.cmd=2;
    printf("Prohibido usar espacios\n");
    printf("Escriba su mensaje de prueba, max %d caracteres\n", LEN);
    printf("|--- ENVIAR SU MENSAJE PRESIONANDO ENTER ---|\n");
    scanf("%s", prueba.data);
    int largo=0;
    for(int i = 0; i < LEN; i++){
        char aux = prueba.data[i];
        if('\0' == aux){
            break;
        }
        largo++;
    }
    prueba.len=largo;
    int max_len=prueba.len+3;
    empaquetar(buff,prueba);

    envia_bytes(buff,max_len);

}

void buscar_archivo(){
    BYTE buff[100]={0};
    frame prueba;
    prueba.cmd=3;
    printf("Prohibido usar espacios\n");
    printf("Escriba el nombre del archivo\nmax %d caracteres\n", LEN);
    printf("|--- ENVIAR SU MENSAJE PRESIONANDO ENTER ---|\n");
    scanf("%s", prueba.data);
    int largo=0;
    for(int i = 0; i < LEN; i++){
        char aux = prueba.data[i];
        if('\0' == aux){
            break;
        }
        largo++;
    }
    prueba.len=largo;
    empaquetar(buff,prueba);
    int max_len=prueba.len+3;
    envia_bytes(buff,max_len);
}

void Imprimir_cont_mnsj(){
    BYTE buff[100]={0};
    frame prueba;
    prueba.cmd=4;
    prueba.len=0;
    empaquetar(buff,prueba);
    int max_len=prueba.len+3;
    envia_bytes(buff,max_len);

}

void crear_archivo(){
    BYTE buff[100]={0};
    frame prueba;
    prueba.cmd=5;
    printf("Prohibido usar espacios\n");
    printf("Escriba el nombre del archivo, max %d caracteres\n", LEN);
    printf("|--- ENVIAR SU MENSAJE PRESIONANDO ENTER ---|\n");
    scanf("%s", prueba.data);
    int largo=0;
    for(int i = 0; i < LEN; i++){
        char aux = prueba.data[i];
        if('\0' == aux){
            break;
        }
        largo++;
    }
    prueba.len=largo;
    empaquetar(buff,prueba);
    //enviar
    int max_len=prueba.len+3;
    envia_bytes(buff,max_len);
    printf("Creando archivo...\n");
    delay(2000);
    printf("Archivo creado, introduzca mensaje a guardar...\n");
    frame mensaje_nuevo;
    mensaje_nuevo.cmd=5;
    printf("Prohibido usar espacios\n");
    printf("Escriba su mensaje, max %d caracteres\n", LEN);
    printf("|--- ENVIAR SU MENSAJE PRESIONANDO ENTER ---|\n");
    scanf("%s", mensaje_nuevo.data);
    largo=0;
    for(int i = 0; i < LEN; i++){
        char aux = mensaje_nuevo.data[i];
        if('\0' == aux){
            break;
        }
        largo++;
    }
    mensaje_nuevo.len=largo;
    empaquetar(buff,mensaje_nuevo);
    //enviar
    max_len=0;
    max_len=mensaje_nuevo.len+3;
    envia_bytes(buff,max_len);
}

void listar_archivos_txt(){
    BYTE buff[100]={0};
    frame prueba;
    prueba.cmd=6;
    prueba.len=0;
    empaquetar(buff,prueba);
    int max_len=prueba.len+3;
    envia_bytes(buff,max_len);
}

void cerrar_receptor(){
    BYTE buff[100]={0};
    frame prueba;
    prueba.cmd=7;
    prueba.len=0;
    empaquetar(buff,prueba);
    int max_len=prueba.len+3;
    envia_bytes(buff,max_len);
}

//ENVIAR BYTES
void cb(BYTE* bytes, int len){

  if(transmissionStarted){
    //Escribe en el pin TX
    if(nbits == 0){
      digitalWrite(TX_PIN, 0); //Bit de inicio
    }else if(nbits < 9){
      digitalWrite(TX_PIN, (bytes[nbytes] >> (nbits-1)) & 0x01); //Bit de dato
//      printf("%d",(bytes[nbytes]>>(nbits-1))&0x01);
    }else if(nbits == 9){
//      printf("\n");
      nones = (bytes[nbytes]&0x01) + ((bytes[nbytes]&0x02)>>1) + ((bytes[nbytes]&0x04)>>2) + ((bytes[nbytes]&0x08)>>3)
      + ((bytes[nbytes]&0x10)>>4) + ((bytes[nbytes]&0x20)>>5) + ((bytes[nbytes]&0x40)>>6) + ((bytes[nbytes]&0x80)>>7);
      digitalWrite(TX_PIN, nones%2==0); //Bit de paridad
    }else{
      digitalWrite(TX_PIN, 1); //Canal libre durante 2 clocks
    }

    //Actualiza contador de bits
    nbits++;

    //Actualiza contador de bytes
    if(nbits == 11){
      nbits = 0;
      nbytes++;

      //Finaliza la comunicación
      if(nbytes==len){
        transmissionStarted = false;
        nbytes = 0;
      }
    }
  }else{
    //Canal en reposo
    digitalWrite(TX_PIN, 1);
  }
}

void startTransmission(){
  transmissionStarted = true;
}

void envia_bytes(BYTE* bytes, int len){
    if(wiringPiSetup() == -1)
        exit(1);
    //CONFIGURA PINES DE ENTRADA SALIDA
    pinMode(TX_PIN, OUTPUT);
    //CONFIGURA INTERRUPCION PIN CLOCK (PUENTEADO A PIN PWM)
    if(wiringPiISR(DELAY_PIN, INT_EDGE_RISING, &cb) < 0){
        printf("Unable to start interrupt function\n");
    }

    //  printf("Delay\n");
    //  delay(5000);
    startTransmission();
    while(transmissionStarted){
        cb(bytes, len);
        delay(2000);
    }

}