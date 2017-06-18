/*  FILE          : AssignmentNo5.ino
 *  PROJECT       : PROG8125-16S - Assingment #5
 *  PROGRAMMER    : Mayur Hadole (5783)
 *  FIRST VERSION : 2016-07-13
 *  DESCRIPTION   :
 *            This program accepts GPS string from the user.Then it Separates the
 *        15 different fields and prints them on serial port in appropriate format.  
 *        following are the 15 different fields and their datatypes
 *        Sentence ID  : string 
 *        UTC Time     : integer in HH hours MM minutes SS.SS seconds
 *        Latitude     : integer in DD degrees MM minutes dddd decimal degrees
 *        Latitude Direction: Character 
 *        Longitude    : integer in DDD degrees MM minutes dddd decimal degrees
 *        Longitude Direction: Character
 *        Position Fix : integer  0 = Invalid
 *                                1 = Valid SPS
 *                                2 = Valid DGPS
 *                                3 = Valid PPS
 *        Sattelites Used: integer
 *        HDOP         : float (Horizontal dilution of precision)
 *        Altitude     : float
 *        Altitude Unit: character
 *        Geoid Separation : interger
 *        Distance Unit : character
 *        DGPS Age     : integer
 *        Checksum     : both character and integer
 *        
 *   Extra Features:
 *          1. Calculates and checks the checksum and shows if entered GPS
 *             string is valid or not.
 *          2. Zero padding if decimal degrees in latitude and longitude are less 
 *             than four digits
 *    
 *    Example of GPS String:
 *    $GPGGA,014729.10,4303.5753,N,08019.0810,W,1,6,1.761,214.682,M,0,M,0,*5D
 *        This code is tested on Teensy 3.1 board
 */



const uint16_t SerialBaudRate = 9600;  //Baud rate for serial communication
const uint16_t delayOf2Seconds = 2000; //wait for 2 seconds
const uint8_t CarriageReturn = 13;     // code of Carriage Return
const uint8_t GPSStringLength = 255;   //maximum length of GPS string
const uint8_t hundred = 100;           // to get last two digits of integer by using modulus
const uint16_t tenThousand = 10000;    // to get hours from the integers by using modulus ( first two digits)
const uint8_t Invalid = 0;   //position fix status
const uint8_t ValidSPS = 1;  //position fix status
const uint8_t ValidDGPS = 2; //position fix status
const uint8_t ValidPPS = 3;  //position fix status
const uint8_t minimumSatellitesUsed = 0;
const uint8_t maximumSatellitesUsed = 12;
const uint8_t firstElement = 0; //to access first element of the array
const uint8_t HexaDecimaL = 16 ; //used as a argument in strtoul function
const uint8_t checksumNotMatched = -1;
const uint8_t checksumMatched = 1;


//prototypes of the functions used in this program
void checkForReceivedGPSString();
void separeteFieldsOfGPSstring();
void printGPSfieldsOnSerialPort();
void printSentenceID();
void converAndPrintUTCtime();
void convertAndPrintLatitude();
void printLatitudeDirection();
void printAndConvertLongitude();
void printLongitudeDirection();
void printPositionFix();
void printSatellitesUsed();
void printHDOP();
void printAltitude();
void printAltitudeUnit();
void printGeoidSeparation();
void printDistanceUnit();
void printDGPSage();
void printChecksum();
void LeftZeroPadding(uint16_t intDecimalDegrees);
uint8_t checkGPSStringForErrors();

