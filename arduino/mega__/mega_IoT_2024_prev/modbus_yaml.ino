/*

modbus hub config
name ( name of hub)

modbus entities config
address ( of coil\register)
name ( name of entity )
...
...
...    
    binary_sensors: #only for reading
      - name: my_relay
        device_address: 1
        address: 100 # register addr
        device_class: door
  #under is for next Funcs:  
      #1 (Read Coils),    (R 1-bit IN|OUT)
      #2 (Read Discrete Inputs),  (R 1-bit IN)
      #3 !(Read Holding Registers),   (R 16-bit IN|OUT)
      #4 (Read Input Registers),    (R 16-bit IN)
      #5 (Write Single Coil),   (W 1-bit IN|OUT) 
      #6 !(Write Single Holding Register)(W 16-bit IN|OUT)
      #15 (Write Multiple Coils),   (W 1-bit IN|OUT) 
      #16 (Write Multiple Holding Registers).(W 16-bit IN|OUT) 
        
  input_type: coil #  discrete_input, coil, holding(reg) or input(reg)
        scan_interval: 15
        slave: 1
        slave_count: 0
        unique_id: my_relay

    switches: #for coils
      - name: Switch1
        address: 700
    sensors:
      - name: sensor1
        address: 600
    climates:
      - name: "Watlow F4T"
        address: 200
    covers:
      - name: Door1
        address: 300
    fans:
      - name: Fan1
        address: 400
    lights:
      - name: light1
        address: 500


# exmpl from https://www.emqx.com/en/blog/home-assistant-modbus
sensor:
  - platform: modbus
    scan_interval: 10
    registers:
      - name: Temperature
        hub: myhub
        slave: 1
        register: 0
  #register_type: input
  #count 2 # у чувачка так стояло на видео 
  #precision: 1 #(округление)
  #data_type: float
        unit_of_measurement: 'C'





 Home Assistant deals with 4 types of device:
1. binary sensors (digital readonly inputs)
2. Switch (or coils in Modbus language: digital output that can also be read)
3. Temperature sensors
4. Climate (a block that act as a thermostat)


DoWriteOutputRegister
DoResetInputRegister
DoProcessOtherRegisters

 
 */
