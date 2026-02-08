#include "utilities.h"
#include "sensor_interrupts.h"
#include <stdint.h>
#include "core_cm7.h"

void SystemClock_Config(void);

as5600_t enc;
float orientacion[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
float velocidad[3] = {0, 0, 0};
float aceleracion[3] = {0, 0, 0};

uint32_t micros(void){
    // SystemCoreClock = Hz del CPU
    return (uint32_t)(DWT->CYCCNT / (SystemCoreClock / 1000000U));
}

int main(void) {
  sys_init(1);

    // Habilita TRC (Trace) para poder usar CYCCNT
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    // Resetea el contador
    DWT->CYCCNT = 0;
    // Habilita el contador de ciclos
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    uint32_t tiempo = 0;

    aceleracion[0] = 0;
    aceleracion[1] = 0;
    aceleracion[2] = 0;
    velocidad[0] = 0;
    velocidad[1] = 0;
    velocidad[2] = 0;
    float x,y,z;

    for (int i = 0; i <= 500; i++) {
      icm_gy_t cal = icm_read_gy();
      x = x + cal.x;
      y = y + cal.y;
      z = z + cal.z;
    }
    x = x / 500;
    y = y / 500;
    z = z / 500;

    icm_accel_t ac = icm_read_acc();

    float ax = ac.x, ay = ac.y, az = ac.z;
    float an = sqrtf(ax*ax + ay*ay + az*az);

    ax /= an; ay /= an; az /= an;

    float alpha = atan2f(ay, az);   // roll


    float c = cosf(alpha), s = sinf(alpha);
    float Rx[3][3] = {
      {1, 0, 0},
      {0, c, -s},
      {0,s, c}
    };

    mat3_transfer(Rx, orientacion);
    mat3_transfer(orientacion, actitud);


  while (1) {

    uint32_t tim = micros();
    uint32_t intervalo = tim - tiempo;
    tiempo = tim;

    if(icm_flag){
      icm_flag = false;
      // Procesar interrupcion del ICM42688P
          icm_accel_t ac = icm_read_acc();
          icm_gy_t gy = icm_read_gy();
    }
    if (lps_flag) {
        lps_flag = false;
        // Procesar interrupcion del LPS22HBTR
        float pressure = lps_read_pressure();
    }
    if (bmigy_flag) {
        bmigy_flag = false;
        // Procesar interrupcion del BMI088 GYRO
        bmi_gy_t gy = bmi_read_gy();
    }
    if (bmiacc_flag) {
        bmiacc_flag = false;
        // Procesar interrupcion del BMI088 ACC
        bmi_accel_t ac = bmi_read_acc();
    }

/*
    icm_accel_t ac = icm_read_acc();
    icm_gy_t gy = icm_read_gy();

    float w = sqrtf(gy.x*gy.x + gy.y*gy.y + gy.z*gy.z);

    float ux = (gy.x - x) / w;
    float uz = -(gy.y - y) / w;
    float uy = (gy.z - z) / w;

    float theta = w * 0.0174533f;

    theta = theta * ((float)intervalo / 1000000.0f);

    float c  = cosf(theta);
    float s  = sinf(theta);
    float one_c = 1.0f - c;


    float rotacion[3][3] = {
        { c + ux*ux*one_c,      ux*uy*one_c - uz*s,   ux*uz*one_c + uy*s },
        { uy*ux*one_c + uz*s,   c + uy*uy*one_c,      uy*uz*one_c - ux*s },
        { uz*ux*one_c - uy*s,   uz*uy*one_c + ux*s,   c + uz*uz*one_c }
    };

    float C[3][3];

    mat3_mul(orientacion, rotacion, C);
    mat3_transfer(C, orientacion);

    mat3_transfer(orientacion, actitud);

    aceleracion[0] = ac.x * orientacion[0][0] + ac.y * orientacion[0][1] +
                     ac.z * orientacion[0][2];
    aceleracion[1] = ac.x * orientacion[1][0] + ac.y * orientacion[1][1] +
                     ac.z * orientacion[1][2];
    aceleracion[2] = ac.x * orientacion[2][0] + ac.y * orientacion[2][1] +
                     ac.z * orientacion[2][2];

    aceleracion[0] = aceleracion[0] * 9.81f;
    aceleracion[1] = aceleracion[1] * 9.81f;
    aceleracion[2] = (aceleracion[2] - 1.0f) * 9.81f;

    velocidad[0] += aceleracion[0] * ((float)intervalo / 1000000.0f);
    velocidad[1] += aceleracion[1] * ((float)intervalo / 1000000.0f);
    velocidad[2] += aceleracion[2] * ((float)intervalo / 1000000.0f);

    posicion[0] += velocidad[0] * ((float)intervalo / 1000000.0f);
    posicion[1] += velocidad[1] * ((float)intervalo / 1000000.0f);
    posicion[2] += velocidad[2] * ((float)intervalo / 1000000.0f);

*/
   //HAL_Delay(1);
  }
}

