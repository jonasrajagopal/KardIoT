#!/bin/bash
echo $1 $2
./data_read $1 |  ./data_plot_in $2 | python3 estimate_pulse_in.py
