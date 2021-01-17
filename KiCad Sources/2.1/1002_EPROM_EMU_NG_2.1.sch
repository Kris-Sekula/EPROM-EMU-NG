EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr USLedger 17000 11000
encoding utf-8
Sheet 1 1
Title "EPROM Emulator NG"
Date "2021-01-01"
Rev "2.1"
Comp "mygeekyhobby.com"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector_Generic:Conn_02x17_Odd_Even J1
U 1 1 5EAF9319
P 14400 3850
F 0 "J1" H 14450 4867 50  0000 C CNN
F 1 "IDC34" H 14450 4950 50  0000 C CNN
F 2 "Connector_IDC:IDC-Header_2x17_P2.54mm_Horizontal_Lock" H 14400 3850 50  0001 C CNN
F 3 "~" H 14400 3850 50  0001 C CNN
	1    14400 3850
	-1   0    0    -1  
$EndComp
Text Label 13850 3350 0    50   ~ 0
E_A15
Text Label 13850 3450 0    50   ~ 0
E_A12
Text Label 13850 3550 0    50   ~ 0
E_A7
Text Label 13850 3650 0    50   ~ 0
E_A6
Text Label 13850 3750 0    50   ~ 0
E_A5
Text Label 13850 3850 0    50   ~ 0
E_A4
Text Label 13850 3950 0    50   ~ 0
E_A3
Text Label 13850 4050 0    50   ~ 0
E_A2
Text Label 13850 4150 0    50   ~ 0
E_A1
Text Label 13850 4250 0    50   ~ 0
E_A0
Text Label 13800 4350 0    50   ~ 0
E_D0
Text Label 13800 4450 0    50   ~ 0
E_D1
Text Label 13800 4550 0    50   ~ 0
E_D2
Text Label 13850 4650 0    50   ~ 0
GND
Wire Wire Line
	14100 4650 13700 4650
Wire Wire Line
	13700 4650 13700 4750
Wire Wire Line
	15000 3250 14650 3250
$Comp
L power:GND #PWR015
U 1 1 5EB0E08B
P 13700 4750
F 0 "#PWR015" H 13700 4500 50  0001 C CNN
F 1 "GND" H 13705 4577 50  0000 C CNN
F 2 "" H 13700 4750 50  0001 C CNN
F 3 "" H 13700 4750 50  0001 C CNN
	1    13700 4750
	1    0    0    -1  
$EndComp
Text Label 14750 4550 0    50   ~ 0
E_D4
Text Label 14750 4450 0    50   ~ 0
E_D5
Text Label 14750 4350 0    50   ~ 0
E_D6
Text Label 14750 4650 0    50   ~ 0
E_D3
Text Label 14750 4250 0    50   ~ 0
E_D7
$Comp
L mgh-symbols:74LS541 U9
U 1 1 5EB1FCFB
P 12400 2300
F 0 "U9" H 12150 3050 50  0000 C CNN
F 1 "74HC541" H 12250 2950 50  0000 C CNN
F 2 "Package_DIP:DIP-20_W7.62mm_Socket" H 12400 2300 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS541" H 12400 2300 50  0001 C CNN
	1    12400 2300
	1    0    0    -1  
$EndComp
Text Label 14800 4150 0    50   ~ 0
E_~CS~
Entry Wire Line
	15050 4250 15150 4150
Entry Wire Line
	15050 4350 15150 4250
Entry Wire Line
	15050 4450 15150 4350
Entry Wire Line
	15050 4550 15150 4450
Entry Wire Line
	15050 4650 15150 4550
Text Label 12950 2200 0    50   ~ 0
E_D4
Text Label 12950 2300 0    50   ~ 0
E_D5
Text Label 12950 2400 0    50   ~ 0
E_D6
Text Label 12950 2100 0    50   ~ 0
E_D3
Text Label 12950 2500 0    50   ~ 0
E_D7
Wire Wire Line
	12900 1900 13200 1900
Wire Wire Line
	12900 1800 13200 1800
Wire Wire Line
	12900 2200 13200 2200
Wire Wire Line
	12900 2000 13200 2000
Entry Wire Line
	13200 1800 13300 1700
Entry Wire Line
	13200 1900 13300 1800
Entry Wire Line
	13200 2000 13300 1900
Entry Wire Line
	13200 2200 13300 2100
Text Label 12950 1800 0    50   ~ 0
E_D0
Text Label 12950 1900 0    50   ~ 0
E_D1
Text Label 12950 2000 0    50   ~ 0
E_D2
Wire Wire Line
	13200 2400 12900 2400
Wire Wire Line
	12900 2500 13200 2500
Wire Wire Line
	12900 2300 13200 2300
Entry Wire Line
	13200 2400 13300 2300
Entry Wire Line
	13200 2300 13300 2200
Entry Wire Line
	13200 2500 13300 2400
Entry Wire Line
	13200 2100 13300 2000
Wire Wire Line
	12900 2100 13200 2100
Text Label 14800 3950 0    50   ~ 0
E_~OE~
Text Label 14650 4050 0    50   ~ 0
E_A10
Text Label 14650 3850 0    50   ~ 0
E_A11
Text Label 14650 3750 0    50   ~ 0
E_A9
Text Label 14650 3650 0    50   ~ 0
E_A8
Text Label 14650 3550 0    50   ~ 0
E_A13
Text Label 14650 3450 0    50   ~ 0
E_A14
Entry Wire Line
	15050 3950 15150 3850
Entry Wire Line
	15050 4150 15150 4050
Entry Wire Line
	15050 3450 15150 3350
Entry Wire Line
	15050 3550 15150 3450
Entry Wire Line
	15050 3650 15150 3550
Entry Wire Line
	15050 3750 15150 3650
Entry Wire Line
	15050 3850 15150 3750
Entry Wire Line
	15050 4050 15150 3950
Text Label 12950 4050 0    50   ~ 0
E_A4
Text Label 12950 4150 0    50   ~ 0
E_A5
Text Label 12950 4250 0    50   ~ 0
E_A6
Text Label 12950 3950 0    50   ~ 0
E_A3
Text Label 12950 4350 0    50   ~ 0
E_A7
Wire Wire Line
	12900 3750 13200 3750
Wire Wire Line
	12900 3650 13200 3650
Wire Wire Line
	12900 4050 13200 4050
Wire Wire Line
	12900 3850 13200 3850
Entry Wire Line
	13200 3650 13300 3550
Entry Wire Line
	13200 3750 13300 3650
Entry Wire Line
	13200 3850 13300 3750
Entry Wire Line
	13200 4050 13300 3950
Text Label 12950 3650 0    50   ~ 0
E_A0
Text Label 12950 3750 0    50   ~ 0
E_A1
Text Label 12950 3850 0    50   ~ 0
E_A2
Wire Wire Line
	13200 4250 12900 4250
Wire Wire Line
	12900 4350 13200 4350
Wire Wire Line
	12900 4150 13200 4150
Entry Wire Line
	13200 4250 13300 4150
Entry Wire Line
	13200 4150 13300 4050
Entry Wire Line
	13200 4350 13300 4250
Entry Wire Line
	13200 3950 13300 3850
Wire Wire Line
	12900 3950 13200 3950
Text Label 12950 5800 0    50   ~ 0
E_A13
Text Label 12950 5300 0    50   ~ 0
E_A8
Text Label 12950 5900 0    50   ~ 0
E_A14
Text Label 12950 5700 0    50   ~ 0
E_A12
Text Label 12950 6000 0    50   ~ 0
E_A15
Wire Wire Line
	12900 5400 13200 5400
Wire Wire Line
	12900 5300 13200 5300
Wire Wire Line
	12900 5700 13200 5700
Wire Wire Line
	12900 5500 13200 5500
Entry Wire Line
	13200 5300 13300 5200
Entry Wire Line
	13200 5400 13300 5300
Entry Wire Line
	13200 5500 13300 5400
Entry Wire Line
	13200 5700 13300 5600
Text Label 12950 5400 0    50   ~ 0
E_A9
Text Label 12950 5500 0    50   ~ 0
E_A10
Text Label 12950 5600 0    50   ~ 0
E_A11
Wire Wire Line
	13200 5900 12900 5900
Wire Wire Line
	12900 6000 13200 6000
Wire Wire Line
	12900 5800 13200 5800
Entry Wire Line
	13200 5900 13300 5800
Entry Wire Line
	13200 5800 13300 5700
Entry Wire Line
	13200 6000 13300 5900
Entry Wire Line
	13200 5600 13300 5500
Wire Wire Line
	12900 5600 13200 5600
$Comp
L mgh-symbols:74LS541 U10
U 1 1 5EB60B5F
P 12400 4150
F 0 "U10" H 12150 4900 50  0000 C CNN
F 1 "74HC541" H 12250 4800 50  0000 C CNN
F 2 "Package_DIP:DIP-20_W7.62mm_Socket" H 12400 4150 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS541" H 12400 4150 50  0001 C CNN
	1    12400 4150
	-1   0    0    -1  
$EndComp
$Comp
L mgh-symbols:74LS541 U11
U 1 1 5EB67F74
P 12400 5800
F 0 "U11" H 12150 6550 50  0000 C CNN
F 1 "74HC541" H 12250 6450 50  0000 C CNN
F 2 "Package_DIP:DIP-20_W7.62mm_Socket" H 12400 5800 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS541" H 12400 5800 50  0001 C CNN
	1    12400 5800
	-1   0    0    -1  
