
/*  This webpage adds an automated frequency increaser. 
 *  The user specifies a maximum frequency, the duration of
 *  each frequency in seconds, and the day(s) in which the start 
 *  frequency must reach the maximum frequency. 
 *  Addition: Automatic refresh using AJAX
 */
#include <WiFi.h>
#include <WebServer.h> 
#include "FS.h"
#include "SD.h"
#include <SPI.h>
#define SD_CS 5


const char *ssid = "Red_ESP_Zoldan"; // Name the local Wifi netork
const char *password = "12345678"; // Set password for local wifi network
//
//IPAddress local_IP(192,168,4,1);
//IPAddress gateway(192,168,1,0);
//IPAddress subnet(255,255,255,0);
//ESP32Time rtc(3600); 

WebServer server(80);

// Function to extract the value of an argument from the HTTP request
float getArgValue(String name){
  for (uint8_t i = 0; i < server.args(); i++)
    if(server.argName(i) == name)
      return server.arg(i).toFloat();
  return -1;
}

String XMLpage;

unsigned long PML1;
unsigned long PML2;
unsigned long PML3;
unsigned long PML4;

float Frequency; //frequency of the electrical pulses
unsigned long interval; // interval in between pulses (ms)
unsigned long Duration;//duration of the electrical pulses (ms)





bool PolState;
bool PinState;
bool BuzState;
bool Log1State;
bool Log2State;
bool PhaseChanged;

// Initializing pin locations
int w1_1=36;
int w1_2=39;
int muxin=34;
int log1Out=32;
int log2Out=33;
int LED=26;
int s0=27;
int s1=14;
int s2=13;
int senseCount;
int senseCountA;
int senseCountB;
int count;
float BiBalance = 0;
int logic1=22;
int logic2=21;
int buz=4;

String dataMessage;

// Setting data storage column names
char dataHeader[] = "Reading ID,Timestamp,Frequency (Hz),Pulse Duration (ms),"
  "Well 1 Voltage (V),Well 1 Resistance (Ohm),Well 1 Current (mA),"
  "Well 2 Voltage (V),Well 2 Resistance (Ohm),Well 2 Current (mA),"
  "Well 3 Voltage (V),Well 3 Resistance (Ohm),Well 3 Current (mA),"
  "Well 4 Voltage (V),Well 4 Resistance (Ohm),Well 4 Current (mA),"
  "Signal Toggle,Pulse A Width,Pulse B Width,"
  "Auto Frequency Toggle, Auto Start Frequency,Auto Max Frequency,"
  "Auto Duration Toggle,True Duration, Auto Start Duration,Auto Max Duration"
  "\r\n";
  
int readingID = 0;
int StimulationToggle; 
bool altToggle;
int SignalToggle; 
int PolChanged;
int BPM;  
bool A;
bool B;
int P1;
int P2;
int P3;
int P4;


int vRead1a = 0;
int vRead2a = 0;
int vRead3a = 0;
int vRead4a = 0;
int vRead5a = 0;
int vRead6a = 0;
int vRead7a = 0;
int vRead8a = 0;
int vRead9a = 0;
int vRead10a = 0;

int vRead1b = 0;
int vRead2b = 0;
int vRead3b = 0;
int vRead4b = 0;
int vRead5b = 0;
int vRead6b = 0;
int vRead7b = 0;
int vRead8b = 0;
int vRead9b = 0;
int vRead10b = 0;

float v1a = 0;
float v2a = 0;
float v3a = 0;
float v4a = 0;
float c1a = 0;
float c2a = 0;
float c3a = 0;
float c4a = 0;
float r1a = 0;
float r2a = 0;
float r3a = 0;
float r4a = 0;


float v1b = 0;
float v2b = 0;
float v3b = 0;
float v4b = 0;
float c1b = 0;
float c2b = 0;
float c3b = 0;
float c4b = 0;
float r1b = 0;
float r2b = 0;
float r3b = 0;
float r4b = 0;


float w1vTotal = 0;
float w1rTotal = 0;
float w1cTotal = 0;
float w2vTotal = 0;
float w2rTotal = 0;
float w2cTotal = 0;
float w3vTotal = 0;
float w3rTotal = 0;
float w3cTotal = 0;
float w4vTotal = 0;
float w4rTotal = 0;
float w4cTotal = 0;