//Globals
static char GPSstring [GPSStringLength];          //character array to save GPS String
static char GPSstringCopy[GPSStringLength];      //character array to save the copy of GPS String
unsigned long longChecksum = 0;             //to save converted checksum received from GPS string
byte dataFields = 0;                //to show number fields of GPS string
const char *delimiter = "," ;       //delimiter "," for separating fields of GPS string
const char* dotDelimiter = "." ;    //delimiter "." to separate latitude and longitude strings 
char *sentenceID = NULL ;          // charecter pointer to save sentence ID of GPS String
char *UTCtime = NULL ;             // charecter pointer to save UTC time of GPS String
char *latitude = NULL ;            // charecter pointer to save latitude of GPS String
char *latitudeDirection = NULL ;   // charecter pointer to save latitude direction of GPS String
char *longitude = NULL ;           // charecter pointer to save longitude of GPS String
char *longitudeDirection = NULL ;  // charecter pointer to save longitude direction of GPS String
char *positionFix = NULL ;         // charecter pointer to save position fix of GPS String
char *satellitesUsed = NULL ;      // charecter pointer to save satellites used of GPS String
char *HDOP = NULL ;                // charecter pointer to save HDOP of GPS String
char *altitudeFromGPS = NULL ;     // charecter pointer to save altitude of GPS String
char *altitudeUnit = NULL ;        // charecter pointer to save altitude unit of GPS String
char *geoidSeparation = NULL ;     // charecter pointer to save geoid separation of GPS String
char *distanceUnit = NULL ;        // charecter pointer to save distance unit of GPS String
char *DGPSage = NULL ;             // charecter pointer to save DGPS age of GPS String
char *Checksum = NULL ;            // charecter pointer to save received checksum from GPS String


// FUNCTION     : checkForReceivedGPSString
// DESCRIPTION  :
//      This function checks for GPS string on Serial port by looking for $ sign
//   to start getting GPS string into character array until carriage returen is recieved.
// PARAMETERS   : void
// RETURNS      : void

void checkForReceivedGPSString()
{
  static uint8_t receivedByteCounter = 0;          //counter to get characters from serial port to array in serial order
  char byteRead;                                   // character variable to get cuurent character from serial port.
  while (Serial.available())                       //only execute this loop if serial data is available on serial port
  {
    if ( receivedByteCounter == 0)                //if received byte counter is 0 then (start of Serial data or GPS String)
    {
      byteRead = Serial.read();                   //read the first character from serial port to variable
      if (byteRead == '$')                        //if that character is $ then its a start of GPS string
      {
        GPSstring[receivedByteCounter] = byteRead; // get the $ character into the GPS string array
        receivedByteCounter++;                    //increment the received byte conter by 1
      }
    }
    if ( receivedByteCounter != 0)                 // if serial byte counter is not equal to 0 means to check if GPS string is detected earlier or not.
    {                                              //if GPS string is detected then
      byteRead = Serial.read();                    // get the character from serial port to the variable
      if (byteRead != CarriageReturn)              //Looking for the end of GPS string by checking for carriage return. If character is not equal to CR then
      {
        GPSstring[receivedByteCounter] = byteRead; // get that charater in a array
        receivedByteCounter++;                     //incrementing the counter
      }
      else                                         //if carriage return is detected then 
      {
        GPSstring[receivedByteCounter] = '\0' ;    // add null value as the end of the GPS string
        receivedByteCounter = 0;                   // reset the counter
      }
    }
  }
}

// FUNCTION     : separeteFieldsOfGPSstring
// DESCRIPTION  :
//      This fuction separaters GPS string into 15 different strings using library function
//    called strtok. Delimiter used to separate string is ",". 
// PARAMETERS   : void
// RETURNS      : void

