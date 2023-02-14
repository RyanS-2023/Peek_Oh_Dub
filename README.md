# Peek_Oh_Dub

Software Architecture
---Use BTstack for Bluetooth connectivity
---Implement 5 profiles for different actions associated with the accessibility switches
---Each profile should have a distinct RGB color associated with it
---Use a push button to cycle through the 5 profiles

Software Implementation
---Step 1: Hardware initialization
         Initialize GPIO pins for the push button
         Initialize GPIO pins for the 5 RGB LEDs
---Step 2: Profile initialization
         Define 5 arrays to hold the actions for each profile
         Define 5 arrays to hold the RGB values for each profile
         Initialize the arrays with the desired actions and RGB values for each profile
---Step 3: Profile cycling
         Set up an interrupt on the push button to cycle through the profiles
         When the button is pressed, the current profile index should be incremented, wrapping around to 0 when it reaches 4
---Step 4: Switch press handling
         Define an interrupt handler function for switch presses that is called whenever one of the accessibility switches is pressed
         In the handler, retrieve the action associated with the current profile index and send it over Bluetooth using BTstack
         Also set the RGB value of the LED associated with the current profile index
---Step 5: Main loop
         In the main loop, sleep to save power

A Pi Pico W Project
