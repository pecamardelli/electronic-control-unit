#include "HolleySniper.h"
#include <cstdio>

HolleySniper::HolleySniper(MCP2515* can) : can_controller(can) {
    // Initialize engine data structure
    memset(&engine_data, 0, sizeof(engine_data));
    engine_data.data_valid = false;
}

bool HolleySniper::init() {
    if (!can_controller) {
        return false;
    }
    
    // Initialize timestamps
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    engine_data.last_rpm_time = current_time;
    engine_data.last_coolant_time = current_time;
    engine_data.last_fuel_flow_time = current_time;
    engine_data.last_update_time = current_time;
    
    printf("Holley Sniper CAN decoder initialized\n");
    return true;
}

float HolleySniper::extractFloat32(const uint8_t* data, uint8_t start_byte, bool big_endian) {
    uint32_t raw_value;
    
    if (big_endian) {
        // Big endian (most significant byte first)
        raw_value = ((uint32_t)data[start_byte] << 24) |
                   ((uint32_t)data[start_byte + 1] << 16) |
                   ((uint32_t)data[start_byte + 2] << 8) |
                   ((uint32_t)data[start_byte + 3]);
    } else {
        // Little endian (least significant byte first)
        raw_value = ((uint32_t)data[start_byte + 3] << 24) |
                   ((uint32_t)data[start_byte + 2] << 16) |
                   ((uint32_t)data[start_byte + 1] << 8) |
                   ((uint32_t)data[start_byte]);
    }
    
    // Convert to float using union
    union {
        uint32_t i;
        float f;
    } converter;
    converter.i = raw_value;
    
    return converter.f;
}

uint64_t HolleySniper::extractUint64(const uint8_t* data, uint8_t start_byte, bool big_endian) {
    uint64_t result = 0;
    
    if (big_endian) {
        for (int i = 0; i < 8; i++) {
            result = (result << 8) | data[start_byte + i];
        }
    } else {
        for (int i = 7; i >= 0; i--) {
            result = (result << 8) | data[start_byte + i];
        }
    }
    
    return result;
}

bool HolleySniper::isDataFresh(uint32_t timestamp, uint32_t max_age_ms) const {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    return (current_time - timestamp) <= max_age_ms;
}

void HolleySniper::processRPMMessage(const CANFrame& frame) {
    if (frame.dlc >= 4) {
        // RPM is a 32-bit float starting at bit 24 (byte 3), big endian
        engine_data.rpm = extractFloat32(frame.data, 3, true);
        engine_data.rpm_valid = true;
        engine_data.last_rpm_time = to_ms_since_boot(get_absolute_time());
        
        // Status byte at bit 56 (byte 7)
        if (frame.dlc >= 8) {
            uint8_t status = frame.data[7];
            // You can interpret status bits here if needed
        }
    }
}

void HolleySniper::processCoolantTempMessage(const CANFrame& frame) {
    if (frame.dlc >= 4) {
        // CTS is a 32-bit float starting at bit 24 (byte 3), big endian
        engine_data.coolant_temp = extractFloat32(frame.data, 3, true);
        engine_data.coolant_temp_valid = true;
        engine_data.last_coolant_time = to_ms_since_boot(get_absolute_time());
        
        // Status byte at bit 56 (byte 7)
        if (frame.dlc >= 8) {
            uint8_t status = frame.data[7];
            // You can interpret status bits here if needed
        }
    }
}

void HolleySniper::processFuelFlowMessage(const CANFrame& frame) {
    if (frame.dlc >= 4) {
        // Fuel_Flow is a 32-bit float starting at bit 24 (byte 3), big endian
        engine_data.fuel_flow = extractFloat32(frame.data, 3, true);
        engine_data.fuel_flow_valid = true;
        engine_data.last_fuel_flow_time = to_ms_since_boot(get_absolute_time());
        
        // Status byte at bit 56 (byte 7)
        if (frame.dlc >= 8) {
            uint8_t status = frame.data[7];
            // You can interpret status bits here if needed
        }
    }
}

void HolleySniper::processMAPMessage(const CANFrame& frame) {
    if (frame.dlc >= 4) {
        engine_data.map_pressure = extractFloat32(frame.data, 3, true);
        engine_data.map_valid = true;
    }
}

void HolleySniper::processTPSMessage(const CANFrame& frame) {
    if (frame.dlc >= 4) {
        engine_data.tps_position = extractFloat32(frame.data, 3, true);
        engine_data.tps_valid = true;
    }
}

void HolleySniper::processAFRMessage(const CANFrame& frame) {
    if (frame.dlc >= 4) {
        engine_data.air_fuel_ratio = extractFloat32(frame.data, 3, true);
        engine_data.afr_valid = true;
    }
}