float well1Voltage = 0;
float well1Resistance = 0;
float well1Current = 0;
float well2Voltage = 0;
float well2Resistance = 0;
float well2Current = 0;
float well3Voltage = 0;
float well3Resistance = 0;
float well3Current = 0;
float well4Voltage = 0;
float well4Resistance = 0;
float well4Current = 0;

String mess = "Polarity Changed";


int AutoFreqToggle;
int PrevAutoFreqToggle;
int AutoFreqDay;
float AutoFreqIncrementIn;
float StartFrequency;
float MaxFrequency;
float AutoFreqIncrementBy;

float trueDur;
int AutoDurToggle;
int PrevAutoDurToggle;
int AutoDurDay;
float AutoDurIncrementIn;
float StartDur;
float MaxDur;
float AutoDurIncrementBy;

//Initializing Website  
void IndexPage() {
  // Update global parameters if new values are provided
  if (getArgValue("FrequencyInput")!=-1 && getArgValue("Duration")!=-1 ){
    Frequency = getArgValue("FrequencyInput");
    Serial.println (getArgValue("FrequencyInput"));
    interval = (1000/Frequency);
    BPM = Frequency*60;
    Duration = getArgValue("Duration");
  }

   // HTML content for the index page
  String message;
  message +="<html><head>";
  message +="<body  onload='process()' bgcolor='#000000'>";
  message +="<table width='1004' cellspacing='0' cellpadding='5' border='0' bgcolor='#003366'>";
  message +="<td colspan='2' align='center'> <font size='6' face='Verdana, Geneva, sans-serif' color='#FFFFFF'><b>Electrostimulation Device Control Page</b></font><a href='/'>";
  //message +="<img src='http://momtahan.droppages.com/ECGGIF.gif' width='960' height='150'>";
  message +="<tr><td colspan='2' align='center'>";
  message +="</td></tr><tr><td width='159' valign='top' bgcolor='#008080' align='left'>";
  message +="<font size='2' face='verdana' color='#ffffff'>";
  message +="<b>Links</b>";
  message +="<p></p><li><a href='/'><b>Home</b></a>";
  message +="<p></p><li><a href='/Settings'><b>More Settings</b></a></font></td>";
  message +="<td width='825' valign='top' bgcolor='#ffffff' align='center'><p align='left'>";
  message +="<font size='5' face='Verdana, Geneva, sans-serif' color='#000000'>";
  message +="Electrical Stimulation is: ";
  if (StimulationToggle==1){
    message +="<b>  ON </b></font></p></p>";
  }else{
    message +="<b>  OFF</b></font></p></p>";
  }
  message +="<p align='left'><b>Electrical Stimulation Frequency is:   ";
  message +="<span id='Frequency'>";
  message +=String(Frequency,3);
  message +="</span> Hz<br>Electrical Stimulation Pulse Duration is:   ";
  message +=Duration;
  message +="ms<br><br>";
  message +="<p align='left'><b>Heart Rate is:   ";
  message +="<span id='BPM'>";
  message +=BPM;
  message +="</span> BPM<br><br><br>";
  message +="<br><br><br><br><br><br>";

  message += "<h2>Circuit Performance</h2>";
  message += "<table border='1'>";
  message += "<tr><th></th><th>Well 1</th><th>Well 2</th><th>Well 3</th><th>Well 4</th></tr>";
  message += "<tr><td>Voltage Delivered (V)</td><td>" + String(well1Voltage, 2) + "</td><td>" + String(well2Voltage, 2) + "</td><td>" + String(well3Voltage, 2) + "</td><td>" + String(well4Voltage, 2) + "</td></tr>";
  message += "<tr><td>Current (mA)</td><td>" + String(well1Current, 2) + "</td><td>" + String(well2Current, 2) + "</td><td>" + String(well3Current, 2) + "</td><td>" + String(well4Current, 2) + "</td></tr>";
  message += "<tr><td>Bioreactor Resistance (Ohms)</td><td>" + String(well1Resistance, 2) + "</td><td>" + String(well2Resistance, 2) + "</td><td>" + String(well3Resistance, 2) + "</td><td>" + String(well4Resistance, 2) + "</td></tr>";
  message += "</table>";
  
  message +="<br><br>";
  message += "<a href='/downloadData'><button>Download Data</button></a>";
  message +="<br><br>";
  
  message +="<a href='/ON'><button>START Electrical Stimulation </button></a>";
  message +=" <a href='/OFF'><button>STOP Electrical Stimulation </button></a><br><br><br><br><br></b></font>";
  message +="<font size='5' face='Verdana, Geneva, sans-serif' color='#000000'><b>";
  message +="Change Settings:   </b></p>";
  message +="<form id='1' action='/' target='response'><p align='left'>";
  message +="Change Frequency To: <input name='FrequencyInput' id='FrequencyInput' type='number' step='any' value='"; //onchange='setThreeNumberDecimal'
  message +=String(Frequency,3);
  message +="' style='width:60px' min='0' max='10'>Hz   <br>";
  message +="Change Pulse Duration To: <input name='Duration' type='number' value='";
  message +=Duration;
  message +="' style='width:60px' min='0' max='1024'>ms   <br>";
  message +="<script>function send(X){ document.getElementById(X).submit();}; </script>";
  message +="<input type='button' value='SET' onclick='send(1)'></form></p></td></tr>";
  message +="<tr><td colspan='2' bgcolor='#003366' align='right'>";
  message +="<font size='1' face='verdana' color='#ffffff'>";
  message +="Webpage designed by Nima Momtahan and Nikhith Kalkunte ";
  message +="</font></td></tr></tbody></table></body></html>";

  message +="\n <SCRIPT>\n if(window.XMLHttpRequest){\n var xmlHttp=new XMLHttpRequest();\n }else{\n var xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');\n }\n";
  message +="function response(){\n";
  message +="xmlResponse=xmlHttp.responseXML;\n";
  message+="xmldoc = xmlResponse.getElementsByTagName('FrequencyXML');\n";    
  message+="message = xmldoc[0].firstChild.nodeValue;\n";
  message+="document.getElementById('Frequency').innerHTML=message;\n";    
  message +="xmldoc = xmlResponse.getElementsByTagName('BPMXML');\n";
  message +="message = xmldoc[0].firstChild.nodeValue;\n";
  message +="document.getElementById('BPM').innerHTML=message;\n";
  message +="}\n";
  message +="function process(){\n";
  message +="xmlHttp.open('PUT','xml',true);\n";
  message +="xmlHttp.onreadystatechange=response;\n";
  message +="xmlHttp.send(null);\n";
  message +="setTimeout('process()',500);\n";                               //500ms referesh rate
  message +="}\n";
  message +="</SCRIPT>\n";
  server.send(200, "text/html", message);
}

