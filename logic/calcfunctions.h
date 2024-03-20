const float targetTemp = 22.0;
const float tendencetreshold = 0.05;
const float tendencebuffertreshold = 0.25;
// Start and stop values for cooling and heating, to avoid too much on and off
const float coolstart = 2.5;
const float coolstop =  1.0;
const float heatstart = 1.4;
const float heatstop =  0.6;

float fertilizer(float x) {
    float R1 = 400;
    float Vin = 3.3;
    float K = 1.3;
    //
    float Vdrop = Vin*x;
    float Rc = (Vdrop*R1)/(Vin-Vdrop);
    float EC = 1000/(Rc*K);
    float EC25 =  EC/(1+  0.019*((id(water).state-25)));
    return EC25*1000;
/*
    35  1,00
    80  1,00 
   510  0,85
   600  0,805
   660  0,81
   800  0,777
  1054  0,77
  1124  0,75
  6000  0,63
*/
}

float calctendence(float tempIn, int maxval) {
    float difference = (tempIn - targetTemp) / maxval;
    const float limit = 5.0f;
    float maxtan = atan(limit);
    if ( difference > maxtan ) {
        difference = maxtan;
    }
      if ( difference < maxtan * -1 ) {
        difference = -maxtan;
      }
    float result = tan(difference);
    //
    // Cap values to avoid overflows and problems with extremes
    if (result > limit) {
        return limit;
    }
    if (result < limit * -1) {
        return limit * -1;
    }
    return result;
}

float calctimetendence() {
    // Create bumps on the evening and morning to anticipate
    // The change from night to day and back
    // heat in the evening (15-18), cool in the morning (6-9)
    auto time = id(sntp_time).now();
    float dayminutes = (time.hour * 60) + time.minute;
    int hour = time.hour;
    if (hour >= 6 && hour < 10) {
    // Use the coolness of the morning to prepare for the day
    return sin((dayminutes-(6.f*60.f))/76.4f);        // 240 / 76.4 = PI
    }
    if (hour >= 16 && hour < 19) {
    // Use the warmth of the evening to prepare for the night
    return sin((dayminutes-(16.0f*60.0f))/57.3f) * -1.0f;  // 180 / 57.3 = PI
    }
    return 0;
}

float moving_min(float current, float min) {
    if (current < min) {
        return (min - 0.005f);  // must also add regular subtract 0.05/60=0.00083
    } else {
        return min;
    }
}

float moving_max(float current, float max) {
    if (current > max) {
        return (max + 0.005f);  // must also add regular subtract 0.05/60=0.00083
    } else {
        return max;
    }
}

void turnSolarOn() {
    id(solarpump).turn_on();
}
void turnSolarOff() {
    id(solarpump).turn_off();
};
void turnBufferOn() {
    id(bufferpump).turn_on();
}
void turnBufferOff() {
    id(bufferpump).turn_off();
};

void updatepumps() {
    if (id(bootcount).state < 2) {
        // No decision before all data is in (after 2 times booting)
        return;
    }
    float tendence = id(tendenceclimate).state;
    float watert   = id(water).state;
    float buffert  = id(buffer).state;
    float solart   = id(solar).state;

    if (abs(tendence) < tendencetreshold) {
        // target reached. Turn off the pump
        ESP_LOGI("climate", "target temperature reached: water: %f", watert);
        turnSolarOff();
    } else {
        if (tendence > 0) {
            // Cooling
            ESP_LOGI("climate", "Cooling needed: water: %f", watert);
            // Can I start cooling?
            if (watert > (solart+coolstart)) {
                ESP_LOGI("climate", "cooling");
                turnSolarOn();
            }
            // Do I need to stop cooling?
            if (watert < (solart+coolstop)) {
                ESP_LOGI("climate", "no cooling: solar %f to high", watert - solart -1);
                turnSolarOff();
            }
            // Similar for the buffer
            // Can the buffer be cooled? Use the double treshold,
            // since temperature transfer to buffer is not as effective
            if (buffert > (watert + coolstart * 2)) {
                ESP_LOGI("climate", "cooling buffer");
                turnBufferOn();
            }
            // Do I need to stop cooling the buffer?
            if (buffert < (watert + coolstop * 2)) {
                turnBufferOff();
            }
            // The buffer can also be used to cool the water
            // These values are only reached when water temperature rised extremly(heatstart*2)
            // and cooling is urgent (tendencebuffertreshold)
            // It will happen in summer when day and night temperatures differ much.
            if ((buffert < (watert - coolstart * 2)) && (abs(tendence) > tendencebuffertreshold)) {
                turnBufferOn();
            }

        } else {
            // Heating
            ESP_LOGI("climate", "Heating needed: water: %f", watert);
            // Can I start heating?
            if (watert < (solart - heatstart)) {
                ESP_LOGI("climate", "heating");
                turnSolarOn();
            }
            // Do I need to stop heating?
            if (watert > (solart - heatstop)) {
                ESP_LOGI("climate", "no heating: solar %f to low", solart - 1 - watert);
                turnSolarOff();
            }
            // Similar for the buffer
            // Can the buffer be heated? Use the double treshold,
            // since temperature transfer to buffer is not as effective
            // maybe not needed since both values change
            if (buffert < (watert - heatstart * 2)) {
                ESP_LOGI("climate", "heating buffer");
                turnBufferOn();
            }
            // Do I need to stop heating the buffer
            if (buffert > (watert - heatstop * 2)) {
                turnBufferOff();
            }
            // The buffer can also be used to heat the water
            // These values are only reached when water temperature dropped extremly(heatstart*2)
            //  and heating is urgent (tendencebuffertreshold)
            if ((buffert > (watert + heatstart * 2)) && (abs(tendence) > tendencebuffertreshold)) {
                turnBufferOn();
            }
        }
    }
}

void turnAirPumpOn() {
    id(airpump).turn_on();
}
void turnAirPumpOff() {
    id(airpump).turn_off();
}

void calc_air_in_water() {
    // This function is called every 5 minutes
    // Estimate (not calculate) the air in the water and control the air pump accordingly
    // subtract some value each 5 mins (a day has 288*5min)
    float tendence = id(tendenceclimate).state;
    float watert   = id(water).state;
    float airt     = id(air).state;
    float solart   = id(solar).state;

    float airTarget = 50;
    if (id(bootcount).state < 4) {
        // No decision before all data is in (after 2 times booting)
        return;
    }

    // Adjust the target according to the need to heat/cool and the climate conditions
    // When the air is warmer than the water and heating is needed, increase the target
    airTarget += (airt - watert) * tendence * -0.8f;

    id(air_target) = airTarget;

    // Air in water
    float aiw = id(air_in_water);
    // up to 0.05 missing due to high temperature (25C°) 0.02 at 10C°
    aiw -= 0.1f + watert/500.0f;          
    if (id(airpump).state == true) {
        // Pumping brings new air
        aiw += 1.2; // About 1/10 of the time pumping
    }
    if (aiw < airTarget) {
        turnAirPumpOn();
    } else {
        turnAirPumpOff();
    }
    id(air_in_water) = aiw;
}