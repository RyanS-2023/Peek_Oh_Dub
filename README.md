# Peek_Oh_Dub

Software Architecture<br>
---Use BTstack for Bluetooth connectivity<br>
---Implement 5 profiles for different actions associated with the accessibility switches<br>
---Each profile should have a distinct RGB color associated with it<br>
---Use a push button to cycle through the 5 profiles<br>

Software Implementation<br>
---Step 1: Hardware initialization<br>
---------Initialize GPIO pins for the push button<br>
---------Initialize GPIO pins for the 5 RGB LEDs<br>
---Step 2: Profile initialization<br>
---------Define 5 arrays to hold the actions for each profile<br>
---------Define 5 arrays to hold the RGB values for each profile<br>
---------Initialize the arrays with the desired actions and RGB values for each profile<br>
---Step 3: Profile cycling<br>
---------Set up an interrupt on the push button to cycle through the profiles<br>
---------When the button is pressed, the current profile index should be incremented, wrapping around to 0 when it reaches 4<br>
---Step 4: Switch press handling<br>
---------Define an interrupt handler function for switch presses that is called whenever one of the accessibility switches is pressed<br>
---------In the handler, retrieve the action associated with the current profile index and send it over Bluetooth using BTstack<br>
---------Also set the RGB value of the LED associated with the current profile index<br>
---Step 5: Main loop<br>
---------In the main loop, sleep to save power<br>
<br>
A Pi Pico W Project
