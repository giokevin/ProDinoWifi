# ProDinoWifi

### What you need
1) A ProDinoWifi board https://kmpelectronics.eu/products/prodino-wifi-v1/

### How to proceed
1) Follow the tutorial here: https://kmpelectronics.eu/tutorials-examples/prodino-wifi-tutorial/
2) Create a Self-signed certificate
  - Open the terminal and run this "openssl genrsa -out private-key.key 1024" if on windows or "openssl genrsa -des3 -out server.key 1024" if on Linux ( Generate a private key )
  - openssl req -new -key server.key -out server.csr ( Generate a Signing Request )
  - openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt ( Generate a self-signed certificate )
3) Change these according to your network:
    - const char* ssid = "Your SSID";
    - const char* password = "Your Wifi Password";
    - const char* www_username = "The Username for the Access";
    - const char* www_password = "The Password for the Username";
4) Copy paste the serverCert and the serverKey previously generated into the sketch
5) Compile and load this sketch with Arduino IDE to the board https://www.arduino.cc/en/Main/Software

  
