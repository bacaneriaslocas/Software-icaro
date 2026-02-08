#include "bmi088.h"
#include <stdint.h>

static SPI_HandleTypeDef *bmi_spi;
static GPIO_TypeDef *bmiacc_cs_port;
static uint16_t bmiacc_cs_pin;
static GPIO_TypeDef *bmigy_cs_port;
static uint16_t bmigy_cs_pin;

#define ACC_CS_LOW()   HAL_GPIO_WritePin(bmiacc_cs_port, bmiacc_cs_pin, GPIO_PIN_RESET)
#define ACC_CS_HIGH()  HAL_GPIO_WritePin(bmiacc_cs_port, bmiacc_cs_pin, GPIO_PIN_SET)

#define GY_CS_LOW()   HAL_GPIO_WritePin(bmigy_cs_port, bmigy_cs_pin, GPIO_PIN_RESET)
#define GY_CS_HIGH()  HAL_GPIO_WritePin(bmigy_cs_port, bmigy_cs_pin, GPIO_PIN_SET)

static void acc_softreset(void) {
  bmigy_write_reg(ACC_SOFTRESET, SOFTRESET_CMD);
  HAL_Delay(1);
}
static void gyr_softreset(void){
    bmigy_write_reg(GYRO_SOFTRESET, 0xB6);
    HAL_Delay(35); // el gyro necesita más tiempo tras reset
}

static HAL_StatusTypeDef bmiacc_read_burst(uint8_t start_reg, uint8_t *buf, uint16_t len) {
      if (buf == NULL || len == 0) {
        return HAL_OK;
    }

    HAL_StatusTypeDef st;
    uint8_t addr = (uint8_t)(start_reg | 0x80); // bit7=1 => READ
    uint8_t dummy = 0;

    ACC_CS_LOW();

    // 1) Enviar dirección de lectura
    st = HAL_SPI_Transmit(bmi_spi, &addr, 1, 100);
    if (st != HAL_OK) { ACC_CS_HIGH(); return st; }

    // 2) Leer dummy byte (NECESARIO en ACC por SPI)
    st = HAL_SPI_Receive(bmi_spi, &dummy, 1, 100);
    if (st != HAL_OK) { ACC_CS_HIGH(); return st; }

    // 3) Leer datos
    st = HAL_SPI_Receive(bmi_spi, buf, len, 100);

    ACC_CS_HIGH();
    return st;
}
static HAL_StatusTypeDef bmigyr_read_burst(uint8_t start_reg, uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) {
        return HAL_OK;
    }

    HAL_StatusTypeDef st;
    uint8_t addr = (uint8_t)(start_reg | 0x80); // bit7=1 => READ (SPI)

    GY_CS_LOW();

    // 1) Enviar dirección de lectura
    st = HAL_SPI_Transmit(bmi_spi, &addr, 1, 100);
    if (st != HAL_OK) {  GY_CS_HIGH(); return st; }

    // 2) Leer datos (en GYRO NO hay dummy byte)
    st = HAL_SPI_Receive(bmi_spi, buf, len, 100);

    GY_CS_HIGH();
    return st;
}

