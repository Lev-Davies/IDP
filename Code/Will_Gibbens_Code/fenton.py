# WG Created: 12/11/22 Modified: 21/11/22
# Python code for Fenton

import fenton_module as fm

fm.leave_start_zone()

for i in range(1, 4): # cycles through for blocks 1, 2 and 3
    block_type = fm.collection_process(i)

    if block_type == "high density":
        block_destination = "red"
    elif block_type == "low density":
        block_destination = "green"
    else:
        block_destination = "green"
        print("An invalid block_type was detected")
        
    fm.delivery_process(block_destination)
