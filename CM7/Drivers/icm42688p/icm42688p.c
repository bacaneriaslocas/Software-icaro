#include "icm42688p.h"

static SPI_HandleTypeDef *icm_spi;
static GPIO_TypeDef *icm_cs_port;
static uint16_t icm_cs_pin;

#define CS_LOW()   HAL_GPIO_WritePin(icm_cs_port, icm_cs_pin, GPIO_PIN_RESET)
#define CS_HIGH()  HAL_GPIO_WritePin(icm_cs_port, icm_cs_pin, GPIO_PIN_SET)


bool  icm_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *CS_port, uint16_t CS_pin)
{

    icm_spi = spi;
    icm_cs_port = CS_port;
    icm_cs_pin = CS_pin;

    icm_write_reg(DEVICE_CONFIG, 0b00000000);
    icm_write_reg(DRIVE_CONFIG, 0b00000101);
    icm_write_reg(INT_CONFIG, 0b00111011);
    icm_write_reg(INT_CONFIG1, 0b01100000); // INT1 as data ready pulse
    icm_write_reg(INT_SOURCE0, 0b00001000); // Activa el bit 3 (UI_DRDY_INT1_EN)
    icm_write_reg(FIFO_CONFIG, 0b00000000);
    icm_write_reg(PWR_MGMT0, 0b00001111);
    icm_write_reg(GYRO_CONFIG0, 0b01000010);    // +-500 dps y 16kHz
    icm_write_reg(ACCEL_CONFIG0, 0b00100010);   // +-8 g y 16kHz
    		/*
    		 * Falta añadir la configuracion de los pines int para la recepcion de señales
    		 */
    uint8_t whoami = icm_read_reg(WHO_AM_I);

    return whoami == 0x47;
}

icm_accel_t icm_read_acc(void)
{
    icm_accel_t raw_data = {0};
    icm_accel_t data = {0};
    uint8_t raw[6];

    raw[0] = icm_read_reg(ACCEL_DATA_X1);
    raw[1] = icm_read_reg(ACCEL_DATA_X0);
    raw[2] = icm_read_reg(ACCEL_DATA_Y1);
    raw[3] = icm_read_reg(ACCEL_DATA_Y0);
    raw[4] = icm_read_reg(ACCEL_DATA_Z1);
    raw[5] = icm_read_reg(ACCEL_DATA_Z0);

    // 2️⃣ Combinar bytes (big-endian, MSB primero)
    int16_t x_raw = (int16_t)((raw[0] << 8) | raw[1]);
    int16_t y_raw = (int16_t)((raw[2] << 8) | raw[3]);
    int16_t z_raw = (int16_t)((raw[4] << 8) | raw[5]);

    raw_data.x = (float)x_raw;
    raw_data.y = (float)y_raw;
    raw_data.z = (float)z_raw;

    float lsb_per_g;

    lsb_per_g = 4096.0f;   // ±8 g creo

    // 4️⃣ Escalar los valores
    data.x = raw_data.x / lsb_per_g - 0.033f;
    data.y = raw_data.y / lsb_per_g + 0.011f;
    data.z = raw_data.z / lsb_per_g + 0.0035f;

    return data;
}

icm_gy_t icm_read_gy(void){
    icm_gy_t raw_data = {0};
    icm_gy_t data = {0};
    uint8_t raw[6];

    raw[0] = icm_read_reg(GYRO_DATA_X1);
    raw[1] = icm_read_reg(GYRO_DATA_X0);
    raw[2] = icm_read_reg(GYRO_DATA_Y1);
    raw[3] = icm_read_reg(GYRO_DATA_Y0);
    raw[4] = icm_read_reg(GYRO_DATA_Z1);
    raw[5] = icm_read_reg(GYRO_DATA_Z0);

    // 2️⃣ Combinar bytes (big-endian, MSB primero)
    int16_t x_raw = (int16_t)((raw[0] << 8) | raw[1]);
    int16_t y_raw = (int16_t)((raw[2] << 8) | raw[3]);
    int16_t z_raw = (int16_t)((raw[4] << 8) | raw[5]);

    raw_data.x = (float)x_raw;
    raw_data.y = (float)y_raw;
    raw_data.z = (float)z_raw;

    float lsb_per_dps;

    lsb_per_dps = 65.5f;   // 500 dps

    // 4️⃣ Escalar los valores
    data.x = raw_data.x / lsb_per_dps;
    data.y = raw_data.y / lsb_per_dps;
    data.z = raw_data.z / lsb_per_dps;

    return data;
}

uint8_t icm_read_reg(uint8_t reg){

    uint8_t tx[2] = { (uint8_t)(0x80 | reg), 0x00 };  // bit7=1 → lectura
    uint8_t rx[2] = {0};
    CS_LOW();
    HAL_SPI_TransmitReceive(icm_spi, tx, rx, 2, 100); // 1er byte: comando, 2º: dato recibido
    CS_HIGH();
    return rx[1];
}
void icm_write_reg(uint8_t reg, uint8_t val){

    uint8_t tx[2] = { (uint8_t)(reg & 0x7F), val };
    CS_LOW();
    HAL_SPI_Transmit(icm_spi, tx, 2, 100);
    CS_HIGH();
}
