    for(uint8_t angle = 180; angle >0; angle = angle -1){
		  Set_Servo_Angle(&htim3, TIM_CHANNEL_3, angle);
		  HAL_Delay(6);
	  }

    uint16_t distance_mm = 0;
	  uint8_t dataReady = 0;

    VL53L1X_CheckForDataReady(VL53L1__ADDR, &dataReady);
    if (dataReady)
	  {
	    VL53L1X_GetDistance(VL53L1__ADDR, &distance_mm);
	    VL53L1X_ClearInterrupt(VL53L1__ADDR);
    }

    sprintf(msg, "** VL53L1_1: %u \r\n", distance_mm);
    uart_print(msg);

    float ps = lps_read_pressure();

    sprintf(msg, "** Barometro: %0.2f \r\n", ps);
    uart_print(msg);

    icm_accel_t acc = icm_read_acc();
    icm_gy_t gy = icm_read_gy();

    sprintf(msg, "** A1x=%.3f g  Ay=%.3f g  Az=%.3f g\r\n", acc.x, acc.y, acc.z);
	  uart_print(msg);

	  sprintf(msg, "** G1x=%.3f g  Gy=%.3f g  Gz=%.3f g\r\n", gy.x, gy.y, gy.z);
    uart_print(msg);

    bmi_accel_t a = bmi_read_acc();
    bmi_gy_t b = bmi_read_gy();
    char msg[80];
    sprintf(msg, "** ax=%.3f ay=%.3f az=%.3f\r\n", a.x, a.y, a.z);
    uart_print(msg);
    sprintf(msg, "** gx=%.3f gy=%.3f gz=%.3f\r\n", b.x, b.y, b.z);
    uart_print(msg);
    
    mmc5983_axes_t mg = mmc5983_read_xyz();
    sprintf(msg, "** MMC5983MA:  Mx=%.2f uT  My=%.2f uT  Mz=%.2f uT\r\n",
    mg.x, mg.y, mg.z);
    uart_print(msg);

    float Ps = getDinamicPressure();
    sprintf(msg, "** Dinamic Pressure: %.2f Pa\r\n", Ps);
    uart_print(msg);