$EndComp
Wire Wire Line
	13200 2700 12900 2700
Wire Wire Line
	12900 2800 13200 2800
Entry Wire Line
	13200 2700 13300 2600
Entry Wire Line
	13200 2800 13300 2700
Text Label 12950 2700 0    50   ~ 0
E_~OE~
Text Label 12950 2800 0    50   ~ 0
E_~CS~
Text Label 15000 3250 2    50   ~ 0
E_RESET
Text Label 13700 3250 0    50   ~ 0
~E_RESET~
Wire Wire Line
	13700 3250 14050 3250
Entry Wire Line
	13300 4450 13400 4550
Entry Wire Line
	13300 4350 13400 4450
Entry Wire Line
	13300 4250 13400 4350
Entry Wire Line
	13300 4150 13400 4250
Entry Wire Line
	13300 3950 13400 4050
Entry Wire Line
	13300 3850 13400 3950
Entry Wire Line
	13300 3750 13400 3850
Entry Wire Line
	13300 3650 13400 3750
Entry Wire Line
	13300 3550 13400 3650
Entry Wire Line
	13300 3450 13400 3550
Entry Wire Line
	13300 3350 13400 3450
Entry Wire Line
	13300 3250 13400 3350
Entry Wire Line
	13300 4050 13400 4150
Wire Wire Line
	13400 3350 14100 3350
Wire Wire Line
	13400 3450 14100 3450
Wire Wire Line
	13400 3550 14100 3550
Wire Wire Line
	13400 3650 14100 3650
Wire Wire Line
	13400 3750 14100 3750
Wire Wire Line
	13400 3850 14100 3850
Wire Wire Line
	13400 3950 14100 3950
Wire Wire Line
	13400 4050 14100 4050
Wire Wire Line
	13400 4150 14100 4150
Wire Wire Line
	13400 4250 14100 4250
Wire Wire Line
	13400 4350 14100 4350
Wire Wire Line
	13400 4450 14100 4450
Wire Wire Line
	13400 4550 14100 4550
Wire Wire Line
	14600 3450 15050 3450
Wire Wire Line
	14600 3550 15050 3550
Wire Wire Line
	14600 3650 15050 3650
Wire Wire Line
	14600 3750 15050 3750
Wire Wire Line
	14600 3850 15050 3850
Wire Wire Line
	14600 3950 15050 3950
Wire Wire Line
	14600 4050 15050 4050
Wire Wire Line
	14600 4150 15050 4150
Wire Wire Line
	14600 4250 15050 4250
Wire Wire Line
	14600 4350 15050 4350
Wire Wire Line
	14600 4450 15050 4450
Wire Wire Line
	14600 4550 15050 4550
Wire Wire Line
	14600 4650 15050 4650
$Comp
L Device:R_Network09 RN1
U 1 1 5EC05CDB
P 13900 2100
F 0 "RN1" V 13283 2100 50  0000 C CNN
F 1 "10K" V 13374 2100 50  0000 C CNN
F 2 "Resistor_THT:R_Array_SIP10" V 14375 2100 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 13900 2100 50  0001 C CNN
	1    13900 2100
	0    1    1    0   
$EndComp
$Comp
L Device:R_Network09 RN2
U 1 1 5EC0D720
P 13900 5600
F 0 "RN2" V 13283 5600 50  0000 C CNN
F 1 "10K" V 13374 5600 50  0000 C CNN
F 2 "Resistor_THT:R_Array_SIP10" V 14375 5600 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 13900 5600 50  0001 C CNN
	1    13900 5600
	0    1    1    0   
$EndComp
Text Label 13450 2200 0    50   ~ 0
E_D4
Text Label 13450 2300 0    50   ~ 0
E_D5
Text Label 13450 2400 0    50   ~ 0
E_D6
Text Label 13450 2100 0    50   ~ 0
E_D3
Text Label 13450 2500 0    50   ~ 0
E_D7
Wire Wire Line
	13400 1900 13700 1900
Wire Wire Line
	13400 1800 13700 1800
Wire Wire Line
	13400 2200 13700 2200
Wire Wire Line
	13400 2000 13700 2000
Text Label 13450 1800 0    50   ~ 0
E_D0
Text Label 13450 1900 0    50   ~ 0
E_D1
Text Label 13450 2000 0    50   ~ 0
E_D2
Wire Wire Line
	13700 2400 13400 2400
Wire Wire Line
	13400 2500 13700 2500
Wire Wire Line
	13400 2300 13700 2300
Wire Wire Line
	13400 2100 13700 2100
Entry Wire Line
	13300 2400 13400 2500
Entry Wire Line
	13300 2300 13400 2400
Entry Wire Line
	13300 2200 13400 2300
Entry Wire Line
	13300 2100 13400 2200
Entry Wire Line
	13300 2000 13400 2100
Entry Wire Line
	13300 1900 13400 2000
Entry Wire Line
	13300 1800 13400 1900
Entry Wire Line
	13300 1700 13400 1800
Text Label 13450 5800 0    50   ~ 0
E_A13
Text Label 15300 2650 0    50   ~ 0
E_A8
Text Label 13450 5900 0    50   ~ 0
E_A14
Text Label 13450 5700 0    50   ~ 0
E_A12
Text Label 13450 6000 0    50   ~ 0
E_A15
Wire Wire Line
	13400 5400 13700 5400
Wire Wire Line
	13400 5300 13700 5300
Wire Wire Line
	13400 5700 13700 5700
Wire Wire Line
	13400 5500 13700 5500
Text Label 13450 5400 0    50   ~ 0
E_A9
Text Label 13450 5500 0    50   ~ 0
E_A10
Text Label 13450 5600 0    50   ~ 0
E_A11
Wire Wire Line
	13700 5900 13400 5900
Wire Wire Line
	13400 6000 13700 6000
Wire Wire Line
	13400 5800 13700 5800
Wire Wire Line
	13400 5600 13700 5600
Entry Wire Line
	13400 5300 13300 5200
Entry Wire Line
	13400 5400 13300 5300
Entry Wire Line
	13400 5500 13300 5400
Entry Wire Line
	13400 5700 13300 5600
Entry Wire Line
	13400 5900 13300 5800
Entry Wire Line
	13400 5800 13300 5700
Entry Wire Line
	13400 6000 13300 5900
Entry Wire Line
	13400 5600 13300 5500
$Comp
L power:VCC #PWR016
U 1 1 5EC2BB78
P 14300 1650
F 0 "#PWR016" H 14300 1500 50  0001 C CNN
F 1 "VCC" H 14317 1823 50  0000 C CNN
F 2 "" H 14300 1650 50  0001 C CNN
F 3 "" H 14300 1650 50  0001 C CNN
	1    14300 1650
	1    0    0    -1  
$EndComp
Wire Bus Line
	13300 1200 15150 1200
Wire Wire Line
	14300 1650 14300 1700
Wire Wire Line
	14300 1700 14100 1700
$Comp
L power:VCC #PWR017
U 1 1 5EC3A5D6
P 14300 5150
F 0 "#PWR017" H 14300 5000 50  0001 C CNN
F 1 "VCC" H 14317 5323 50  0000 C CNN
F 2 "" H 14300 5150 50  0001 C CNN
F 3 "" H 14300 5150 50  0001 C CNN
	1    14300 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	14300 5150 14300 5200
Wire Wire Line
	14300 5200 14100 5200
Text Label 9250 2400 0    50   ~ 0
D0
Text Label 9250 2500 0    50   ~ 0
D1
Text Label 9250 2600 0    50   ~ 0
D2
Text Label 9250 2700 0    50   ~ 0
D3
Text Label 9250 2800 0    50   ~ 0
D4
Text Label 9250 2900 0    50   ~ 0
D5
Text Label 9250 3000 0    50   ~ 0
D6
Text Label 9250 3100 0    50   ~ 0
D7
Text Label 8600 6450 2    50   ~ 0
A14
Text Label 8600 6000 2    50   ~ 0
A13
Text Label 8600 5550 2    50   ~ 0
A12
Text Label 8600 5100 2    50   ~ 0
A11
Text Label 7850 3400 0    50   ~ 0
A10
Text Label 7850 3300 0    50   ~ 0
A9
Text Label 7850 3200 0    50   ~ 0
A8
Text Label 7850 3100 0    50   ~ 0
A7
Text Label 7850 3000 0    50   ~ 0
A6
Text Label 7850 2900 0    50   ~ 0
A5
Text Label 7850 2800 0    50   ~ 0
A4
Text Label 7850 2700 0    50   ~ 0
A3
Text Label 7850 2600 0    50   ~ 0
A2
Text Label 7850 2500 0    50   ~ 0
A1
Text Label 7850 2400 0    50   ~ 0
A0
Text Label 6800 1250 2    50   ~ 0
D0
Text Label 6800 1350 2    50   ~ 0
D1
Text Label 6800 1450 2    50   ~ 0
D2
Text Label 6800 1550 2    50   ~ 0
D3
Text Label 6800 1650 2    50   ~ 0
D4
Text Label 6800 1750 2    50   ~ 0
D5
Text Label 6800 1850 2    50   ~ 0
D6
Text Label 6800 1950 2    50   ~ 0
D7
Text Label 11700 5500 0    50   ~ 0
A10
Text Label 11750 5400 0    50   ~ 0
A9
Text Label 11750 5300 0    50   ~ 0
A8
Text Label 11700 4350 0    50   ~ 0
A7
Text Label 11700 4250 0    50   ~ 0
A6
Text Label 11700 4150 0    50   ~ 0
A5
Text Label 11700 4050 0    50   ~ 0
A4
Text Label 11700 3950 0    50   ~ 0
A3
Text Label 11700 3850 0    50   ~ 0
A2
Text Label 11700 3750 0    50   ~ 0
A1
Text Label 11700 3650 0    50   ~ 0
A0
Text Label 11900 1800 2    50   ~ 0
D0
Text Label 11900 1900 2    50   ~ 0
D1
Text Label 11900 2000 2    50   ~ 0
D2
Text Label 11900 2100 2    50   ~ 0
D3
Text Label 11900 2200 2    50   ~ 0
D4
Text Label 11900 2300 2    50   ~ 0
D5
Text Label 11900 2400 2    50   ~ 0
D6
Text Label 11900 2500 2    50   ~ 0
D7
Entry Wire Line
	11400 1700 11500 1800