// Function to handle XML requests
void XML(){
  XMLpage ="<?xml version='1.0'?>";
  XMLpage +="<parent>";
  // for span tag updating user
  XMLpage+="<FrequencyXML>";  
  XMLpage+=String(Frequency, 3);
  XMLpage+="</FrequencyXML>";
  // for input tag allowing user input
  XMLpage +="<BPMXML>";  
  XMLpage +=Frequency*60;
  XMLpage +="</BPMXML>";
  XMLpage +="</parent>";
  server.send(200,"text/xml", XMLpage);
}

// Function to handle the settings page
void Settings() {
  if (getArgValue("SignalToggle")!=-1 && getArgValue("AutoFreqToggle")!=-1 && getArgValue("AutoDurToggle")!=-1){
    Serial.println("new values:");
    SignalToggle = getArgValue("SignalToggle");
    AutoFreqToggle = getArgValue("AutoFreqToggle");
    AutoDurToggle = getArgValue("AutoDurToggle");

    if (AutoFreqToggle){
        PML3 = millis();
        if (getArgValue("MaxFrequency")!=-1 && getArgValue("AutoFreqDay")!=-1 && getArgValue("AutoFreqIncrementIn")!=-1){
        MaxFrequency = getArgValue("MaxFrequency");
        AutoFreqDay = getArgValue("AutoFreqDay");
        AutoFreqIncrementIn = getArgValue("AutoFreqIncrementIn");
        StartFrequency = getArgValue("StartFrequency");  
      }
         
    }
    if (AutoDurToggle){
        PML4 = millis();
        if (getArgValue("MaxDur")!=-1 && getArgValue("AutoDurDay")!=-1 && getArgValue("AutoDurIncrementIn")!=-1){
        MaxFrequency = getArgValue("MaxFrequency");
        AutoDurDay = getArgValue("AutoDurDay");
        AutoDurIncrementIn = getArgValue("AutodurIncrementIn");
        StartDur = getArgValue("StartDuration"); 
      }      
    }
  }

  String message;
  message +="<html><head>";
  message +="<body  bgcolor='#000000'>";
  message +="<table width='1004' cellspacing='0' cellpadding='5' border='0' bgcolor='#003366'>";
  message +="<td colspan='2' align='center'> <font size='6' face='Verdana, Geneva, sans-serif' color='#FFFFFF'><b>Welcome to the Electrostimulation Device Control Page</b></font><a href='/'>";
  message +="<tr><td colspan='2' align='center'>";
  message +="</td></tr><tr><td width='159' valign='top' bgcolor='#008080' align='left'>";
  message +="<font size='2' face='verdana' color='#ffffff'>";
  message +="<b>Links</b>";
  message +="<p></p><li><a href='/'><b>Home</b></a>";
  message +="<p></p><li><a href='/Settings'><b>More Settings</b></a></font></td>";
  message +="<td width='825' valign='top' bgcolor='#ffffff' align='center'><p align='left'>";
  message +="<font size='5' face='Verdana, Geneva, sans-serif' color='#000000'>";
  message +="<form id='1' action='/Settings' target='response'><p align='left'>";

  //Stim Signal Input
  message +="Stimulation signal: <br>";
  message +=" <input type='radio' name='SignalToggle' value='1'";
  if (SignalToggle==1){message +=" checked";}
  message +="> Biphasic Stimulation<br>";
  message +=" <input type='radio' name='SignalToggle' value='2'";
  if (SignalToggle==2){message +=" checked";}
  message +="> Monophasic Stimulation<br>";
  message +=" <input type='radio' name='SignalToggle' value='3'";
  if (SignalToggle==3){message +=" checked";}
  message +="> Monophasic Stimulation - Alternating Polarity<br>";
  message +=" <input type='radio' name='SignalToggle' value='4'";
  if (SignalToggle==4){message +=" checked";}
  message +="> Constant DC Stimulation<br>";

  // Dynamic Frequency Settings
  message +="Auto frequency increase: <br>";
  message +=" <input type='radio' name='AutoFreqToggle' value='1'";
  if (AutoFreqToggle==1){message +=" checked";}
  message +="> ON<br>";
  message +=" <input type='radio' name='AutoFreqToggle' value='0'";
  if (AutoFreqToggle==0){message +=" checked";}
  message +="> OFF<br>";
  message +="Start Frequency: <input name='StartFrequency' type='number' value='";
  message += StartFrequency;
  message += "' style='width:60px' min='0' max='1024'>Hz   <br>";
  message +="Maximum Frequency: <input name='MaxFrequency' type='number' value='";
  message += MaxFrequency;
  message += "' style='width:60px' min='0' max='1024'>Hz   <br>";
  message += "Length of time over which frequency increases: <input name='AutoFreqDay' type='number' step='any' value='";
  message += AutoFreqDay;
  message +="' style='width:60px' >day(s)   <br>"; 
  message +="Rate of Frequency Increase : <input name='AutoFreqIncrementIn' type='number' value='";
  message += AutoFreqIncrementIn;
  message += "' style='width:60px' min='1' max='600000'>s   <br>";


   // Dynamic Duration Settings
  message +="Auto duration increase: <br>";
  message +=" <input type='radio' name='AutoDurToggle' value='1'";
  if (AutoDurToggle==1){message +=" checked";}
  message +="> ON<br>";
  message +=" <input type='radio' name='AutoDurToggle' value='0'";
  if (AutoDurToggle==0){message +=" checked";}
  message +="> OFF<br>";
  message +="Start Duration: <input name='StartFrequency' type='number' value='";
  message += StartDur;
  message += "' style='width:60px' min='0' max='1024'>Hz   <br>";
  message +="End Duration: <input name='MaxFrequency' type='number' value='";
  message += MaxDur;
  message += "' style='width:60px' min='0' max='1024'>Hz   <br>";
  message += "Length of time over which duration increases: <input name='AutoFreqDay' type='number' step='any' value='";
  message += AutoDurDay;
  message +="' style='width:60px' >day(s)   <br>"; 
  message +="Rate of Duration Increase : <input name='AutoFreqIncrementIn' type='number' value='";
  message += AutoDurIncrementIn;
  message += "' style='width:60px' min='1' max='600000'>s   <br>";
  

  message +="<script>function send(X){ document.getElementById(X).submit();}; </script>";
  message +="<input type='button' value='SET' onclick='send(1)'></form></p></td></tr>";
  message +="<tr><td colspan='2' bgcolor='#003366' align='right'>";
  message +="<font size='1' face='verdana' color='#ffffff'>";
  message +="Webpage designed by Nima Momtahan ";
  message +="</font></td></tr></tbody></table></body></html>";
  server.send(200, "text/html", message);
  
}

