
import requests
import json

# Server endpoint
server_url = 'http://192.168.137.11:3000/data'
data={
      "vehicle_id": "VH002",
      "data": {
        "payload": 37,
        "awss": 23,
        "speed": 23,
        "latitude": 11.572,
        "longitude": 79.5035,
        "pressure": {
          "565423": 93,
          "676567": 98,
          "898789": 100
        }
      }
    }

def post(data_=None):
    data["data"]["pressure"]["5654223"]=data_
    try:
        print("start")
        response = requests.post(server_url, json=data)
        print(f"Sent dummy data: {data} - Server response: {response.status_code}")
        print("end")
    except requests.exceptions.RequestException as e:
        print(f"Error sending data: {e}")
if __name__=="__main__":
    
    post(543)
