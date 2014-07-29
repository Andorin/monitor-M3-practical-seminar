
#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY    "thnkng_sk1ns;001" //exactly the same 16 characters/bytes on all nodes!
#define NETWORKID     100  //the same on all nodes that talk to each other

#define ACK_TIME      100 // max # of ms to wait for an ack
#define MAX_RETRY     10
#define CUSTOMLENGTH 52
#define PAKETLENGTH 9

// TYPE for our sensortypes
typedef enum Klasse{  
  ACK_TEMP   = 0xFF01,
  TEMP       = 0x0001,
  ACK_SOUND  = 0xFF02,
  SOUND      = 0x0002,
  ACK_ROTARY = 0xFF03,
  ROTARY     = 0x0003,
  ACK_BATTERY= 0xFF04,
  BATTERY    = 0x0004,
  ACK_MOTION = 0xFF05,
  MOTION     = 0x0005,
  ACK_OTHER  = 0xFFFF,
  OTHER      = 0x00FF
} klasse;


RFM69 radio; // ... initialize transciever
byte nextSend[61] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  //next sending paket
byte lastRec[61] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // last received paket

byte senderID = 0x00;    // last rec sender ID
int dataleng = 0;     // last rec datalength
int rx_rssi = 0;      // last rec rx_rssi
bool _ackSent = false;    // ack sent for last rec?



byte getFirstByte(word wert){
  wert = wert >> 8;
  return byte(wert);
}

byte getLastByte(word wert){
  
  wert = wert << 8;
  wert = wert >> 8;
  return byte(wert);
  
}
byte getLastByte(long wert){
  
  wert = wert << 24;
  wert = wert >> 24;
  return byte(wert);
  
}
byte getFirstByte(long wert){
  
  wert = wert >> 24;
  return byte(wert);
  
}
byte getFirstMiddleByte(long wert){
  
  wert = wert << 8;
  wert = wert >> 24;
  return byte(wert);
  
}
byte getLastMiddleByte(long wert){
  
  wert = wert << 16;
  wert = wert >> 24;
  return byte(wert);
  
}

// Actual Sending!
boolean sendRFM(word gateway){
  
  // ACK OR NO ACK?
  if(nextSend[0] == 0xFF){
    
    // Sending with ACK-Request
    return radio.sendWithRetry(gateway, nextSend, CUSTOMLENGTH + PAKETLENGTH, MAX_RETRY, ACK_TIME);
    
  } else {
    
    // Sending without ACK-Request
    radio.send(gateway, nextSend, CUSTOMLENGTH); 
    return true;
    
  }
}

// set next send paket data
boolean setNextSend(word klasse, word id, byte pos, long wert, byte custom[], int customLength){

  
  // max sending length: 61 (CUSTIOMLENGTH)
  if(customLength > CUSTOMLENGTH) return false;
  
  //fill nextSend with data
  for(int i = 0; i < customLength + PAKETLENGTH; i++){
    switch(i){
       case 0: 
         nextSend[i] = getFirstByte(klasse);
         break;
       case 1: 
         nextSend[i] = getLastByte(klasse);
         break;
       case 2: 
         nextSend[i] = getFirstByte(id);
         break;
       case 3: 
         nextSend[i] = getLastByte(id);
         break;
       case 4: 
         nextSend[i] = pos;
         break;
       case 5: 
         nextSend[i] = getFirstByte(wert);
         break;
       case 6: 
         nextSend[i] = getFirstMiddleByte(wert);
         break;   
       case 7: 
         nextSend[i] = getLastMiddleByte(wert);
         break;
       case 8: 
         nextSend[i] = getLastByte(wert);
         break; 
       default:
         nextSend[i] = custom[i - PAKETLENGTH];
         break;
    }
  }
  
  //if custom was too little, wipe out old data
  if(customLength < CUSTOMLENGTH){
    for(int i = customLength; i < CUSTOMLENGTH; i++){
      nextSend[i + PAKETLENGTH] = 0x00;
    } 
  }
  
}


// set next send paket data
boolean setNextSend(word klasse, word id, byte pos, float wert, byte custom[], int customLength){

  
  // max sending length: 61 (CUSTIOMLENGTH)
  if(customLength > CUSTOMLENGTH) return false;
  
  char *ptr = (char *)&wert;
  nextSend[5] = ptr[0];
  nextSend[6] = ptr[1];
  nextSend[7] = ptr[2];
  nextSend[8] = ptr[3]; 
  
  
  
  //fill nextSend with data
  for(int i = 0; i < customLength + PAKETLENGTH; i++){
    switch(i){
       case 0: 
         nextSend[i] = getFirstByte(klasse);
         break;
       case 1: 
         nextSend[i] = getLastByte(klasse);
         break;
       case 2: 
         nextSend[i] = getFirstByte(id);
         break;
       case 3: 
         nextSend[i] = getLastByte(id);
         break;
       case 4: 
         nextSend[i] = pos;
         break;
       default:
         nextSend[i] = custom[i - PAKETLENGTH];
         break;
    }
  }
  
  //if custom was too little, wipe out old data
  if(customLength < CUSTOMLENGTH){
    for(int i = customLength; i < CUSTOMLENGTH; i++){
      nextSend[i + PAKETLENGTH] = 0x00;
    } 
  }
  
}

