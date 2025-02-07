
esphome:
  name: esp111env
  friendly_name: ESP111 датчик CO2 возле включателя света

esp32:
  board: nodemcu-32s
  framework:
    type: arduino    
# Enable Home Assistant API
api:
  encryption:
    key: "r+kMEhJ9gdwxKLZHGF15n0zaonXnvzyY/9OIDSFA3H8="

ota:
  - platform: esphome
    # password: "1234567890"

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  manual_ip:
    static_ip: 192.168.10.111
    gateway: 192.168.10.1
    subnet: 255.255.255.0  
  # Enable fallback hotspot (captive portal) in case wifi connection fails
  ap:
    ssid: "Esp111Env Fallback Hotspot"
    password: "1234567890"

captive_portal:
    
web_server:
  port: 80

logger:
  level: DEBUG
  
i2c:
  sda: GPIO21
  scl: GPIO22
  scan: true
  id: bus_a

switch:
  - platform: gpio
    pin:
      number: 2
      inverted: False
    name: "Small LED"
    id: small_led
    
    
esp32_touch:
  setup_mode: false

binary_sensor:
  - platform: esp32_touch
    name: "esp111env Touch Pad"
    pin: GPIO12
    threshold: 1000
    on_press:
      then:
        - switch.turn_on: small_led
    on_release:
      then:
        - switch.turn_off: small_led
    
sensor:
  - platform: wifi_signal
    name: "ESPhome111 env wifi strength"
    update_interval: 30s

  - platform: scd4x
    id: esp111_co2
    co2:
      name: "esp111env CO2"
    temperature:
      name: "esp111env Temp"
      id: esp111_temp
    humidity:
      name: "esp111env Humidity"
      id: esp111_hum
# кнопочка в интерфейсе для калибровки 
button:
  - platform: restart
    name: "N1 ESP111-Env restart"
  - platform: template
    name: "НЕ ТРОГАТЬ! CO2 calibration"
    on_press:
      - logger.log: Calibration Pressed
      - scd4x.perform_forced_calibration:
          value: 430 # outside average Jan 2025
          id: esp111_co2
