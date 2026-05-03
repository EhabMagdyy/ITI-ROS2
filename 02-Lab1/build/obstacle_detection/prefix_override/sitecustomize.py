import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/ehab/Documents/ITI_9Months/ROS/02-Lab1/install/obstacle_detection'
