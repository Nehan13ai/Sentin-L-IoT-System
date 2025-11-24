/*
 * ======================================================================================
 * PROJECT: Sentin-L | Industrial IoT Predictive Maintenance System
 * DOMAIN:  IoT (Internet of Things) & AI (Predictive Analytics)
 * AUTHOR:  Neha Nala
 *
 * DESCRIPTION:
 * This system simulates a factory motor's sensors (Temperature & Vibration).
 * It logs data to a CSV database and uses a Linear Regression algorithm (AI)
 * to predict "Time-to-Failure" before the machine actually breaks.
 * ======================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // Standard library for sleep() on Linux/Online Compilers
#include <string.h>

// --- CONFIGURATION ---
#define DATA_FILE "machine_logs.csv"
#define CRITICAL_TEMP 100.0
#define CRITICAL_VIBRATION 50.0

// --- DATA STRUCTURES ---
typedef struct {
    int timestamp;
    float temperature;
    float vibration;
    int status; // 0=OK, 1=WARNING, 2=CRITICAL
} SensorData;

// --- MODULE 1: HARDWARE SIMULATION (IoT) ---
// Simulates reading from physical sensors on a machine
SensorData read_sensors(int time_step) {
    SensorData data;
    data.timestamp = time_step;
    
    // Simulate a machine getting hotter and shakier over time (Degradation)
    float random_noise_temp = ((float)rand() / RAND_MAX) * 2.0;
    float random_noise_vib = ((float)rand() / RAND_MAX) * 1.0;

    // *** UPDATED SPEED: Rising by 3.0 degrees per second (Fast Simulation) ***
    data.temperature = 40.0 + (time_step * 3.0) + random_noise_temp; 
    
    // *** UPDATED SPEED: Vibration increasing faster ***
    data.vibration = 10.0 + (time_step * 1.5) + random_noise_vib;

    // Determine Status
    if (data.temperature > CRITICAL_TEMP || data.vibration > CRITICAL_VIBRATION) {
        data.status = 2; // CRITICAL
    } else if (data.temperature > CRITICAL_TEMP * 0.8) {
        data.status = 1; // WARNING
    } else {
        data.status = 0; // OK
    }

    return data;
}

// --- MODULE 2: DATABASE MANAGEMENT ---
// Saves sensor data to a CSV file for Excel/History analysis
void log_to_database(SensorData data) {
    FILE *fp = fopen(DATA_FILE, "a"); // 'a' means append mode
    if (fp == NULL) {
        printf("ERROR: Could not access database file!\n");
        return;
    }
    
    // Format: Time, Temperature, Vibration, Status
    fprintf(fp, "%d,%.2f,%.2f,%d\n", data.timestamp, data.temperature, data.vibration, data.status);
    fclose(fp);
}

// --- MODULE 3: AI PREDICTION ENGINE ---
// Uses Linear Regression logic to predict WHEN the machine will fail
void predict_failure(SensorData current, SensorData previous) {
    printf("\n   [ AI ANALYTICS ENGINE ]\n");
    
    // Calculate Rate of Change (Slope)
    float temp_rate = current.temperature - previous.temperature;
    
    if (temp_rate <= 0) {
        printf("   >> Status: Stable. No immediate risk detected.\n");
        return;
    }

    // Predictive Math: (Target - Current) / Rate
    float temp_diff = CRITICAL_TEMP - current.temperature;
    float steps_to_failure = temp_diff / temp_rate;

    if (steps_to_failure < 0) steps_to_failure = 0;

    printf("   >> Trend Detected: Temperature rising by %.2f C/sec\n", temp_rate);
    
    if (steps_to_failure < 10) {
        printf("   >> ALERT: PREDICTED FAILURE IN %.0f SECONDS!\n", steps_to_failure);
        printf("   >> ACTION: RECOMMENDING EMERGENCY SHUTDOWN.\n");
    } else {
        printf("   >> PREDICTION: Safe operation for next %.0f seconds.\n", steps_to_failure);
    }
}

// --- MODULE 4: USER INTERFACE ---
void print_dashboard(SensorData data) {
    // UPDATED: Using 'clear' for Linux/Online Compilers to avoid errors
    system("clear"); 

    printf("====================================================\n");
    printf("  SENTIN-L | INDUSTRIAL MONITORING SYSTEM (IoT)\n");
    printf("====================================================\n");
    printf("  SENSOR READINGS:\n");
    printf("  [+] Temperature : %.2f C  ", data.temperature);
    
    // Visual Bar for Temp
    if(data.temperature < 60) printf("[====      ]\n");
    else if(data.temperature < 90) printf("[========  ]\n");
    else printf("[==========] !!!\n");

    printf("  [+] Vibration   : %.2f Hz \n", data.vibration);
    
    printf("----------------------------------------------------\n");
    printf("  SYSTEM STATUS   : ");
    if (data.status == 0) printf("NORMAL [ OK ]\n");
    if (data.status == 1) printf("WARNING [ ! ]\n");
    if (data.status == 2) printf("CRITICAL [ X ]\n");
    printf("====================================================\n");
}

int main() {
    srand(time(0)); // Initialize random seed
    
    // Create CSV Header if file doesn't exist
    FILE *fp = fopen(DATA_FILE, "w");
    fprintf(fp, "Timestamp,Temperature,Vibration,Status\n");
    fclose(fp);

    printf("Booting Sentin-L System...\n");
    printf("Connecting to IoT Sensors...\n");
    sleep(1); 
    
    SensorData current_data;
    SensorData prev_data = {0, 0.0, 0.0, 0};
    int time_step = 0;

    // The Main Monitoring Loop (Real-time System)
    while (1) {
        time_step++;
        
        // 1. Acquire Data (IoT)
        current_data = read_sensors(time_step);

        // 2. Log Data (Database)
        log_to_database(current_data);

        // 3. Update UI
        print_dashboard(current_data);

        // 4. Run AI Analysis
        if (time_step > 1) { // Need at least 2 points for prediction
            predict_failure(current_data, prev_data);
        }

        // 5. Emergency Cutoff Logic
        if (current_data.status == 2) {
            printf("\n*** CRITICAL FAILURE DETECTED ***\n");
            printf("*** SYSTEM HALTED TO PREVENT EXPLOSION ***\n");
            break;
        }

        prev_data = current_data;
        
        // Wait before next scan (1 second)
        sleep(1); 
    }

    printf("\nSession Data saved to '%s'.\n", DATA_FILE);
    return 0;
}