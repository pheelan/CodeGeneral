/*
  ===============================================================================================================
  QMC5883LCompass.h Library XYZ Example Sketch
  Learn more at [https://github.com/mprograms/QMC5883LCompass]

  This example shows how to get the XYZ values from the sensor.

  ===============================================================================================================
  Release under the GNU General Public License v3
  [https://www.gnu.org/licenses/gpl-3.0.en.html]
  ===============================================================================================================
*/
#include <QMC5883LCompass.h>
#include <Wire.h>
#include "MedianFilterLib2.h"


MedianFilter2<int> medianFilter(20);
QMC5883LCompass compass;

void setup() {
  Serial.begin(9600);
  compass.init();
  compass.setCalibration(-1188, 2128, -1777, 1608, -1235, 1947);

  //compass.setSmoothing(5, true);

}

void loop() {
  int x, y, z;
  int angle;

  // Read compass values
  compass.read();

  // Return XYZ readings
  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  //Serial.print("X: ");
  //Serial.println(x);
  //Serial.print(" Y: ");
  //Serial.println(y);
  //Serial.print(" Z: ");
  //Serial.print(z);
  //Serial.println();

  float heading = atan2(y, x);

  // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
  // Find yours here: http://www.magnetic-declination.com/
  // Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
  // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
  float declinationAngle = 0.22;
  heading += declinationAngle;

  // Check for wrap due to addition of declination.
  if (heading < 0)    heading += 2 * PI; // Correct for when signs are reversed.
  if (heading > 2 * PI) heading -= 2 * PI; // Correct for when heading exceeds 360-degree, especially when declination is included

  // Convert radians to degrees for readability.
  angle = int(heading * 180 / M_PI);

  //int mAngle = medianFilter.AddValue(angle);
  int mAngle = medianFilter.AddValue(x);

  //Serial.print("Heading (degrees): "); Serial.println(angle);
  //Serial.print("Heading (MEd degrees): "); Serial.println(mAngle);

  delay(10); //Lower delay causes a very eratic reading?? (500ms seems ok)

  int current;

  current = map(x, 20, 360, 0, 20);

  Serial.print("Current): "); Serial.println(current);
  
}
