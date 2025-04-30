#!/bin/bash
echo $1 $2
./data_read /dev/$1 |  ./data_plot_in ./data/$2