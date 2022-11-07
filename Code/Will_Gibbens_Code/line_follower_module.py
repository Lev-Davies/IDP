# WG Created: 6/11/22 Modified: 7/11/22
# python code for a basic line following algorithm. Will likely need to be converted to arduino at some point

path_lengths = {2:16, 4:68, 6:232, 8:38, 10:38, 12:108, 14:76, 16:68, 17:16, 19:18, 20:18, 21:16}

def follow_path(path_number):
    """Code to follow a path without any junctions"""
    path_length = path_lengths(path_number)
    return path_length

def node_type_1(node_number):
    """Code to handle going straight on with a path off to the right"""
    pass

def node_type_2(node_number):
    """Code to handle going straight on with a path off to the left"""
    pass

def node_type_3(node_number):
    """Code to handle turnign right at a T-junction"""
    pass

def node_type_4(node_number):
    """Code to handle turning left at a T-junction"""
    pass

def node_type_5(node_number):
    """Code to handle going straight on at a cross roads"""
    pass

def node_type_6(node_number):
    """Code to handle turning right at a cross roads"""
    pass

def node_type_7(node_number):
    """Code to handle turning left at a cross roads"""
    pass

def node_type_8(node_number):
    """Code to handle going straight on across a region with no tape"""
    pass

def node_type_9(node_number):
    """Code to handle turning right"""
    pass

def node_type_10(node_number):
    """Code to handle turning left"""
    pass

def determine_block_type():
    """Code to determine whether blocks are high or low density"""
    pass




# Higher level functions
def collection_process(block_location):
    """Navigate around the collection half of the table"""
    if block_location == 1:
        node_type_2(7)
        follow_path(8)
        node_type_5(9)
        follow_path(10)
        node_type_2(11)
    elif block_location == 2:
        node_type_10(7)
        # Path 19
        pass
    elif block_location == 3:
        pass
    else:
        print("That is not a valid block location")
    # Path 12
    follow_path(12)
    # Node 13
    node_type_8()
    # Path 14
    follow_path(14)

def delivery_process():
    """Navigate around the delivery half of the table"""
    pass