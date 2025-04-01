extends Node


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var _json_data = {
		"test": "this is a test string",
		"test_byte_array": PackedByteArray([1,2,3,0]),
		"test_int_array": PackedInt32Array([12,32,43]),
		"test_color_array": PackedColorArray([Color.RED, Color.GREEN]),
		"test_array": [PackedColorArray([Color.RED, Color.GREEN]), ["test"]],
		"test_vector": Vector3(0, 1, 2)
	}
	
	print(JSON.stringify(_json_data))
	var _test_data = var_to_bytes([PackedColorArray([Color.RED, Color.GREEN]), ["test"]])
	print(_test_data)
