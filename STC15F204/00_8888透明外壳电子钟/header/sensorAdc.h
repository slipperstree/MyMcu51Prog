#ifndef __TEMP_BRIGHT_SENSOR_ADC_H_
#define __TEMP_BRIGHT_SENSOR_ADC_H_

enum EnumADCBrightMode {
	ADC_BRIGHT_MODE_DAY,
    ADC_BRIGHT_MODE_CLOUD,
	ADC_BRIGHT_MODE_NIGHT
    };

void ADC_init();

unsigned char ADC_GetTempretureAdcValue();
unsigned char ADC_GetBrightAdcValue();
unsigned char ADC_GetTempreture();
enum EnumADCBrightMode ADC_GetBright();

#endif
