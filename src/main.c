#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
//*** 2 library บน สำคัญที่สุด ***
#include <driver/gpio.h>
#include "freertos/event_groups.h" //จะมองในระดับ bit
#include <esp_log.h>

#define TAG "EVENT_GROUP"

// Example
// bit0 = 1
// bit1 = 1 0
// bit2 = 1 0 0
// #define LED_RED GPIO_NUM_1
// #define LED_YELLOW GPIO_NUM_3
// #define LED_GREEN GPIO_NUM_4

#define USER_PIN GPIO_NUM_0
#define D1_GPIO_PIN GPIO_NUM_4   // LED: D1 on my board
#define D2_GPIO_PIN GPIO_NUM_3  // LED: D2 on my board
#define D3_GPIO_PIN GPIO_NUM_3 // LED: D3 on my board
#define D4_GPIO_PIN GPIO_NUM_1   // LED: D4 on my board
#define SW1_GPIO_PIN GPIO_NUM_10 // SW1 on my board
#define SW2_GPIO_PIN GPIO_NUM_7 // SW2 on my board
#define SW3_GPIO_PIN GPIO_NUM_6 // SW3 on my board
#define SW4_GPIO_PIN GPIO_NUM_5 // SW4 on my board

EventGroupHandle_t event_group;
const int task1_event = BIT0;
const int task2_event = BIT1;
const int event_bit = task1_event | task2_event;

static task1_task(void *pvParameter)
{
    while (true)
    {
        // int switch_state = gpio_get_level(SWITCH_GPIO_PIN);

        if (gpio_get_level(SW1_GPIO_PIN) == 0)
        {
            ESP_LOGI(TAG, "Living Room Light ON");
            gpio_set_level(D1_GPIO_PIN, 1);
            xEventGroupSetBits(event_group, task1_event);
            // ESP_LOGI(TAG, "Set event form task1");
            // vTaskDelay(5000 / portTICK_PERIOD_MS);
        }

        else
        {
            ESP_LOGI(TAG, "Living Room Light OFF");
            gpio_set_level(D1_GPIO_PIN, 0);
            // vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

static task2_task(void *pvParameter)
{
    while (true)
    {
        if (gpio_get_level(SW2_GPIO_PIN) == 0)
        {
            ESP_LOGI(TAG, "Kitchen Light ON");
            gpio_set_level(D2_GPIO_PIN, 1);
            xEventGroupSetBits(event_group, task2_event);
            // ESP_LOGI(TAG, "Set event form task2");
            // vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
        else
        {
            ESP_LOGI(TAG, "Kitchen Light OFF");
            gpio_set_level(D2_GPIO_PIN, 0);
            // vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

static void set_up()
{
    // gpio_config_t io_conf = {
    //     .pin_bit_mask = (1ULL << USER_PIN), // เป็นการ shift bit
    //     .mode = GPIO_MODE_OUTPUT
    // };
    // gpio_config(&io_conf);

    // gpio_config_t input_conf = {
    //     .pin_bit_mask = (1ULL << SWITCH_GPIO_PIN), // เป็นการ shift bit
    //     .mode = GPIO_MODE_INPUT
    // };
    // gpio_config(&input_conf);
    gpio_reset_pin(USER_PIN);
    gpio_set_direction(USER_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(D1_GPIO_PIN);
    gpio_set_direction(D1_GPIO_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(D2_GPIO_PIN);
    gpio_set_direction(D2_GPIO_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(D3_GPIO_PIN);
    gpio_set_direction(D3_GPIO_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(D3_GPIO_PIN, 0);

    gpio_reset_pin(D4_GPIO_PIN);
    gpio_set_direction(D4_GPIO_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(SW1_GPIO_PIN);
    gpio_set_direction(SW1_GPIO_PIN, GPIO_MODE_INPUT);

    gpio_reset_pin(SW2_GPIO_PIN);
    gpio_set_direction(SW2_GPIO_PIN, GPIO_MODE_INPUT);

    gpio_reset_pin(SW3_GPIO_PIN);
    gpio_set_direction(SW3_GPIO_PIN, GPIO_MODE_INPUT);

    gpio_reset_pin(SW4_GPIO_PIN);
    gpio_set_direction(SW4_GPIO_PIN, GPIO_MODE_INPUT);
}

void app_main()
{
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    set_up();

    // gpio_set_level(LED_RED, 1);
    // gpio_set_level(LED_YELLOW, 1);
    // gpio_set_level(LED_GREEN, 1);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // gpio_set_level(LED_RED, 0);
    // gpio_set_level(LED_YELLOW, 0);
    // gpio_set_level(LED_GREEN, 0);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);

    event_group = xEventGroupCreate();
    xTaskCreate(task1_task, "task1_task", 2048, NULL, 1, NULL);
    xTaskCreate(task2_task, "task2_task", 2048, NULL, 2, NULL);

    while (true)
    {

        EventBits_t uxBits = xEventGroupWaitBits(event_group, event_bit, pdTRUE, pdFALSE, portMAX_DELAY); // มีใคร set bit มารึป่าว โดยจะต้องเป็น bit ที่ตรงกับ event_group ที่ตั้งไว้
        // task 1
        if ((uxBits & task1_event))
        {
            // ESP_LOGI(TAG, "Event form task1");
            ESP_LOGI(TAG, "Action: Living Room was on.");
        }

        // task 2
        if ((uxBits & task2_event))
        {
            // ESP_LOGI(TAG, "Event form task2");
            ESP_LOGI(TAG, "Action: Kitchen was on.");
        }
    }
}