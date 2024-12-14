def calculate_checksum(cmd_type, data_len, data):
    """Calculate checksum for the package"""
    sum = cmd_type + data_len
    for d in data:
        sum += d
    return sum & 0xFF  # Keep only the lowest byte

def create_motion_package(motion_type, speed):
    """Create a motion control package
    
    Args:
        motion_type: Motion command type (e.g., MOTION_FORWARD = 0x01)
        speed: Motion speed (0-255)
    """
    # Constants from C code
    FRAME_HEADER = 0xAA
    CMD_MOTION = 0x01
    
    # Create package
    data = [motion_type, speed]
    data_len = len(data)
    checksum = calculate_checksum(CMD_MOTION, data_len, data)
    
    # Assemble package
    package = [
        FRAME_HEADER,    # Header
        CMD_MOTION,      # Command type
        data_len,        # Data length
        *data,          # Motion type and speed
        checksum        # Checksum
    ]
    
    return bytes(package)

MOTION_FORWARD = 0x04
forward_package = create_motion_package(MOTION_FORWARD, 100)

# 打印数据包内容（十六进制格式）
print("Forward Motion Package:", " ".join([f"{b:02X}" for b in forward_package]))