# Section 4.4

"""CircuitPython Essentials HID Mouse example"""
import time
import board
from adafruit_lsm6ds.lsm6ds3 import LSM6DS3
from adafruit_lsm6ds import Rate, AccelRange, GyroRange


# Setup connection
i2c = board.I2C()  # uses board.SCL and board.SDA
sensor = LSM6DS3(i2c)
D0 = board.D0
D1 = board.D1
D2 = board.D2
D3 = board.D3

# Setup param and service


# Setup global constant
sensor.accelerometer_range = AccelRange.RANGE_8G
print(
    "Accelerometer range set to: %d G" % AccelRange.string[sensor.accelerometer_range]
)

sensor.gyro_range = GyroRange.RANGE_2000_DPS
print("Gyro range set to: %d DPS" % GyroRange.string[sensor.gyro_range])

sensor.accelerometer_data_rate = Rate.RATE_1_66K_HZ
print("Accelerometer rate set to: %d HZ" % Rate.string[sensor.accelerometer_data_rate])

sensor.gyro_data_rate = Rate.RATE_1_66K_HZ
print("Gyro rate set to: %d HZ" % Rate.string[sensor.gyro_data_rate])


# Initialization

def fill_pin(pin,gyro_x,gyro_y,gyro_z):
    if pin is D0:

        D0.fill(((gyro_x+2000)+(gyro_y+2000))/4000*256)



while True:
    acc_x,acc_y,acc_z = sox.acceleration # in m/s^2
    gyro_x, gyro_y, gyro_z = sox.gyro # in radians/s

    print("Acceleration: X:%.2f, Y: %.2f, Z: %.2f m/s^2"%(sox.acceleration))
    print("Gyro X:%.2f, Y: %.2f, Z: %.2f radians/s"%(sox.gyro))
    print("")

    led1 = 0
    led2 = 0
    led3 = 0
    led4 = 0

    if gyro_x < 0:
        led1, led2 = abs(gyro_x)/2000*V_max
    else:
        led3,led4 = abs(gyro_x)/2000*V_max
    if gyro_y < 0:
        led2, led3 = abs(gyro_y)/2000*V_max
    else:
        led1,led4 = abs(gyro_y)/2000*V_max
    if gyro_z < 0:
        led1, led3 = abs(gyro_z)/2000*V_max
    else:
        led2,led4 = abs(gyro_z)/2000*V_max

    
    if acc_x < 0:
        led1, led2 = abs(acc_x)/2000*V_max
    else:
        led3,led4 = abs(acc_x)/2000*V_max
    if acc_y < 0:
        led2, led3 = abs(acc_y)/2000*V_max
    else:
        led1,led4 = abs(acc_y)/2000*V_max
    if acc_z < 0:
        led1, led3 = abs(acc_z)/2000*V_max 
    else:
        led2,led4 = abs(acc_z)/2000*V_max

    D0.fill(led1)
    D1.fill(led2)
    D2.fill(led3)
    D3.fill(led4)

    # Tracking sensor data every 0.5s
    time.sleep(0.5)