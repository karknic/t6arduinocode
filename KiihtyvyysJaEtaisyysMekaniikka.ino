// Määritellään kytkentänavat ultraäänitutkalle HC-SR04
const int GNDPin = 11; // ultraäänianturin maa-napa
const int echoPin = 10; // Echo Pin (kaiku, eli vastaanotinpuoli)
const int trigPin = 9; // Trigger Pin (ultraääni-lähtetin)
const int VccPin = 8; // Anturin käyttöjännite

// Määritellään kytkentänavat kiihtyvyysanturille:
const int VccPin2 = A0;  // Käyttöjännite
const int xPin   = A1;   // x-kanavan mittaus
const int yPin   = A2;   // y-kanava
const int zPin   = A3;   // z-kanava
const int GNDPin2 = A4;  // laitteen maa-napa

// Muuttujamäärittelyt. Huomaa, että desimaalierotin on piste!
float maximumRange = 300.0; // Maksimietäisyys jota tällä on tarkoitus mitata; pidempääkin voit kokeilla
float minimumRange = 2.0; // Minimietäisyys. Lyhyellä etäisyydellä sivusuuntainen kulkumatka tulee merkittäväksi
unsigned long kesto = 0; // Lähetetyn uä-pulssin kulkuaika mikrosekunteina
float etaisyys = 0.0; // Äänen kulkuaika kohteeseen ja takaisin; etäisyys
unsigned long aika = 0; // Aikaleima (ms), tyyppinä "pitkä, merkitön" muoto, koska INT-tyyppisenä numeroavaruus tulee n. puolessa minuutissa täyteen.
int SisaanTunniste = 0; // Käyttöjännitteen asettamiseen liittyvä tunniste. Reagoidaan kun mennään ohjelmaan
// eka kerran sisään.
int x = 0; //x-kanavan sensoriarvo
int y = 0;
int z = 0;

int laskuri = 0;
int currentState = 0;
int previousState = 0;

float ax = 0.0;  // x-kanavan kiihtyvyysarvo SI-muodossa (m/s^2)
float ay = 0.0;
float az = 0.0;

float maxArvo = 0;
float minArvo = 1023;

int toisto = 0;
boolean isUp = false;

// Alustetaan kytkentänavat ja sarjayhteys
void setup() {
  Serial.begin (19200); // Tämä täytyy valita myös Serial Monitorista samaksi

  // Ultraäänianturin napojen määrittely:
  pinMode(GNDPin, OUTPUT); // Maadoitus; tämäkin on output-napa joka antaa 0V:n jännitteen
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(VccPin, OUTPUT); // Käyttöjännite

  // Kiihtvyys-anturi:
  pinMode(VccPin2, OUTPUT);     // Kiihtyvyysanturin käyttöjännite Vcc
  pinMode(GNDPin2, OUTPUT);     // Kiihtyvyysanturin GND

  // Asetetaan syöttöjännite (5V UNO-BOARDILLA, 3.3V Genuino 101:llä) ja maa-arvot (0V):
  digitalWrite(VccPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(VccPin2, HIGH);
  delayMicroseconds(2);
  digitalWrite(GNDPin, LOW);
  delayMicroseconds(2);
  digitalWrite(GNDPin2, LOW);
  delayMicroseconds(2);
  delay(200);
}

void loop() {

  long kesto = 0;
  float etaisyys = 0;
  float keskiarvo = 0;
  int lukumaara = 10;

  delay(1);
  // trigPin/echoPin kierros määrittää etäisyyden kohteeseen, josta lähtenyt äänipulssi heijastuu takaisin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  kesto = pulseIn(echoPin, HIGH); // Äänen kulkuaika mikrosekunteina saadaan kaiusta
  // Aikaleima (ms)
  aika = millis(); // Aikaleima luetaan heti kun etäisyys on mitattu ja juuri mennään lukemaan kiihtyvyydet
  // Kiihtyvyysmittaus
  x = analogRead(xPin);  // Luetaan x-sensoriarvo, joka tulee 10-bittisellä koodauksella eli lukualueella 0 - 1023. 0 = 0V ja 1023 = Vcc.
  y = analogRead(yPin);
  z = analogRead(zPin);

  ax = 0.1422 * x - 48.857; //Kalibrointiyhtälö x-akselin sensoriarvosta x-suunnan kiihtyvyydeksi.
  ay = 0.1382 * y - 47.116;
  az = 0.1443 * z - 50.718;

  //etaisyys = kesto * 0.034 / 2;
  etaisyys = (kesto * 0.016854 + 1.0595); //etäisyys (cm).

  if ( etaisyys > 25 && isUp ) {
    isUp = false;
  }
  if ( etaisyys < 10 && etaisyys != 0 && !isUp) {
    isUp = true;
    toisto ++;
  }
  if (etaisyys > maxArvo)
    maxArvo = etaisyys;
  if (etaisyys < minArvo)
    minArvo = etaisyys;

  //Tulostukset sarjaportille
  if (etaisyys >= maximumRange || etaisyys <= minimumRange) {
    // Jos etäisyystulos on epäkelpo, lähetetään negatiivinen tulos periaatteella: "out of range" eli -50(cm)
    Serial.print(aika);
    Serial.print(" ");
    Serial.print(-50); //Negatiivinen etäisyyslukema virheen merkiksi
    Serial.print(" ");
    Serial.print(ax);
    Serial.print(" ");
    Serial.print(ay);
    Serial.print(" ");
    Serial.print(az);
    Serial.print(" ");         //toistomäärät
    Serial.print(toisto);
    Serial.print(" ");
    Serial.print(maxArvo);
    Serial.print(" ");
    Serial.println(minArvo);

  }
  else {

    // Tulostetaan tulokset sarjaportille kun mittaus on onnistunut myös etäisyyden osalta.
    // Kiihtyvyys-arvot ovat yleensä kunnossa jos kytkennät on kunnossa.
    Serial.print(aika);          // Aikaleima(ms)
    Serial.print(" ");           // Välilyönti... tämä on tunniste Exceliin siirrossa!
    Serial.print(etaisyys);      // Etäisyys (cm)
    Serial.print(" ");
    Serial.print(ax);            // x-suunnan kiihtyvyys (m/s^2)
    Serial.print(" ");
    Serial.print(ay);            // y-suunnan kiihtyvyys (m/s^2)
    Serial.print(" ");
    Serial.print(az);          // z-suunnan kiihtyvyys (m/s^2)
    Serial.print(" ");         //toistomäärät
    Serial.print(toisto);
    Serial.print(" ");
    Serial.print(maxArvo);
    Serial.print(" ");
    Serial.println(minArvo);


  }
  delay(2);
}
