# Robotic Seed Planter
This is my final year project for my Mechatronics Engineering degree.
The project goal is to build an autonomous planter for Maize and Beans.
The planter has two ESP32's:
    One is used as the server and hosts the interface with which the farmer can interact.
    The other acts as the brain of the planter robot. It collects required parameters from the server as supplied by the farmer, and uses those to decide on which crop to plant and how it will carry out all needed operations.

## The Server
    The server is built on the first ESP using ESPAsyncWebServer and the pages are held in the SPIFFS (Serial Peripheral Interface Flash File System). It uses Access Point mode so all operations are completely offline.
    The server has 8 webpages: Index, Login, SignUp, Homepage, Dashboard, Planting Parameters, Manual Control, and Explore.
    The server uses ESPAsyncWebServer library to host all webpages and uses WebSockets to update all webpages with the required information. WebSockets is also used to collect the control values from the Manual Control webpage which is a joystick.
    All data is shared as Json between webpages and server.
    All user details are also saved into a json file after successful registration.

## The Brain (a.k.a the Controller)


## Update: 3rd of August 2023
- Updated the server-side code to accept form data from the signup webpage.
- The signup page sends user details using raw POST method.
- The server-side also saves every new user detail into a json database file.

## Update: 17th of August 2023
- Added navigation side window to the Dasboard and Manual Control pages.
- Added planting process indicator to the Dashboard.
- Login page sends the user details using raw POST method and the server receives them. No verification added yet.
- Server now collects manual control data from the webpage using raw POST method.
- Server now collects Planting Parameters instructions from webpage successfully.
- Added a "Stop Planting" button to the Dashboard and connected it to the server-side.

## Update: 25th of August 2023
- Added a DNS feature.
- Added a Captive Portal feature to the Access Point.
- Removed the user details saving to database on the server-side feature.
- Changed the Login, Sign-Up, Planting Parameters, and Manual Controller webpages to send their data in json format to the server using fetch API.
- Updated the server-side code to collect the json format response from all the webpages successfully.

## Update: 28th of August 2023
- Added signup details verification feature on server-side.
- Restored and updated the code for json database to save user details upon successful registeration.
- Added login verification code to server-side.
- Added websocket server to update the Dashboard webpage with required information.

## Update: 4th Sep 2023
- Added server code to send error message to Login and SignUp pages upon failed verification.
- Connected all webpages to previously created websocket server.
- Added dedicated websocket server to collect control values from Manual Control webpage instead of POST method, thereby increasing speed of transmission.
- Added dynamic "Start/Stop Planting" button for Dashboard.