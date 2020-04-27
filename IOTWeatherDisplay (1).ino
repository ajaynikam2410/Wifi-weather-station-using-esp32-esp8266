#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);
const char* ssid     = "your ssid";      // SSID of local network
const char* password = "ypur password";   // Password on network
String APIKEY = "your api key";
String CityID = "your city id"; //put your city id from openwheathermap


WiFiClient client;
char servername[] = "api.openweathermap.org"; // remote server we will connect to
String result;

int  counter = 60;

String weatherDescription = "";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;
float Wind;
float Clouds;



void setup() {
  Serial.begin(115200);
  Serial.println();
  lcd.begin();
  lcd.backlight();
  lcd.clear();
 
  // connect to wifi.
   WiFi.begin(ssid, password);
  lcd.print("   Connecting");
  Serial.println("Connecting");
  

   while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    lcd.print(".");
    delay(500);
  }
  
  lcd.clear();
  lcd.print("   Connected!");
  Serial.println("Connected");
  delay(1000);

}

void loop() {
  if (counter == 60) //Get new data every 10 minutes
  {
    counter = 0;
    displayGettingData();
    delay(1000);
    getWeatherData();
  } else
  {
    counter++;
    displayWeather(weatherLocation, weatherDescription);
    delay(5000);
    serialMonitorData();
    displayConditions(Temperature, Humidity, Pressure, Wind, Clouds);
    delay(5000);
  }
}

void getWeatherData() //client function to send/receive GET request data.
{
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/weather?id=" + CityID + "&units=metric&APPID=" + APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  while (client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    result = result + c;
  }

  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);

  char jsonArray [result.length() + 1];
  result.toCharArray(jsonArray, sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';

  StaticJsonBuffer<1024> json_buf;
  JsonObject &root = json_buf.parseObject(jsonArray);
  if (!root.success())
  {
    Serial.println("parseObject() failed");
  }

  String location = root["name"];
  String country = root["sys"]["country"];
  float temperature = root["main"]["temp"];
  float humidity = root["main"]["humidity"];
  String weather = root["weather"]["main"];
  String description = root["weather"]["description"];
  float pressure = root["main"]["pressure"];
  float wind = root["wind"]["speed"];
  float clouds = root["clouds"]["all"];

  weatherDescription = description;
  weatherLocation = location;
  Country = country;
  Temperature = temperature;
  Humidity = humidity;
  Pressure = pressure;
  Wind = wind;
  Clouds = clouds;
}

void displayWeather(String location, String description)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(location);
  lcd.print(", ");
  lcd.print(Country);
  lcd.setCursor(0, 1);
  lcd.print(description);

}

void displayConditions(float Temperature, float Humidity, float Pressure, float Wind, float Clouds)
{
  lcd.clear();
  lcd.print("Temp : ");
  lcd.print(Temperature, 1);
  lcd.print((char)223);
  lcd.print("C ");

  //Printing Humidity
  lcd.setCursor(0, 1);
  lcd.print("Hum  : ");
  lcd.print(Humidity, 1);
  lcd.print("%");

  delay(5000);
  lcd.clear();

 //Printing clouds
  lcd.setCursor(0, 0);
  lcd.print("Clouds : ");
  lcd.print(Clouds, 1);
  lcd.print(" %");

  //Printing Wind
  lcd.setCursor(0, 1);
  lcd.print("Wind : ");
  lcd.print(Wind, 1);
  lcd.print(" m/s");

  delay(5000);
  lcd.clear();



  //Printing Pressure
  lcd.setCursor(0, 0);
  lcd.print("Pres : ");
  lcd.setCursor(0, 1);
  lcd.print(Pressure, 1);
  lcd.print(" hPa");


  
}

void displayGettingData()
{
  lcd.clear();
  lcd.print("Getting data");
}

void serialMonitorData()
{
  Serial.print("Temp : ");
  Serial.print(Temperature, 1);
  Serial.print("*C ");

  Serial.print("  Hum  : ");
  Serial.print(Humidity, 1);
  Serial.print("%");

  Serial.print(" Pres : ");
  Serial.print(Pressure, 1);
  Serial.print(" hPa");

  Serial.print("  Wind : ");
  Serial.print(Wind, 1);
  Serial.print(" m/s");

  Serial.print("  Clouds : ");
  Serial.print(Clouds, 1);
  Serial.println(" %");
}
