#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "btstack.h"

#define PROFILE_BUTTON_PIN 2
#define PROFILE_LED_RED_PIN 16
#define PROFILE_LED_GREEN_PIN 17
#define PROFILE_LED_BLUE_PIN 18

#define SWITCH_COUNT 5
#define SWITCH_GPIO_PINS {20, 21, 22, 26, 27}

#define SWITCH_ACTION_ENTER 0x01
#define SWITCH_ACTION_BACKSPACE 0x02
#define SWITCH_ACTION_SPACE 0x03
#define SWITCH_ACTION_TAB 0x04
#define SWITCH_ACTION_SHIFT_TAB 0x05

#define PROFILE_COLORS {{0xFF, 0x00, 0x00}, {0x00, 0xFF, 0x00}, {0x00, 0x00, 0xFF}, {0xFF, 0xFF, 0x00}, {0x00, 0xFF, 0xFF}}

static btstack_packet_callback_registration_t hci_event_callback_registration;

static uint8_t current_profile_index = 0;

static uint8_t switch_gpio_pins[SWITCH_COUNT] = SWITCH_GPIO_PINS;

static uint8_t switch_action_uuids[SWITCH_COUNT] = {
        SWITCH_ACTION_ENTER,
        SWITCH_ACTION_BACKSPACE,
        SWITCH_ACTION_SPACE,
        SWITCH_ACTION_TAB,
        SWITCH_ACTION_SHIFT_TAB
};

static uint8_t profile_led_pins[3] = {
        PROFILE_LED_RED_PIN,
        PROFILE_LED_GREEN_PIN,
        PROFILE_LED_BLUE_PIN
};

// Helper function to set the RGB LED color based on the current profile index
static void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue) {
    gpio_put(profile_led_pins[0], red);
    gpio_put(profile_led_pins[1], green);
    gpio_put(profile_led_pins[2], blue);
}

// Helper function to send a switch action via Bluetooth
static void send_switch_action(uint8_t profile_index, uint8_t switch_action) {
    uint8_t event[10];
    int pos = 0;

    // Set the Bluetooth UUID for the switch action service
    event[pos++] = 0x02;
    event[pos++] = 0x01;
    event[pos++] = 0x06;
    event[pos++] = 0x03;
    event[pos++] = 0x03;
    event[pos++] = 0xF0;
    event[pos++] = 0xFF;

    // Set the profile index and switch action values
    event[pos++] = profile_index;
    event[pos++] = switch_action;

    // Send the Bluetooth event
    hci_send_acl_packet(event, pos);
}

// BTstack packet handler function
static void btstack_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    // Parse incoming Bluetooth data
    switch (packet_type) {
        case HCI_EVENT_PACKET:
            switch (hci_event_packet_get_type(packet)) {
                case HCI_EVENT_DISCONNECTION_COMPLETE:
                    printf("BT Connection closed\n");
                    break;
                case HCI_EVENT_COMMAND_COMPLETE:
                    if (HCI_EVENT_IS_COMMAND_COMPLETE(packet, hci_read_local_version_information)) {
                        // Log Bluetooth firmware version information
                        btstack_assert(hci_event_get_data_length(packet) >= sizeof(btstack_event_command_complete_t));
                        btstack_event_command_complete_t* event = (btstack_event_command_complete_t*) packet;
                        printf("BTstack version: %s\n", btstack_get_version());
                            printf("HCI Version: 0x%02x\n", hci_version);

    // Set up profiles
    AccessibilityProfile profile_1 = AccessibilityProfile({ENTER, BACKSPACE, SPACE, TAB, SHIFT_TAB}, {255, 0, 0}); // Red
    AccessibilityProfile profile_2 = AccessibilityProfile({ENTER, SPACE, SPACE, SPACE, SHIFT_TAB}, {0, 255, 0}); // Green
    AccessibilityProfile profile_3 = AccessibilityProfile({ENTER, TAB, TAB, SPACE, SHIFT_TAB}, {0, 0, 255}); // Blue
    AccessibilityProfile profile_4 = AccessibilityProfile({ENTER, TAB, SPACE, SPACE, SHIFT_TAB}, {255, 255, 0}); // Yellow
    AccessibilityProfile profile_5 = AccessibilityProfile({ENTER, BACKSPACE, BACKSPACE, SPACE, SHIFT_TAB}, {255, 0, 255}); // Purple
    AccessibilityProfile profiles[] = {profile_1, profile_2, profile_3, profile_4, profile_5};
    const int num_profiles = sizeof(profiles) / sizeof(profiles[0]);
    int current_profile_index = 0;

    // Set up RGB LED
    const uint LED_RED_PIN = 0;
    const uint LED_GREEN_PIN = 1;
    const uint LED_BLUE_PIN = 2;
    gpio_init(LED_RED_PIN);
    gpio_init(LED_GREEN_PIN);
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_put(LED_RED_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
    gpio_put(LED_BLUE_PIN, 0);

    // Set up button
    const uint BUTTON_PIN = 3;
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Main loop
    while (true) {
        // Get the state of the button
        bool button_pressed = !gpio_get(BUTTON_PIN);

        // If the button is pressed and was not previously pressed, cycle to the next profile index
        if (button_pressed && !prev_button_state) {
            current_profile_index = (current_profile_index + 1) % num_profiles;
            AccessibilityProfile current_profile = profiles[current_profile_index];
            printf("Switched to Profile %d\n", current_profile_index + 1);
            gpio_put(LED_RED_PIN, current_profile.color.r);
            gpio_put(LED_GREEN_PIN, current_profile.color.g);
            gpio_put(LED_BLUE_PIN, current_profile.color.b);
        }

        // Update the previous button state
        prev_button_state = button_pressed;

        // Handle any pending BTstack events
        while (btstack_run_loop_get_state() != BTSTACK_RUN_LOOP_STATE_IDLE) {
            btstack_run_loop_embedded_execute_once();
        }

        // Check for switch events
        for (int i = 0; i < NUM_SWITCHES; i++) {
            if (switches[i].check_event()) {
                AccessibilityAction action = profiles[current_profile_index].actions[i];
                printf("Switch %d action: %s\n", i + 1, action_names[action].c_str());
                send_bt_command(action);
            }
        }
    }

    // Cleanup
    close_bt();
    return 0;
}