Entry Wire Line
	11400 1900 11500 2000
Entry Wire Line
	11400 1800 11500 1900
Entry Wire Line
	11400 2000 11500 2100
Entry Wire Line
	11400 2100 11500 2200
Entry Wire Line
	11400 2200 11500 2300
Entry Wire Line
	11400 2300 11500 2400
Entry Wire Line
	11400 2400 11500 2500
Entry Wire Line
	9400 2400 9500 2500
Entry Wire Line
	9400 2500 9500 2600
Entry Wire Line
	9400 2600 9500 2700
Entry Wire Line
	9400 2700 9500 2800
Entry Wire Line
	9400 2800 9500 2900
Entry Wire Line
	9400 2900 9500 3000
Entry Wire Line
	9400 3000 9500 3100
Entry Wire Line
	9400 3100 9500 3200
Wire Wire Line
	9400 3000 9050 3000
Wire Wire Line
	9400 2900 9050 2900
Wire Wire Line
	9400 2800 9050 2800
Wire Wire Line
	9400 2700 9050 2700
Wire Wire Line
	9400 2600 9050 2600
Wire Wire Line
	9400 2500 9050 2500
Wire Wire Line
	9400 2400 9050 2400
Wire Wire Line
	11500 1800 11900 1800
Wire Wire Line
	11500 1900 11900 1900
Wire Wire Line
	11500 2000 11900 2000
Wire Wire Line
	11500 2100 11900 2100
Wire Wire Line
	11500 2200 11900 2200
Wire Wire Line
	11500 2300 11900 2300
Wire Wire Line
	11500 2400 11900 2400
Wire Wire Line
	11500 2500 11900 2500
Wire Wire Line
	11900 5300 11750 5300
Wire Wire Line
	11900 5400 11750 5400
Wire Wire Line
	11900 5500 11700 5500
$Comp
L 74xx:74LS08 U2
U 1 1 5EC4440A
P 8100 5200
F 0 "U2" H 8100 5200 50  0000 C CNN
F 1 "74HC08" H 8350 5050 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_Socket" H 8100 5200 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS08" H 8100 5200 50  0001 C CNN
	1    8100 5200
	-1   0    0    1   
$EndComp
$Comp
L 74xx:74LS08 U2
U 2 1 5EC45B6F
P 8100 5650
F 0 "U2" H 8100 5650 50  0000 C CNN
F 1 "74HC08" H 8350 5500 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_Socket" H 8100 5650 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS08" H 8100 5650 50  0001 C CNN
	2    8100 5650
	-1   0    0    1   
$EndComp
$Comp
L 74xx:74LS08 U2
U 3 1 5EC47CD0
P 8100 6100
F 0 "U2" H 8100 6100 50  0000 C CNN
F 1 "74HC08" H 8350 5950 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_Socket" H 8100 6100 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS08" H 8100 6100 50  0001 C CNN
	3    8100 6100
	-1   0    0    1   
$EndComp
$Comp
L 74xx:74LS08 U2
U 4 1 5EC49F02
P 8100 6550
F 0 "U2" H 8100 6550 50  0000 C CNN
F 1 "74HC08" H 8350 6400 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_Socket" H 8100 6550 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS08" H 8100 6550 50  0001 C CNN
	4    8100 6550
	-1   0    0    1   
$EndComp
$Comp
L 74xx:74LS08 U2
U 5 1 5EC4DE51
P 3400 9300
F 0 "U2" H 3350 9300 50  0000 L CNN
F 1 "74HC08" H 3250 8700 50  0000 L CNN
F 2 "Package_DIP:DIP-14_W7.62mm_Socket" H 3400 9300 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS08" H 3400 9300 50  0001 C CNN
	5    3400 9300
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS00 U1
U 1 1 5ED65403
P 8100 7100
F 0 "U1" H 8100 7100 50  0000 C CNN
F 1 "74HC00" H 8100 7350 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_Socket" H 8100 7100 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74ls00" H 8100 7100 50  0001 C CNN
	1    8100 7100
	-1   0    0    1   
$EndComp
$Comp
L 74xx:74LS00 U1
U 2 1 5ED679D4
P 7100 7100
F 0 "U1" H 7100 7100 50  0000 C CNN
F 1 "74HC00" H 7100 7350 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_Socket" H 7100 7100 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74ls00" H 7100 7100 50  0001 C CNN
	2    7100 7100
	-1   0    0    1   
$EndComp
$Comp
L 74xx:74LS00 U1
U 3 1 5ED6B7F8
P 6300 2700
F 0 "U1" H 6300 3025 50  0000 C CNN
F 1 "74HC00" H 6300 2934 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_Socket" H 6300 2700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74ls00" H 6300 2700 50  0001 C CNN
	3    6300 2700
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS00 U1
U 5 1 5ED6E800
P 2850 9300
F 0 "U1" H 2700 9300 50  0000 L CNN
F 1 "74HC00" H 2700 8700 50  0000 L CNN
F 2 "Package_DIP:DIP-14_W7.62mm_Socket" H 2850 9300 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74ls00" H 2850 9300 50  0001 C CNN
	5    2850 9300
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 3100 9400 3100
Wire Wire Line
	7400 7000 7400 7100
Text Label 8400 5300 0    50   ~ 0
EN_A11
Text Label 8400 5750 0    50   ~ 0
EN_A12
Text Label 8400 6200 0    50   ~ 0
EN_A13
Text Label 8400 6650 0    50   ~ 0
EN_A14
Text Label 8450 7200 0    50   ~ 0
EN_A15
Wire Wire Line
	8400 5300 8750 5300
Wire Wire Line
	8400 5750 8750 5750
Wire Wire Line
	8400 6200 8750 6200
Wire Wire Line
	8400 6650 8750 6650
Wire Wire Line
	8400 7200 8750 7200
Connection ~ 7400 7100
Wire Wire Line
	7400 7100 7400 7200
$Comp
L power:VCC #PWR09
U 1 1 5F00620E
P 8550 1950
F 0 "#PWR09" H 8550 1800 50  0001 C CNN
F 1 "VCC" H 8567 2123 50  0000 C CNN
F 2 "" H 8550 1950 50  0001 C CNN
F 3 "" H 8550 1950 50  0001 C CNN
	1    8550 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	8550 4400 8550 4550
$Comp
L power:GND #PWR010
U 1 1 5F015B4E
P 8550 4550
F 0 "#PWR010" H 8550 4300 50  0001 C CNN
F 1 "GND" H 8555 4377 50  0000 C CNN
F 2 "" H 8550 4550 50  0001 C CNN
F 3 "" H 8550 4550 50  0001 C CNN
	1    8550 4550
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7400 5200 7800 5200
Wire Wire Line
	7400 5650 7800 5650
Wire Wire Line
	7400 6550 7800 6550
Wire Wire Line
	7800 6100 7400 6100
$Comp
L mgh-symbols:Arduino_Nano_v2.x M1
U 1 1 5F1390F9
P 3300 4200
F 0 "M1" H 3000 5150 50  0000 C CNN
F 1 "Arduino Nano v3.0" V 3300 4150 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 3300 4200 50  0001 C CIN
F 3 "https://www.arduino.cc/en/uploads/Main/ArduinoNanoManual23.pdf" H 3300 4200 50  0001 C CNN
	1    3300 4200
	-1   0    0    -1  
$EndComp
$Comp
L mgh-symbols:74HC595 U4
U 1 1 5EB51D7E
P 6250 1650
F 0 "U4" H 6000 2450 50  0000 C CNN
F 1 "74HC595" H 6100 2350 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm_Socket" H 6250 1650 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 6250 1650 50  0001 C CNN
	1    6250 1650
	1    0    0    -1  
$EndComp
$Comp
L mgh-symbols:74HC595 U5
U 1 1 5EB5397B
P 6250 4400
F 0 "U5" H 5750 5150 50  0000 C CNN
F 1 "74HC595" H 5850 5050 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm_Socket" H 6250 4400 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 6250 4400 50  0001 C CNN
	1    6250 4400
	1    0    0    -1  
