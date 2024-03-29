waybar-memory(5)

# NAME

waybar - memory module

# DESCRIPTION

The *memory* module displays the current date and time.

# CONFIGURATION

Addressed by *memory*

*interval*: ++
	typeof: integer++
	default: 30 ++
	The interval in which the information gets polled.

*format*: ++
	typeof: string ++
	default: {percentage}% ++
	The format, how information should be displayed.

*rotate*: ++
	typeof: integer ++
	Positive value to rotate the text label.

*states*: ++
	typeof: array ++
	A number of memory utilization states which get activated on certain percentage thresholds. See *waybar-states(5)*.

*max-length*: ++
	typeof: integer ++
	The maximum length in character the module should display.

*on-click*: ++
	typeof: string ++
	Command to execute when clicked on the module.

*on-click-right*: ++
	typeof: string ++
	Command to execute when you right clicked on the module.

*on-scroll-up*: ++
	typeof: string ++
	Command to execute when scrolling up on the module.

*on-scroll-down*: ++
	typeof: string ++
	Command to execute when scrolling down on the module.

*smooth-scrolling-threshold*: ++
	typeof: double ++
	Threshold to be used when scrolling.

*tooltip*: ++
	typeof: bool ++
	default: true ++
	Option to disable tooltip on hover.

# FORMAT REPLACEMENTS

*{percentage}*: Percentage of memory in use.

*{total}*: Amount of total memory available in GiB.

*{used}*: Amount of used memory in GiB.

*{avail}*: Amount of available memory in GiB.

# EXAMPLES

```
"memory": {
	"interval": 30,
	"format": "{}% ",
	"max-length": 10
}
```

## FORMATTED MEMORY VALUES

```
"memory": {
	"interval": 30,
	"format": "{used:0.1f}G/{total:0.1f}G "
}
```

# STYLE

- *#memory*
