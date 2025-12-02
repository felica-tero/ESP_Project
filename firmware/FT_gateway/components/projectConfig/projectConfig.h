/**
 * @file projectConfig.h
 * @author your name (you@domain.com)
 * @brief 
 * @details
 * @version 0.1
 * @date 2025-05-21
 * 
 * @copyright Public Domain (or CC0 licensed, at your option.) 2025
 * 
 */

#ifndef __PROJECT_CONFIG_LIB__
#define __PROJECT_CONFIG_LIB__

// #define BOARD_ESP32C6
#define BOARD_ESP32S3

#define TRUE    1
#define FALSE   0

#define HIGH    1
#define LOW     0

// RGB LED GPIOs
#if defined BOARD_ESP32C6
#define RGB_LED_RED_GPIO	12
#define RGB_LED_GREEN_GPIO	13
#define RGB_LED_BLUE_GPIO	14

#elif defined BOARD_ESP32S3
#define RGB_LED_RED_GPIO	2
#define RGB_LED_GREEN_GPIO	3
#define RGB_LED_BLUE_GPIO	4

#endif
    // RGB LED color mix channels
#define RGB_LED_CHANNEL_NUM	3 ///> qty of led rgb pins


// WI-FI
#define WIFI_AP_SSID			"WAKE_N_DANCE"		// Access Point SSID
#define WIFI_AP_PASSWORD		"12345678"			// AP password
#define WIFI_AP_CHANNEL			1					// AP channel
#define WIFI_AP_SSID_HIDDEN		0					// AP visible = 0
#define WIFI_AP_MAX_CONNECTIONS	5					// how many people can connect to AP
#define WIFI_AP_BEACON_INTERVAL	100					// MAYBE IT SHOULD BE BIGGER FOR NOT GETTING IN THE WAY OF SONG
#define WIFI_AP_IP				"192.168.0.1"		// AP default IP
#define WIFI_AP_GATEWAY			"192.168.0.1"		// AP default gateway
#define WIFI_AP_NETMASK			"255.255.255.0"     // AP netmask
#define WIFI_AP_BANDWIDTH		WIFI_BW_HT20		// AP bandwidth = 20 MHz, the 40 will be for station
#define WIFI_STA_POWER_SAVE		WIFI_PS_NONE		// Power save is not being used
#define WIFI_SSID_LENGTH		32					// IEEE standard maximum
#define WIFI_PASSWORD_LENGTH	64					// IEEE standard maximum
#define MAX_CONNECTION_RETRIES	5					// retry numbers on disconnect

// DISPLAY OLED
#if defined BOARD_ESP32C6
#define DISPLAY_OLED_SDA_PIN    17
#define DISPLAY_OLED_SCL_PIN    18
#define DISPLAY_OLED_PRESENT    FALSE

#elif defined BOARD_ESP32S3
#define DISPLAY_OLED_SDA_PIN    17
#define DISPLAY_OLED_SCL_PIN    18
#define DISPLAY_OLED_PRESENT    FALSE

#endif



// DIGITAL OUTPUTS
#if defined BOARD_ESP32C6
#define DIG_OUT_0       4
#define DIG_OUT_1       5
#define DIG_OUT_2       6
#define DIG_OUT_3       7

#elif defined BOARD_ESP32S3
#define DIG_OUT_0       15
#define DIG_OUT_1       16
#define DIG_OUT_2       17
#define DIG_OUT_3       18

#endif
// QTY of DIG_OUT pins
#define QTD_DIG_OUTS 	4

#endif //__PROJECT_CONFIG_LIB__