$EndComp
$Comp
L mgh-symbols:74HC595 U6
U 1 1 5EB550E1
P 6250 5800
F 0 "U6" H 5800 6500 50  0000 C CNN
F 1 "74HC595" H 5900 6400 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm_Socket" H 6250 5800 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 6250 5800 50  0001 C CNN
	1    6250 5800
	1    0    0    -1  
$EndComp
Text Label 7700 3500 0    50   ~ 0
GT_A11
Text Label 7700 3600 0    50   ~ 0
GT_A12
Text Label 7700 3700 0    50   ~ 0
GT_A13
Text Label 7700 3800 0    50   ~ 0
GT_A14
Wire Wire Line
	6850 5400 6650 5400
Wire Wire Line
	6850 5500 6650 5500
Wire Wire Line
	6850 5600 6650 5600
Wire Wire Line
	6850 5700 6650 5700
Wire Wire Line
	6850 5800 6650 5800
Wire Wire Line
	6850 5900 6650 5900
Wire Wire Line
	6850 6000 6650 6000
Wire Wire Line
	6850 6100 6650 6100
Wire Wire Line
	6800 4000 6650 4000
Wire Wire Line
	6800 4100 6650 4100
Wire Wire Line
	6800 4200 6650 4200
Wire Wire Line
	6800 4300 6650 4300
Wire Wire Line
	6800 4400 6650 4400
Wire Wire Line
	6800 4500 6650 4500
Wire Wire Line
	6800 4600 6650 4600
Wire Wire Line
	6800 4700 6650 4700
Entry Wire Line
	7100 1950 7200 1850
Entry Wire Line
	7100 1850 7200 1750
Entry Wire Line
	7100 1750 7200 1650
Entry Wire Line
	7100 1650 7200 1550
Entry Wire Line
	7100 1550 7200 1450
Entry Wire Line
	7100 1450 7200 1350
Entry Wire Line
	7100 1350 7200 1250
Entry Wire Line
	7100 1250 7200 1150
Wire Wire Line
	7100 1250 6650 1250
Wire Wire Line
	7100 1350 6650 1350
Wire Wire Line
	7100 1450 6650 1450
Wire Wire Line
	7100 1550 6650 1550
Wire Wire Line
	7100 1650 6650 1650
Wire Wire Line
	7100 1750 6650 1750
Wire Wire Line
	7100 1850 6650 1850
Wire Wire Line
	7100 1950 6650 1950
Text Label 7400 6550 0    50   ~ 0
GT_A14
Text Label 7400 6100 0    50   ~ 0
GT_A13
Text Label 7400 5650 0    50   ~ 0
GT_A12
Text Label 7400 5200 0    50   ~ 0
GT_A11
Text Label 8600 7000 2    50   ~ 0
A15
Wire Wire Line
	8050 3400 7850 3400
Wire Wire Line
	8050 3300 7850 3300
Wire Wire Line
	8050 3200 7850 3200
Wire Wire Line
	8050 3100 7850 3100
Wire Wire Line
	8050 3000 7850 3000
Wire Wire Line
	8050 2900 7850 2900
Wire Wire Line
	8050 2800 7850 2800
Wire Wire Line
	8050 2700 7850 2700
Wire Wire Line
	8050 2600 7850 2600
Wire Wire Line
	8050 2500 7850 2500
Wire Wire Line
	8050 2400 7850 2400
Text Label 6800 4000 2    50   ~ 0
A0
Text Label 6800 4100 2    50   ~ 0
A1
Text Label 6800 4200 2    50   ~ 0
A2
Text Label 6800 4300 2    50   ~ 0
A3
Text Label 6800 4400 2    50   ~ 0
A4
Text Label 6800 4500 2    50   ~ 0
A5
Text Label 6800 4600 2    50   ~ 0
A6
Text Label 6800 4700 2    50   ~ 0
A7
Text Label 6850 5400 2    50   ~ 0
A8
Text Label 6850 5500 2    50   ~ 0
A9
Text Label 6850 5600 2    50   ~ 0
A10
Text Label 6850 5700 2    50   ~ 0
A11
Text Label 6850 5800 2    50   ~ 0
A12
Text Label 6850 5900 2    50   ~ 0
A13
Text Label 6850 6000 2    50   ~ 0
A14
Text Label 6850 6100 2    50   ~ 0
A15
Wire Wire Line
	8600 5100 8400 5100
Wire Wire Line
	8600 5550 8400 5550
Wire Wire Line
	8600 6000 8400 6000
Wire Wire Line
	8600 6450 8400 6450
Wire Wire Line
	8600 7000 8400 7000
Wire Wire Line
	11700 3650 11900 3650
Wire Wire Line
	11700 3750 11900 3750
Wire Wire Line
	11700 3850 11900 3850
Wire Wire Line
	11700 3950 11900 3950
Wire Wire Line
	11700 4050 11900 4050
Wire Wire Line
	11700 4150 11900 4150
Wire Wire Line
	11700 4250 11900 4250
Wire Wire Line
	11700 4350 11900 4350
Text Label 11700 5600 0    50   ~ 0
A11
Text Label 11700 5700 0    50   ~ 0
A12
Text Label 11700 5800 0    50   ~ 0
A13
Text Label 11700 5900 0    50   ~ 0
A14
Text Label 11700 6000 0    50   ~ 0
A15
Wire Wire Line
	11900 5600 11700 5600
Wire Wire Line
	11700 5700 11900 5700
Wire Wire Line
	11700 5800 11900 5800
Wire Wire Line
	11700 5900 11900 5900
Wire Wire Line
	11700 6000 11900 6000
Entry Wire Line
	8750 7200 8850 7300
Entry Wire Line
	8750 6650 8850 6750
Entry Wire Line
	8750 6200 8850 6300
Entry Wire Line
	8750 5750 8850 5850
Entry Wire Line
	8750 5300 8850 5400
Entry Wire Line
	7300 3600 7400 3500
Entry Wire Line
	7300 3700 7400 3600
Entry Wire Line
	7300 3800 7400 3700
Entry Wire Line
	7300 3900 7400 3800
Entry Wire Line
	7300 5100 7400 5200
Entry Wire Line
	7300 5550 7400 5650
Entry Wire Line
	7300 6000 7400 6100
Entry Wire Line
	7300 6450 7400 6550
Wire Wire Line
	6800 7100 6550 7100
Text Label 9350 3600 2    50   ~ 0
~WE~
Text Label 11550 4600 0    50   ~ 0
RESET
Wire Wire Line
	11900 4550 11850 4550
Wire Wire Line
	11850 4550 11850 4600
Wire Wire Line
	11850 4650 11900 4650
Wire Wire Line
	11850 4600 11550 4600
Connection ~ 11850 4600
Wire Wire Line
	11850 4600 11850 4650
Text Label 11550 6250 0    50   ~ 0
RESET
Wire Wire Line
	11900 6200 11850 6200
Wire Wire Line
	11850 6200 11850 6250
Wire Wire Line
	11850 6300 11900 6300
Wire Wire Line
	11850 6250 11550 6250
Connection ~ 11850 6250
Wire Wire Line
	11850 6250 11850 6300
Wire Wire Line
	9050 3600 9350 3600
Text Label 5850 2700 2    50   ~ 0
~RESET~
Wire Wire Line
	6000 2800 5900 2800
Wire Wire Line
	5900 2800 5900 2700
Wire Wire Line
	5900 2600 6000 2600
Wire Wire Line
	5400 2700 5900 2700
Connection ~ 5900 2700
Wire Wire Line
	5900 2700 5900 2600
Wire Wire Line
	3700 2600 3550 2600
Wire Wire Line
	3550 2800 3700 2800
Wire Wire Line
	6600 2700 6850 2700
Text Label 6850 2700 2    50   ~ 0
RESET
$Comp
L power:VCC #PWR07
U 1 1 5F09EE73
P 7850 9050
F 0 "#PWR07" H 7850 8900 50  0001 C CNN
F 1 "VCC" H 7867 9223 50  0000 C CNN
F 2 "" H 7850 9050 50  0001 C CNN
F 3 "" H 7850 9050 50  0001 C CNN
	1    7850 9050
	1    0    0    -1  
$EndComp
Text Label 2500 4700 0    50   ~ 0
EN_A15
Text Label 2500 4600 0    50   ~ 0
EN_A14
Text Label 2500 4500 0    50   ~ 0
EN_A13
Text Label 2500 4400 0    50   ~ 0
EN_A12
Text Label 2500 4300 0    50   ~ 0
EN_A11
Entry Wire Line
	2450 4300 2350 4400
Entry Wire Line
	2450 4400 2350 4500
Entry Wire Line
	2450 4500 2350 4600
Entry Wire Line
	2450 4600 2350 4700
Entry Wire Line
	2450 4700 2350 4800
$Comp
L power:GND #PWR04
U 1 1 5F4D4016
P 3250 5200
F 0 "#PWR04" H 3250 4950 50  0001 C CNN
F 1 "GND" H 3400 5200 50  0000 C CNN
F 2 "" H 3250 5200 50  0001 C CNN
F 3 "" H 3250 5200 50  0001 C CNN
	1    3250 5200
	1    0    0    -1  
