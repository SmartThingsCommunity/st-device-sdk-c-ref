# Capability sample

## 1. Introduction

capability_sample contain capability sample files.
Each capability sample file consists of basic functionality by using capability helper file.
You can use sample files by copying them to your app path.

## 2. Details

capability helper file : iot_caps_helper_{CAPABILITY}.h 
- exist in iot_core(IOT_CORE/src/include/caps/)
- explain schema of capability as C style
- define strings required for capability operation
- example: ID of capability, name of attribute, string enum value.

capability sample file : caps_{CAPABILITY}.h, caps_{CAPABILITY}.c
- exist in app path
- function and variable for attribute and command of capability.
- initialize caps_data by using caps_{CAPABILITY}_initialize
- set/get attribute value by using set_{ATTRIBUTE}_value / get_{ATTRIBUTE}_value
- send attribute value by using attr_{ATTRIBUTE}_send
- when command is called, set attribute value and call cmd_{COMMAND}_usr_cb, and send attrivute value.

## 3. How to use capability helper
initialize & register callback function
- initialize caps_data and set init callback by using caps_{CAPABILITY}_initialize
- set user command callback, if needed.
```
cap_switch_data = caps_switch_initialize(ctx, "main", cap_switch_init_cb, NULL);
cap_switch_data->cmd_on_usr_cb = cap_switch_cmd_cb;
cap_switch_data->cmd_off_usr_cb = cap_switch_cmd_cb
```

send attribute value
- set attribute value by using set_{ATTRIBUTE}_value
- send attribute value by using attr_{ATTRIBUTE}_send
```
cap_threeAxis_data->set_threeAxis_value(cap_threeAxis_data, x, y, z);
cap_threeAxis_data->attr_threeAxis_send(cap_threeAxis_data);
```


command callback function
- before callback function call, attribute values was setted in normal case.
- get attribute value by using get_{ATTRIBUTE}_value and do action.
- after callback function call, attribute value will be sent to server.
```
static void cap_switchLevel_cmd_cb(struct caps_switchLevel_data *caps_data){
        int value = caps_data->get_level_value(caps_data);
        control_switch_level(value);
}
```

