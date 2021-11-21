#ifndef CONFIG_H
#define CONFIG_H

#define ADS_TEST_APP   0U
#define SDS_TEST_APP   0U
#define DEEP_SLEEP_APP 0U
#define MAIN_APP       1U

/* Timeout for wifi connection, after this time ESP is rebooted */
#define WIFI_TIMEOUT 30U

/* Number of attempts (resets) to connect to wifi before deepsleep */
#define WIFI_CONNECTION_ATTEMPTS 3U

#endif /* CONFIG_H */