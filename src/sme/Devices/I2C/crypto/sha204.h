/*
 * sha204.h
 *
 *  Created on: Mar 21, 2015
 *      Author: mfontane
 */

#ifndef SHA204_H_
#define SHA204_H_

#include <stdbool.h>

bool sha204_init(void);


/** \brief This function reads all 88 bytes from the configuration zone.
 *
Obtain the data by putting a breakpoint after every read and inspecting "response".

<b>Factory Defaults of Configuration Zone</b><BR>
01 23 76 ab 00 04 05 00 0c 8f b7 bd ee 55 01 00 c8 00 55 00 8f 80 80 a1 82 e0 a3 60 94 40 a0 85<BR>
86 40 87 07 0f 00 89 f2 8a 7a 0b 8b 0c 4c dd 4d c2 42 af 8f ff 00 ff 00 ff 00 1f 00 ff 00 1f 00<BR>
ff 00 ff 00 1f ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff 00 00 55 55<BR>

<b>Slot Summary</b><BR>
Slot 1 is parent key, and slot 1 is child key (DeriveKey-Roll).\n
Slot 2 is parent key, and slot 0 is child key (DeriveKey-Roll).\n
Slot 3 is parent key, and child key has to be given in Param2 (DeriveKey-Roll).\n
Slots 4, 13, and 14 are CheckOnly.\n
Slots 5 and 15 are single use.\n
Slot 8 is plain text.\n
Slot 10 is parent key and slot 10 is child key (DeriveKey-Create).\n
Slot 12 is not allowed as target.\n

<b>Slot Details</b><BR>
Byte # \t          Name    \t\t\t  Value \t\t\t  Description\n
0 - 3 \t   SN[0-3]           \t\t 012376ab   \t part of the serial number\n
4 - 7 \t   RevNum            \t\t 00040500   \t device revision (= 4)\n
8 - 12\t   SN[4-8]           \t\t 0c8fb7bdee \t part of the serial number\n
13    \t\t Reserved        \t\t\t 55       \t\t set by Atmel (55: First 16 bytes are unlocked / special case.)\n
14    \t\t I2C_Enable        \t\t 01       \t\t SWI / I2C (1: I2C)\n
15    \t\t Reserved        \t\t\t 00       \t\t set by Atmel\n
16    \t\t I2C_Address       \t\t c8       \t\t default I2C address\n
17    \t\t RFU         \t\t\t\t\t 00       \t\t reserved for future use; must be 0\n
18    \t\t OTPmode         \t\t\t 55       \t\t 55: consumption mode, not supported at this time\n
19    \t\t SelectorMode      \t\t 00       \t\t 00: Selector can always be written with UpdateExtra command.\n
20    \t\t slot  0, read   \t\t\t 8f       \t\t 8: Secret. f: Does not matter.\n
21    \t\t slot  0, write  \t\t\t 80       \t\t 8: Never write. 0: Does not matter.\n
22    \t\t slot  1, read   \t\t\t 80       \t\t 8: Secret. 0: CheckMac copy\n
23		\t\t slot  1, write  \t\t\t a1       \t\t a: MAC required (roll). 1: key id\n
24		\t\t slot  2, read   \t\t\t 82       \t\t 8: Secret. 2: Does not matter.\n
25		\t\t slot  2, write  \t\t\t e0       \t\t e: MAC required (roll) and write encrypted. 0: key id\n
26		\t\t slot  3, read   \t\t\t a3       \t\t a: Single use. 3: Does not matter.\n
27		\t\t slot  3, write  \t\t\t 60       \t\t 6: Encrypt, MAC not required (roll). 0: Does not matter.\n
28		\t\t slot  4, read   \t\t\t 94       \t\t 9: CheckOnly. 4: Does not matter.\n
29		\t\t slot  4, write  \t\t\t 40       \t\t 4: Encrypt. 0: key id\n
30		\t\t slot  5, read   \t\t\t a0       \t\t a: Single use. 0: key id\n
31		\t\t slot  5, write  \t\t\t 85       \t\t 8: Never write. 5: Does not matter.\n
32		\t\t slot  6, read   \t\t\t 86       \t\t 8: Secret. 6: Does not matter.\n
33		\t\t slot  6, write  \t\t\t 40       \t\t 4: Encrypt. 0: key id\n
34		\t\t slot  7, read   \t\t\t 87       \t\t 8: Secret. 7: Does not matter.\n
35		\t\t slot  7, write  \t\t\t 07       \t\t 0: Write. 7: Does not matter.\n
36		\t\t slot  8, read   \t\t\t 0f       \t\t 0: Read. f: Does not matter.\n
37		\t\t slot  8, write  \t\t\t 00       \t\t 0: Write. 0: Does not matter.\n
38		\t\t slot  9, read   \t\t\t 89       \t\t 8: Secret. 9: Does not matter.\n
39		\t\t slot  9, write  \t\t\t f2       \t\t f: Encrypt, MAC required (create). 2: key id\n
40		\t\t slot 10, read   \t\t\t 8a       \t\t 8: Secret. a: Does not matter.\n
41		\t\t slot 10, write  \t\t\t 7a       \t\t 7: Encrypt, MAC not required (create). a: key id\n
42		\t\t slot 11, read   \t\t\t 0b       \t\t 0: Read. b: Does not matter.\n
43		\t\t slot 11, write  \t\t\t 8b       \t\t 8: Never Write. b: Does not matter.\n
44		\t\t slot 12, read   \t\t\t 0c       \t\t 0: Read. c: Does not matter.\n
45		\t\t slot 12, write  \t\t\t 4c       \t\t 4: Encrypt, not allowed as target. c: key id\n
46		\t\t slot 13, read   \t\t\t dd       \t\t d: CheckOnly. d: key id\n
47		\t\t slot 13, write  \t\t\t 4d       \t\t 4: Encrypt, not allowed as target. d: key id\n
48		\t\t slot 14, read   \t\t\t c2       \t\t c: CheckOnly. 2: key id\n
49		\t\t slot 14, write  \t\t\t 42       \t\t 4: Encrypt. 2: key id\n
50		\t\t slot 15, read   \t\t\t af       \t\t a: Single use. f: Does not matter.\n
51		\t\t slot 15, write  \t\t\t 8f       \t\t 8: Never write. f: Does not matter.\n
52		\t\t UseFlag 0     \t\t\t\t ff       \t\t 8 uses\n
53		\t\t UpdateCount 0     \t\t 00       \t\t count = 0\n
54		\t\t UseFlag 1     \t\t\t\t ff       \t\t 8 uses\n
55		\t\t UpdateCount 1     \t\t 00       \t\t count = 0\n
56		\t\t UseFlag 2     \t\t\t\t ff       \t\t 8 uses\n
57		\t\t UpdateCount 2     \t\t 00       \t\t count = 0\n
58		\t\t UseFlag 3     \t\t\t\t 1f       \t\t 5 uses\n
59		\t\t UpdateCount 3     \t\t 00       \t\t count = 0\n
60		\t\t UseFlag 4     \t\t\t\t ff       \t\t 8 uses\n
61		\t\t UpdateCount 4     \t\t 00       \t\t count = 0\n
62		\t\t UseFlag 5     \t\t\t\t 1f       \t\t 5 uses\n
63		\t\t UpdateCount 5     \t\t 00       \t\t count = 0\n
64		\t\t UseFlag 6     \t\t\t\t ff       \t\t 8 uses\n
65		\t\t UpdateCount 6     \t\t 00       \t\t count = 0\n
66		\t\t UseFlag 7     \t\t\t\t ff       \t\t 8 uses\n
67		\t\t UpdateCount 7     \t\t 00       \t\t count = 0\n
68 - 83 \t LastKeyUse      \t\t\t 1fffffffffffffffffffffffffffffff\n
84		\t\t UserExtra\n
85		\t\t Selector    \t\t\t\t\t 00       \t\t Pause command with chip id 0 leaves this device active.\n
86		\t\t LockValue     \t\t\t\t 55       \t\t OTP and Data zones are not locked.\n
87		\t\t LockConfig    \t\t\t\t 55       \t\t Configuration zone is not locked.\n

 * \param[in]  device_id host or client device
 * \param[out] config_data pointer to all 88 bytes in configuration zone.
               Not used if NULL.
 * \return status of the operation
 */
uint8_t sha204_read_config_zone(uint8_t *config_data);
#endif /* SHA204_H_ */