$EndComp
Text Label 14900 3350 2    50   ~ 0
E_VCC
Wire Wire Line
	14900 3350 14600 3350
Text Label 9650 9200 2    50   ~ 0
E_VCC
Wire Wire Line
	2800 4300 2450 4300
Wire Wire Line
	2800 4400 2450 4400
Wire Wire Line
	2800 4500 2450 4500
Wire Wire Line
	2800 4600 2450 4600
Wire Wire Line
	2800 4700 2450 4700
$Comp
L Device:CP_Small C1
U 1 1 5FD68EF5
P 2250 9250
F 0 "C1" H 2000 9550 50  0000 L CNN
F 1 "10uF" H 2000 9450 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 2250 9250 50  0001 C CNN
F 3 "~" H 2250 9250 50  0001 C CNN
	1    2250 9250
	1    0    0    -1  
$EndComp
Connection ~ 3400 8800
$Comp
L power:VCC #PWR02
U 1 1 5FF5F9D9
P 3400 8800
F 0 "#PWR02" H 3400 8650 50  0001 C CNN
F 1 "VCC" H 3417 8973 50  0000 C CNN
F 2 "" H 3400 8800 50  0001 C CNN
F 3 "" H 3400 8800 50  0001 C CNN
	1    3400 8800
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5FFDBCAE
P 4350 9300
F 0 "C3" H 4400 9500 50  0000 L CNN
F 1 "100nF" H 4100 9400 50  0001 L CNN
F 2 "Capacitor_THT:C_Disc_D4.7mm_W2.5mm_P5.00mm" H 4388 9150 50  0001 C CNN
F 3 "~" H 4350 9300 50  0001 C CNN
	1    4350 9300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 5FFDC260
P 5100 9300
F 0 "C6" H 5150 9500 50  0000 L CNN
F 1 "100nF" H 4850 9400 50  0001 L CNN
F 2 "Capacitor_THT:C_Disc_D4.7mm_W2.5mm_P5.00mm" H 5138 9150 50  0001 C CNN
F 3 "~" H 5100 9300 50  0001 C CNN
	1    5100 9300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 600046F7
P 4600 9300
F 0 "C4" H 4650 9500 50  0000 L CNN
F 1 "100nF" H 4350 9400 50  0001 L CNN
F 2 "Capacitor_THT:C_Disc_D4.7mm_W2.5mm_P5.00mm" H 4638 9150 50  0001 C CNN
F 3 "~" H 4600 9300 50  0001 C CNN
	1    4600 9300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 6002B954
P 4850 9300
F 0 "C5" H 4900 9500 50  0000 L CNN
F 1 "100nF" H 4600 9400 50  0001 L CNN
F 2 "Capacitor_THT:C_Disc_D4.7mm_W2.5mm_P5.00mm" H 4888 9150 50  0001 C CNN
F 3 "~" H 4850 9300 50  0001 C CNN
	1    4850 9300
	1    0    0    -1  
$EndComp
Text Notes 3900 8700 0    50   ~ 0
5x 100nF IC Decoupling Capacitors
Wire Wire Line
	3400 8800 3800 8800
Wire Wire Line
	2850 9800 3100 9800
Wire Wire Line
	3400 9800 3650 9800
Connection ~ 3400 9800
Wire Wire Line
	5600 9450 5600 9800
Wire Wire Line
	5100 9450 5100 9800
Wire Wire Line
	5100 9800 5600 9800
Wire Wire Line
	4850 9450 4850 9800
Connection ~ 4850 9800
Wire Wire Line
	4850 9800 5100 9800
Wire Wire Line
	4600 9450 4600 9800
Connection ~ 4600 9800
Wire Wire Line
	4600 9800 4850 9800
Wire Wire Line
	4350 9450 4350 9800
Connection ~ 4350 9800
Wire Wire Line
	4350 9800 4600 9800
Wire Wire Line
	4100 9450 4100 9800
Connection ~ 4100 9800
Wire Wire Line
	4100 9800 4350 9800
Wire Wire Line
	4100 9150 4100 8800
Connection ~ 4100 8800
Wire Wire Line
	4100 8800 4350 8800
Wire Wire Line
	4350 9150 4350 8800
Connection ~ 4350 8800
Wire Wire Line
	4350 8800 4600 8800
Wire Wire Line
	4600 9150 4600 8800
Connection ~ 4600 8800
Wire Wire Line
	4600 8800 4850 8800
Wire Wire Line
	4850 9150 4850 8800
Connection ~ 4850 8800
Wire Wire Line
	4850 8800 5100 8800
Wire Wire Line
	5100 9150 5100 8800
Wire Wire Line
	5100 8800 5600 8800
Wire Wire Line
	5600 9150 5600 8800
Wire Wire Line
	2250 9150 2250 8800
Connection ~ 2850 8800
Wire Wire Line
	2250 9350 2250 9800
Connection ~ 2850 9800
$Comp
L power:GND #PWR03
U 1 1 5FF87689
P 3650 9850
F 0 "#PWR03" H 3650 9600 50  0001 C CNN
F 1 "GND" H 3655 9677 50  0000 C CNN
F 2 "" H 3650 9850 50  0001 C CNN
F 3 "" H 3650 9850 50  0001 C CNN
	1    3650 9850
	1    0    0    -1  
$EndComp
NoConn ~ 6650 4900
NoConn ~ 6650 6300
NoConn ~ 6650 2150
NoConn ~ 3200 3200
NoConn ~ 3400 3200
NoConn ~ 2800 3700
NoConn ~ 2800 4000
NoConn ~ 2800 4800
NoConn ~ 2800 4900
NoConn ~ 14100 3150
NoConn ~ 14600 3150
NoConn ~ 3800 3600
NoConn ~ 3800 3700
Wire Wire Line
	3800 4700 4000 4700
Wire Wire Line
	4000 4700 4000 6500
Wire Wire Line
	4000 6500 3700 6500
Wire Wire Line
	3700 6600 3900 6600
Wire Wire Line
	3900 6600 3900 4800
Wire Wire Line
	3900 4800 3800 4800
Wire Wire Line
	3800 4600 4100 4600
Wire Wire Line
	4100 4600 4100 6950
Wire Wire Line
	2900 6950 2900 6600
$Comp
L power:GND #PWR06
U 1 1 5EF9788F
P 3300 6800
F 0 "#PWR06" H 3300 6550 50  0001 C CNN
F 1 "GND" H 3100 6800 50  0000 C CNN
F 2 "" H 3300 6800 50  0001 C CNN
F 3 "" H 3300 6800 50  0001 C CNN
	1    3300 6800
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR05
U 1 1 5EFEEA58
P 3300 6200
F 0 "#PWR05" H 3300 6050 50  0001 C CNN
F 1 "VCC" H 3317 6373 50  0000 C CNN
F 2 "" H 3300 6200 50  0001 C CNN
F 3 "" H 3300 6200 50  0001 C CNN
	1    3300 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 6950 4100 6950
Wire Wire Line
	3300 6200 2900 6200
Connection ~ 3300 6200
Wire Wire Line
	3800 4900 3800 6400
Wire Wire Line
	3800 6400 3700 6400
$Comp
L Device:LED D3
U 1 1 5F2754B3
P 3800 9500
F 0 "D3" V 3900 9400 50  0000 R CNN
F 1 "LED" V 3800 9400 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 3800 9500 50  0001 C CNN
F 3 "~" H 3800 9500 50  0001 C CNN
	1    3800 9500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3650 9850 3650 9800
Connection ~ 3650 9800
Wire Wire Line
	3650 9800 3800 9800
$Comp
L Device:R R2
U 1 1 5F2D1B3A
P 3800 9150
F 0 "R2" H 3850 9400 50  0000 L CNN
F 1 "330R" H 3850 9300 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 3730 9150 50  0001 C CNN
F 3 "~" H 3800 9150 50  0001 C CNN
	1    3800 9150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 9000 3800 8800
Wire Wire Line
	3800 9300 3800 9350
Wire Wire Line
	3800 9650 3800 9800
Wire Wire Line
	3800 8800 4100 8800
Connection ~ 3800 8800
Wire Wire Line
	3800 9800 4100 9800
Connection ~ 3800 9800
Text Notes 3800 9950 0    50   ~ 0
Power LED - Green
$Comp
L Device:Polyfuse_Small F1
U 1 1 5F4C94AD
P 8950 9200
F 0 "F1" V 8700 9050 50  0000 C CNN
F 1 "Polyfuse_200mA" V 8800 9300 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D10.0mm_W2.5mm_P5.00mm" H 9000 9000 50  0001 L CNN
F 3 "~" H 8950 9200 50  0001 C CNN
	1    8950 9200
	0    1    1    0   
$EndComp
Wire Wire Line
	9050 9200 9650 9200
Wire Wire Line
	3100 2600 3100 2800
