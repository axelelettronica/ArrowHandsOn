/*
 * sha204.c
 *
 * Created: 4/14/2015 11:06:19 PM
 *  Author: smkk
 */ 

#include <stdint-gcc.h>
#include <stdbool.h>
#include "model\sha204_comm.h"
#include "model\sha204_comm_marshaling.h"
#include "model\sha204_lib_return_codes.h"
#include <string.h>

uint8_t sha204_read_config_zone(uint8_t *config_data)
{
    // declared as "volatile" for easier debugging
    volatile uint8_t ret_code;

    uint16_t config_address;

    // Make the command buffer the size of the Read command.
    uint8_t command[READ_COUNT];

    // Make the response buffer the size of the maximum Read response.
    uint8_t response[READ_32_RSP_SIZE];

    // Use this buffer to read the last 24 bytes in 4-byte junks.
    uint8_t response_read_4[READ_4_RSP_SIZE];

    uint8_t *p_response;


    // Read first 32 bytes. Put a breakpoint after the read and inspect "response" to obtain the data.

    memset(response, 0, sizeof(response));
    config_address = 0;
    ret_code = sha204m_read(command, response, SHA204_ZONE_CONFIG | READ_ZONE_MODE_32_BYTES, config_address);
    //sha204p_sleep(); IS THIS REALLY NEED ??????
    if (ret_code != SHA204_SUCCESS)
        return ret_code;

    if (config_data) {
        memcpy(config_data, &response[SHA204_BUFFER_POS_DATA], SHA204_ZONE_ACCESS_32);
        config_data += SHA204_ZONE_ACCESS_32;
    }
    // Read second 32 bytes. Put a breakpoint after the read and inspect "response" to obtain the data.
    config_address += SHA204_ZONE_ACCESS_32;
    memset(response, 0, sizeof(response));
    ret_code = sha204m_read(command, response, SHA204_ZONE_CONFIG | READ_ZONE_MODE_32_BYTES, config_address);
    //sha204p_sleep();IS THIS REALLY NEED ??????
    if (ret_code != SHA204_SUCCESS)
        return ret_code;

    if (config_data) {
        memcpy(config_data, &response[SHA204_BUFFER_POS_DATA], SHA204_ZONE_ACCESS_32);
        config_data += SHA204_ZONE_ACCESS_32;
    }

    // Read last 24 bytes in six four-byte junks.
    memset(response, 0, sizeof(response));
    ret_code = sha204c_wakeup(response); 
	if (ret_code != SHA204_SUCCESS)
		return ret_code;
     
    config_address += SHA204_ZONE_ACCESS_32;
    response[SHA204_BUFFER_POS_COUNT] = 0;
    p_response = &response[SHA204_BUFFER_POS_DATA];
    memset(response, 0, sizeof(response));
    while (config_address < SHA204_CONFIG_SIZE) {
        memset(response_read_4, 0, sizeof(response_read_4));
        ret_code = sha204m_read(command, response_read_4, SHA204_ZONE_CONFIG, config_address);
        if (ret_code != SHA204_SUCCESS) {
            //sha204p_sleep();IS THIS REALLY NEED ??????
            return ret_code;
        }
        memcpy(p_response, &response_read_4[SHA204_BUFFER_POS_DATA], SHA204_ZONE_ACCESS_4);
        p_response += SHA204_ZONE_ACCESS_4;
        response[SHA204_BUFFER_POS_COUNT] += SHA204_ZONE_ACCESS_4; // Update count byte in virtual response packet.
        config_address += SHA204_ZONE_ACCESS_4;
    }
    // Put a breakpoint here and inspect "response" to obtain the data.
    //sha204p_sleep();IS THIS REALLY NEED ??????

    if (ret_code == SHA204_SUCCESS && config_data)
        memcpy(config_data, &response[SHA204_BUFFER_POS_DATA], SHA204_CONFIG_SIZE - 2 * SHA204_ZONE_ACCESS_32);

    return ret_code;
}



uint8_t config_data[SHA204_CONFIG_SIZE];
bool sha204_init(void) {
    uint8_t dataPage[SHA204_RSP_SIZE_MIN];
    int i;
    
    for (i=0; i++; i<SHA204_RSP_SIZE_MIN)
        dataPage[i]=0;
        
    bool ret=true;
    ret = sha204c_wakeup(dataPage)==SHA204_SUCCESS;

    if  (ret)
        ret = sha204_read_config_zone(config_data);

    return ret;
}