boolean setNextSend(word klasse, word id, byte pos, long wert){
  
  //fill nextSend with data
  for(int i = 0; i < PAKETLENGTH; i++){
    switch(i){
       case 0: 
         nextSend[i] = getFirstByte(klasse);
         break;
       case 1: 
         nextSend[i] = getLastByte(klasse);
         break;
       case 2: 
         nextSend[i] = getFirstByte(id);
         break;
       case 3: 
         nextSend[i] = getLastByte(id);
         break;
       case 4: 
         nextSend[i] = pos;
         break;
       case 5: 
         nextSend[i] = getFirstByte(wert);
         break;
       case 6: 
         nextSend[i] = getFirstMiddleByte(wert);
         break;   
       case 7: 
         nextSend[i] = getLastMiddleByte(wert);
         break;
       case 8: 
         nextSend[i] = getLastByte(wert);
         break;
    }
  }
  
   for(int i = 0; i < CUSTOMLENGTH; i++){
      nextSend[i + PAKETLENGTH] = 0x00;
    }
}

boolean setNextSend(word klasse, word id, byte pos, float wert){
  
  
  char *ptr = (char *)&wert;
  nextSend[5] = ptr[0];
  nextSend[6] = ptr[1];
  nextSend[7] = ptr[2];
  nextSend[8] = ptr[3]; 
  
  
  
  //fill nextSend with data
  for(int i = 0; i < PAKETLENGTH; i++){
    switch(i){
       case 0: 
         nextSend[i] = getFirstByte(klasse);
         break;
       case 1: 
         nextSend[i] = getLastByte(klasse);
         break;
       case 2: 
         nextSend[i] = getFirstByte(id);
         break;
       case 3: 
         nextSend[i] = getLastByte(id);
         break;
       case 4: 
         nextSend[i] = pos;
         break;
    }
  }
  for(int i = 0; i < CUSTOMLENGTH; i++){
      nextSend[i + PAKETLENGTH] = 0x00;
    } 
}

// Not actual sending, only setting the paketdata, then run the sending
boolean sendRFM(word gateway, word klasse, word id, byte pos, long wert, byte custom[], int customSize){
   
  //setting up the data
  setNextSend(klasse, id, pos, wert, custom, customSize);
  
  //sending
  return sendRFM(gateway);
  
}


// Not actual sending, only setting the paketdata, then run the sending
boolean sendRFM(word gateway, word klasse, word id, byte pos, float wert, byte custom[], int customSize){
   
  //setting up the data
  setNextSend(klasse, id, pos, wert, custom, customSize);
  
  //sending
  return sendRFM(gateway);
  
}



byte getPos(){
  byte res;
  res = byte(lastRec[4]);
  return res;
}

word getKlasse(){
  word res;
  res = res + lastRec[0];
  res = res << 8;
  res = res + lastRec[1];
  return res;
  
}

word getId(){
  word res;
  res = res + lastRec[2];
  res = res << 8;
  res = res + lastRec[3];
  return res;
}

long getWert(){
  long res;
  res = res + lastRec[5];
  res = res << 8;
  res = res + lastRec[6];
  res = res << 8;
  res = res + lastRec[7];
  res = res << 8;
  res = res + lastRec[8];
  return res;
}
float getFloatWert(){
  
  float res;
  char *ptr = (char *)&res;
  ptr[0] = lastRec[5];
  ptr[1] = lastRec[6];
  ptr[2] = lastRec[7];
  ptr[3] = lastRec[8];
  return res;
}

byte* getCustom(){
  return &lastRec[9];
}

int getCustomLength(){
  return CUSTOMLENGTH;
}

void setNextKlasse(word wert){
  nextSend[0] = getFirstByte(wert);
  nextSend[1] = getLastByte(wert);
}

void setNextId(word wert){
  nextSend[2] = getFirstByte(wert);
  nextSend[3] = getLastByte(wert);
}

void setNextPos(byte wert){
  nextSend[4] = wert;
}

void setNextWert(long wert){
  nextSend[5] = getFirstByte(wert);
  nextSend[6] = getFirstMiddleByte(wert);
  nextSend[7] = getLastMiddleByte(wert);
  nextSend[8] = getLastByte(wert);
}

// ********** FLOAT WITH BRAEAKDOWN
void setNextWert(float wert){
  char *ptr = (char *)&wert;
  nextSend[5] = ptr[0];
  nextSend[6] = ptr[1];
  nextSend[7] = ptr[2];
  nextSend[8] = ptr[3]; 
}

void setNextCoustom( byte* array, int length){
   
   for(int i = 0; i < CUSTOMLENGTH; i++){
     if( i < length){
       nextSend[PAKETLENGTH + i] = array[i];
     }
     else {
       nextSend[PAKETLENGTH + i] = 0x00;
     }
   }
}




// Not actual sending, only setting the paketdata in one Array, then run the sending
boolean sendRFM(word gateway, byte data[]){
  
  
  //when manual byte array, it has to be 61 long
  if(sizeof(data) != 61){ return false;}
  
  // setting the nextsend
  for(int i = 0;i < 61; i++){
    nextSend[i] = data[i];
  }
  
  //send it
  return sendRFM(gateway);
}


bool ackSent(){
  return _ackSent;
}


bool receiveRFM(){
    
    //somthing there?
    if(radio.receiveDone()){   
     
     // filling the data 
     senderID = radio.SENDERID;
      for (byte i = 0; i < radio.DATALEN; i++)
        lastRec[i] = radio.DATA[i];
      rx_rssi = radio.RSSI;
      dataleng = radio.DATALEN;
      
      
      //serialOutputRFMData();      
      
      
      //if ack_rwuested, then send it out
      if (radio.ACK_REQUESTED)
      {
        radio.sendACK();
		_ackSent = true;        
      }
	  else
	  {
		_ackSent = false;
	  }
	 
	  return true;
    }
	
	return false;
}