void HolleySniper::processTargetAFRMessage(const CANFrame& frame) {
    if (frame.dlc >= 4) {
        engine_data.target_afr = extractFloat32(frame.data, 3, true);
    }
}

void HolleySniper::processIgnitionTimingMessage(const CANFrame& frame) {
    if (frame.dlc >= 4) {
        engine_data.ignition_timing = extractFloat32(frame.data, 3, true);
        engine_data.timing_valid = true;
    }
}

void HolleySniper::processBatteryMessage(const CANFrame& frame) {
    if (frame.dlc >= 4) {
        engine_data.battery_voltage = extractFloat32(frame.data, 3, true);
        engine_data.battery_valid = true;
    }
}

void HolleySniper::processMATMessage(const CANFrame& frame) {
    if (frame.dlc >= 4) {
        engine_data.manifold_air_temp = extractFloat32(frame.data, 3, true);
        engine_data.mat_valid = true;
    }
}

bool HolleySniper::processCANMessages() {
    if (!can_controller) {
        return false;
    }
    
    bool processed_any = false;
    CANFrame frame;
    
    // Process all available messages
    while (can_controller->readFrame(frame)) {
        if (processCANMessage(frame)) {
            processed_any = true;
        }
    }
    
    // Update overall data validity
    engine_data.data_valid = isEngineDataValid();
    
    if (processed_any) {
        engine_data.last_update_time = to_ms_since_boot(get_absolute_time());
    }
    
    return processed_any;
}

bool HolleySniper::processCANMessage(const CANFrame& frame) {
    // Only process extended frames (29-bit IDs)
    if (!frame.extended) {
        return false;
    }
    
    switch (frame.id) {
        case HOLLEY_RPM_ID:
            processRPMMessage(frame);
            return true;
            
        case HOLLEY_CTS_ID:
            processCoolantTempMessage(frame);
            return true;
            
        case HOLLEY_FUEL_FLOW_ID:
            processFuelFlowMessage(frame);
            return true;
            
        case HOLLEY_MAP_ID:
            processMAPMessage(frame);
            return true;
            
        case HOLLEY_TPS_ID:
            processTPSMessage(frame);
            return true;
            
        case HOLLEY_AIRFUEL_RATIO_ID:
            processAFRMessage(frame);
            return true;
            
        case HOLLEY_TARGET_AFR_ID:
            processTargetAFRMessage(frame);
            return true;
            
        case HOLLEY_IGNITION_TIMING_ID:
            processIgnitionTimingMessage(frame);
            return true;
            
        case HOLLEY_BATTERY_ID:
            processBatteryMessage(frame);
            return true;
            
        case HOLLEY_MAT_ID:
            processMATMessage(frame);
            return true;
            
        default:
            return false;  // Unknown message ID
    }
}

bool HolleySniper::isEngineDataValid() const {
    // Consider data valid if we have at least RPM and it's fresh
    return isRPMValid();
}

void HolleySniper::printEngineData() const {
    printf("=== Holley Sniper Engine Data ===\n");
    
    if (isRPMValid()) {
        printf("RPM: %.1f\n", engine_data.rpm);
    } else {
        printf("RPM: INVALID\n");
    }
    
    if (isCoolantTempValid()) {
        printf("Coolant Temp: %.1f°F (%.1f°C)\n", 
               engine_data.coolant_temp, 
               convertFahrenheitToCelsius(engine_data.coolant_temp));
    } else {
        printf("Coolant Temp: INVALID\n");
    }
    
    if (isFuelFlowValid()) {
        printf("Fuel Flow: %.2f lb/hr (%.2f gal/hr)\n", 
               engine_data.fuel_flow,
               convertLbHrToGalHr(engine_data.fuel_flow));
    } else {
        printf("Fuel Flow: INVALID\n");
    }
    
    if (engine_data.map_valid) {
        printf("MAP: %.1f\n", engine_data.map_pressure);
    }
    
    if (engine_data.tps_valid) {
        printf("TPS: %.1f%%\n", engine_data.tps_position);
    }
    
    if (engine_data.afr_valid) {
        printf("AFR: %.2f (Target: %.2f)\n", engine_data.air_fuel_ratio, engine_data.target_afr);
    }
    
    if (engine_data.timing_valid) {
        printf("Ignition Timing: %.1f°\n", engine_data.ignition_timing);
    }
    
    if (engine_data.battery_valid) {
        printf("Battery: %.1fV\n", engine_data.battery_voltage);
    }
    
    if (engine_data.mat_valid) {
        printf("MAT: %.1f°F\n", engine_data.manifold_air_temp);
    }
    
    printf("Data Valid: %s\n", engine_data.data_valid ? "YES" : "NO");
    printf("================================\n");
}
