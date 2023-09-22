void packetsSent(uint16_t frames) {
  byte framesLow = lowByte(frames);
  byte framesHigh = highByte(frames);

  payload[1] = framesLow;
  payload[2] = framesHigh;
}

void getTHP(uint16_t) {
  float humidity;
  float temperature;
  float pressure;
  float avTemp;
  float avHum;
  float avPress;

  humidity = bme.readHumidity();
  temperature = bme.readTemperature();
  pressure = bme.readPressure() / 100;

  avTemp = tempMedian.AddValue(temperature);
  avHum = humMedian.AddValue(humidity);
  avPress = pressMedian.AddValue(pressure);

  Serial.print("Median Temperature: "); Serial.print(avTemp); Serial.print(": time: "); Serial.println(millis());
  Serial.print("Median Humidity: "); Serial.print(avHum); Serial.print(": time: "); Serial.println(millis());
  Serial.print("Median Pressure: "); Serial.print(avPress); Serial.print(": time: "); Serial.println(millis());

  avHum = avHum ; // don't bother about decimals;
  avTemp = avTemp * 10;


  int16_t payloadTemp = (int16_t)avTemp;
  byte tempLow = lowByte(payloadTemp);
  byte tempHigh = highByte(payloadTemp);
  // place the bytes into the payload
  payload[3] = tempLow;
  payload[4] = tempHigh;

  uint16_t payloadHumid = (uint16_t)avHum;
  byte humidLow = lowByte(payloadHumid);
  byte humidHigh = highByte(payloadHumid);
  payload[5] = humidLow;
  payload[6] = humidHigh;

  uint16_t payloadPressure = (uint16_t)avPress;
  byte pressureLow = lowByte(payloadPressure);
  byte pressureHigh = highByte(payloadPressure);
  payload[37] = pressureLow;
  payload[38] = pressureHigh;

}

void getVOC() {
  int16_t voc, avVOC;

  voc = ads.readADC_SingleEnded(0);
  avVOC = vocMedian.AddValue(voc);

  byte VOCLow = lowByte(avVOC);
  byte VOCHigh = highByte(avVOC);

  Serial.print("VOC: "); Serial.print(voc); Serial.print(": time: "); Serial.println(millis());
  Serial.print("Median VOC: "); Serial.print(avVOC); Serial.print(": time: "); Serial.println(millis());

  payload[7] = VOCLow;
  payload[8] = VOCHigh;

}