void separeteFieldsOfGPSstring()
{
  sentenceID = strtok(GPSstring, delimiter);            //separates sentence id string from GPS string by adding NULL in place of comma
  if (sentenceID != NULL)                               //if address of sentence id pointer is not equal to NULL then
  {
    // data fields are not set as magic numbers because they are just to show the order number of fields
    dataFields = 1;
    UTCtime = strtok(NULL, delimiter);                  // separates UTC time string from GPS string by adding NULL in place comma. 
                                                       //starting pointer is NULL because we separated string earlier from GPS string
    if (UTCtime != NULL)                              //if address of UTCtime pointer is not equal to NULL then
    {
      dataFields = 2;         
      latitude = strtok(NULL, delimiter);            // separates latitude string from GPS string by adding NULL in place comma. 
                                                      //starting pointer is NULL because we separated string earlier from GPS string
      if (latitude != NULL)                           //if address of latitude pointer is not equal to NULL then
      {
        dataFields = 3;
        latitudeDirection = strtok(NULL, delimiter);  // separates longitude string from GPS string by adding NULL in place comma.
        if ( latitudeDirection != NULL )             //if address of latitude direction pointer is not equal to NULL then
        {
          dataFields = 4;
          longitude = strtok(NULL, delimiter);       // separates UTC time string from GPS string by adding NULL in place comma.
          if ( longitude != NULL )                   //if address of longitude pointer is not equal to NULL then
          {
            dataFields = 5;
            longitudeDirection = strtok(NULL, delimiter);   // separates longitude direction string from GPS string by adding NULL in place comma.
            if ( longitudeDirection != NULL )              //if address of longitude direction pointer is not equal to NULL then
            {
              dataFields = 6;
              positionFix = strtok(NULL, delimiter);      // separates position fix string from GPS string by adding NULL in place comma.
              if ( positionFix != NULL )                  //if address of position fix pointer is not equal to NULL then
              {
                dataFields = 7;
                satellitesUsed = strtok(NULL, delimiter);  // separates satellites used string from GPS string by adding NULL in place comma.
                if ( satellitesUsed != NULL )              //if address of satellites used pointer is not equal to NULL then
                {
                  dataFields = 8;
                  HDOP = strtok(NULL, delimiter);          // separates HDOP string from GPS string by adding NULL in place comma.
                  if ( HDOP != NULL )                      //if address of HDOP pointer is not equal to NULL then
                  {
                    dataFields = 9;
                    altitudeFromGPS = strtok(NULL, delimiter);     // separates altitude string from GPS string by adding NULL in place comma.
                    if ( altitudeFromGPS != NULL)                   //if address of altitude pointer is not equal to NULL then
                    {
                      dataFields = 10;
                      altitudeUnit = strtok(NULL, delimiter);    // separates altitude unit string from GPS string by adding NULL in place comma.
                      if ( altitudeUnit != NULL )                 //if address of altitude unit pointer is not equal to NULL then
                      {
                        dataFields = 11;
                        geoidSeparation = strtok(NULL, delimiter);     // separates geaid separation string from GPS string by adding NULL in place comma.
                        if ( geoidSeparation != NULL )                //if address of geoid separation pointer is not equal to NULL then
                        {
                          dataFields = 12;
                          distanceUnit = strtok(NULL, delimiter);     // separates diatance unit string from GPS string by adding NULL in place comma.
                          if ( distanceUnit != NULL )                 //if address of diatance unit pointer is not equal to NULL then
                          {
                            dataFields = 13;
                            DGPSage = strtok(NULL, delimiter);        // separates DGPSAge string from GPS string by adding NULL in place comma.
                            if ( DGPSage != NULL )                    //if address of DGPSage pointer is not equal to NULL then
                            {
                              dataFields = 14;
                              Checksum = strtok(NULL, delimiter);     // separates Checksum string from GPS string by adding NULL in place comma.
                              if ( Checksum != NULL )                 //if address of Checksum pointer is not equal to NULL then
                              {
                                dataFields = 15;
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}


// FUNCTION     : printGPSfieldsOnSerialPort
// DESCRIPTION  :
//      This function prints all the fields on serial port with appropriate formats by calling separate
//    function for each field. 
// PARAMETERS   : void
// RETURNS      : void

void printGPSfieldsOnSerialPort()
{
  printSentenceID();          // prints sentence id on serial port
  converAndPrintUTCtime();    // converts UTC time into appropriate format and prints on serial port
  convertAndPrintLatitude();  // converts latidute into appropriate format and prints on serial port
  printLatitudeDirection();   // prints latitude direction on serial port
  printAndConvertLongitude(); // converts longitude into appropriate format and prints on serial port
  printLongitudeDirection();  // prints longitude direction on serial port
  printPositionFix();         // prints position fix on serial port in approriate format
  printSatellitesUsed();      // pribts number of satellites used on serial port
  printHDOP();                // prints HDOP on serial port
  printAltitude();            // converts and  prints altitude on serial port
  printAltitudeUnit();        // prints altitude unit on serial port
  printGeoidSeparation();     // prints geoid separation on serial port
  printDistanceUnit();        // prints distance unit on serial port
  printDGPSage();             // converts prints DGPS age on serial port
  printChecksum();            // prints checksum on serial port
}


// FUNCTION     : printSentenceID
// DESCRIPTION  :
//      This function prints sentence ID on serial port 
// PARAMETERS   : void
// RETURNS      : void

void printSentenceID()
{
  if ( sentenceID != NULL)          // if the address of sentence id pointer in not equal to NULL then
  {
    //prints on serial port
    Serial.println("Following are the feilds of entered GPS string");
    Serial.printf("Sentence ID     :");
    Serial.printf("%s  : GPS position\n", sentenceID);  // prints sentence ID o serila port
  }
}


// FUNCTION     : converAndPrintUTCtime
// DESCRIPTION  :
//      This function first separates the UTCtime string into two strings using dot delimiter and then
//  converts them into integer.After that UTC time is printed on serial port with the following format 
//        format:  HH hours MM minutes SS.SS seconds  
// PARAMETERS   : void
// RETURNS      : void

void converAndPrintUTCtime()
{
  char* hourMinutesAndSeconds = NULL;  // character pointer to save first string of UTC time string. i.e the string before the dot delimiter
  char* milliSeconds = NULL;           // character pointer to save second string of UTC time string. i.e the string after the dot delimiter
  uint16_t intHourMinuteSecond = 0;    // to save the integer converted value hourMinuteSecond string
  uint8_t intMilliseconds = 0;         // to save the integer converted value milliSeconds string
  uint8_t hours = 0;                   // to save the hours
  uint8_t minutes = 0;                 // to save the minutes
  uint8_t seconds = 0;                 //to save the seconds
  if (UTCtime != NULL);        // if the character pointer address is not equal to NULL then
  {
    hourMinutesAndSeconds = strtok(UTCtime , dotDelimiter);  //separates hourMinutesAndSeconds string from UTCtime string by adding NULL in place of dot
    if (hourMinutesAndSeconds != NULL)                       // if the character pointer address is not equal to NULL then
    {
      milliSeconds = strtok( NULL , dotDelimiter);           //separates milliSeconds string from UTCtime string by adding NULL in place of dot
                                                             //starting pointer is NULL because we separated string earlier from UTC string
      intHourMinuteSecond = atoi(hourMinutesAndSeconds);     // converts the hourMinutesAndSeconds string into integer 
      intMilliseconds = atoi(milliSeconds);                  // converts the milliSeconds string into integer
                                                  
      seconds = intHourMinuteSecond % hundred;    // gets only units and hundreth digits of the integer ( last two digits)
                                                  //((intHourMinuteSecond % tenThousand) - seconds)  this removes the hours and seconds from the integer
                                                  //      / hundred removes the two zeros from the integer                                
      minutes = ((intHourMinuteSecond % tenThousand) - seconds) / hundred;
                                                   //    (intHourMinuteSecond - seconds ) removes seconds from the integer
                                                   //    - (minutes * hundred) removes minutes from the integer 
                                                   //    / tenThousand removes four zeros from the integer
      hours = ((intHourMinuteSecond - seconds ) - (minutes * hundred)) / tenThousand;
      //prints the UTC time in HH MM SS.SS format
      Serial.printf("UTC Time    :%d hours %d minutes %d.%d seconds\n", hours, minutes, seconds, intMilliseconds);
    }
  }
}


// FUNCTION     : convertAndPrintLatitude
// DESCRIPTION  :
//      This function first separates the latitude string into two strings using dot delimiter and then
//  converts them into integer.After that, latitude is printed on serial port with the following format 
//        format:  DD degrees MM minutes DDDD decimal Degrees. 
// PARAMETERS   : void
// RETURNS      : void

void convertAndPrintLatitude()
{
  char *degreesMinutes = NULL ;      // a char pointer to point degrees minutes string( the string before dot delimeter)
  char *decimalDegrees = NULL ;      // a char pointer to point decimal degrees string( the string after dot delimiter)
  uint16_t intDegreesMinutes = 0;    // a  variable to save integer converted degrees and minute
  uint8_t intDegrees = 0;            // a variable to save integer converted degrees
  uint8_t intMinutes = 0;            // a variable to save integer converted minutes
  uint16_t intDecimalDegrees = 0;    //a variable to save integer converted decimal degrees 
  if ( latitude != NULL)        // if the character pointer address is not equal to NULL then
  {
    degreesMinutes = strtok( latitude , dotDelimiter );              //separates degreeMinutes string from latitude string by adding NULL in place of dot
    if (degreesMinutes != NULL)                                      // if the character pointer address is not equal to NULL then
    {
      decimalDegrees = strtok ( NULL , dotDelimiter);                //separates decimal degrees string from latitude string by adding NULL in place of dot
                                                                     //starting pointer is NULL because we separated string earlier from latitude string
      
    }
    intDegreesMinutes = atoi(degreesMinutes);       // converts string into integer
    intDecimalDegrees = atoi(decimalDegrees);      // converts string into integer
    intMinutes = intDegreesMinutes % hundred ;     // gets only units and hundreth digits of the integer ( last two digits)
    intDegrees = (intDegreesMinutes - intMinutes ) / hundred ;  //       (intDegreesMinutes - intMinutes ) subtracts minutes from the degree minites integer
                                                                //        / hundred    removed two zeros from the right side
    Serial.println("GPS Coordinates");                                   //prints on serial port
    Serial.printf("\tLatidude    :%d degrees %d minutes ", intDegrees, intMinutes);
    LeftZeroPadding(intDecimalDegrees);       //in a number digits after decimal points are important so this function makes sure if the decimall digit
                                              // is only one digit on 10 raised to -3 position then it will print 00D decimal digits on serial port 
  }
}


// FUNCTION     : LeftZeroPadding
// DESCRIPTION  :
//      This function adds zeros in front of decimal degrees if its values are like 0.0001 ,0.001, 01 etc.
//   this function is required because integer datatype neglects the zeros of left side of number.
// PARAMETERS   :
//          uint8_t intDecimalDegrees: a argument that sends decimal degrees 
// RETURNS      : void

void LeftZeroPadding(uint16_t intDecimalDegrees)
{
  if (intDecimalDegrees < 10)                                       //if decimal degrees is less than 10 then
  {
    Serial.printf("000%d decimal degrees\n", intDecimalDegrees);    //Pad the number with 3 zeros 
  }
  else if (intDecimalDegrees < 100 && intDecimalDegrees > 9 )       //if decimal degrees are greater than 9 and less than 100 then     
  {
    Serial.printf("00%d decimal degrees\n", intDecimalDegrees);    //Pad the number with two zeros
  }
  else if (intDecimalDegrees < 1000 && intDecimalDegrees > 99)     //if decimal degrees is are greater than 99 and less than 1000
  {
    Serial.printf("0%d decimal degrees\n", intDecimalDegrees);     //Pad the number with one zero
  }
  else if (intDecimalDegrees > 999)                                //if decimal degrees are greater than 999 then
  {
    Serial.printf("%d decimal degrees\n", intDecimalDegrees);     //don't pad the number with zero
  }
}


// FUNCTION     : printLatitudeDirection
// DESCRIPTION  :
//      This function prints the latitude direction on the serial port
// PARAMETERS   : void
// RETURNS      : void

void printLatitudeDirection()
{
  if (latitudeDirection != NULL)              // if the character pointer address is not equal to character N then
  {
    if (latitudeDirection[firstElement] == 'N')          //if the latitude direction is equal to character N then
    {
      Serial.printf("\t\t\tMeasured North of the equator\n");  // print North on serial port
    }
    else if (latitudeDirection[firstElement] == 'S')    // if the character pointer address is not equal to character S then
    {
      Serial.printf("\t\t\tMeasured South of the equator\n"); // print South on serial port
    }
  }
}


// FUNCTION     : convertAndPrintLongitude
// DESCRIPTION  :
//      This function first separates the longitude string into two strings using dot delimiter and then
//  converts them into integer.After that, longitude is printed on serial port with the following format 
//        format:  DDD degrees MM minutes DDDD decimal Degrees. 
// PARAMETERS   : void
// RETURNS      : void


void printAndConvertLongitude()
{
  char *degreesMinutes = NULL;               // a char pointer to point degrees minutes string( the string before dot delimeter)
  char *decimalDegrees = NULL;               // a char pointer to point decimal degrees string( the string after dot delimeter)
  uint32_t intDegreesMinutes;                // a  variable to save integer converted degrees and minute
  uint16_t intDegrees = 0;                   // a  variable to save integer converted degrees
  uint8_t intMinutes = 0;                    // a  variable to save integer converted minutes
  uint16_t intDecimalDegrees = 0;            // a  variable to save integer converted decimal degrees
  if (longitude != NULL)                     // if the character pointer address is not equal to NULL then
  {
    degreesMinutes = strtok( longitude, dotDelimiter );           //separates degreeMinutes string from longitude string by adding NULL in place of dot
    if (degreesMinutes != NULL)                                   // if the character pointer address is not equal to NULL then
    {
      decimalDegrees = strtok (NULL , dotDelimiter);              //separates decimal degrees string from longitude string by adding NULL in place of dot
                                                                  //starting pointer is NULL because we separated string earlier from longitude string
    }
    intDegreesMinutes = atoi(degreesMinutes);     // converts string into integer
    intDecimalDegrees = atoi(decimalDegrees);     // converts string into integer
    intMinutes = intDegreesMinutes % hundred ;    // gets only units and hundreth digits of the integer ( last two digits)
    intDegrees = (intDegreesMinutes - intMinutes ) / hundred;        //       (intDegreesMinutes - intMinutes ) subtracts minutes from the degree minites integer
                                                                     //        / hundred    removed two zeros from the right side
    Serial.printf("\tLongitude   :%d degrees %d minutes ", intDegrees, intMinutes);     // prints on serial port
    LeftZeroPadding(intDecimalDegrees);             //in a number digits after decimal points are important so this function makes sure if the decimall digit
                                                    // is only one digit on 10 raised to -3 position then it will print 00D decimal digits on serial port 
  }
}


// FUNCTION     : printLongitudeDirection
// DESCRIPTION  :
//      This function prints the longitude direction on the serial port
// PARAMETERS   : void
// RETURNS      : void

void printLongitudeDirection()
{
  if (longitudeDirection != NULL)            // if the character pointer address is not equal to NULL then
  {
    if (longitudeDirection[firstElement] == 'W')        //if the latitude direction is equal to character W then
    {
      Serial.printf("\t\t\tMeasured WEST of the Greenwich England\n");     // print West on serial port
    }
    else if (latitudeDirection[firstElement] == 'E')    //if the latitude direction is equal to character E then
    {
      Serial.printf("\t\t\tMeasured EAST of the Greenwich England\n");    // print East on serial port
    }
  }
}


// FUNCTION     : printPositionFix
// DESCRIPTION  :
//      This function prints the position fix on the serial port with following format
//     Position Fix : integer     0 = Invalid
//                                1 = Valid SPS
//                                2 = Valid DGPS
//                                3 = Valid PPS
// PARAMETERS   : void
// RETURNS      : void

void printPositionFix()
{
  uint8_t intPositionFix = 0;  // variable to save integer converted position fix value
  if (positionFix != NULL )                     // if the character pointer address is not equal to NULL then
  {
    Serial.printf("Position Fix :");           //prints on serial port
    intPositionFix = atoi(positionFix);        // converts string into integer
    if (intPositionFix == Invalid)   // if position fix is equal to 0
    {
      Serial.println("Invalid");  //prints on serial port
    }
    else if (intPositionFix == ValidSPS) // if position fix is equal to 1
    {
      Serial.println("Valid SPS");       //prints on serial port  
    }
    else if (intPositionFix == ValidDGPS)  // if position fix is equal to 2
    {
      Serial.println("Valid DGPS");        //prints on serial port
    }
    else if (intPositionFix == ValidPPS)  // if position fix is equal to 3
    {
      Serial.println("Valid PPS");         //prints on serial port
    }
  }
}


// FUNCTION     : printSatellitesUsed
// DESCRIPTION  :
//      This function prints the Number of satellites used by GPS system on serial port.
//  number of satellites being used can be only between o and 12
// PARAMETERS   : void
// RETURNS      : void

void printSatellitesUsed()
{
  uint8_t intSatellitesUsed = 0;
  if ( satellitesUsed != 0)                   // if the character pointer address is not equal to NULL then
  {
    intSatellitesUsed = atoi(satellitesUsed);  // converts string into integer
    //if the satellites used are between 0 and 12 then
    if (intSatellitesUsed >= minimumSatellitesUsed  && intSatellitesUsed <= maximumSatellitesUsed)
    {
      Serial.printf("Satellites used :%d\n", intSatellitesUsed); // prints on serial port
    }
  }
}


// FUNCTION     : printHDOP
// DESCRIPTION  :
//      This function prints the Horizontal dilution of precision (HDOP) on serial port by converting it into
//   float first.
// PARAMETERS   : void
// RETURNS      : void

void printHDOP()
{
  float floatHDOP = 0;         // variable to save float converted HDOP value
  if (HDOP != NULL);                           // if the character pointer address is not equal to NULL then
  {
    floatHDOP = atof(HDOP);                   // converts string into float
    Serial.printf("Horizontal dilution of precision(HDOP) :%.3f\n", floatHDOP);  //prints hdop value on serial port with only three decimal digits
  }
}


// FUNCTION     : printAltitude
// DESCRIPTION  :
//      This function prints the altitude on serial port by converting it into
//   float first.
// PARAMETERS   : void
// RETURNS      : void

void printAltitude()
{
  float floatAltitudeFromGPS = 0 ;    //variable to save float converted altitude value
  if (altitudeFromGPS != NULL)                      // if the character pointer address is not equal to NULL then
  {
    floatAltitudeFromGPS = atof(altitudeFromGPS);   // converts string into float
    Serial.printf("Altitude : %.3f ", floatAltitudeFromGPS);   //print on serial port with three decimal digits
  }
}


// FUNCTION     : printAltitudeUnit
// DESCRIPTION  :
//      This function prints the altitude unit on serial port.
// PARAMETERS   : void
// RETURNS      : void


void printAltitudeUnit()
{
  if (altitudeUnit != NULL)                         // if the character pointer address is not equal to NULL then
  {
    if (altitudeUnit[firstElement] == 'M')      //if the altitude[0] is equat to character M then
    {
      Serial.printf("Meters\n");   // print meters on serial port
      Serial.printf("Alititude Units : Meters\n");
    }
  }
}


// FUNCTION     : printGeoidSeparation
// DESCRIPTION  :
//      This function prints the geoid separation on serial port after converting it into integer.
// PARAMETERS   : void
// RETURNS      : void

void printGeoidSeparation()
{
  uint8_t intGeoidSeparation = 0;     // variable to save integer converted geoid separation value
  if (geoidSeparation != NULL)                            // if the character pointer address is not equal to NULL then
  {
    intGeoidSeparation = atoi( geoidSeparation);          // converts string into integer
    Serial.printf("Geoid Separation : %d\n", intGeoidSeparation);  //prints on serial port
  }
}


// FUNCTION     : printDistanceUnit
// DESCRIPTION  :
//      This function prints the distance unit on serial port.
// PARAMETERS   : void
// RETURNS      : void

void printDistanceUnit()
{
  if (distanceUnit != NULL);                          // if the character pointer address is not equal to NULL then
  {
    if (distanceUnit[firstElement] == 'M')            //if the distanceUnit[0] is equal to character M then
    {
      Serial.printf("Distance Unit  : Meters\n");     //prints Meters on Serial Port
    }
  }
}


// FUNCTION     : printDGPSage
// DESCRIPTION  :
//      This function prints the DGPS age on serial port after converting it into integer.
// PARAMETERS   : void
// RETURNS      : void

void printDGPSage()
{
  uint8_t intDGPSage = 0;   //variable to save integer converted DGP age value
  if (DGPSage != NULL)                                        // if the character pointer address is not equal to NULL then
  {
    intDGPSage = atoi(DGPSage);                               // converts string into integer
    Serial.printf("DGPS Age  : %d Seconds\n", intDGPSage);  //prints on Serial port
  }
}


// FUNCTION     : printChecksum
// DESCRIPTION  :
//      This function detects the the checksum by looking for asterisk symbol and then removes the asterisk from string
//   using strtok function. After that it converts the checksum string into HEX using strtoul funtion. Then it prints the checksum
//   on Serial port
// PARAMETERS   : void
// RETURNS      : void

void printChecksum()
{
  const char *asteriskDelimiter = "*";  //delimiter used to detect checksum 
  char *checksum;                      // char pointer to point asterist removed checksum string
  if (Checksum != NULL)                                        // if the character pointer address is not equal to NULL then
  {
    checksum = strtok( Checksum , asteriskDelimiter);           //separates asterisk from checksum string by adding NULL in place of dot
    longChecksum = strtoul(checksum, NULL, HexaDecimaL);            //this functions converts the  string checksum string from start to NULL character into hexadecimal unsigned long value 
    Serial.printf("Checksum dec:%d\n\t hex:%X \n", longChecksum, longChecksum);  //prints checksum in HEX and Decimal
  }
}


// FUNCTION     : checkGPSStringForErrors
// DESCRIPTION  :
//      This function calculates checksum of GPS string and compares it with checksum provided in the GPS string. if it matches
//   function will return 1 else it will return -1
// PARAMETERS   : void
// RETURNS      : 
//        uint8_t       1    if it matched
//                     -1    if it isn't 

uint8_t checkGPSStringForErrors()
{
  const char *checksumDelimiter ="$*" ;                //delimiter to separate string from$ sign to * sign
  char *GPSstringForChecksum;                         //pointer to point string used to calculate checksum
  GPSstringForChecksum = strtok(GPSstringCopy,checksumDelimiter);
  Serial.println(GPSstringForChecksum);             // prints on serial port
  char checksumByte = 0;                           // to save the calculated checksum result
  //this for loop will XOR i'th character with (i + 1)th character of string and store the result in checkbyte character
  for(uint8_t i = 0 ; i < strlen(GPSstringForChecksum); i++)
  {
    checksumByte = char (checksumByte ^ GPSstringForChecksum[i]);  // XOR operation
  }
  Serial.printf("Calculated Checksum :%X\n",checksumByte);              //prints on serial port
  Serial.printf("Received Checksum :%X\n",longChecksum);               //prints on serial port
  if(checksumByte == longChecksum)                                     //if calculated checksum and received checksum are equal then
  {
    return checksumMatched;    //return 1
  }
  else
  {
    return checksumNotMatched; //return -1
  }
}
void setup()
{
  Serial.begin(SerialBaudRate);            //initialize serial port with 9600 baud rate
  delay(delayOf2Seconds);                 // wait for serial port tobe initialized
  Serial.println("Enter the GPS String"); //prints on serial port

}


void loop()
{
  uint8_t cheksumResult;  // to save the return value of checkGPSStringForErrors function
  if (Serial.available() > 0) // if data in serial port is more than zero then
  {
    checkForReceivedGPSString();         //gets the GPS string from serial port by looking for $ sign
    Serial.printf("%s \n", GPSstring);   //prints GPS string on serial port
    strcpy(GPSstringCopy,GPSstring);     //makes the copy of GPS string
    separeteFieldsOfGPSstring();         //separates the fields of GPS Field
    printGPSfieldsOnSerialPort();        //print GPS fields in appropriate datatypes and formats on serial port
    cheksumResult = checkGPSStringForErrors();  //calls the funtion to calculate checksum and saves the return value in checksum result
    if(cheksumResult == checksumMatched)  // if the checksums are matched
    {
      Serial.println("Entered GPS String is corect"); //print on serial port
      
    }
    else
    {
      Serial.println("Entered GPS String has errors"); //print on serial port
    } 

  }
}