$Comp
L Device:LED D2
U 1 1 5F741749
P 2200 5050
F 0 "D2" V 2250 5250 50  0000 R CNN
F 1 "LED" V 2150 5300 50  0000 R CNN
F 2 "LED_THT:LED_D3.0mm" H 2200 5050 50  0001 C CNN
F 3 "~" H 2200 5050 50  0001 C CNN
	1    2200 5050
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5F7484FC
P 2200 5400
F 0 "#PWR01" H 2200 5150 50  0001 C CNN
F 1 "GND" H 2100 5450 50  0000 C CNN
F 2 "" H 2200 5400 50  0001 C CNN
F 3 "" H 2200 5400 50  0001 C CNN
	1    2200 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 5200 2200 5400
Wire Wire Line
	2200 4900 2200 4700
Wire Wire Line
	2200 4200 2800 4200
Wire Wire Line
	2200 4400 2200 4200
Text Notes 1900 4900 0    50   ~ 0
"RUN"
$Comp
L Switch:SW_Push SW1
U 1 1 5F947AA6
P 4250 6500
F 0 "SW1" V 3850 6600 50  0000 R CNN
F 1 "SW_Push" V 3800 6600 50  0001 R CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4250 6700 50  0001 C CNN
F 3 "~" H 4250 6700 50  0001 C CNN
	1    4250 6500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4250 4500 3800 4500
Text Notes 4400 6600 1    50   ~ 0
LOAD
$Comp
L Device:R R3
U 1 1 5FB7CC84
P 3100 2050
F 0 "R3" H 3150 2050 50  0000 L CNN
F 1 "10k" V 3100 2000 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 3030 2050 50  0001 C CNN
F 3 "~" H 3100 2050 50  0001 C CNN
	1    3100 2050
	-1   0    0    1   
$EndComp
Wire Wire Line
	4250 6700 4250 6800
Wire Wire Line
	3300 6800 4250 6800
$Comp
L Device:Q_NPN_EBC Q1
U 1 1 5FE90F3C
P 14200 8200
F 0 "Q1" H 14391 8246 50  0000 L CNN
F 1 "2N4401" H 14391 8155 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline_Wide" H 14400 8300 50  0001 C CNN
F 3 "~" H 14200 8200 50  0001 C CNN
	1    14200 8200
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR014
U 1 1 5FE95FEA
P 14300 6600
F 0 "#PWR014" H 14300 6450 50  0001 C CNN
F 1 "VCC" H 14317 6773 50  0000 C CNN
F 2 "" H 14300 6600 50  0001 C CNN
F 3 "" H 14300 6600 50  0001 C CNN
	1    14300 6600
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 5FE96868
P 14300 8450
F 0 "#PWR013" H 14300 8200 50  0001 C CNN
F 1 "GND" H 14305 8277 50  0000 C CNN
F 2 "" H 14300 8450 50  0001 C CNN
F 3 "" H 14300 8450 50  0001 C CNN
	1    14300 8450
	-1   0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 5FF872CF
P 14600 7400
F 0 "R6" V 14500 7400 50  0000 L CNN
F 1 "10R" V 14700 7350 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 14530 7400 50  0001 C CNN
F 3 "~" H 14600 7400 50  0001 C CNN
	1    14600 7400
	0    -1   1    0   
$EndComp
$Comp
L Device:Q_PNP_EBC Q2
U 1 1 5FE922B1
P 14200 6850
F 0 "Q2" H 14391 6804 50  0000 L CNN
F 1 "2N4403" H 14391 6895 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline_Wide" H 14400 6950 50  0001 C CNN
F 3 "~" H 14200 6850 50  0001 C CNN
	1    14200 6850
	1    0    0    1   
$EndComp
$Comp
L Device:R R5
U 1 1 5FFE9668
P 14600 7750
F 0 "R5" V 14500 7650 50  0000 L CNN
F 1 "10R" V 14700 7650 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 14530 7750 50  0001 C CNN
F 3 "~" H 14600 7750 50  0001 C CNN
	1    14600 7750
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 5FFE9AA2
P 13800 8200
F 0 "R4" V 13700 8100 50  0000 L CNN
F 1 "4k7" V 13900 8100 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 13730 8200 50  0001 C CNN
F 3 "~" H 13800 8200 50  0001 C CNN
	1    13800 8200
	0    1    1    0   
$EndComp
$Comp
L Device:R R7
U 1 1 5FFE9E7E
P 13800 6850
F 0 "R7" V 13700 6750 50  0000 L CNN
F 1 "10k" V 13900 6750 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 13730 6850 50  0001 C CNN
F 3 "~" H 13800 6850 50  0001 C CNN
	1    13800 6850
	0    -1   1    0   
$EndComp
Text Label 15100 7750 2    50   ~ 0
~E_RESET~
Text Label 15100 7400 2    50   ~ 0
E_RESET
Text Label 13300 8200 0    50   ~ 0
RESET
Text Label 13250 6850 0    50   ~ 0
~RESET~
Wire Wire Line
	14000 6850 13950 6850
Wire Wire Line
	14300 7400 14450 7400
Wire Wire Line
	14450 7750 14300 7750
Wire Wire Line
	14000 8200 13950 8200
Connection ~ 7300 4800
Wire Wire Line
	13650 6850 13250 6850
Wire Wire Line
	15100 7400 14750 7400
Wire Wire Line
	14750 7750 15100 7750
Wire Wire Line
	13300 8200 13650 8200
Wire Bus Line
	2350 7750 8850 7750
Wire Wire Line
	7400 7100 7800 7100
$Comp
L power:PWR_FLAG #FLG02
U 1 1 5F60952E
P 3100 9850
F 0 "#FLG02" H 3100 9925 50  0001 C CNN
F 1 "PWR_FLAG" H 3100 10023 50  0000 C CNN
F 2 "" H 3100 9850 50  0001 C CNN
F 3 "~" H 3100 9850 50  0001 C CNN
	1    3100 9850
	-1   0    0    1   
$EndComp
Wire Wire Line
	3100 9850 3100 9800
Connection ~ 3100 9800
Wire Wire Line
	3100 9800 3400 9800
Text Label 3950 3900 0    50   ~ 0
DATA
Text Label 3950 4000 0    50   ~ 0
ADLO
Text Label 3950 4100 0    50   ~ 0
ADHI
Text Label 3950 4200 0    50   ~ 0
DATA_LD
Text Label 3950 4300 0    50   ~ 0
SCLK
$Comp
L Device:R R1
U 1 1 5F7445F6
P 2200 4550
F 0 "R1" H 2300 4600 50  0000 L CNN
F 1 "330R" V 2200 4450 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2130 4550 50  0001 C CNN
F 3 "~" H 2200 4550 50  0001 C CNN
	1    2200 4550
	-1   0    0    1   
$EndComp
Wire Wire Line
	3550 2800 3550 3050
Connection ~ 3550 2800
Wire Wire Line
	3550 2600 3550 2800
Wire Wire Line
	3800 3800 4300 3800
Text Label 2450 4200 0    50   ~ 0
RUN_LED
Wire Wire Line
	3550 3050 4300 3050
Wire Wire Line
	4300 3050 4300 3800
Text Label 4250 5650 3    50   ~ 0
LD_BTN
Text Label 4100 5800 3    50   ~ 0
SS
Text Label 4000 5750 3    50   ~ 0
MOSI
Text Label 3900 5750 3    50   ~ 0
MISO
Text Label 3800 5750 3    50   ~ 0
SCK
$Comp
L Device:D_Small_ALT D5
U 1 1 5F9EC06A
P 8350 9200
F 0 "D5" H 8250 9500 50  0000 L CNN
F 1 "BAT48" H 8250 9350 50  0000 L CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" V 8350 9200 50  0001 C CNN
F 3 "~" V 8350 9200 50  0001 C CNN
	1    8350 9200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7950 9200 8250 9200
$Comp
L power:VCC #PWR025
U 1 1 5FD43C1B
P 2800 2500
F 0 "#PWR025" H 2800 2350 50  0001 C CNN
F 1 "VCC" H 2817 2673 50  0000 C CNN
F 2 "" H 2800 2500 50  0001 C CNN
F 3 "" H 2800 2500 50  0001 C CNN
	1    2800 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 2500 2800 2600
Wire Wire Line
	2800 2600 3100 2600
Wire Wire Line
	2850 8800 3400 8800
$Comp
L Mechanical:MountingHole H1
U 1 1 5EC40E4A
P 11000 10150
F 0 "H1" H 11100 10196 50  0000 L CNN
F 1 "MountingHole" H 10950 9950 50  0001 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 11000 10150 50  0001 C CNN
F 3 "~" H 11000 10150 50  0001 C CNN
	1    11000 10150
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5EC42B14
P 11300 10150
F 0 "H2" H 11400 10196 50  0000 L CNN
F 1 "MountingHole" H 11500 9950 50  0001 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 11300 10150 50  0001 C CNN
F 3 "~" H 11300 10150 50  0001 C CNN
	1    11300 10150
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 5EC43E36
P 11600 10150
F 0 "H3" H 11700 10196 50  0000 L CNN
F 1 "MountingHole" H 12050 9950 50  0001 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 11600 10150 50  0001 C CNN
F 3 "~" H 11600 10150 50  0001 C CNN
	1    11600 10150
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 5EC45DB4
P 11900 10150
F 0 "H4" H 12000 10196 50  0000 L CNN
F 1 "MountingHole" H 12550 9950 50  0001 L CNN
F 2 "MountingHole:MountingHole_2.2mm_M2" H 11900 10150 50  0001 C CNN
F 3 "~" H 11900 10150 50  0001 C CNN
	1    11900 10150
	1    0    0    -1  
