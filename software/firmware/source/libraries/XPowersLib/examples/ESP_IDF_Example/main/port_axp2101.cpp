#include <stdio.h>
#include <cstring>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_err.h"

#ifdef CONFIG_XPOWERS_CHIP_AXP2102

#define XPOWERS_CHIP_AXP2102
#include "XPowersLib.h"
static const char *TAG = "AXP2101";

static XPowersPMU PMU;

extern int pmu_register_read(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);
extern int pmu_register_write_byte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t len);


esp_err_t pmu_init()
{
    if (PMU.begin(AXP2101_SLAVE_ADDRESS, pmu_register_read, pmu_register_write_byte)) {
        ESP_LOGI(TAG, "Init PMU SUCCESS!");
    } else {
        ESP_LOGE(TAG, "Init PMU FAILED!");
        return ESP_FAIL;
    }

    //Turn off not use power channel
    PMU.disableDC2();
    PMU.disableDC3();
    PMU.disableDC4();
    PMU.disableDC5();

    PMU.disableALDO1();
    PMU.disableALDO2();
    PMU.disableALDO3();
    PMU.disableALDO4();
    PMU.disableBLDO1();
    PMU.disableBLDO2();

    PMU.disableCPUSLDO();
    PMU.disableDLDO1();
    PMU.disableDLDO2();


    //ESP32s3 Core VDD
    PMU.setDC3Voltage(3300);
    PMU.enableDC3();

    //Extern 3.3V VDD
    PMU.setDC1Voltage(3300);
    PMU.enableDC1();

    // CAM DVDD  1500~1800
    PMU.setALDO1Voltage(1800);
    // PMU.setALDO1Voltage(1500);
    PMU.enableALDO1();

    // CAM DVDD 2500~2800
    PMU.setALDO2Voltage(2800);
    PMU.enableALDO2();

    // CAM AVDD 2800~3000
    PMU.setALDO4Voltage(3000);
    PMU.enableALDO4();

    // PIR VDD 3300
    PMU.setALDO3Voltage(3300);
    PMU.enableALDO3();

    // OLED VDD 3300
    PMU.setBLDO1Voltage(3300);
    PMU.enableBLDO1();

    // MIC VDD 33000
    PMU.setBLDO2Voltage(3300);
    PMU.enableBLDO2();

    ESP_LOGI(TAG, "DCDC=======================================================================");
    ESP_LOGI(TAG, "DC1  :%s   Voltage:%u mV ",  PMU.isEnableDC1()  ? "ENABLE " : "DISABLE", PMU.getDC1Voltage());
    ESP_LOGI(TAG, "DC2  :%s   Voltage:%u mV ",  PMU.isEnableDC2()  ? "ENABLE " : "DISABLE", PMU.getDC2Voltage());
    ESP_LOGI(TAG, "DC3  :%s   Voltage:%u mV ",  PMU.isEnableDC3()  ? "ENABLE " : "DISABLE", PMU.getDC3Voltage());
    ESP_LOGI(TAG, "DC4  :%s   Voltage:%u mV ",  PMU.isEnableDC4()  ? "ENABLE " : "DISABLE", PMU.getDC4Voltage());
    ESP_LOGI(TAG, "DC5  :%s   Voltage:%u mV ",  PMU.isEnableDC5()  ? "ENABLE " : "DISABLE", PMU.getDC5Voltage());
    ESP_LOGI(TAG, "ALDO=======================================================================");
    ESP_LOGI(TAG, "ALDO1:%s   Voltage:%u mV",  PMU.isEnableALDO1()  ? "ENABLE " : "DISABLE", PMU.getALDO1Voltage());
    ESP_LOGI(TAG, "ALDO2:%s   Voltage:%u mV",  PMU.isEnableALDO2()  ? "ENABLE " : "DISABLE", PMU.getALDO2Voltage());
    ESP_LOGI(TAG, "ALDO3:%s   Voltage:%u mV",  PMU.isEnableALDO3()  ? "ENABLE " : "DISABLE", PMU.getALDO3Voltage());
    ESP_LOGI(TAG, "ALDO4:%s   Voltage:%u mV",  PMU.isEnableALDO4()  ? "ENABLE " : "DISABLE", PMU.getALDO4Voltage());
    ESP_LOGI(TAG, "BLDO=======================================================================");
    ESP_LOGI(TAG, "BLDO1:%s   Voltage:%u mV",  PMU.isEnableBLDO1()  ? "ENABLE " : "DISABLE", PMU.getBLDO1Voltage());
    ESP_LOGI(TAG, "BLDO2:%s   Voltage:%u mV",  PMU.isEnableBLDO2()  ? "ENABLE " : "DISABLE", PMU.getBLDO2Voltage());
    ESP_LOGI(TAG, "CPUSLDO=======================================================================");
    ESP_LOGI(TAG, "CPUSLDO:%s Voltage:%u mV",  PMU.isEnableCPUSLDO()   ? "ENABLE " : "DISABLE", PMU.getCPUSLDOVoltage());
    ESP_LOGI(TAG, "DLDO=======================================================================");
    ESP_LOGI(TAG, "DLDO1:%s   Voltage:%u mV",  PMU.isEnableDLDO1()  ? "ENABLE " : "DISABLE", PMU.getDLDO1Voltage());
    ESP_LOGI(TAG, "DLDO2:%s   Voltage:%u mV",  PMU.isEnableDLDO2()  ? "ENABLE " : "DISABLE", PMU.getDLDO2Voltage());
    ESP_LOGI(TAG, "=======================================================================");

    PMU.clearIrqStatus();

    PMU.enableVbusVoltageMeasure();
    PMU.enableBattVoltageMeasure();
    PMU.enableSystemVoltageMeasure();
    PMU.enableTemperatureMeasure();

    // It is necessary to disable the detection function of the TS pin on the board
    // without the battery temperature detection function, otherwise it will cause abnormal charging
    PMU.disableTSPinMeasure();

    // Disable all interrupts
    PMU.disableIRQ(XPOWERS_ALL_IRQ);
    // Clear all interrupt flags
    PMU.clearIrqStatus();
    // Enable the required interrupt function
    PMU.enableIRQ(
        XPOWERS_BAT_INSERT_IRQ    | XPOWERS_BAT_REMOVE_IRQ      |   //BATTERY
        XPOWERS_VBUS_INSERT_IRQ   | XPOWERS_VBUS_REMOVE_IRQ     |   //VBUS
        XPOWERS_PKEY_SHORT_IRQ    | XPOWERS_PKEY_LONG_IRQ       |   //POWER KEY
        XPOWERS_BAT_CHG_DONE_IRQ  | XPOWERS_BAT_CHG_START_IRQ       //CHARGE
        // XPOWERS_PKEY_NEGATIVE_IRQ | XPOWERS_PKEY_POSITIVE_IRQ   |   //POWER KEY
    );

    // Set the precharge charging current
    PMU.setPrechargeCurr(XPOWERS_PRECHARGE_50MA);
    // Set constant current charge current limit
    PMU.setChargerConstantCurr(XPOWERS_ICC_CHG_200MA);
    // Set stop charging termination current
    PMU.setChargerTerminationCurr(XPOWERS_CHG_ITERM_25MA);

    // Set charge cut-off voltage
    PMU.setChargerVoltageLimit(XPOWERS_CHG_VOL_4V1);

    // Set the watchdog trigger event type
    // PMU.setWatchdogConfig(XPOWERS_WDT_IRQ_TO_PIN);
    // Set watchdog timeout
    // PMU.setWatchdogTimeout(XPOWERS_WDT_TIMEOUT_4S);
    // Enable watchdog to trigger interrupt event
    // PMU.enableWatchdog();
    return ESP_OK;
}


