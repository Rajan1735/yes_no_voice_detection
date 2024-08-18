 CONSOLE APPLICATION : yes_no Project Overview(Was made in ms visual studio 2010) 

Program Overview

This program records audio data for 3 seconds, saves it to a file, and then analyzes the recorded data to determine if it corresponds yes or no voice. 
It calculates the energy and zero-crossing rate of the recorded audio and  give output whether it was yes or no

Functionalities : 

1. Recording Audio: The program records audio for 3 seconds using the Windows Multimedia API.It saves the recorded audio data to a file named input.txt.

Energy and Zero-Crossing Rate Calculation: The program calculates the energy and zero-crossing rate (ZCR) for each frame of the recorded data.

2. Energy Calculation: It calculates  the sum of squares of the data points within each frame. This provides a measure of the signal's amplitude.

3. Zero-Crossing Rate (ZCR): Calculated by counting the number of times the signal changes its sign (crosses zero) within each frame. This shows the rate of sign changes in the audio signal.

4.Average Calculation: The program calculates the average energy and average ZCR across all frames of the recorded data.

5.Decision Making: Based on predefined thresholds for energy and ZCR, the program determines whether the recorded data falls within a specified range.

6. Calculated thresholds of energy and zcr are: 
	Energy:
	Low threshold: 1.0e+6
	High threshold: 1.2e+7
	ZCR:
	Low threshold: 50
	High threshold: 90

6.Range of yes: Average energy(1.0e+6- 1.2e+7) and average ZCR (50-90). then it gives yes output
7. When the average energy and average zcr does not fall in that range then it is gives NO output
========================================================================
