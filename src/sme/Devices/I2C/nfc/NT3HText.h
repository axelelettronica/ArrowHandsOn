/*
 * NT3HText.h
 *
 *  Created on: Mar 26, 2015
 *      Author: smkk
 */
#ifndef NT3HTEXT_H_
#define NT3HTEXT_H_

#include <stdint-gcc.h>


/*
 * Prepare the Header of the first Record into the nfc page
 *
 * return: the used bytes within the NFC_PAGE_SIZE
 */
int firstTextRecord(uint8_t *param, uint8_t textSize);

int addTextRecord(uint8_t *param, uint8_t textSize);
#endif /* NT3HTEXT_H_ */
