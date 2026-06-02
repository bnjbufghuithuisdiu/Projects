
from RPLCD.i2c import CharLCD
import time

lcd = CharLCD("PCF8574", 0x27, cols=16, rows=2)

data = [
    ["Welcome ", "User"],
    ["Connecting", " ...."]
]
lcd.clear()
lcd.cursor_pos=(0,2)
lcd.write_string(data[0][0] + str(data[0][1]))
lcd.cursor_pos = (1, 0)
lcd.write_string(data[1][0] )
for i in range(0,6):
    lcd.write_string('.')
    time.sleep(0.5)
time.sleep(3)
lcd.clear()
def write(data, ind_val=0):
    lcd.cursor_pos = (0, 0)
    lcd.write_string(data[ind_val][0] + str(data[ind_val][1]))
    lcd.cursor_pos = (1, 0)
    if ind_val < 5:
        lcd.write_string(data[ind_val + 1][0] + str(data[ind_val + 1][1]))
    else:
        lcd.cursor_pos = (1, 0)
        lcd.write_string("        ")
def warn(tyr_id,mess):
    lcd.clear()
    while(True):
        lcd.cursor_pos=(0,3)
        lcd.write_string("Warning!")
        lcd.cursor_pos=(1,0)
        lcd.write_string("{t}-{m}".format(t=tyr_id,m=mess))
        time.sleep(1)
        lcd.clear()
        time.sleep(0.25)
if __name__=="__main__":
    warn('PRESSURE_RF','LOW')
