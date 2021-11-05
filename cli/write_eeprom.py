import serial
import argparse
import serial.tools.list_ports
import time

def ser_write_int(arduino, value):
    arduino.write((str(value) + '\n').encode())


def ser_read_line(arduino):
    return arduino.readline().decode("utf-8").strip()


def write_eeprom_value(arduino, address, value):
    ser_write_int(arduino, 1)
    ser_write_int(arduino, address)
    ser_write_int(arduino, value)
    return ser_read_line(arduino)


def read_eeprom_value(arduino, address):
    ser_write_int(arduino, 2)
    ser_write_int(arduino, address)
    return int(ser_read_line(arduino))


def write_data_to_eeprom(arduino, data):
    for address, value in data.items():
        print(write_eeprom_value(arduino, address, value))


def validate_eeprom_data(arduino, data):
    invalid_data = dict()
    for address, expected_value in data.items():
        eeprom_value = read_eeprom_value(arduino, address)
        if eeprom_value != expected_value:
            invalid_data[address] = eeprom_value

    return invalid_data


def format_validation_msg(invalid_data, data):
    if not invalid_data:
        return "Data is valid"

    msg = ""
    for address, value in invalid_data.items():
        msg += f"Invalid data at address [{address}] expected [{data[address]}] got [{value}]\n"

    return msg


def data_from_file(file_name):
    with open(file_name) as f:
        content = f.read().splitlines()

    result = dict()
    for idx, value in enumerate(content):
        result[idx] = int(value)

    return result


availablePorts = ','.join(map(lambda port: port.device, serial.tools.list_ports.comports()))
parser = argparse.ArgumentParser(description='Writes bytes to eeprom')
parser.add_argument('--portName', required=True, help='Port name, pick one of available: [' + availablePorts + ']')
parser.add_argument('--baudRate', type=int, default=9600, help='Baud Rate (default 9600)')
parser.add_argument('--lsvFile', type=str, help='File containing Line Separated Values to write')
parser.add_argument('--validate', type=bool, default=False, help='Reads eeprom and checks if data written is the same')
args = parser.parse_args()

data = data_from_file(args.lsvFile)
with serial.Serial(port = args.portName, baudrate = args.baudRate, timeout = 3, rtscts=True) as arduino:
    # Toggle DTR to reset Arduino
    arduino.setDTR(False)
    time.sleep(1)
    # toss any data already received, see
    # http://pyserial.sourceforge.net/pyserial_api.html#serial.Serial.flushInput
    arduino.flushInput()
    arduino.setDTR(True)

    print(ser_read_line(arduino))

    write_data_to_eeprom(arduino, data)
    if args.validate:
        print("validating data")
        invalid_data = validate_eeprom_data(arduino, data)
        print(format_validation_msg(invalid_data, data))