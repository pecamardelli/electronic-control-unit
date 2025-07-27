#ifndef HOLLEY_SNIPER_H
#define HOLLEY_SNIPER_H

#include "MCP2515.h"
#include <cstring>

// Holley Sniper CAN IDs (from JSON - extended frame format)
#define HOLLEY_RTC_ID               503320576   // 0x1E000000
#define HOLLEY_RPM_ID               503336960   // 0x1E004000  
#define HOLLEY_INJ_PW_ID            503353344   // 0x1E008000
#define HOLLEY_DUTY_CYCLE_ID        503369728   // 0x1E00C000
#define HOLLEY_CL_COMP_ID           503386112   // 0x1E010000
#define HOLLEY_TARGET_AFR_ID        503402496   // 0x1E014000
#define HOLLEY_AIRFUEL_RATIO_ID     503418880   // 0x1E018000
#define HOLLEY_AIR_TEMP_ENR_ID      503435264   // 0x1E01C000
#define HOLLEY_COOLANT_ENR_ID       503451648   // 0x1E020000
#define HOLLEY_COOLANT_AFR_OFFSET_ID 503468032  // 0x1E024000
#define HOLLEY_AFTERSTART_ENR_ID    503484416   // 0x1E028000
#define HOLLEY_CURRENT_LEARN_ID     503500800   // 0x1E02C000
#define HOLLEY_CL_STATUS_ID         503517184   // 0x1E030000
#define HOLLEY_LEARN_STATUS_ID      503533568   // 0x1E034000
#define HOLLEY_FUEL_FLOW_ID         503549952   // 0x1E038000 - FUEL CONSUMPTION
#define HOLLEY_MAP_ROC_ID           503566336   // 0x1E03C000
#define HOLLEY_TPS_ROC_ID           503582720   // 0x1E040000
#define HOLLEY_ESTIMATED_VE_ID      503599104   // 0x1E044000
#define HOLLEY_IGNITION_TIMING_ID   503615488   // 0x1E048000
#define HOLLEY_MAIN_REV_LIMIT_ID    503631872   // 0x1E04C000
#define HOLLEY_REV_LIMIT_1_ID       503648256   // 0x1E050000
#define HOLLEY_LAUNCH_RETARD_ID     503664640   // 0x1E054000
#define HOLLEY_IAC_POSITION_ID      503681024   // 0x1E058000
#define HOLLEY_MAP_ID               503697408   // 0x1E05C000
#define HOLLEY_TPS_ID               503713792   // 0x1E060000
#define HOLLEY_MAT_ID               503730176   // 0x1E064000
#define HOLLEY_CTS_ID               503746560   // 0x1E068000 - ENGINE TEMPERATURE
#define HOLLEY_BATTERY_ID           503762944   // 0x1E06C000
#define HOLLEY_CUSTOM1_ID           503779328   // 0x1E070000
#define HOLLEY_CUSTOM2_ID           503795712   // 0x1E074000
#define HOLLEY_CUSTOM3_ID           503812096   // 0x1E078000

// Engine data structure
struct HolleyEngineData {
    // Primary engine parameters
    float rpm;                  // Engine RPM
    float coolant_temp;         // Coolant temperature (°F)
    float fuel_flow;            // Fuel flow rate (lb/hr or gal/hr)
    float map_pressure;         // Manifold pressure
    float tps_position;         // Throttle position (%)
    float air_fuel_ratio;       // Current AFR
    float target_afr;           // Target AFR
    float ignition_timing;      // Ignition timing (degrees)
    float battery_voltage;      // Battery voltage
    float manifold_air_temp;    // MAT sensor
    
    // Status flags
    bool data_valid;            // Overall data validity
    bool rpm_valid;
    bool coolant_temp_valid;
    bool fuel_flow_valid;
    bool map_valid;
    bool tps_valid;
    bool afr_valid;
    bool timing_valid;
    bool battery_valid;
    bool mat_valid;
    
    // Timestamps for data freshness
    uint32_t last_rpm_time;
    uint32_t last_coolant_time;
    uint32_t last_fuel_flow_time;
    uint32_t last_update_time;
};

class HolleySniper {
private:
    MCP2515* can_controller;
    HolleyEngineData engine_data;
    
    // Data parsing helpers
    float extractFloat32(const uint8_t* data, uint8_t start_byte, bool big_endian = true);
    uint64_t extractUint64(const uint8_t* data, uint8_t start_byte, bool big_endian = true);
    bool isDataFresh(uint32_t timestamp, uint32_t max_age_ms = 5000) const;  // 5 second timeout
    
    // Message processing
    void processRPMMessage(const CANFrame& frame);
    void processCoolantTempMessage(const CANFrame& frame);
    void processFuelFlowMessage(const CANFrame& frame);
    void processMAPMessage(const CANFrame& frame);
    void processTPSMessage(const CANFrame& frame);  
    void processAFRMessage(const CANFrame& frame);
    void processTargetAFRMessage(const CANFrame& frame);
    void processIgnitionTimingMessage(const CANFrame& frame);
    void processBatteryMessage(const CANFrame& frame);
    void processMATMessage(const CANFrame& frame);
    
public:
    // Constructor
    HolleySniper(MCP2515* can);
    
    // Initialization
    bool init();
    
    // Data processing
    bool processCANMessages();
    bool processCANMessage(const CANFrame& frame);
    
    // Data access
    const HolleyEngineData& getEngineData() const { return engine_data; }
    
    // Individual parameter access
    float getRPM() const { return engine_data.rpm; }
    float getCoolantTemp() const { return engine_data.coolant_temp; }
    float getFuelFlow() const { return engine_data.fuel_flow; }
    float getMAP() const { return engine_data.map_pressure; }
    float getTPS() const { return engine_data.tps_position; }
    float getAFR() const { return engine_data.air_fuel_ratio; }
    float getTargetAFR() const { return engine_data.target_afr; }
    float getIgnitionTiming() const { return engine_data.ignition_timing; }
    float getBatteryVoltage() const { return engine_data.battery_voltage; }
    float getManifoldAirTemp() const { return engine_data.manifold_air_temp; }
    
    // Data validity checks
    bool isRPMValid() const { return engine_data.rpm_valid && isDataFresh(engine_data.last_rpm_time); }
    bool isCoolantTempValid() const { return engine_data.coolant_temp_valid && isDataFresh(engine_data.last_coolant_time); }
    bool isFuelFlowValid() const { return engine_data.fuel_flow_valid && isDataFresh(engine_data.last_fuel_flow_time); }
    bool isEngineDataValid() const;
    
    // Utility functions
    float convertCelsiusToFahrenheit(float celsius) const { return (celsius * 9.0f / 5.0f) + 32.0f; }
    float convertFahrenheitToCelsius(float fahrenheit) const { return (fahrenheit - 32.0f) * 5.0f / 9.0f; }
    float convertLbHrToGalHr(float lb_hr, float fuel_density = 6.0f) const { return lb_hr / fuel_density; } // Gasoline ~6 lb/gal
    
    // Diagnostic functions
    void printEngineData() const;
    uint32_t getLastUpdateTime() const { return engine_data.last_update_time; }
};

#endif // HOLLEY_SNIPER_H
