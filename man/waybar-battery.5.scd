waybar-battery(5)

# NAME

waybar - battery module

# DESCRIPTION

The *battery* module displays the current capacity and state (eg. charging) of your battery.

# CONFIGURATION

*bat* ++
	typeof: string ++
	The battery to monitor, as in /sys/class/power_supply/ instead of auto detect.

*adapter* ++
	typeof: string ++
	The adapter to monitor, as in /sys/class/power_supply/ instead of auto detect.

*interval* ++
	typeof: integer ++
	default: 60 ++
	The interval in which the information gets polled.

*states* ++
	typeof: array ++
	A number of battery states which get activated on certain capacity levels. See *waybar-states(5)*.

*format* ++
	typeof: string ++
	default: {capacity}% ++
	The format, how the time should be displayed.

*format-icons*
	typeof: array/object
	Based on the current capacity, the corresponding icon gets selected. ++
	The order is *low* to *high*. Or by the state if it is an object.

*max-length* ++
	typeof: integer++
	The maximum length in character the module should display.

*rotate* ++
	typeof: integer++
	Positive value to rotate the text label.

*on-click* ++
	typeof: string ++
	Command to execute when clicked on the module.

*on-click-right* ++
	typeof: string ++
	Command to execute when you right clicked on the module.

*on-scroll-up* ++
	typeof: string ++
	Command to execute when scrolling up on the module.

*on-scroll-down* ++
	typeof: string ++
	Command to execute when scrolling down on the module.

*smooth-scrolling-threshold* ++
	typeof: double ++
	Threshold to be used when scrolling.

*tooltip* ++
	typeof: bool ++
	default: true ++
	Option to disable tooltip on hover.

# FORMAT REPLACEMENTS

*{capacity}*: Capacity in percentage

*{icon}*: Icon, as defined in *format-icons*.

*{time}*: Estimate of time until full or empty. Note that this is based on the power draw at the last refresh time, not an average.

# CUSTOM FORMATS

The *battery* module allows to define custom formats based on up to two factors. The best fitting format will be selected.

*format-<state>*: With *states*, a custom format can be set depending on the capacity of your battery.

*format-<status>*: With the status, a custom format can be set depending on the status in /sys/class/power_supply/<bat>/status (in lowercase).

*format-<status>-<state>*: You can also set a custom format depending on both values.

# STATES

- Every entry (*state*) consists of a *<name>* (typeof: *string*) and a *<value>* (typeof: *integer*).
	- The state can be addressed as a CSS class in the *style.css*. The name of the CSS class is the *<name>* of the state.	Each class gets activated when the current capacity is equal or below the configured *<value>*.
	- Also each state can have its own *format*. Those con be configured via *format-<name>*. Or if you want to differentiate a bit more even as *format-<status>-<state>*. For more information see *custom-formats*.



# EXAMPLES

```
"battery": {
    "bat": "BAT2",
    "interval": 60,
    "states": {
        "warning": 30,
        "critical": 15
    },
    "format": "{capacity}% {icon}",
    "format-icons": ["", "", "", "", ""],
    "max-length": 25
}
```

# STYLE

- *#battery*
- *#battery.<status>*
	- *<status>* is the value of /sys/class/power_supply/<bat>/status in lowercase.
- *#battery.<state>*
	- *<state>* can be defined in the *config*. For more information see *states*.
- *#battery.<status>.<state>*
	- Combination of both *<status>* and *<state>*.
