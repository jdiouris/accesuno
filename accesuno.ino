#define FSPI 1
#if FSPI
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 10);
NfcAdapter nfc = NfcAdapter(pn532spi);
#else
 
// NFC
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
#endif
#include <EEPROM.h>

#include <LiquidCrystal_I2C.h>

char appId[]="Acces V2.0";


LiquidCrystal_I2C lcd(0x27,16,2);

int T1=3;
int T2=2;
int Gache=4;
char  m1[17];
char m2[17];
char message[17];

#define IDLENGTH 7
byte tagid[IDLENGTH];
byte idlength;
char strTagId[16];

void addId()
{
  int ad=0;
  while (EEPROM.read(ad)!=0) ad++;
  EEPROM.write(ad,idlength); 
  ad++;
  for (int i=0; i<idlength; i++) 
  {
    EEPROM.write(ad,tagid[i]); 
    ad++;
  }

   EEPROM.write(ad,0); ad++;

   ad=0;
    byte c=EEPROM.read(ad); Serial.println(c,HEX);
    while (c!=0) { ad++; c=EEPROM.read(ad); Serial.println(c,HEX); }
}

void createFile()
{
  int ad=0;
  EEPROM.write(ad,4); ad++;
  EEPROM.write(ad,0x4A); ad++;
  EEPROM.write(ad,0x75); ad++;
  EEPROM.write(ad,0x9D); ad++;
  EEPROM.write(ad,0x3F); ad++;

  EEPROM.write(ad,4); ad++;
  EEPROM.write(ad,0x47); ad++;
  EEPROM.write(ad,0x75); ad++;
  EEPROM.write(ad,0x85); ad++;
  EEPROM.write(ad,0x26); ad++;

  
  EEPROM.write(ad,4); ad++;
  EEPROM.write(ad,0xA2); ad++;
  EEPROM.write(ad,0x3C); ad++;
  EEPROM.write(ad,0xC7); ad++;
  EEPROM.write(ad,0x23); ad++;

   EEPROM.write(ad,0); ad++;
  
  ad=0;
    byte c=EEPROM.read(ad); Serial.println(c,HEX);
    while (c!=0) { ad++; c=EEPROM.read(ad); Serial.println(c,HEX); }
//  nf.println("#47 75 85 26;Jean-Francois;Diouris;a");
//  nf.println("#4A 75 9D 3F;Jean-Francois;Diouris;n");
//  nf.close();
}



int readKeys()
{
  int key=0;
 /// Serial.println(digitalRead(T1));
  if (digitalRead(T1)==LOW) 
  {
    key=1;
    delay( 20);
    while (digitalRead(T1)==LOW);
    delay(20);
  }
  else if (digitalRead(T2)==LOW)  
  {
    key=2;
    delay( 20);
    while (digitalRead(T2)==LOW);
    delay(20);
  }
  return key;
}



void printMenu()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(message);
  lcd.setCursor((8-strlen(m1))/2,1);
  lcd.print(m1);
  lcd.setCursor((8-strlen(m2))/2+7,1);
  lcd.print(m2);
//  lcd.setCursor(7,1);
//  lcd.print("|");
}

void setMenu(char * mm1, char * mm2)
{
  strcpy(m1,mm1);
  strcpy(m2,mm2);
  printMenu();
}
void setMessage(char * m)
{
  strcpy(message,m);
  printMenu();
}

void setup(void) {
  // Gache
  pinMode(Gache,OUTPUT);
  digitalWrite(Gache,LOW);
    Serial.begin(9600);
    Serial.println("NDEF Reader");
    // NFC
    nfc.begin();
//    // SD
//    if (!SD.begin(4)) Serial.println("Erreur SD"); //début de la communication avec la carte sur le pin 4
  // createFile();
   // LCD
    lcd.init();
    lcd.backlight();
    //lcd.print(appId);
    setMessage("BADGE?");
    setMenu("","");
    // Boutons
  pinMode(T1,INPUT_PULLUP);
  pinMode(T2,INPUT_PULLUP);
}

// ------------------------------------------------------------
// Cherche l'ID dans la base
// Retour 0 : n'est pas dans la base, sinon rang dans la base
// ------------------------------------------------------------

int findId()
{
  
  int trouve=0;
  int ad=0;
  int ni=1;
  byte nl;
  byte c=EEPROM.read(ad);
   Serial.println(c,HEX); 
  while (c!=0)
  {
    nl=c;
    if (nl==idlength)
    {
      int t=1;
      for (int i=0; i<nl; i++)
      {       
       if (tagid[i]!=EEPROM.read(ad+i+1)) t=0;
      }
      if (t==1) trouve=ni;
    }
    ad=ad+nl+1;
    ni++;
    c=EEPROM.read(ad); 
    Serial.println(c,HEX);    
  }
  Serial.print("Trouve="); Serial.println(trouve);
  if (trouve>0) Serial.println("OK");
  return trouve;
}