void pmu_isr_handler()
{
// Get PMU Interrupt Status Register
    PMU.getIrqStatus();

    if (PMU.isDropWarningLevel2Irq()) {
        ESP_LOGI(TAG, "isDropWarningLevel2");
    }
    if (PMU.isDropWarningLevel1Irq()) {
        ESP_LOGI(TAG, "isDropWarningLevel1");
    }
    if (PMU.isGaugeWdtTimeoutIrq()) {
        ESP_LOGI(TAG, "isWdtTimeout");
    }
    if (PMU.isBatChargerOverTemperatureIrq()) {
        ESP_LOGI(TAG, "isBatChargeOverTemperature");
    }
    if (PMU.isBatWorkOverTemperatureIrq()) {
        ESP_LOGI(TAG, "isBatWorkOverTemperature");
    }
    if (PMU.isBatWorkUnderTemperatureIrq()) {
        ESP_LOGI(TAG, "isBatWorkUnderTemperature");
    }
    if (PMU.isVbusInsertIrq()) {
        ESP_LOGI(TAG, "isVbusInsert");
    }
    if (PMU.isVbusRemoveIrq()) {
        ESP_LOGI(TAG, "isVbusRemove");
    }
    if (PMU.isBatInsertIrq()) {
        ESP_LOGI(TAG, "isBatInsert");
    }
    if (PMU.isBatRemoveIrq()) {
        ESP_LOGI(TAG, "isBatRemove");
    }
    if (PMU.isPekeyShortPressIrq()) {
        ESP_LOGI(TAG, "isPekeyShortPress");
    }
    if (PMU.isPekeyLongPressIrq()) {
        ESP_LOGI(TAG, "isPekeyLongPress");
    }
    if (PMU.isPekeyNegativeIrq()) {
        ESP_LOGI(TAG, "isPekeyNegative");
    }
    if (PMU.isPekeyPositiveIrq()) {
        ESP_LOGI(TAG, "isPekeyPositive");
    }
    if (PMU.isWdtExpireIrq()) {
        ESP_LOGI(TAG, "isWdtExpire");
    }
    if (PMU.isLdoOverCurrentIrq()) {
        ESP_LOGI(TAG, "isLdoOverCurrentIrq");
    }
    if (PMU.isBatfetOverCurrentIrq()) {
        ESP_LOGI(TAG, "isBatfetOverCurrentIrq");
    }
    if (PMU.isBatChagerDoneIrq()) {
        ESP_LOGI(TAG, "isBatChagerDone");
    }
    if (PMU.isBatChagerStartIrq()) {
        ESP_LOGI(TAG, "isBatChagerStart");
    }
    if (PMU.isBatDieOverTemperatureIrq()) {
        ESP_LOGI(TAG, "isBatDieOverTemperature");
    }
    if (PMU.isChagerOverTimeoutIrq()) {
        ESP_LOGI(TAG, "isChagerOverTimeout");
    }
    if (PMU.isBatOverVoltageIrq()) {
        ESP_LOGI(TAG, "isBatOverVoltage");
    }
    // Clear PMU Interrupt Status Register
    PMU.clearIrqStatus();
}

#endif  /*CONFIG_XPOWERS_CHIP_AXP2102*/