void ON() {
  StimulationToggle=1;
  Serial.println("START Electrical Stimulation");
  Serial.println(StimulationToggle);
  String message = "<meta http-equiv='refresh' content='0; url=/' />";
  server.send(200, "text/html", message);
  PML1 = 0;
  PML2 = 0;
 
  
}

void OFF() {
  StimulationToggle=0;
  Serial.println("STOP Electrical Stimulation");
  Serial.println(StimulationToggle);
  String message = "<meta http-equiv='refresh' content='0; url=/' />";
  server.send(200, "text/html", message);
  PML1 = 0;
  PML2 = 0;
}

void downloadData() {
  File dataFile = SD.open("/data.txt");
  if (dataFile) {
    server.streamFile(dataFile, "text/plain");
    dataFile.close();
  } else {
    server.send(404, "text/plain", "Data file not found.");
  }
}

void setup() {
  pinMode(logic1, OUTPUT);
  pinMode(logic2, OUTPUT);
  pinMode(buz,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(s0,OUTPUT);
  pinMode(s1,OUTPUT);
  pinMode(s2,OUTPUT);
  
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();
  
  // Setting up WiFi network
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid,password) ? "Ready" : "Failed!");
  
  Serial.print("Setting soft-AP configuration ... ");

  
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  
  // Start the server
  server.begin();
  Serial.println("Server started");
   
  
  server.on("/", IndexPage);
  server.on("/xml", XML);
  server.on("/Settings", Settings);
  server.on("/OFF", OFF);        
  server.on("/ON", ON);
  server.on("/downloadData", HTTP_GET, downloadData);  

  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", dataHeader);
  }
  else {
    Serial.println("File already exists...Deleting previous file");  
    SD.remove("/data.txt");
    Serial.println("Creating file...");
    writeFile(SD, "/data.txt", dataHeader);
  }
  file.close();
  
  //Default Values
  Frequency = 1; //Hz
  interval = ((1000/Frequency)); //ms
  BPM = Frequency*60; //BPM
  Duration = 4; //ms
  StimulationToggle=1;
  SignalToggle=1;
  PML1 = 0;
  PML2 = 0;
  PolState = true;
  altToggle = true;
  PinState == 1;
  senseCount=0;
  senseCountA=0;
  senseCountB=0;
  count = 0;
  PrevAutoFreqToggle = 0;
  AutoFreqToggle = 0;
  StartFrequency = 1;
  MaxFrequency = 2;
  AutoFreqDay = 10.0;
  AutoFreqIncrementIn = 1; // 1 second
  PML3 = 0;
  PrevAutoDurToggle = 0;
  AutoDurToggle = 1;
  StartDur = 4;
  MaxDur = 8;
  AutoDurDay = 10.0;
  AutoDurIncrementIn = 1; // 1 second
  PML4 = 0;
}