int readTag()
{
  int found=0;

  if (nfc.tagPresent())
    {
        Serial.println("\nTag found\n");
        NfcTag tag = nfc.read();
        idlength= tag.getUidLength();
        
        tag.getUid(tagid,idlength);
        for (int i=0; i<idlength; i++) Serial.print(tagid[i],HEX);
         Serial.print("UID: ");Serial.println(tag.getUidString());
        found=1;
        delay(1000);
    }
   return found;
}

char hexConv(byte x)
{
  char c;
  if (x<10) c=x+'0';
  else c=x-10+'A';
  return c;
}
void convId()
{
  int n=0;
  for (int i=0; i<idlength; i++)
  {
    strTagId[n++]=hexConv(tagid[i]/16); 
    strTagId[n++]=hexConv(tagid[i]%16); 
  }
  strTagId[n++]=0;
  Serial.println(strTagId);
}

void menuAjout()
{
  setMessage("BADGE A AUT?");
  setMenu("","");
  int r=0;
  int n=0;
  while (((r=readTag())!=1)&&(n<2)) n++;
  if (r==1) 
  {
    convId();
    sprintf(message,"%s ?",strTagId);
    //setMessage("AUT"+convId()+"?");
    setMenu("OUI","NON");
    int k;
    while ((k=readKeys())==0) ;
    if (k==1) addId();
  }
  delay(1000);
}

void sup(int ad)
{
  idlength=EEPROM.read(ad);
  int ad2=ad+idlength+1;
  char c=EEPROM.read(ad2);
  while (c!=0)
  {
    EEPROM.write(ad++,c);
    ad2++;
    c=EEPROM.read(ad2);
  }
  EEPROM.write(ad++,0);
}

void menuLister()
{
  int ad=0;
  byte c=EEPROM.read(ad);
  int ni=1;
  while (c!=0)
  {
    idlength=c;
    for (int i=0; i<idlength; i++) tagid[i]=EEPROM.read(ad+i+1);
    convId();
    sprintf(message,"%d:%s",ni,strTagId);
    //  setMessage(String(ni)+" : "+convId());
      setMenu("Sup","Suivant");
      int k;
      while ((k=readKeys())==0) ;
      if (k==2)
      {
        ad+=idlength+1;
        ni++;
      } else sup(ad);
      c=EEPROM.read(ad);
  }
  delay(1000);
}

void menuAdm()
{
  int niveau=1;
  
  
  while(digitalRead(T1)==LOW);
  delay(100);
   setMessage("Administration");
  setMenu("Ajout","Suite");
  int k=0;
  int fin=0;
  while (fin==0)
  {
    k=readKeys();
    switch (niveau)
    {
      case 1:
        if (k==1) 
                {
                  menuAjout();
                  setMessage("Administration");
                  setMenu("Ajout","Suite"); 
                }
        if (k==2) 
          { 
            niveau=2;
            setMenu("Lister","Suite");
          }
          break;
      case 2:
        if (k==1) 
        {
          menuLister(); 
          setMessage("Administration");
          setMenu("Lister","Suite");
        }
        if (k==2) 
          { 
            niveau=3;
            setMenu("Fin","Suite");
          }
          break;
      case 3:
        if (k==1) fin=1;
        if (k==2) 
          { 
            niveau=1;
            setMenu("Ajout","Suite");
          }
          break;
    }
  }
    setMessage("BADGE?");
    setMenu("",""); 
}
int cpt=0;
void loop(void) {
    //Serial.println("\nScan a NFC tag\n");
    if (readTag()==1) 
    {
      if (findId()!=0) 
      {
        Serial.println("ACCES AUTORISE");
        setMessage("ACCES AUTORISE!");
        digitalWrite(Gache,HIGH);
        delay(5000);
        digitalWrite(Gache,LOW);
        cpt=0;
      }
      else 
      {
        Serial.println("ACCES NON AUTORISE");
        setMessage("ACCES NON AUTORISE");
        cpt=0;
      }
    }
    //Serial.print(cpt);
    if (cpt==2)
    {
      setMessage("BADGE?");
    } else cpt++;
    if  (digitalRead(T1)==LOW) 
    { 
      setMessage("Autorisation?");
      setMenu("","");
      int n=0;
      while ((readTag()!=1)&&(n<2)) n++;
      int xx=findId();
       if ((xx==1)||(xx==5)) menuAdm();  
       else 
       {
        setMessage("BADGE?");
        setMenu("",""); 
       }
    }

    //delay(5000);
}
