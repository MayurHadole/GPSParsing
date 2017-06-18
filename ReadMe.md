/*  FILE          : GPSParsing.ino
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