void loop() {
  if (StimulationToggle==1){
  
  pulseTypeSet(); // Function to assess the type of pulse to use (Monophasic, Biphaisc, Alternating Monophasic)
  autoDur(); // Function to check if Dynamic Duration is initialized and if so, calculates pulse duration for this round of pulse
  autoFreq(); //Function to check if Dynamic Frequency is initialized and if so, calcualtes frequency for this round of pulse
   
  if ((millis() - PML1) >= interval && PinState == 0 && SignalToggle!=4) {
    PML2 = micros();
    PML1 = millis();
    digitalWrite(logic1, P1); 
    digitalWrite(logic2, P2);
    digitalWrite(buz,HIGH);
    digitalWrite(LED,HIGH);
    PhaseChanged = 0;
    PinState = 1; 
    readVals1();
    
  }

  if (PinState == 1 && PhaseChanged == 0) {
  senseCountA++;
  }

  if ((micros() - PML2) >= (Duration*500) && PinState == 1 && PhaseChanged == 0 && SignalToggle!=4) {
    digitalWrite(logic1, P3); 
    digitalWrite(logic2, P4);
    PhaseChanged = 1;
    Serial.println(mess);
    readVals2(); 
     
  }

  if (PinState == 1 && PhaseChanged == 1) {
  senseCountB++;
  }
  
  if ((micros() - PML2) >= (Duration*1000) && PinState == 1 && SignalToggle!=4) {
    digitalWrite(logic1, LOW);
    digitalWrite(logic2, LOW);
    digitalWrite(buz,LOW);
    digitalWrite(LED,LOW);
    calcVals();
    PinState = 0;
    logSDCard();
    readingID++; // increment readingID on every new reading
    BiBalance = senseCountA/senseCountB ;
    senseCountA = 0;
    senseCountB = 0;
    altToggle=!altToggle;
  }

  // Constant DC voltage
  if(SignalToggle==4){
    digitalWrite(logic1, HIGH);
    digitalWrite(logic2, LOW);
    digitalWrite(buz,HIGH);
    digitalWrite(LED,HIGH);
    readVals1();
    readVals2();
    calcVals();
    logSDCard();
    readingID++;
    Serial.println("Constant DC voltage");
  }
  }
  
server.handleClient();
}