$EndComp
Text Notes 11250 10400 0    50   ~ 0
Mounting Holes
NoConn ~ 2800 3600
Connection ~ 5100 8800
Connection ~ 5100 9800
Text Label 13450 5300 0    50   ~ 0
E_~OE~
Text Label 13450 5200 0    50   ~ 0
E_~CS~
Wire Wire Line
	13400 5200 13700 5200
Entry Wire Line
	13300 5100 13400 5200
Wire Wire Line
	3200 5200 3250 5200
Wire Wire Line
	5850 5900 5200 5900
Wire Wire Line
	5200 4500 5850 4500
Wire Wire Line
	5850 1750 5200 1750
Wire Wire Line
	4700 1250 4700 3900
Wire Wire Line
	3800 3900 4700 3900
Connection ~ 5200 4500
Wire Wire Line
	5850 5600 5000 5600
Wire Wire Line
	5000 4200 5850 4200
Wire Wire Line
	5000 4200 5000 1450
Wire Wire Line
	5000 1450 5850 1450
Connection ~ 5000 4200
Wire Wire Line
	5200 1750 5200 4500
Wire Wire Line
	5000 4200 5000 4700
$Comp
L power:VCC #PWR0101
U 1 1 6002B186
P 5700 1550
F 0 "#PWR0101" H 5700 1400 50  0001 C CNN
F 1 "VCC" V 5700 1750 50  0000 C CNN
F 2 "" H 5700 1550 50  0001 C CNN
F 3 "" H 5700 1550 50  0001 C CNN
	1    5700 1550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5850 1550 5700 1550
$Comp
L power:VCC #PWR0102
U 1 1 60062895
P 5700 5700
F 0 "#PWR0102" H 5700 5550 50  0001 C CNN
F 1 "VCC" V 5700 5900 50  0000 C CNN
F 2 "" H 5700 5700 50  0001 C CNN
F 3 "" H 5700 5700 50  0001 C CNN
	1    5700 5700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5850 5700 5700 5700
$Comp
L power:VCC #PWR0103
U 1 1 6009AC51
P 5700 4300
F 0 "#PWR0103" H 5700 4150 50  0001 C CNN
F 1 "VCC" V 5700 4500 50  0000 C CNN
F 2 "" H 5700 4300 50  0001 C CNN
F 3 "" H 5700 4300 50  0001 C CNN
	1    5700 4300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5850 4300 5700 4300
Wire Wire Line
	5850 6000 5400 6000
Wire Wire Line
	5400 4600 5850 4600
Wire Wire Line
	5400 4600 5400 2700
Connection ~ 5400 4600
Wire Wire Line
	5400 1850 5850 1850
Connection ~ 5400 2700
Wire Wire Line
	5400 2700 5400 1850
$Comp
L 74xx:74LS00 U1
U 4 1 5ED6D149
P 4000 2700
F 0 "U1" H 4000 3025 50  0000 C CNN
F 1 "74HC00" H 4000 2934 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_Socket" H 4000 2700 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74ls00" H 4000 2700 50  0001 C CNN
	4    4000 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4700 1250 5850 1250
Wire Wire Line
	3800 4000 5850 4000
Wire Wire Line
	5850 5400 4700 5400
Wire Wire Line
	4700 5400 4700 4100
Wire Wire Line
	3800 4100 4700 4100
Wire Wire Line
	5200 4500 5200 5900
Wire Wire Line
	5200 4500 4550 4500
Wire Wire Line
	4550 4500 4550 4200
Wire Wire Line
	3800 4200 4550 4200
Connection ~ 5000 4700
Wire Wire Line
	5000 4700 5000 5600
Connection ~ 3250 5200
Wire Wire Line
	3250 5200 3300 5200
Wire Wire Line
	14300 7750 14300 8000
Wire Wire Line
	14300 8450 14300 8400
Wire Wire Line
	14300 6650 14300 6600
Wire Wire Line
	14300 7050 14300 7400
Wire Wire Line
	2250 8800 2500 8800
Wire Wire Line
	2250 9800 2500 9800
$Comp
L Connector:TestPoint TP1
U 1 1 5EFA61D2
P 2500 8800
F 0 "TP1" H 2600 8950 50  0001 L CNN
F 1 "VCC" H 2600 8850 50  0001 L CNN
F 2 "TestPoint:TestPoint_THTPad_D1.5mm_Drill0.7mm" H 2700 8800 50  0001 C CNN
F 3 "~" H 2700 8800 50  0001 C CNN
	1    2500 8800
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint TP2
U 1 1 5EFA7C6F
P 2500 9800
F 0 "TP2" H 2600 9700 50  0001 R CNN
F 1 "GND" H 2650 10050 50  0001 R CNN
F 2 "TestPoint:TestPoint_THTPad_D1.5mm_Drill0.7mm" H 2700 9800 50  0001 C CNN
F 3 "~" H 2700 9800 50  0001 C CNN
	1    2500 9800
	-1   0    0    1   
$EndComp
Connection ~ 2500 9800
Wire Wire Line
	2500 9800 2600 9800
Connection ~ 2500 8800
Wire Wire Line
	2500 8800 2600 8800
$Comp
L Connector:TestPoint TP3
U 1 1 5EFA92B9
P 2600 9800
F 0 "TP3" H 2650 9700 50  0001 R CNN
F 1 "GND" H 2650 10050 50  0001 R CNN
F 2 "TestPoint:TestPoint_THTPad_D1.5mm_Drill0.7mm" H 2800 9800 50  0001 C CNN
F 3 "~" H 2800 9800 50  0001 C CNN
	1    2600 9800
	-1   0    0    1   
$EndComp
Connection ~ 2600 9800
Wire Wire Line
	2600 9800 2850 9800
Wire Wire Line
	4400 4700 5000 4700
Wire Wire Line
	3800 4300 4400 4300
Wire Wire Line
	4400 4300 4400 4700
Wire Wire Line
	5400 6000 5400 4600
Wire Wire Line
	3800 4400 4150 4400
Text Label 4200 3800 2    50   ~ 0
EN_RST
Wire Wire Line
	4300 2700 5400 2700
Text Label 4150 4400 2    50   ~ 0
~WE~
$Comp
L Connector:TestPoint TP4
U 1 1 5F3BA202
P 2600 8800
F 0 "TP4" H 2700 8950 50  0001 L CNN
F 1 "VCC" H 2700 8850 50  0001 L CNN
F 2 "TestPoint:TestPoint_THTPad_D1.5mm_Drill0.7mm" H 2800 8800 50  0001 C CNN
F 3 "~" H 2800 8800 50  0001 C CNN
	1    2600 8800
	1    0    0    -1  
$EndComp
Connection ~ 2600 8800
Wire Wire Line
	2600 8800 2850 8800
$Comp
L Device:C C7
U 1 1 5FFDC459
P 5600 9300
F 0 "C7" H 5650 9500 50  0000 L CNN
F 1 "22nF" H 5650 9400 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D4.7mm_W2.5mm_P5.00mm" H 5638 9150 50  0001 C CNN
F 3 "~" H 5600 9300 50  0001 C CNN
	1    5600 9300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 5FD67446
P 4100 9300
F 0 "C2" H 4150 9500 50  0000 L CNN
F 1 "100nF" H 3850 9400 50  0001 L CNN
F 2 "Capacitor_THT:C_Disc_D4.7mm_W2.5mm_P5.00mm" H 4138 9150 50  0001 C CNN
F 3 "~" H 4100 9300 50  0001 C CNN
	1    4100 9300
	1    0    0    -1  
$EndComp
Wire Wire Line
	14650 3250 14650 3050
Wire Wire Line
	14650 3050 14600 3050
Connection ~ 14650 3250
Wire Wire Line
	14650 3250 14600 3250
Wire Wire Line
	14050 3250 14050 3050
Wire Wire Line
	14050 3050 14100 3050
Connection ~ 14050 3250
Wire Wire Line
	14050 3250 14100 3250
$Comp
L Mechanical:MountingHole H5
U 1 1 5F96F8A9
P 600 10400
F 0 "H5" H 700 10446 50  0001 L CNN
F 1 "MountingHole" H 700 10355 50  0001 L CNN
F 2 "Symbol:OSHW-Logo2_9.8x8mm_SilkScreen" H 600 10400 50  0001 C CNN
F 3 "~" H 600 10400 50  0001 C CNN
	1    600  10400
	1    0    0    -1  
$EndComp
Text Label 9350 3500 2    50   ~ 0
RESET
Wire Wire Line
	15250 1950 15550 1950
Wire Wire Line
	15250 1850 15550 1850
Wire Wire Line
	15250 2250 15550 2250
Wire Wire Line
	15250 2050 15550 2050
Wire Wire Line
	15550 2450 15250 2450
Wire Wire Line
	15250 2550 15550 2550
Wire Wire Line
	15250 2350 15550 2350
Wire Wire Line
	15250 2150 15550 2150
Entry Wire Line
	15150 2450 15250 2550
Entry Wire Line
	15150 2350 15250 2450
