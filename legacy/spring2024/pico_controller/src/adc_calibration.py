import machine
import utime


pot = machine.ADC(26)
led_blue = machine.Pin(14, machine.Pin.OUT)
led_yellow = machine.Pin(15, machine.Pin.OUT)
led_red = machine.Pin(17, machine.Pin.OUT)
led_green = machine.Pin(16, machine.Pin.OUT)

led_blue.value(0)
led_red.value(0)
led_green.value(0)
led_yellow.value(0)

state = 0;

DR1_PIN = machine.Pin(10, machine.Pin.OUT)
DR2_PIN = machine.Pin(11, machine.Pin.OUT)
DR3_PIN = machine.Pin(12, machine.Pin.OUT)
DR4_PIN = machine.Pin(13, machine.Pin.OUT)

ESTOP = machine.Pin(18, machine.Pin.IN)

TARGET = machine.ADC(27)

seq = [
    [1,1,0,0],
    [0,1,1,0],
    [0,0,1,1],
    [1,0,0,1]
]

print("ADC Calibration Startup...")

while True:
        val = pot.read_u16()
        volt = val*(3.3 / 65535)
        #print(volt)
        
        target = TARGET.read_u16()
        target_volt = target*(3.3/65535)
        #print(target_volt)
        
        
        
        #if state == 0:
            #print("0")
        #elif state == 1:
            #print("1")
        #elif state == 2:
            #print("2")
        #elif state == 3:
            #print("3")
        target_volt_min = target_volt - 0.1
        target_volt_max = target_volt + 0.1
        
        #print(ESTOP.value())
        estop = ESTOP.value()
        if(estop):
            led_red.value(1)
            led_blue.value(0)
            led_yellow.value(0)
            led_green.value(0)
            #print("ESTOP")
            
            DR1_PIN.value(0)
            DR2_PIN.value(0)
            DR3_PIN.value(0)
            DR4_PIN.value(0)
        
        elif(volt < target_volt_max and volt > target_volt_min):
            # In target area
            #print("=Target")
            led_red.value(0)
            led_blue.value(0)
            led_yellow.value(0)
            led_green.value(1)
            
            DR1_PIN.value(0)
            DR2_PIN.value(0)
            DR3_PIN.value(0)
            DR4_PIN.value(0)
        elif(volt < target_volt_min):
            # Clockwise
            #print("<Target")
            state = (state-1) % 4
            led_red.value(0)
            led_green.value(0)
            led_blue.value(1)
            
            current_seq = seq[state % len(seq)]

            DR1_PIN.value(current_seq[0])
            DR2_PIN.value(current_seq[1])
            DR3_PIN.value(current_seq[2])
            DR4_PIN.value(current_seq[3])
            utime.sleep(0.005)
        elif(volt > target_volt_max):
            # Counter-Clockwise
            #print(">Target")
            state = (state+1) % 4
            led_red.value(0)
            led_green.value(0)
            led_yellow.value(1)
            
            current_seq = seq[state % len(seq)]

            DR1_PIN.value(current_seq[0])
            DR2_PIN.value(current_seq[1])
            DR3_PIN.value(current_seq[2])
            DR4_PIN.value(current_seq[3])
            utime.sleep(0.005)
            
        