void pulseTypeSet(){
  //setting pins for Biphasic Pulses 
  if(SignalToggle==1){
  P1 = HIGH;
  P2 = LOW;
  P3 = LOW;
  P4 = HIGH;
  mess = "Bi-Phasic: Polarity Changed";
  }

  //setting pins for Monophasic Pulses 
  if(SignalToggle==2){
  P1 = HIGH;
  P2 = LOW;
  P3 = HIGH;
  P4 = LOW;
  mess = "Monophasic";
  }

  //setting pins for Alternating Monophasic Pulses 
 if(SignalToggle==3){
  if (altToggle==1){
    P1 = HIGH;
    P2 = LOW;
    P3 = HIGH;
    P4 = LOW;
    Serial.println("Alternating Monophasic: Positive");
    }
  if (altToggle==0){
    P1 = LOW;
    P2 = HIGH;
    P3 = LOW;
    P4 = HIGH;
    Serial.println("Alternating Monophasic: Negative");
    }
  mess = "Alternating Monophasic";
  }
}

void autoFreq(){
    if (AutoFreqToggle && (Frequency + AutoFreqIncrementBy) <= MaxFrequency){
      if (millis() - PML3 >= AutoFreqIncrementIn*1000) {
        PML3 = millis();
        if (!PrevAutoFreqToggle){
        AutoFreqIncrementBy = ((MaxFrequency - StartFrequency) / (AutoFreqDay * 24 * (60*60*AutoFreqIncrementIn)));
        }
        Frequency += AutoFreqIncrementBy;
        interval = 1000/Frequency;
        PrevAutoFreqToggle = 1;
      }
    }
    else {
    AutoFreqToggle = 0;
    PrevAutoFreqToggle = 0;
  }
}

void autoDur() {  
  if (AutoDurToggle && (Duration + AutoDurIncrementBy) <= MaxDur){
      if (millis() - PML4 >= AutoDurIncrementIn*1000) {
        
        PML4 = millis();
        if (!PrevAutoDurToggle){
          AutoDurIncrementBy = ((MaxDur - StartDur) / (AutoDurDay * 24 * (60*60*AutoDurIncrementIn)));
          trueDur = StartDur;
        }
        trueDur += AutoDurIncrementBy;
        PrevAutoDurToggle = 1;
       if (StimulationToggle==1){
          Duration = roundToEven(trueDur);
       } 
       else {
        Duration = trueDur;
       }
      }
    }
    else {
    AutoDurToggle = 0;
    PrevAutoDurToggle = 0;
  }
}


int roundToEven(float number) {
  int rounded = int(number); // Round to the nearest integer

  // Check if the number is odd
  if (rounded % 2 != 0) {
    // If it's odd, adjust the rounded value to the nearest even integer
    if (number > 0) {
      rounded++;
    } else {
      rounded--;
    }
  }

  return rounded;
}



float valtoV(int sR){
  float v = (((sR)*0.0039)+0.7479);
  return v;
}

