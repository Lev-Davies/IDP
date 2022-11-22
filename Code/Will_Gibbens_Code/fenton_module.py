# WG Created: 6/11/22 Modified: 12/11/22
# Python code for the overarching route planning

import serial
import time

ArduinoSerial = serial.Serial('com7',9600) #Create Serial port object called arduinoSerialData INSERT COM INTO 'COM18'
time.sleep(2) # Wait two seconds for the connection to establish
print(ArduinoSerial.readline()) #read the serial data and print it as line


path_lengths = {2:16, 4:68, 6:232, 8:38, 10:38, 12:108, 14:76, 16:68, 17:16, 19:18, 20:18, 21:16}

def follow_path(path_number, direction = "forward"):
    """Code to follow a path until the next junctions"""
    allowable_locations = [2, 4, 6, 8, 10, 12, 14, 16, 17, 19, 20, 21]
    if path_number not in allowable_locations:
        return "There is an error. {0} is not a path that can be followed.".format(path_number)
    path_length = path_lengths[path_number]
    serial_data = "follow_path"
    ArduinoSerial.write(serial_data.encode())
    return path_length

def node_type_1(node_number, direction = "forward"):
    """Code to handle going straight on with a path off to the right"""
    allowable_locations = [1, 3, 5, 7, 11, 15]
    if node_number not in allowable_locations:
        return "There is an error. {0} is not a T-junction node.".format(node_number)
    serial_data = "node_type_1"
    ArduinoSerial.write(serial_data.encode())
    pass

def node_type_2(node_number, direction = "forward"):
    """Code to handle going straight on with a path off to the left"""
    allowable_locations = [1, 3, 5, 7, 11, 15]
    if node_number not in allowable_locations:
        return "There is an error. {0} is not a T-junction node.".format(node_number)
    pass

def node_type_3(node_number, direction = "forward"):
    """Code to handle turnign right at a T-junction"""
    allowable_locations = [1, 3, 5, 7, 11, 15]
    if node_number not in allowable_locations:
        return "There is an error. {0} is not a T-junction node.".format(node_number)
    pass

def node_type_4(node_number, direction = "forward"):
    """Code to handle turning left at a T-junction"""
    allowable_locations = [1, 3, 5, 7, 11, 15]
    if node_number not in allowable_locations:
        return "There is an error. {0} is not a T-junction node.".format(node_number)
    pass

def node_type_5(node_number, direction = "forward"):
    """Code to handle going straight on at a cross roads"""
    allowable_locations = [9]
    if node_number not in allowable_locations:
        return "There is an error. {0} is not a X-junction node.".format(node_number)
    pass

def node_type_6(node_number, direction = "forward"):
    """Code to handle turning right at a cross roads"""
    allowable_locations = [9]
    if node_number not in allowable_locations:
        return "There is an error. {0} is not a X-junction node.".format(node_number)
    pass

def node_type_7(node_number, direction = "forward"):
    """Code to handle turning left at a cross roads"""
    allowable_locations = [9]
    if node_number not in allowable_locations:
        return "There is an error. {0} is not a X-junction node.".format(node_number)
    pass

def node_type_8(node_number, direction = "forward"):
    """Code to handle going straight on across a region with no tape"""
    allowable_locations = [13]
    if node_number not in allowable_locations:
        return "There is an error. {0} is not a tunnel node.".format(node_number)
    # Follow ultrasonic distance sensor
    pass

def node_type_9(node_number, direction = "forward"):
    """Code to handle turning right"""
    allowable_locations = [1, 3, 5, 7, 11, 15]
    pass

def node_type_10(node_number, direction = "forward"):
    """Code to handle turning left"""
    allowable_locations = [1, 3, 5, 7, 11, 15]
    pass

def grab_block():
    """Code to squeeze the grabber onto a block"""
    pass

def release_block():
    """Code to release a block from the grabber"""
    pass

def determine_block_type():
    """Code to determine whether blocks are high or low density"""
    return "low density"




# Higher level functions
def leave_start_zone():
    follow_path(2)
    node_type_3(3)
    follow_path(4)
    node_type_1(5)
    follow_path(6)

def collection_process(block_location):
    """Navigate around the collection half of the table"""
    if block_location == 1:
        node_type_2(7)
        follow_path(8)
        node_type_5(9)
        grab_block()
        block_type = determine_block_type()
        follow_path(10)
        node_type_2(11)
    elif block_location == 2:
        node_type_10(7)
        follow_path(19)
        grab_block()
        block_type = determine_block_type()
        follow_path(19, "backward")
        node_type_3(7, "backward")
        node_type_2(7)
        follow_path(8)
        node_type_5(9)
        follow_path(10)
        node_type_2(11)
    elif block_location == 3:
        node_type_2(7)
        follow_path(8)
        node_type_5(9)
        follow_path(10)
        node_type_10(11)
        follow_path(20)
        grab_block()
        block_type = determine_block_type()
        follow_path(20, "backward")
        node_type_10(11, "backward")
        node_type_2(11)
    else:
        return "That is not a valid block location"
    follow_path(12)
    node_type_8(13)
    return block_type

def delivery_process(block_destination):
    """Navigate around the delivery half of the table"""
    follow_path(14)
    if block_destination == "green":
        node_type_9(15)
        follow_path(21)
        release_block()
        follow_path(21, "backward")
        node_type_4(15, "backward")
    node_type_1(15)
    follow_path(16)
    node_type_1(3)
    follow_path(4)
    if block_destination == "red":
        node_type_9(5)
        follow_path(17)
        release_block()
        follow_path(17, "backward")
        node_type_4(5, "backward")
    node_type_1(5)
    follow_path(6)

