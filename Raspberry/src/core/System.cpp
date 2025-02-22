#include "System.h"

System::System(std::string programName)
{
    mainRelay.state(HIGH);
    config = loadConfig("/etc/" + programName + "/config.ini");
    dataPath = "/var/" + programName;
    totalMileageFileName = dataPath + "/total_mileage";
    partialMileageFileName = dataPath + "/partial_mileage";

    loadMileage();

    // Display parsed configuration
    // for (const auto &[section, values] : config)
    // {
    //     std::cout << "[" << section << "]" << std::endl;
    //     for (const auto &[key, value] : values)
    //     {
    //         std::cout << key << " = " << value << std::endl;
    //     }
    //     std::cout << std::endl;
    // }
}

System::~System()
{
}

void System::setProgramName(const char *fullName) {}

void System::shutdown()
{
    logger.info("Shutting down...");
    // system("init 0");
    exit(0);
}

uint64_t System::uptime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * 1000000 + ts.tv_nsec / 1000;
}

ConfigMap System::loadConfig(const std::string &filename)
{
    ConfigMap parsedConfig;
    ConfigMap result = defaultConfigValues;
    std::ifstream file(filename);

    if (!file)
    {
        logger.warning("[Config] Could not open INI file: " + filename + ". Using defaults!");
        return result;
    }

    std::string line, currentSection = "global"; // Default section
    parsedConfig[currentSection] = {};           // Initialize global section
    size_t lineNumber = 0;

    while (std::getline(file, line))
    {
        ++lineNumber;
        line = trim(line);

        // Skip empty lines and comments (starting with '#' or ';')
        if (line.empty() || line[0] == '#' || line[0] == ';')
        {
            continue;
        }

        // Check for section headers (e.g., [section_name])
        if (line[0] == '[')
        {
            if (line.back() != ']')
            {
                logger.error("[Config] Malformed section header at line " + std::to_string(lineNumber) + ": " + line);
                continue;
            }
            currentSection = trim(line.substr(1, line.size() - 2)); // Extract section name
            if (currentSection.empty())
            {
                logger.error("[Config] Empty section name at line " + std::to_string(lineNumber));
                continue;
            }
            parsedConfig[currentSection] = {}; // Initialize section in the map
        }
        else
        {
            // Parse key-value pairs
            size_t pos = line.find('=');
            if (pos == std::string::npos)
            {
                logger.error("[Config] Missing '=' in key-value pair at line " + std::to_string(lineNumber) + ": " + line);
                continue;
            }

            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            if (key.empty())
            {
                logger.error("[Config] Empty key in key-value pair at line " + std::to_string(lineNumber));
                continue;
            }

            parsedConfig[currentSection][key] = value;
        }
    }

    // Merge with default values.
    for (const auto &[section, values] : parsedConfig)
    {
        for (const auto &[key, value] : values)
        {
            result[section][key] = value; // Override defaults with parsed values
        }
    }

    return result;
}

SectionMap System::getConfig(const std::string &section)
{
    // Locate the section
    auto sectionIt = config.find(section);
    if (sectionIt == config.end())
    {
        throw std::runtime_error("Section not found: " + section);
    }

    return sectionIt->second; // Dereference the iterator to get the section
}

std::string System::readFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

void System::writeFile(const std::string &filename, const std::string &data)
{
    std::ofstream file(filename);
    if (!file)
    {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    file << data;

    if (!file)
    {
        throw std::runtime_error("Error writing to file: " + filename);
    }
}

void System::loadMileage()
{
    std::string fileContent;

    try
    {
        fileContent = readFile(totalMileageFileName);
        mileage->total = std::stoul(fileContent);
        fileContent = readFile(partialMileageFileName);
        mileage->partial = std::stof(fileContent);
    }
    catch (const std::exception &e)
    {
        logger.error("Error converting mileage value: " + std::string(e.what()));
    }
}

void System::saveMileage()
{
    writeFile(totalMileageFileName, std::to_string(mileage->total));
    writeFile(partialMileageFileName, std::to_string(mileage->partial));
}