int vm(int sp){
  int sensorValue = analogRead(sp);
  return sensorValue;
}

int readMux(int channel){
  int controlPin[] = {s0, s1, s2};

  int muxChannel[8][3]={
    {0,0,0}, //channel 0
    {1,0,0}, //channel 1
    {0,1,0}, //channel 2
    {1,1,0}, //channel 3
    {0,0,1}, //channel 4
    {1,0,1}, //channel 5
    {0,1,1}, //channel 6
    {1,1,1}, //channel 7
  };

  //loop through the 3 sig
  for(int i = 0; i < 3; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the Z pin
  int sensorValue = analogRead(muxin);
  return sensorValue;
}

void readVals1(){
  vRead1a = vm(log1Out);
  vRead2a = vm(log2Out);
  vRead3a = vm(w1_1);
  vRead4a = vm(w1_2);
  vRead5a = readMux(0);
  vRead6a = readMux(1);
  vRead7a = readMux(7);
  vRead8a = readMux(5);
  vRead9a = readMux(6);
  vRead10a = readMux(4);
}

void readVals2(){
  vRead1b = vm(log1Out);
  vRead2b = vm(log2Out);
  vRead3b = vm(w1_1);
  vRead4b = vm(w1_2);
  vRead5b = readMux(0);
  vRead6b = readMux(1);
  vRead7b = readMux(7);
  vRead8b = readMux(5);
  vRead9b = readMux(6);
  vRead10b = readMux(4);
}

void avVals (){
  r1a = v1a/c1a;
  r1b = v1b/c1b;
  well1Voltage = (v1a+v1b)/2;
  well1Current = ((c1a+c1b)/2)*1000; 
  well1Resistance = (r1a+r1b)/2;

  r2a = v2a/c2a;
  r2b = v2b/c2b;
  well2Voltage = (v2a+v2b)/2;
  well2Current = ((c2a+c2b)/2)*1000; 
  well2Resistance = (r2a+r2b)/2;

  r3a = v3a/c3a;
  r3b = v2b/c3b;
  well3Voltage = (v3a+v3b)/2;
  well3Current = ((c3a+c3b)/2)*1000; 
  well3Resistance = (r3a+r3b)/2;

  r4a = v4a/c4a;
  r4b = v4b/c4b;
  well4Voltage = (v4a+v4b)/2;
  well4Current = ((c4a+c4b)/2)*1000; 
  well4Resistance = (r4a+r4b)/2;
}


void calcVals (){
  if(SignalToggle==1){
    v1a = valtoV(vRead3a)-valtoV(vRead4a);
    v1b = valtoV(vRead4b)-valtoV(vRead3b);
    c1a = (valtoV(vRead1a)-valtoV(vRead3a))/10;
    c1b = (valtoV(vRead2b)-valtoV(vRead4b))/10;
    
    v2a = valtoV(vRead5a)-valtoV(vRead6a);
    v2b = valtoV(vRead6b)-valtoV(vRead5b);
    c2a = (valtoV(vRead1a)-valtoV(vRead5a))/10;
    c2b = (valtoV(vRead2b)-valtoV(vRead6b))/10;
   
    v3a = valtoV(vRead7a)-valtoV(vRead8a);
    v3b = valtoV(vRead8b)-valtoV(vRead7b);
    c3a = (valtoV(vRead1a)-valtoV(vRead7a))/10;
    c3b = (valtoV(vRead2b)-valtoV(vRead8b))/10;
    
    v4a = valtoV(vRead9a)-valtoV(vRead10a);
    v4b = valtoV(vRead10b)-valtoV(vRead9b);
    c4a = (valtoV(vRead1a)-valtoV(vRead9a))/10;
    c4b = (valtoV(vRead2b)-valtoV(vRead10b))/10;
    }
  
  if(SignalToggle==2 | SignalToggle==4 ){
    v1a = valtoV(vRead3a)-valtoV(vRead4a);
    v1b = valtoV(vRead3b)-valtoV(vRead4b);
    c1a = (valtoV(vRead1a)-valtoV(vRead3a))/10;
    c1b = (valtoV(vRead1b)-valtoV(vRead3b))/10;
    
    v2a = valtoV(vRead5a)-valtoV(vRead6a);
    v2b = valtoV(vRead5b)-valtoV(vRead6b);
    c2a = (valtoV(vRead1a)-valtoV(vRead5a))/10;
    c2b = (valtoV(vRead1b)-valtoV(vRead5b))/10;
   
    v3a = valtoV(vRead7a)-valtoV(vRead8a);
    v3b = valtoV(vRead7b)-valtoV(vRead8b);
    c3a = (valtoV(vRead1a)-valtoV(vRead7a))/10;
    c3b = (valtoV(vRead1b)-valtoV(vRead7b))/10;
  
    v4a = valtoV(vRead9a)-valtoV(vRead10a);
    v4b = valtoV(vRead9b)-valtoV(vRead10b);
    c4a = (valtoV(vRead1a)-valtoV(vRead9a))/10;
    c4b = (valtoV(vRead1b)-valtoV(vRead9b))/10;    
    }

  if(SignalToggle==3){
    if (altToggle==1){
      v1a = valtoV(vRead3a)-valtoV(vRead4a);
      v1b = valtoV(vRead3b)-valtoV(vRead4b);
      c1a = (valtoV(vRead1a)-valtoV(vRead3a))/10;
      c1b = (valtoV(vRead1b)-valtoV(vRead3b))/10;
  
      v2a = valtoV(vRead5a)-valtoV(vRead6a);
      v2b = valtoV(vRead5b)-valtoV(vRead6b);
      c2a = (valtoV(vRead1a)-valtoV(vRead5a))/10;
      c2b = (valtoV(vRead1b)-valtoV(vRead5b))/10;
 
      v3a = valtoV(vRead7a)-valtoV(vRead8a);
      v3b = valtoV(vRead7b)-valtoV(vRead8b);
      c3a = (valtoV(vRead1a)-valtoV(vRead7a))/10;
      c3b = (valtoV(vRead1b)-valtoV(vRead7b))/10;

      v4a = valtoV(vRead9a)-valtoV(vRead10a);
      v4b = valtoV(vRead9b)-valtoV(vRead10b);
      c4a = (valtoV(vRead1a)-valtoV(vRead9a))/10;
      c4b = (valtoV(vRead1b)-valtoV(vRead9b))/10; 
      }
    if (altToggle==0){
      v1a = valtoV(vRead4a)-valtoV(vRead3a);
      v1b = valtoV(vRead4b)-valtoV(vRead3b);
      c1a = (valtoV(vRead2a)-valtoV(vRead4a))/10;
      c1b = (valtoV(vRead2b)-valtoV(vRead4b))/10;
  
      v2a = valtoV(vRead6a)-valtoV(vRead5a);
      v2b = valtoV(vRead6b)-valtoV(vRead5b);
      c2a = (valtoV(vRead2a)-valtoV(vRead6a))/10;
      c2b = (valtoV(vRead2b)-valtoV(vRead6b))/10;
 
      v3a = valtoV(vRead8a)-valtoV(vRead7a);
      v3b = valtoV(vRead8b)-valtoV(vRead7b);
      c3a = (valtoV(vRead2a)-valtoV(vRead8a))/10;
      c3b = (valtoV(vRead2b)-valtoV(vRead8b))/10;

      v4a = valtoV(vRead10a)-valtoV(vRead9a);
      v4b = valtoV(vRead10b)-valtoV(vRead9b);
      c4a = (valtoV(vRead2a)-valtoV(vRead10a))/10;
      c4b = (valtoV(vRead2b)-valtoV(vRead10b))/10; 
      }
    }
  avVals ();
}


// Write the sensor readings on the SD card
void logSDCard() {
  dataMessage = String(readingID)+","+ String(millis())+","+String(Frequency)+","+String(Duration)+
  ","+String(well1Voltage)+","+String(well1Resistance)+","+String(well1Current)+
  ","+String(well2Voltage)+","+String(well2Resistance)+","+String(well2Current)+
  ","+String(well3Voltage)+","+String(well3Resistance)+","+String(well3Current)+
  ","+String(well4Voltage)+","+String(well4Resistance)+","+String(well4Current)+
  ","+String(SignalToggle)+","+String(senseCountA)+","+String(senseCountB)+
  ","+String(AutoFreqToggle)+","+String(StartFrequency)+","+String(MaxFrequency)+
  ","+String(AutoDurToggle)+","+String(trueDur)+","+String(StartDur)+","+String(MaxDur)+
  "\r\n";
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.txt", dataMessage.c_str());
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
    