bool bmi_init(SPI_HandleTypeDef *spi, GPIO_TypeDef *ACC_CS_port, uint16_t ACC_CS_pin, GPIO_TypeDef *GY_CS_port, uint16_t GY_CS_pin)
{

    bmi_spi = spi;
    bmiacc_cs_port = ACC_CS_port;
    bmiacc_cs_pin = ACC_CS_pin;
    bmigy_cs_port = GY_CS_port;
    bmigy_cs_pin = GY_CS_pin;

    ACC_CS_LOW();
	HAL_Delay(1);
    ACC_CS_HIGH();
    HAL_Delay(50);

    uint8_t accid = 0;
    uint8_t gyid = 0;

    accid = bmiacc_read_reg(ACC_CHIP_ID);
    gyid = bmigy_read_reg(GYRO_CHIP_ID);

    acc_softreset();
    HAL_Delay(50);

    //  configuracion acelerometro
    bmiacc_write_reg(ACC_PWR_CONF, 0x00); // ACC_PWR_CONF (modo activo)
    bmiacc_write_reg(ACC_PWR_CTRL, 0x04); // ACC_PWR_CTRL (enable accel)
    HAL_Delay(5);

    bmiacc_write_reg(ACC_CONF,0b10101100);     // 1600hz
    bmiacc_write_reg(ACC_RANGE, 0b00000010);   // +-12 g
    HAL_Delay(1);

    // ACC interrupts: map data-ready to INT1 (INT1 -> PE0)
    // Note: values assume BMI088 default mapping (INT1_DRDY bit = 1)
    bmiacc_write_reg(INT1_IO_CTRL,0b00001010); // push-pull, active high, output enabled
    bmiacc_write_reg(INT2_IO_CTRL,0b00000000); // disable INT2
    bmiacc_write_reg(INT_MAP_DATA, 0b00000100); // DRDY -> INT1
    HAL_Delay(1);

    gyr_softreset();
    HAL_Delay(50);
    //  configuracion giroscopio
    bmigy_write_reg(GYRO_RANGE,0b00000001);          // 1000 dps
    HAL_Delay(1);
    bmigy_write_reg(GYRO_BANDWIDTH,0b00000001);      // 2000hz

    // GY interrupts: data-ready on INT3 (INT3 -> PI4)
    bmigy_write_reg(GYRO_INT_CTRL, 0b10000000); // enable data-ready interrupt
    HAL_Delay(1);
    bmigy_write_reg(INT3_INT4_IO_CONF,0b00000001);   // open drain, active high,
    HAL_Delay(1);
    bmigy_write_reg(INT3_INT4_IO_MAP,0b00000001);    // DRDY -> INT3


    return (accid == 0x1E) && (gyid == 0x0F);
}



bmi_accel_t bmi_read_acc(void)
{
    bmi_accel_t data = {0};

	uint8_t raw[6];
	bmiacc_read_burst(ACC_X_LSB, raw, 6);

	int16_t x_raw = (int16_t)((raw[1] << 8) | raw[0]);
	int16_t y_raw = (int16_t)((raw[3] << 8) | raw[2]);
	int16_t z_raw = (int16_t)((raw[5] << 8) | raw[4]);

    // Ajusta esto al rango real que hayas configurado en ACC_RANGE
    float lsb_per_g = 2730.0f; 

    data.x = (float)x_raw / lsb_per_g;
    data.y = (float)y_raw / lsb_per_g;
    data.z = (float)z_raw / lsb_per_g;

    return data;
}

bmi_gy_t bmi_read_gy(void){
    bmi_gy_t data = {0};

    uint8_t raw[6];
    bmigyr_read_burst(RATE_X_LSB, raw, 12);

    // En el gyro: Rate_X = MSB*256 + LSB (LSB primero en dirección baja) :contentReference[oaicite:5]{index=5}
     int16_t x_raw = (int16_t)((raw[1] << 8) | raw[0]);
    int16_t y_raw = (int16_t)((raw[3] << 8) | raw[2]);
    int16_t z_raw = (int16_t)((raw[5] << 8) | raw[4]);

    float lsb_per_dps = 32.768f;

    data.x = (float)x_raw / lsb_per_dps;
    data.y = (float)y_raw / lsb_per_dps;
    data.z = (float)z_raw / lsb_per_dps;

    return data;
}

uint8_t bmiacc_read_reg(uint8_t reg) {

  	uint8_t txBuf[3] = {reg | 0x80, 0x00, 0x00};
	uint8_t rxBuf[3];

    ACC_CS_LOW();
	uint8_t status = (HAL_SPI_TransmitReceive(bmi_spi, txBuf, rxBuf, 3, 100) == HAL_OK);
    ACC_CS_HIGH();

	if (status == 1) {

		return rxBuf[2];
	}
}
void bmiacc_write_reg(uint8_t reg, uint8_t val){

    uint8_t tx[2] = { (uint8_t)(reg & 0x7F), val };
    ACC_CS_LOW();
    HAL_SPI_Transmit(bmi_spi, tx, 2, 100);
    ACC_CS_HIGH();
}

uint8_t bmigy_read_reg(uint8_t reg){

  	uint8_t txBuf[2] = {reg | 0x80, 0x00};
	uint8_t rxBuf[2];

    GY_CS_LOW();
	uint8_t status = (HAL_SPI_TransmitReceive(bmi_spi, txBuf, rxBuf, 2, 100) == HAL_OK);
    GY_CS_HIGH();

	if (status == 1) {

		return rxBuf[1];
	}
}
void bmigy_write_reg(uint8_t reg, uint8_t val){

    uint8_t tx[2] = { (uint8_t)(reg & 0x7F), val };
    GY_CS_LOW();
    HAL_SPI_Transmit(bmi_spi, tx, 2, 100);
    GY_CS_HIGH();
}