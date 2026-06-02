
from Data_transmission_unit import post_server as pt
from Data_transmission_unit import display_unit as lcd
from Data_transmission_unit import radio_receiver_unit as rf
from Data_transmission_unit import GPS_MODULE as gps
from Data_transmission_unit import payload as pl
import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)
GPIO.setup(27, GPIO.IN)
GPIO.setup(17, GPIO.IN)
GPIO.setup(25, GPIO.OUT)
GPIO.setup(18, GPIO.OUT)
GPIO.setup(23, GPIO.OUT)
GPIO.setup(24, GPIO.OUT)
GPIO.setup(7, GPIO.OUT)

ind_val = 0

def bundle_data(data_rf, data_pl, data_gps):
    global data
    data = [
        ['PRESSURE_RF : ', data_rf['pr_rf']],
        ['PRESSURE_RR : ', data_rf['pr_rr']],
        ['PRESSURE_FR : ', data_rf['pr_fr']],
        ['PRESSURE_FL : ', data_rf['pr_fl']],
        ['CUR_SF_LOAD : ', data_pl],
        ['CUR_SPEED : ', data_gps[-1]]
    ]

def swt_ind():
    global ind_val
    if ind_val < 5 and GPIO.input(27):
        ind_val += 1
    elif ind_val > 0 and GPIO.input(17):
        ind_val -= 1
def flush():
    GPIO.output(18, 0)
    GPIO.output(23, 0)
    GPIO.output(24, 0)
    GPIO.output(7, 0)
    break

while True:
    try:
        GPIO.output(18,1)
        data_gps = gps.read_gps_data()
        data_rf = rf.read_rf_data()
        data_pl = 20
        bundle_data(data_rf, data_pl, data_gps)
        swt_ind()

        print(ind_val)
        print(GPIO.input(17), GPIO.input(27))
        lcd.write(data, ind_val)
        if data[0][1] > 90:
            GPIO.output(25, 1)
            print("alert activated-HG")
            mess='HIGH'
            warn(data[0][0],mess)
            GPIO.output(23,1)
        elif(data[0][1]<60):
            GPIO.output(25, 1)
            GPIO.output(23,1)
            print("alert activated-LW")
            mess='LOW'
            warn(data[0][0],mess)
        time.sleep(1)
        GPIO.output(25, 0)
        GPIO.output(23,0)
        pt.post(data[0][1])
    except:
        flush()
        
