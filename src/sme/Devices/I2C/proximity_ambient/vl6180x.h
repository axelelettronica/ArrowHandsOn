/*
 * vl6180x.h
 *
 * Created: 3/1/2015 10:32:19 PM
 *  Author: mfontane
 */ 


#ifndef VL6180X_H_
#define VL6180X_H_

/* ATTENTION CALIBRATION NEED
The factory calibrated ALS lux resolution is 0.32 lux/count for an analog gain of 1 (calibrated
without glass). The ALS lux resolution will require re-calibration in the final system where
cover glass is used. This can be done by recording the count output with and without glass
under the same conditions and multiplying the ALS lux resolution by the ratio of the two
counts as follows:


ALS lux resolution (with glass) = (RESULT__ALS_VAL (without glass) / RESULT__ALS_VAL (with glass)) × ALS lux resolution (without glass)

*/

bool vl6180x_init(void);


/* 
read both ligth and proximity
*/
bool vl6180x_get_light_range_polling(char *value);


/*
Light level (in lux) = ALS lux resolution × RESULT__ALS_VAL/ Analog Gain × 100ms /ALS integration time

The integration period is the time over which a single ALS measurement is made. The
default integration period is 100ms. Integration times in the range 50-100 ms are
recommended to reduce impact of light flicker from artificial lighting.

Eight analog gain settings are available which can be selected manually depending on the
range and resolution required.

*/
bool vl6180x_get_light_polling(char *value);

bool vl6180x_get_range__polling(char *value);


bool vl6180x_decode(uint16_t *buffer, uint16_t *data1, uint16_t *data2,  uint16_t *data3);

#endif /* VL6180X_H_ */