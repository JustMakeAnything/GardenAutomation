const float targetTemp = 22.0;

float fertilizer(float &x) {
    float R1 = 400;
    float Vin = 3.3;
    float K = 1.3;
    //
    float Vdrop = Vin*x;
    float Rc = (Vdrop*R1)/(Vin-Vdrop);
    float EC = 1000/(Rc*K);
    float EC25 =  EC/(1+  0.019*((id(water).state-25)));
    return EC25*1000;
}

float calctendence(float tempIn, int maxval) {
    float difference = tempIn - targetTemp;
      // Cap values to avoid overflows and problems with extremes
      if ( difference > maxval ) {
        return 10.0f;
      }
      if ( difference < maxval * -1 ) {
        return -10.0f;
      }
      return tan( difference / (maxval));
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

void turn_on() {
    id(usb).turn_on();
}

void wateraction() {
float tendencetreshold = 0.05;
    if(id(bootcount).state<2){
        // No decision before all data is in (after 2 times booting)
        return;
    }
    float tendence = id(tendenceclimate).state;
    float watert   = id(water).state;
    float solart   = id(solar).state;

    auto turnOn = [](){
        id(usb).turn_on();
    };
    auto turnOff = [](){
        id(usb).turn_off();
    };

    if (abs(tendence) < tendencetreshold) {
        // No need to do anything. Turn off the pump
        ESP_LOGI("climate", "Nothing to do: water: %f",watert);
        turnOff();
    }else{
        if(tendence >0){
            // Cooling
            ESP_LOGI("climate", "Cooling needed: water: %f",watert);
            // Can I start cooling?
            if(watert > (solart+2.5f)) {
            ESP_LOGI("climate", "cooling" );
            turnOn();
            }
            // Do I need to stop cooling?
            if(watert < (solart+1.0f)) {
            ESP_LOGI("climate", "no cooling: solar %f to high",watert - solart -1 );
            turnOff();
            }
        }else{
            // Heating
            ESP_LOGI("climate", "Heating needed: water: %f",watert);
            // Can I start heating?
            if(watert < (solart-1.4f)) {
            ESP_LOGI("climate", "heating" );
            turnOn();
            }
            // Do I need to stop heating?
            if(watert > (solart-0.6f)) {
            ESP_LOGI("climate", "no heating: solar %f to low",solart - 1 - watert );
            turnOff();
            }
        }
    }
}