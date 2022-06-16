# SmartMeterAdapter
With this project contains code with enables and Arduino UNO Wifi to be connected via the MBUS interface to an Landys-Gyr SmartMeter. 

The code is tested with with an SmartMeter provided by NetzBurgenland (Austria, Burgenland)

---

## Info
I wrote most of the code years ago and don't had time do document it. By the time I wrote the documentation I forgot some stuff I did. I remember that I had to modify the code for some libraries before they worked as expected.

### ArduinoHttpClient
I had problems with the HTTP Client performance in general and especially when the body in the HTTP response was empty. I did the modifications years ago forgot what I exactly did. I hate myself for not documenting those changes now...

### Crypto
Same goes for the crypto library. It was not compatible for the MicroController on the ArduinoUno Wifi Rev. 2. I did some modifications to get the one algorithm working which I needed....

---

## Credits
The code is was developed with infos and help from https://www.photovoltaikforum.com/thread/137994-landis-gyr-e450-auslesen-wie-ich-es-mache .

---

I'm not and c++ dev so the code is not that nice but it works...
