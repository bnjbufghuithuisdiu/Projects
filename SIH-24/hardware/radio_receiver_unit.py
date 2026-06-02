
from RF24 import RF24 as rf, RF24_PA_HIGH
import time

# Configuration
# Creating radio object
radio = rf(22, 0)  # 27-CE, 22-CSN
radio.begin()
# radio.setPALevel(rf.PA_HIGH)
radio.openReadingPipe(5, b"00001")
radio.startListening()
data = {
    "pr_rf": 30,
    "pr_rr": 30,
    "pr_fr": 30,
    "pr_fl": 30
}

def read_rf_data():
    if radio.available():
        readings = radio.read(35).decode("utf-8")
        try:
            print(readings)
            data["pr_rf"] = int(readings[:7]) / 100000
        except:
            print("error")
            pass
    return data
