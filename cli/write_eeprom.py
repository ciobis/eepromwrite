import serial
import argparse
import serial.tools.list_ports
from multiprocessing import Process


def ser_write_int(ser, value):
    ser.write((str(value) + '\n').encode())


def ser_read_line(ser):
    return ser.readline().decode("utf-8").strip()


def write_eeprom_value(ser, address, value):
    ser_write_int(ser, 1)
    ser_write_int(ser, address)
    ser_write_int(ser, value)
    return ser_read_line(ser)


def read_eeprom_value(ser, address):
    ser_write_int(ser, 2)
    ser_write_int(ser, address)
    return int(ser_read_line(ser))


def write_data_to_eeprom(ser, data):
    for address, value in data.items():
        print(write_eeprom_value(ser, address, value))


def validate_eeprom_data(ser, data):
    invalid_data = dict()
    for address, expected_value in data.items():
        eeprom_value = read_eeprom_value(ser, address)
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


def arduino_handshake(ser):
    proc = Process(target=lambda: (ser_write_int(ser, 1), ser_read_line(ser)))
    proc.start()
    proc.join(timeout=1)
    proc.terminate()

    if proc.exitcode != 0:
        arduino_handshake(ser)


availablePorts = ','.join(map(lambda port: port.device, serial.tools.list_ports.comports()))
parser = argparse.ArgumentParser(description='Writes bytes to eeprom')
parser.add_argument('--portName', required=True, help='Port name, pick one of available: [' + availablePorts + ']')
parser.add_argument('--baudRate', type=int, default=9600, help='Baud Rate (default 9600)')
parser.add_argument('--lsvFile', type=str, help='File containing Line Separated Values to write')
parser.add_argument('--validate', type=bool, default=False, help='Reads eeprom and checks if data written is the same')
args = parser.parse_args()

data = data_from_file(args.lsvFile)
with serial.Serial(args.portName, args.baudRate) as ser:
    arduino_handshake(ser)
    print("Handshake complete")

    write_data_to_eeprom(ser, data)
    if args.validate:
        invalid_data = validate_eeprom_data(ser, data)
        print(format_validation_msg(invalid_data, data))
