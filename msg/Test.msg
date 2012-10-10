# challenge_msgs/Link
# contains information about a directed link between two nodes.
#

string from_node                   # The source node's unique id
string to_node                     # The destination node's unique id

# link properties
uint8[8] quality                 # link quality [1-255]
uint32 a
uint32 b
uint16 c
uint8  d
int16[] i16arr
int32   x