Entry Wire Line
	15150 2250 15250 2350
Entry Wire Line
	15150 2150 15250 2250
Entry Wire Line
	15150 2050 15250 2150
Entry Wire Line
	15150 1950 15250 2050
Entry Wire Line
	15150 1850 15250 1950
Entry Wire Line
	15150 1750 15250 1850
Entry Wire Line
	15150 2550 15250 2650
Wire Wire Line
	15550 2650 15250 2650
Text Label 15300 1850 0    50   ~ 0
E_A0
Text Label 15300 1950 0    50   ~ 0
E_A1
Text Label 15300 2050 0    50   ~ 0
E_A2
Text Label 15300 2150 0    50   ~ 0
E_A3
Text Label 15300 2250 0    50   ~ 0
E_A4
Text Label 15300 2350 0    50   ~ 0
E_A5
Text Label 15300 2450 0    50   ~ 0
E_A6
Text Label 15300 2550 0    50   ~ 0
E_A7
NoConn ~ 13700 1700
$Comp
L Device:Q_PMOS_SGD Q3
U 1 1 5FB752EC
P 8350 9800
F 0 "Q3" V 8692 9800 50  0000 C CNN
F 1 "PMOS" V 8601 9800 50  0000 C CNN
F 2 "Package_TO_SOT_THT:TO-92L_Inline_Wide" H 8550 9900 50  0001 C CNN
F 3 "~" H 8350 9800 50  0001 C CNN
	1    8350 9800
	0    1    -1   0   
$EndComp
Wire Wire Line
	8550 9700 8700 9700
Wire Wire Line
	8150 9700 7950 9700
Wire Wire Line
	7950 9700 7950 9200
$Comp
L power:GND #PWR08
U 1 1 5FCAC526
P 8350 10050
F 0 "#PWR08" H 8350 9800 50  0001 C CNN
F 1 "GND" H 8355 9877 50  0000 C CNN
F 2 "" H 8350 10050 50  0001 C CNN
F 3 "" H 8350 10050 50  0001 C CNN
	1    8350 10050
	1    0    0    -1  
$EndComp
Wire Wire Line
	8350 10000 8350 10050
Wire Wire Line
	7950 9200 7850 9200
Wire Wire Line
	7850 9200 7850 9050
Connection ~ 7950 9200
Wire Wire Line
	16150 1850 15950 1850
Wire Wire Line
	16150 1800 16150 1850
$Comp
L power:VCC #PWR0104
U 1 1 5FAB8CD1
P 16150 1800
F 0 "#PWR0104" H 16150 1650 50  0001 C CNN
F 1 "VCC" H 16167 1973 50  0000 C CNN
F 2 "" H 16150 1800 50  0001 C CNN
F 3 "" H 16150 1800 50  0001 C CNN
	1    16150 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Network09 RN3
U 1 1 5FAB8CAF
P 15750 2250
F 0 "RN3" V 15133 2250 50  0000 C CNN
F 1 "10K" V 15224 2250 50  0000 C CNN
F 2 "Resistor_THT:R_Array_SIP10" V 16225 2250 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 15750 2250 50  0001 C CNN
	1    15750 2250
	0    1    1    0   
$EndComp
Wire Notes Line
	8900 10350 8900 9350
Wire Notes Line
	8900 9350 7800 9350
Wire Notes Line
	7800 9350 7800 10350
Wire Notes Line
	7800 10350 8900 10350
Text Notes 7750 10450 0    50   ~ 0
Experimental, do not populate!
Text Notes 14150 5750 0    50   ~ 0
RN2\nSome platforms like ZX Spectrum will not boot with \npull-up on CS/OE lines, so you may need to remove it.\nProvide "precision" socket, so RN2 can be changed or \nremoved based on "target" platform requirements.
Text Notes 12200 1050 0    79   ~ 0
RN1, RN3 - optional. If populated, use "precision" sockets\n so RN1-3 can be changed based on "target" platform requirements.
$Comp
L Memory_RAM_1:AS6C1008-55PCN U7
U 1 1 5FD993B0
P 8550 3300
F 0 "U7" H 7750 4600 50  0000 C CNN
F 1 "AS6C1008-55PCN" H 8050 4500 50  0000 C CNN
F 2 "Package_DIP:DIP-32_W15.24mm_Socket" H 8550 3400 50  0001 C CNN
F 3 "https://www.alliancememory.com/wp-content/uploads/pdf/AS6C4008.pdf" H 8550 3400 50  0001 C CNN
	1    8550 3300
	1    0    0    -1  
$EndComp
Text Label 6550 7100 0    50   ~ 0
GT_A15
Text Label 7700 3900 0    50   ~ 0
GT_A15
Wire Wire Line
	8550 1950 8550 2200
Wire Bus Line
	7300 4800 9500 4800
Entry Wire Line
	7300 4000 7400 3900
Wire Wire Line
	7400 3500 8050 3500
Wire Wire Line
	7400 3600 8050 3600
Wire Wire Line
	7400 3700 8050 3700
Wire Wire Line
	7400 3800 8050 3800
Wire Wire Line
	7400 3900 8050 3900
$Comp
L power:VCC #PWR0105
U 1 1 6039B482
P 7600 4100
F 0 "#PWR0105" H 7600 3950 50  0001 C CNN
F 1 "VCC" H 7617 4273 50  0000 C CNN
F 2 "" H 7600 4100 50  0001 C CNN
F 3 "" H 7600 4100 50  0001 C CNN
	1    7600 4100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8050 4100 7600 4100
$Comp
L power:GND #PWR0106
U 1 1 603D0FFB
P 7750 4000
F 0 "#PWR0106" H 7750 3750 50  0001 C CNN
F 1 "GND" H 7755 3827 50  0000 C CNN
F 2 "" H 7750 4000 50  0001 C CNN
F 3 "" H 7750 4000 50  0001 C CNN
	1    7750 4000
	0    1    -1   0   
$EndComp
Wire Wire Line
	8050 4000 7750 4000
Wire Bus Line
	7200 900  9500 900 
Connection ~ 9500 900 
Wire Bus Line
	9500 900  11400 900 
$Comp
L power:GND #PWR0107
U 1 1 5FE03E5D
P 9250 3300
F 0 "#PWR0107" H 9250 3050 50  0001 C CNN
F 1 "GND" H 9255 3127 50  0000 C CNN
F 2 "" H 9250 3300 50  0001 C CNN
F 3 "" H 9250 3300 50  0001 C CNN
	1    9250 3300
	0    -1   1    0   
$EndComp
Wire Wire Line
	9050 3500 9350 3500
Wire Wire Line
	9150 3400 9150 3300
Wire Wire Line
	9150 3300 9250 3300
Wire Wire Line
	9050 3400 9150 3400
$Comp
L power:GND #PWR0108
U 1 1 5FE7C64D
P 7750 4200
F 0 "#PWR0108" H 7750 3950 50  0001 C CNN
F 1 "GND" H 7755 4027 50  0000 C CNN
F 2 "" H 7750 4200 50  0001 C CNN
F 3 "" H 7750 4200 50  0001 C CNN
	1    7750 4200
	0    1    -1   0   
$EndComp
Wire Wire Line
	8050 4200 7750 4200
Wire Wire Line
	4250 4500 4250 6300
Wire Wire Line
	3100 2600 3100 2200
Connection ~ 3100 2600
Text Label 3450 1650 2    50   ~ 0
~WE~
Wire Wire Line
	3100 1650 3450 1650
Wire Wire Line
	3100 1650 3100 1900
Connection ~ 3300 6800
Wire Wire Line
	2900 6400 2900 6500
Wire Wire Line
	2900 6200 2900 6400
Connection ~ 2900 6400
$Comp
L Memory_EEPROM:AT25xxx U3
U 1 1 5EE0EEA1
P 3300 6500
F 0 "U3" H 2800 6950 50  0000 C CNN
F 1 "25LC512" H 2950 6850 50  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm_Socket" H 3300 6500 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8707-SEEPROM-AT25010B-020B-040B-Datasheet.pdf" H 3300 6500 50  0001 C CNN
	1    3300 6500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 5FFA595A
P 3300 2800
F 0 "R8" V 3400 2800 50  0000 L CNN
F 1 "10k" V 3300 2750 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 3230 2800 50  0001 C CNN
F 3 "~" H 3300 2800 50  0001 C CNN
	1    3300 2800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3450 2800 3550 2800
Wire Wire Line
	3150 2800 3100 2800
Connection ~ 3100 2800
Wire Wire Line
	3100 2800 3100 3200
Wire Wire Line
	8450 9200 8700 9200
Wire Wire Line
	8700 9700 8700 9200
Wire Bus Line
	2350 4400 2350 7750
Wire Bus Line
	7300 4800 7300 6450
Wire Bus Line
	7300 3600 7300 4800
Wire Bus Line
	8850 5400 8850 7750
Wire Bus Line
	11400 900  11400 2400
Wire Bus Line
	9500 900  9500 4800
Wire Bus Line
	7200 900  7200 1850
Wire Bus Line
	15150 1200 15150 4550
Wire Bus Line
	13300 1200 13300 5900
Connection ~ 8700 9200
Wire Wire Line
	8700 9200 8850 9200
$EndSCHEMATC
