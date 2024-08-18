#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream> 

#pragma comment(lib, "winmm.lib")

using namespace std; 

// this are constants declarations
const int FRAME_SIZE = 300;
const int MAX_DATA_SIZE = 16025 * 3;
short waveIn[MAX_DATA_SIZE];

//these are functions declarations of all the functions i have used in this program
void PlayRecord();
void StartRecord();
void calculate_energy(const short data[], int number_of_data_in_file, double energy[]);
void cal_ZCR(const short data[], int number_of_data_in_file, int zcr[]);
void calculate_avg_energy(const double energy[], int number_of_frames, double& average_energy);
void cal_avg_ZCR(const int zcr[], int number_of_frames, int& average_ZCR);
void writedataTofile(const short data[], int number_of_data_in_file);


// this function starts recording audio for a fixed duration (3 seconds)
void StartRecord() {
    const int NUMPTS = 16025 * 3;   // 3 seconds
    int sampleRate = 16025;
    HWAVEIN hWaveIn;
    MMRESULT result;
    WAVEFORMATEX pFormat;

    pFormat.wFormatTag = WAVE_FORMAT_PCM;
    pFormat.nChannels = 1;
    pFormat.nSamplesPerSec = sampleRate;
    pFormat.nAvgBytesPerSec = sampleRate * 2;
    pFormat.nBlockAlign = 2;
    pFormat.wBitsPerSample = 16;
    pFormat.cbSize = 0;

    result = waveInOpen(&hWaveIn, WAVE_MAPPER, &pFormat, 0L, 0L, WAVE_FORMAT_DIRECT);
    WAVEHDR WaveInHdr;

    WaveInHdr.lpData = (LPSTR)waveIn;
    WaveInHdr.dwBufferLength = NUMPTS * 2;
    WaveInHdr.dwBytesRecorded = 0;
    WaveInHdr.dwUser = 0L;
    WaveInHdr.dwFlags = 0L;
    WaveInHdr.dwLoops = 0L;

    waveInPrepareHeader(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));
    result = waveInAddBuffer(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));
    result = waveInStart(hWaveIn);

    cout << "Recording for 3 seconds..." << endl;
    Sleep(3 * 1000);

    waveInStop(hWaveIn);
    waveInClose(hWaveIn);

    // saving the audio input data into a file
    writedataTofile(waveIn, NUMPTS);

    PlayRecord();
}

// Plays back the recorded audio data
void PlayRecord() {
    const int NUMPTS = 16025 * 3;
    int sampleRate = 16025;
    HWAVEOUT hWaveOut;
    WAVEFORMATEX pFormat;

    pFormat.wFormatTag = WAVE_FORMAT_PCM;
    pFormat.nChannels = 1;
    pFormat.nSamplesPerSec = sampleRate;
    pFormat.nAvgBytesPerSec = sampleRate * 2;
    pFormat.nBlockAlign = 2;
    pFormat.wBitsPerSample = 16;
    pFormat.cbSize = 0;

    waveOutOpen(&hWaveOut, WAVE_MAPPER, &pFormat, 0, 0, WAVE_FORMAT_DIRECT);

    cout << "Analyzing recorded data..." << endl;

    double energy[MAX_DATA_SIZE / FRAME_SIZE];
    int zcr[MAX_DATA_SIZE / FRAME_SIZE];

    int number_of_frames = MAX_DATA_SIZE / FRAME_SIZE;

    calculate_energy(waveIn, MAX_DATA_SIZE, energy);
    cal_ZCR(waveIn, MAX_DATA_SIZE, zcr);

    double average_energy = 0;
    int average_ZCR = 0;

    calculate_avg_energy(energy, number_of_frames, average_energy);
    cal_avg_ZCR(zcr, number_of_frames, average_ZCR);

    //cout << "Average energy = " << average_energy << endl;
    //cout << "Average ZCR = " << average_ZCR << endl;

    // Updated thresholds are from the data that i have used in the previous assignemnt with some changes. i.e threshold change
    const double energy_low_threshold = 1.0e+6;
    const double energy_high_threshold = 1.2e+7;
    const int zcr_low_threshold = 50;
    const int zcr_high_threshold = 90;

    if ((average_energy >= energy_low_threshold && average_energy <= energy_high_threshold) &&
        (average_ZCR >= zcr_low_threshold && average_ZCR <= zcr_high_threshold)) {
        cout << "Output: YES" << endl;
    } else {
        cout << "Output: NO" << endl;
    }

    waveOutClose(hWaveOut);
}


// this is writing the audio data in a text file 
void writedataTofile(const short data[], int number_of_data_in_file) {
    ofstream file("input.txt", ios::binary); 
    if (!file) {
        cerr << "Error opening file for writing." << endl;
        return;
    }
    file.write(reinterpret_cast<const char*>(data), number_of_data_in_file * sizeof(short));
    file.close();
    cout << "Recorded data saved to input.txt" << endl;
}

//this function is for calculating the average energy  frame by frame
void calculate_energy(const short data[], int number_of_data_in_file, double energy[]) {
    int number_of_frames = number_of_data_in_file / FRAME_SIZE;
    for (int i = 0; i < number_of_frames; i++) {
        double energySum = 0;
        for (int j = 0; j < FRAME_SIZE; j++) {
            int index = i * FRAME_SIZE + j;
            energySum += data[index] * data[index];
        }
        energy[i] = energySum;
        cout << "Frame " << i << " Energy: " << energy[i] << endl;
    }
}

// this function is for calculating the average ZCR  from  zcr array
void cal_ZCR(const short data[], int number_of_data_in_file, int zcr[]) {
    int number_of_frames = number_of_data_in_file / FRAME_SIZE;
    for (int i = 0; i < number_of_frames; i++) {
        int elements_in_array = 0;
        for (int j = 0; j < FRAME_SIZE; j++) {
            int index = i * FRAME_SIZE + j;
            if (j > 0 && ((data[index - 1] < 0 && data[index] > 0) || (data[index - 1] > 0 && data[index] < 0))) {
                elements_in_array++;
            }
        }
        zcr[i] = elements_in_array;
        cout << "Frame " << i << " ZCR: " << zcr[i] << endl; // Debug print
    }
}



//this fun tion is for calculating the average energy
void calculate_avg_energy(const double energy[], int number_of_frames, double& average_energy) {
    average_energy = 0;
    for (int i = 0; i < number_of_frames; i++) {
        average_energy += energy[i];
    }
    average_energy /= number_of_frames;
    cout << "Average Energy: " << average_energy << endl; 
}
//this fun tion is for calculating the average zcr
void cal_avg_ZCR(const int zcr[], int number_of_frames, int& average_ZCR) {
    average_ZCR = 0;
    for (int i = 0; i < number_of_frames; i++) {
        average_ZCR += zcr[i];
    }
    average_ZCR /= number_of_frames;
    cout << "Average ZCR: " << average_ZCR << endl; 
}


//this is the main function from where the program starts top execute.Stating pointing of the program
int _tmain(int argc, _TCHAR* argv[]) {
    StartRecord();
    return 0;
}

