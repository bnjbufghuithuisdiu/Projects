import serial
port_address=""
uart_obj=serial.Serial(port_address)
uart_obj.baudrate=9600
uart_obj.open()
def print_resp():
    if(uart_obj.available()):
        resp=uart_obj.readline()
        print("Response:",resp)
def write_command(comm):
    print("command:",comm)
    uart_obj.write(comm)
    print_resp()
write_command(b'AT')
write_command(b'AT+CGNSPWR=1')
write_command(b'AT+CGNSTST=1')
while True:
    print_resp()
uart_obj.close()



        
        