void getPMS() {
  Serial.println("In PMS");
  uint16_t med_pm1_0s, med_pm2_5s, med_pm5_0s, med_pm10_0s; //Standard particle median variables
  uint16_t med_pm1_0e, med_pm2_5e, med_pm5_0e, med_pm10_0e; //Enviornmental particle median variables
  uint16_t med_pc0_3c, med_pc0_5c, med_pc1_0c, med_pc2_5c, med_pc5_0c, med_pc10_0c; //Median particle counts

  byte send_data1[8] = {0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71}; //For active mode
  //byte send_data[1] = {0x42};
  pmsSerial.write(send_data1, 7);
  //delay(50);

  if (readPMSdata(&pmsSerial)) {
    // reading data was successful!
    med_pm1_0s = pm1_0s_Median.AddValue(data.pm10_standard);
    med_pm2_5s = pm2_5s_Median.AddValue(data.pm25_standard);
    med_pm10_0s = pm10_0s_Median.AddValue(data.pm25_standard);

    med_pm1_0e = pm1_0e_Median.AddValue(data.pm10_env);
    med_pm2_5e = pm2_5e_Median.AddValue(data.pm25_env);
    med_pm10_0e = pm10_0e_Median.AddValue(data.pm25_env);

    med_pc0_3c = pc0_3c_Median.AddValue(data.particles_03um);
    med_pc0_5c = pc0_5c_Median.AddValue(data.particles_05um);
    med_pc1_0c = pc1_0c_Median.AddValue(data.particles_10um);
    med_pc2_5c = pc2_5c_Median.AddValue(data.particles_25um);
    med_pc5_0c = pc5_0c_Median.AddValue(data.particles_50um);
    med_pc10_0c = pc10_0c_Median.AddValue(data.particles_100um);

    Serial.println();
    Serial.println("FILTERED ------------------");
    Serial.println("Concentration Units (standard)");
    Serial.print("PM 1.0 Filtered: "); Serial.print(med_pm1_0s);
    Serial.print("\t\tPM 2.5 Filtered: "); Serial.print(med_pm2_5s);
    Serial.print("\t\tPM 10 Filtered: "); Serial.println(med_pm10_0s);
    Serial.println("---------------------------------------");
    Serial.println("Concentration Units (environmental)");
    Serial.print("PM 1.0 Filtered: "); Serial.print(med_pm1_0e);
    Serial.print("\t\tPM 2.5 Filtered: "); Serial.print(med_pm2_5e);
    Serial.print("\t\tPM 10 Filtered: "); Serial.println(med_pm10_0e);
    Serial.println("---------------------------------------");
    Serial.print("Particles > 0.3um / 0.1L air Filtered:"); Serial.println(med_pc0_3c);
    Serial.print("Particles > 0.5um / 0.1L air Filtered:"); Serial.println(med_pc0_5c);
    Serial.print("Particles > 1.0um / 0.1L air Filtered:"); Serial.println(med_pc1_0c);
    Serial.print("Particles > 2.5um / 0.1L air Filtered:"); Serial.println(med_pc2_5c);
    Serial.print("Particles > 5.0um / 0.1L air Filtered:"); Serial.println(med_pc5_0c);
    Serial.print("Particles > 10.0 um / 0.1L air Filtered:"); Serial.println(med_pc10_0c);
    Serial.println("---------------------------------------");


    payload[9] = lowByte(med_pm1_0s);
    payload[10] = highByte(med_pm1_0s);
    payload[11] = lowByte(med_pm2_5s);
    payload[12] = highByte(med_pm2_5s);
    payload[13] = lowByte(med_pm10_0s);
    payload[14] = highByte(med_pm10_0s);
    payload[15] = lowByte(med_pm1_0e);
    payload[16] = highByte(med_pm1_0e);
    payload[17] = lowByte(med_pm2_5e);
    payload[18] = highByte(med_pm2_5e);
    payload[19] = lowByte(med_pm10_0e);
    payload[20] = highByte(med_pm10_0e);
    payload[21] = lowByte(med_pc0_3c);
    payload[22] = highByte(med_pc0_3c);
    payload[23] = lowByte(med_pc0_5c);
    payload[24] = highByte(med_pc0_5c);
    payload[25] = lowByte(med_pc1_0c);
    payload[26] = highByte(med_pc1_0c);
    payload[27] = lowByte(med_pc2_5c);
    payload[28] = highByte(med_pc2_5c);
    payload[29] = lowByte(med_pc5_0c);
    payload[30] = highByte(med_pc5_0c);
    payload[31] = lowByte(med_pc10_0c);
    payload[32] = highByte(med_pc10_0c);
  }
}
boolean readPMSdata(Stream * s) {
  if (! s->available()) {
    Serial.println("1");
    return false;
  }

  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    Serial.println("2");
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    Serial.println("3");
    return false;
  }

  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // get checksum ready
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  for (uint8_t i = 2; i < 32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();


  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}



void getCO2() {
  uint8_t buf[8];
  uint8_t i = 0;
  uint16_t CO2 = 0;
  uint16_t avCO2 = 0;


  Wire.requestFrom(0x8, 6);

  while (Wire.available()) {
    buf[i] = Wire.read();
    i++;
  }

  byte CO2Low = buf[5]; //Low Byte
  byte CO2High = buf[4];

  CO2 = CO2High * 256;  //Turn into single int16
  CO2 = CO2 + CO2Low;

  avCO2 = co2Median.AddValue(CO2);  //Apply median filter

  payload[34] = highByte(avCO2); //High Byte
  payload[33] = lowByte(avCO2); //Low Byte


  Serial.print("Median CO2: "); Serial.print(avCO2); Serial.print(": time: "); Serial.println(millis());

}

void pid() {
  int16_t pid;
  int16_t avPID;

  pid = ads.readADC_SingleEnded(1);
  avPID = pidMedian.AddValue(pid);

  byte PIDLow = lowByte(avPID);
  byte PIDHigh = highByte(avPID);

  Serial.print("PID: "); Serial.print(avPID); Serial.print(": time: "); Serial.println(millis());

  payload[35] = PIDLow;
  payload[36] = PIDHigh;
}
